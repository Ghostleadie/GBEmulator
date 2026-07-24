//
// Created by Jack_ on 24/07/2026.
//

#include "apu.h"

#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_init.h>

#include <algorithm>

#include "../log/log.h"

namespace
{
	// Master clock (T-cycles/second) and the host PCM rate we resample down to.
	constexpr int CPU_FREQ    = 4194304;
	constexpr int SAMPLE_RATE = 48000;
	constexpr int CHANNELS    = 2;

	// The frame sequencer steps at 512 Hz: one step every 8192 T-cycles.
	constexpr int FRAME_SEQ_PERIOD = CPU_FREQ / 512;

	// Push finished audio to SDL in blocks of this many stereo frames. Small
	// enough for low latency, large enough to avoid a syscall per sample.
	constexpr int SAMPLES_PER_BLOCK = 512;

	constexpr int BYTES_PER_SECOND = SAMPLE_RATE * CHANNELS * static_cast<int>(sizeof(float));

	// Audio is produced in bursts tied to the emulation frame loop (a frame's
	// worth of samples, then a sleep), so the SDL queue must hold enough to ride
	// over a lagging frame without starving the device. We seed this much silence
	// up front as a latency cushion, and rebuild it whenever a spike drains the
	// queue below the low-water mark.
	constexpr int PRIME_BYTES     = BYTES_PER_SECOND / 16;   // ~62 ms standing cushion
	constexpr int LOW_WATER_BYTES = BYTES_PER_SECOND / 40;   // ~25 ms: rebuild below this

	// Drop generated audio once SDL already has this many bytes queued, so a
	// faster-than-real-time emulation thread cannot pile up unbounded latency.
	constexpr int MAX_QUEUED_BYTES = BYTES_PER_SECOND / 5;   // ~200 ms ceiling

	// Duty patterns for the square channels, one bit per 1/8 of the period.
	constexpr uint8_t DUTY_TABLE[4][8] = {
		{0, 0, 0, 0, 0, 0, 0, 1},   // 12.5%
		{1, 0, 0, 0, 0, 0, 0, 1},   // 25%
		{1, 0, 0, 0, 0, 1, 1, 1},   // 50%
		{0, 1, 1, 1, 1, 1, 1, 0},   // 75%
	};

	// Noise frequency-timer divisors selected by NR43 bits 0-2.
	constexpr uint16_t NOISE_DIVISOR[8] = {8, 16, 32, 48, 64, 80, 96, 112};

	// Bits that always read back as 1 for each register 0xFF10-0xFF26.
	constexpr uint8_t READ_MASK[0x17] = {
		0x80, 0x3F, 0x00, 0xFF, 0xBF,   // FF10-FF14  NR10-NR14
		0xFF, 0x3F, 0x00, 0xFF, 0xBF,   // FF15-FF19  (FF15 unused) NR21-NR24
		0x7F, 0xFF, 0x9F, 0xFF, 0xBF,   // FF1A-FF1E  NR30-NR34
		0xFF, 0xFF, 0x00, 0x00, 0xBF,   // FF1F-FF23  (FF1F unused) NR41-NR44
		0x00, 0x00, 0x70,               // FF24-FF26  NR50-NR52
	};

	// digital 0-15 -> analog -1.0..+1.0 across the DAC.
	inline float dac(uint8_t sample)
	{
		return (static_cast<float>(sample) / 7.5f) - 1.0f;
	}

	// The frame sequencer clocks the length counters on its even steps; used to
	// decide the extra length clock when a channel's length is enabled mid-step.
	inline bool nextStepClocksLength(int frameSeqStep)
	{
		return (frameSeqStep % 2) == 0;
	}
}

//============================================================================
// Square channel
//============================================================================

void apu::SquareChannel::setFrequency(uint16_t f)
{
	nrx3 = static_cast<uint8_t>(f & 0xFF);
	nrx4 = static_cast<uint8_t>((nrx4 & 0xF8) | ((f >> 8) & 0x07));
}

uint16_t apu::SquareChannel::calcSweepFreq(bool& overflow) const
{
	const uint8_t shift = nrx0 & 0x07;
	uint16_t delta = sweepShadow >> shift;
	uint16_t next = (nrx0 & 0x08) ? (sweepShadow - delta) : (sweepShadow + delta);
	overflow = next > 2047;
	return next;
}

void apu::SquareChannel::trigger(int frameSeqStep)
{
	enabled = dacEnabled;

	if (lengthCounter == 0)
	{
		lengthCounter = 64;
		// Triggering with length enabled on a non-length step still clocks once.
		if (lengthEnabled && !nextStepClocksLength(frameSeqStep))
		{
			lengthCounter--;
		}
	}

	freqTimer = (2048 - frequency()) * 4;

	envTimer = envPeriod;
	volume = (nrx2 >> 4) & 0x0F;
	envAdd = nrx2 & 0x08;

	if (hasSweep)
	{
		sweepShadow = frequency();
		const uint8_t sweepPeriod = (nrx0 >> 4) & 0x07;
		const uint8_t shift = nrx0 & 0x07;
		sweepTimer = sweepPeriod ? sweepPeriod : 8;
		sweepEnabled = (sweepPeriod != 0) || (shift != 0);

		// A trigger with a non-zero shift runs an immediate overflow check.
		if (shift != 0)
		{
			bool overflow = false;
			calcSweepFreq(overflow);
			if (overflow)
			{
				enabled = false;
			}
		}
	}
}

void apu::SquareChannel::tickFrequency()
{
	if (--freqTimer <= 0)
	{
		freqTimer = (2048 - frequency()) * 4;
		dutyPos = (dutyPos + 1) & 0x07;
	}
}

void apu::SquareChannel::clockLength()
{
	if (lengthEnabled && lengthCounter > 0)
	{
		if (--lengthCounter == 0)
		{
			enabled = false;
		}
	}
}

void apu::SquareChannel::clockEnvelope()
{
	if (envPeriod == 0)
	{
		return;
	}
	if (envTimer > 0)
	{
		envTimer--;
	}
	if (envTimer == 0)
	{
		envTimer = envPeriod;
		if (envAdd && volume < 15)
		{
			volume++;
		}
		else if (!envAdd && volume > 0)
		{
			volume--;
		}
	}
}

void apu::SquareChannel::clockSweep(bool& channelOff)
{
	if (!hasSweep)
	{
		return;
	}
	if (sweepTimer > 0)
	{
		sweepTimer--;
	}
	if (sweepTimer != 0)
	{
		return;
	}

	const uint8_t sweepPeriod = (nrx0 >> 4) & 0x07;
	sweepTimer = sweepPeriod ? sweepPeriod : 8;

	if (!sweepEnabled || sweepPeriod == 0)
	{
		return;
	}

	bool overflow = false;
	const uint16_t next = calcSweepFreq(overflow);
	if (overflow)
	{
		enabled = false;
		channelOff = true;
		return;
	}

	if ((nrx0 & 0x07) != 0)
	{
		sweepShadow = next;
		setFrequency(next);

		// A second overflow check with the new shadow value.
		bool overflow2 = false;
		calcSweepFreq(overflow2);
		if (overflow2)
		{
			enabled = false;
			channelOff = true;
		}
	}
}

uint8_t apu::SquareChannel::output() const
{
	if (!enabled || !dacEnabled)
	{
		return 0;
	}
	const uint8_t duty = (nrx1 >> 6) & 0x03;
	return DUTY_TABLE[duty][dutyPos] ? volume : 0;
}

//============================================================================
// Wave channel
//============================================================================

void apu::WaveChannel::trigger(int frameSeqStep)
{
	enabled = dacEnabled;

	if (lengthCounter == 0)
	{
		lengthCounter = 256;
		if (lengthEnabled && !nextStepClocksLength(frameSeqStep))
		{
			lengthCounter--;
		}
	}

	freqTimer = (2048 - frequency()) * 2;
	samplePos = 0;
}

void apu::WaveChannel::tickFrequency()
{
	if (--freqTimer <= 0)
	{
		freqTimer = (2048 - frequency()) * 2;
		samplePos = (samplePos + 1) & 0x1F;
	}
}

void apu::WaveChannel::clockLength()
{
	if (lengthEnabled && lengthCounter > 0)
	{
		if (--lengthCounter == 0)
		{
			enabled = false;
		}
	}
}

uint8_t apu::WaveChannel::output() const
{
	if (!enabled || !dacEnabled)
	{
		return 0;
	}

	// Two 4-bit samples per byte, high nibble first.
	const uint8_t byte = ram[samplePos / 2];
	const uint8_t sample = (samplePos & 1) ? (byte & 0x0F) : (byte >> 4);

	switch ((nr32 >> 5) & 0x03)
	{
		case 0:  return 0;             // mute
		case 1:  return sample;        // 100%
		case 2:  return sample >> 1;   // 50%
		default: return sample >> 2;   // 25%
	}
}

//============================================================================
// Noise channel
//============================================================================

void apu::NoiseChannel::trigger(int frameSeqStep)
{
	enabled = dacEnabled;

	if (lengthCounter == 0)
	{
		lengthCounter = 64;
		if (lengthEnabled && !nextStepClocksLength(frameSeqStep))
		{
			lengthCounter--;
		}
	}

	const uint8_t shift = (nr43 >> 4) & 0x0F;
	freqTimer = NOISE_DIVISOR[nr43 & 0x07] << shift;

	envTimer = envPeriod;
	volume = (nr42 >> 4) & 0x0F;
	envAdd = nr42 & 0x08;

	lfsr = 0x7FFF;
}

void apu::NoiseChannel::tickFrequency()
{
	if (--freqTimer <= 0)
	{
		const uint8_t shift = (nr43 >> 4) & 0x0F;
		freqTimer = NOISE_DIVISOR[nr43 & 0x07] << shift;

		const uint16_t xorBit = (lfsr & 1) ^ ((lfsr >> 1) & 1);
		lfsr = (lfsr >> 1) | (xorBit << 14);
		if (nr43 & 0x08)   // 7-bit width: also feed bit 6
		{
			lfsr = (lfsr & ~(1 << 6)) | (xorBit << 6);
		}
	}
}

void apu::NoiseChannel::clockLength()
{
	if (lengthEnabled && lengthCounter > 0)
	{
		if (--lengthCounter == 0)
		{
			enabled = false;
		}
	}
}

void apu::NoiseChannel::clockEnvelope()
{
	if (envPeriod == 0)
	{
		return;
	}
	if (envTimer > 0)
	{
		envTimer--;
	}
	if (envTimer == 0)
	{
		envTimer = envPeriod;
		if (envAdd && volume < 15)
		{
			volume++;
		}
		else if (!envAdd && volume > 0)
		{
			volume--;
		}
	}
}

uint8_t apu::NoiseChannel::output() const
{
	if (!enabled || !dacEnabled)
	{
		return 0;
	}
	// Output the inverted low bit of the LFSR, gated by the envelope volume.
	return (~lfsr & 1) ? volume : 0;
}

//============================================================================
// APU
//============================================================================

apu::~apu()
{
	shutdown();
}

void apu::init()
{
	powerOff();
	m_sampleBuffer.reserve(SAMPLES_PER_BLOCK * CHANNELS);

	if (!SDL_WasInit(SDL_INIT_AUDIO))
	{
		if (!SDL_InitSubSystem(SDL_INIT_AUDIO))
		{
			LOG_WARN("apu: SDL audio subsystem init failed: {} -- running silent", SDL_GetError());
			return;
		}
	}

	SDL_AudioSpec spec{};
	spec.format = SDL_AUDIO_F32;
	spec.channels = CHANNELS;
	spec.freq = SAMPLE_RATE;

	m_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
	if (!m_stream)
	{
		LOG_WARN("apu: could not open audio device: {} -- running silent", SDL_GetError());
		return;
	}

	// Streams start paused; resume so queued data plays.
	SDL_ResumeAudioStreamDevice(m_stream);

	// Seed a latency cushion so the first lagging frame does not immediately
	// starve the device.
	pushSilence(PRIME_BYTES);

	LOG_INFO("apu: audio device opened ({} Hz, {} ch, f32)", SAMPLE_RATE, CHANNELS);
}

void apu::shutdown()
{
	// The emulator (and this APU) is destroyed at static teardown, which on the
	// normal exit path runs after SDL_Quit() in SDL_AppQuit. Only touch the
	// stream while the audio subsystem is still up; otherwise it is already gone.
	if (m_stream && SDL_WasInit(SDL_INIT_AUDIO))
	{
		SDL_DestroyAudioStream(m_stream);
	}
	m_stream = nullptr;
}

void apu::setMasterVolume(int percent)
{
	percent = std::clamp(percent, 0, 100);
	m_masterVolume.store(static_cast<float>(percent) / 100.0f, std::memory_order_relaxed);
}

void apu::powerOff()
{
	m_ch1 = SquareChannel{};
	m_ch1.hasSweep = true;
	m_ch2 = SquareChannel{};

	// Wave RAM survives a power cycle on the DMG; keep it across the reset.
	const auto savedWave = m_ch3.ram;
	m_ch3 = WaveChannel{};
	m_ch3.ram = savedWave;

	m_ch4 = NoiseChannel{};

	m_nr50 = 0;
	m_nr51 = 0;
	m_frameSeqStep = 0;
}

void apu::tick()
{
	// Frequency timers keep running while powered; when off, every channel is
	// silent anyway (powerOff cleared them), so this is a cheap no-op then.
	m_ch1.tickFrequency();
	m_ch2.tickFrequency();
	m_ch3.tickFrequency();
	m_ch4.tickFrequency();

	if (++m_frameSeqCounter >= FRAME_SEQ_PERIOD)
	{
		m_frameSeqCounter = 0;
		stepFrameSequencer();
	}

	// Bresenham resampler: emit one host frame each time the accumulator, bumped
	// by SAMPLE_RATE every T-cycle, crosses the master clock rate.
	m_sampleCounter += SAMPLE_RATE;
	if (m_sampleCounter >= CPU_FREQ)
	{
		m_sampleCounter -= CPU_FREQ;
		generateSample();
	}
}

void apu::stepFrameSequencer()
{
	bool sweepOff = false;
	switch (m_frameSeqStep)
	{
		case 0: m_ch1.clockLength(); m_ch2.clockLength(); m_ch3.clockLength(); m_ch4.clockLength(); break;
		case 2:
			m_ch1.clockLength(); m_ch2.clockLength(); m_ch3.clockLength(); m_ch4.clockLength();
			m_ch1.clockSweep(sweepOff);
			break;
		case 4: m_ch1.clockLength(); m_ch2.clockLength(); m_ch3.clockLength(); m_ch4.clockLength(); break;
		case 6:
			m_ch1.clockLength(); m_ch2.clockLength(); m_ch3.clockLength(); m_ch4.clockLength();
			m_ch1.clockSweep(sweepOff);
			break;
		case 7:
			m_ch1.clockEnvelope(); m_ch2.clockEnvelope(); m_ch4.clockEnvelope();
			break;
		default:
			break;
	}
	m_frameSeqStep = (m_frameSeqStep + 1) & 0x07;
}

void apu::generateSample()
{
	float left = 0.0f;
	float right = 0.0f;

	if (m_powered)
	{
		const uint8_t out[4] = {
			m_ch1.output(), m_ch2.output(), m_ch3.output(), m_ch4.output()
		};
		const bool dacOn[4] = {
			m_ch1.dacEnabled, m_ch2.dacEnabled, m_ch3.dacEnabled, m_ch4.dacEnabled
		};

		for (int c = 0; c < 4; ++c)
		{
			if (!dacOn[c])
			{
				continue;
			}
			const float a = dac(out[c]);
			if (m_nr51 & (1 << c))            // bits 0-3: right
			{
				right += a;
			}
			if (m_nr51 & (1 << (c + 4)))      // bits 4-7: left
			{
				left += a;
			}
		}

		// Average the four channels, then apply the NR50 master volume (1-8).
		const float leftVol  = static_cast<float>((m_nr50 >> 4) & 0x07) + 1.0f;
		const float rightVol = static_cast<float>(m_nr50 & 0x07) + 1.0f;
		left  = (left  / 4.0f) * (leftVol  / 8.0f);
		right = (right / 4.0f) * (rightVol / 8.0f);
	}

	// DC blocker (y[n] = x[n] - x[n-1] + R*y[n-1]); ~7 Hz cutoff at 48 kHz.
	constexpr float R = 0.999f;
	const float hpL = left  - m_hpPrevInL + R * m_hpPrevOutL;
	m_hpPrevInL = left;  m_hpPrevOutL = hpL;
	const float hpR = right - m_hpPrevInR + R * m_hpPrevOutR;
	m_hpPrevInR = right; m_hpPrevOutR = hpR;

	const float gain = m_masterVolume.load(std::memory_order_relaxed);
	m_sampleBuffer.push_back(hpL * gain);
	m_sampleBuffer.push_back(hpR * gain);

	if (static_cast<int>(m_sampleBuffer.size()) >= SAMPLES_PER_BLOCK * CHANNELS)
	{
		pushSamples();
	}
}

void apu::pushSamples()
{
	if (m_stream)
	{
		const int queued = static_cast<int>(SDL_GetAudioStreamQueued(m_stream));

		if (queued > MAX_QUEUED_BYTES)
		{
			// Overrun: emulation outran playback. Drop this block to bound latency.
			m_sampleBuffer.clear();
			return;
		}

		// A lag spike nearly drained the queue: pad with silence to rebuild the
		// cushion before the next spike starves the device outright. One click of
		// silence beats repeated underruns.
		if (queued < LOW_WATER_BYTES)
		{
			pushSilence(PRIME_BYTES - queued);
		}

		SDL_PutAudioStreamData(m_stream, m_sampleBuffer.data(),
			static_cast<int>(m_sampleBuffer.size() * sizeof(float)));
	}
	m_sampleBuffer.clear();
}

void apu::pushSilence(int bytes)
{
	if (!m_stream || bytes <= 0)
	{
		return;
	}
	// Zero-filled f32 frames; size rounded to a whole stereo frame.
	const int frames = bytes / (CHANNELS * static_cast<int>(sizeof(float)));
	const std::vector<float> silence(static_cast<size_t>(frames) * CHANNELS, 0.0f);
	SDL_PutAudioStreamData(m_stream, silence.data(),
		static_cast<int>(silence.size() * sizeof(float)));
}

//============================================================================
// Register access
//============================================================================

uint8_t apu::read(uint16_t address)
{
	if (address >= 0xFF30 && address <= 0xFF3F)
	{
		return m_ch3.ram[address - 0xFF30];
	}

	if (address < 0xFF10 || address > 0xFF3F)
	{
		return 0xFF;
	}

	// The unused holes between the register blocks read all-ones.
	if (address == 0xFF15 || address == 0xFF1F || (address >= 0xFF27 && address <= 0xFF2F))
	{
		return 0xFF;
	}

	const uint8_t mask = READ_MASK[address - 0xFF10];

	uint8_t raw = 0;
	switch (address)
	{
		case 0xFF10: raw = m_ch1.nrx0; break;
		case 0xFF11: raw = m_ch1.nrx1; break;
		case 0xFF12: raw = m_ch1.nrx2; break;
		case 0xFF13: raw = m_ch1.nrx3; break;
		case 0xFF14: raw = m_ch1.nrx4; break;

		case 0xFF16: raw = m_ch2.nrx1; break;
		case 0xFF17: raw = m_ch2.nrx2; break;
		case 0xFF18: raw = m_ch2.nrx3; break;
		case 0xFF19: raw = m_ch2.nrx4; break;

		case 0xFF1A: raw = m_ch3.nr30; break;
		case 0xFF1B: raw = m_ch3.nr31; break;
		case 0xFF1C: raw = m_ch3.nr32; break;
		case 0xFF1D: raw = m_ch3.nr33; break;
		case 0xFF1E: raw = m_ch3.nr34; break;

		case 0xFF20: raw = m_ch4.nr41; break;
		case 0xFF21: raw = m_ch4.nr42; break;
		case 0xFF22: raw = m_ch4.nr43; break;
		case 0xFF23: raw = m_ch4.nr44; break;

		case 0xFF24: raw = m_nr50; break;
		case 0xFF25: raw = m_nr51; break;
		case 0xFF26:
			// NR52: power bit + live per-channel status.
			raw = (m_powered ? 0x80 : 0x00)
				| (m_ch1.enabled ? 0x01 : 0x00)
				| (m_ch2.enabled ? 0x02 : 0x00)
				| (m_ch3.enabled ? 0x04 : 0x00)
				| (m_ch4.enabled ? 0x08 : 0x00);
			break;
		default: break;
	}

	return raw | mask;
}

void apu::write(uint16_t address, uint8_t value)
{
	// Wave RAM is writable regardless of power state.
	if (address >= 0xFF30 && address <= 0xFF3F)
	{
		m_ch3.ram[address - 0xFF30] = value;
		return;
	}

	// NR52 controls power and is always writable; everything else is frozen
	// while the APU is powered off.
	if (address == 0xFF26)
	{
		const bool turnOn = value & 0x80;
		if (turnOn && !m_powered)
		{
			m_powered = true;
			m_frameSeqStep = 0;
		}
		else if (!turnOn && m_powered)
		{
			powerOff();
			m_powered = false;
		}
		return;
	}

	if (!m_powered)
	{
		return;
	}

	switch (address)
	{
		//--- Channel 1 (square + sweep) -------------------------------------
		case 0xFF10:
			m_ch1.nrx0 = value;
			break;
		case 0xFF11:
			m_ch1.nrx1 = value;
			m_ch1.lengthCounter = 64 - (value & 0x3F);
			break;
		case 0xFF12:
			m_ch1.nrx2 = value;
			m_ch1.envPeriod = value & 0x07;
			m_ch1.dacEnabled = (value & 0xF8) != 0;
			if (!m_ch1.dacEnabled) m_ch1.enabled = false;
			break;
		case 0xFF13:
			m_ch1.nrx3 = value;
			break;
		case 0xFF14:
		{
			const bool wasEnabled = m_ch1.lengthEnabled;
			m_ch1.nrx4 = value;
			m_ch1.lengthEnabled = value & 0x40;
			if (!wasEnabled && m_ch1.lengthEnabled && nextStepClocksLength(m_frameSeqStep) == false
				&& m_ch1.lengthCounter > 0)
			{
				if (--m_ch1.lengthCounter == 0 && !(value & 0x80)) m_ch1.enabled = false;
			}
			if (value & 0x80) m_ch1.trigger(m_frameSeqStep);
			break;
		}

		//--- Channel 2 (square) ---------------------------------------------
		case 0xFF16:
			m_ch2.nrx1 = value;
			m_ch2.lengthCounter = 64 - (value & 0x3F);
			break;
		case 0xFF17:
			m_ch2.nrx2 = value;
			m_ch2.envPeriod = value & 0x07;
			m_ch2.dacEnabled = (value & 0xF8) != 0;
			if (!m_ch2.dacEnabled) m_ch2.enabled = false;
			break;
		case 0xFF18:
			m_ch2.nrx3 = value;
			break;
		case 0xFF19:
		{
			const bool wasEnabled = m_ch2.lengthEnabled;
			m_ch2.nrx4 = value;
			m_ch2.lengthEnabled = value & 0x40;
			if (!wasEnabled && m_ch2.lengthEnabled && nextStepClocksLength(m_frameSeqStep) == false
				&& m_ch2.lengthCounter > 0)
			{
				if (--m_ch2.lengthCounter == 0 && !(value & 0x80)) m_ch2.enabled = false;
			}
			if (value & 0x80) m_ch2.trigger(m_frameSeqStep);
			break;
		}

		//--- Channel 3 (wave) -----------------------------------------------
		case 0xFF1A:
			m_ch3.nr30 = value;
			m_ch3.dacEnabled = (value & 0x80) != 0;
			if (!m_ch3.dacEnabled) m_ch3.enabled = false;
			break;
		case 0xFF1B:
			m_ch3.nr31 = value;
			m_ch3.lengthCounter = 256 - value;
			break;
		case 0xFF1C:
			m_ch3.nr32 = value;
			break;
		case 0xFF1D:
			m_ch3.nr33 = value;
			break;
		case 0xFF1E:
		{
			const bool wasEnabled = m_ch3.lengthEnabled;
			m_ch3.nr34 = value;
			m_ch3.lengthEnabled = value & 0x40;
			if (!wasEnabled && m_ch3.lengthEnabled && nextStepClocksLength(m_frameSeqStep) == false
				&& m_ch3.lengthCounter > 0)
			{
				if (--m_ch3.lengthCounter == 0 && !(value & 0x80)) m_ch3.enabled = false;
			}
			if (value & 0x80) m_ch3.trigger(m_frameSeqStep);
			break;
		}

		//--- Channel 4 (noise) ----------------------------------------------
		case 0xFF20:
			m_ch4.nr41 = value;
			m_ch4.lengthCounter = 64 - (value & 0x3F);
			break;
		case 0xFF21:
			m_ch4.nr42 = value;
			m_ch4.envPeriod = value & 0x07;
			m_ch4.dacEnabled = (value & 0xF8) != 0;
			if (!m_ch4.dacEnabled) m_ch4.enabled = false;
			break;
		case 0xFF22:
			m_ch4.nr43 = value;
			break;
		case 0xFF23:
		{
			const bool wasEnabled = m_ch4.lengthEnabled;
			m_ch4.nr44 = value;
			m_ch4.lengthEnabled = value & 0x40;
			if (!wasEnabled && m_ch4.lengthEnabled && nextStepClocksLength(m_frameSeqStep) == false
				&& m_ch4.lengthCounter > 0)
			{
				if (--m_ch4.lengthCounter == 0 && !(value & 0x80)) m_ch4.enabled = false;
			}
			if (value & 0x80) m_ch4.trigger(m_frameSeqStep);
			break;
		}

		//--- Control --------------------------------------------------------
		case 0xFF24:
			m_nr50 = value;
			break;
		case 0xFF25:
			m_nr51 = value;
			break;
		default:
			break;
	}
}
