//
// Created by Jack_ on 24/07/2026.
//

#ifndef GAMEBOYEMULATOR_APU_H
#define GAMEBOYEMULATOR_APU_H
#include "../interfaces/IComponentMessanger.h"
#include "../interfaces/IClocked.h"
#include <array>
#include <atomic>
#include <cstdint>
#include <vector>

// SDL is confined to apu.cpp so the header can be pulled into the SDL-free
// headless build (bus.cpp, which routes the APU registers, is linked into the
// romrunner/tests targets that do not have SDL). Only a forward declaration
// lives here.
struct SDL_AudioStream;

// Game Boy (DMG) audio processing unit.
//
// Owns the four sound channels (two squares, one wave, one noise), the 512 Hz
// frame sequencer that clocks their length/envelope/sweep units, and the
// resampler that turns the 4 MHz digital channel output into host PCM. It is a
// bus device (registers 0xFF10-0xFF3F) and a clocked device (ticked once per
// T-cycle, in step with the timer and PPU).
//
// tick() runs on the emulation thread; it generates samples and hands finished
// blocks to SDL, which plays them on its own audio thread. setMasterVolume()
// runs on the main/UI thread, so the volume it feeds in is an atomic. The frame
// sequencer here is driven by an internal 8192-T-cycle counter rather than by
// the timer's DIV bit; that is accurate enough for games but does not reproduce
// the DIV-write APU quirk.
class apu : public IComponentMessanger, public IClocked
{
public:
	apu() = default;
	~apu() override;

	/**
	 * Opens the SDL audio device/stream and resets every channel to power-on
	 * state. Safe to run with no audio device: it logs and carries on silently.
	 * SDL_INIT_AUDIO must already have been initialised by the frontend.
	 */
	void init();

	/** Stops and destroys the SDL audio stream. Also called by the destructor. */
	void shutdown();

	/**
	 * Reads an APU register (0xFF10-0xFF3F), OR-ing in the bits that always read
	 * back as 1 for that address; wave RAM (0xFF30-0xFF3F) reads straight through.
	 * @param address Global I/O address in the APU range.
	 * @return Register value with the hardware read-mask applied, or 0xFF for the unused holes.
	 */
	uint8_t read(uint16_t address) override;

	/**
	 * Writes an APU register (0xFF10-0xFF3F). Writes to the channel/control
	 * registers are ignored while the APU is powered off (NR52 bit 7 = 0); NR52
	 * and wave RAM are always writable.
	 * @param address Global I/O address in the APU range.
	 * @param value Byte to store; a trigger (bit 7 of NRx4) starts the channel.
	 */
	void write(uint16_t address, uint8_t value) override;

	/**
	 * Advances the APU one T-cycle: clocks every channel's frequency timer, steps
	 * the frame sequencer every 8192 T-cycles, and emits one stereo PCM frame each
	 * time the sample accumulator rolls over the host sample rate.
	 */
	void tick() override;

	/**
	 * Sets the user master volume from the settings slider.
	 * @param percent 0-100; stored as a 0.0-1.0 gain applied to the final mix. Thread-safe.
	 */
	void setMasterVolume(int percent);

private:
	// One square-wave channel. Channel 1 additionally runs the frequency sweep;
	// channel 2 leaves the sweep fields inert (hasSweep = false).
	struct SquareChannel
	{
		bool hasSweep = false;

		// Raw last-written register bytes, kept for read-back.
		uint8_t nrx0 = 0, nrx1 = 0, nrx2 = 0, nrx3 = 0, nrx4 = 0;

		bool enabled = false;      // channel currently producing sound
		bool dacEnabled = false;   // DAC on (NRx2 upper 5 bits non-zero)

		int freqTimer = 0;         // T-cycles until the next duty step
		uint8_t dutyPos = 0;       // 0-7 position in the duty pattern

		bool lengthEnabled = false;
		uint16_t lengthCounter = 0;

		uint8_t volume = 0;        // current envelope volume 0-15
		uint8_t envPeriod = 0;
		uint8_t envTimer = 0;
		bool envAdd = false;

		// Sweep (channel 1 only).
		uint16_t sweepShadow = 0;
		uint8_t sweepTimer = 0;
		bool sweepEnabled = false;
		bool sweepDidCalc = false;

		uint16_t frequency() const { return static_cast<uint16_t>(((nrx4 & 0x07) << 8) | nrx3); }
		void setFrequency(uint16_t f);

		void trigger(int frameSeqStep);
		void tickFrequency();
		void clockLength();
		void clockEnvelope();
		void clockSweep(bool& channelOff);
		uint16_t calcSweepFreq(bool& overflow) const;
		uint8_t output() const;    // digital 0-15
	};

	// The wave channel plays 32 4-bit samples from wave RAM.
	struct WaveChannel
	{
		uint8_t nr30 = 0, nr31 = 0, nr32 = 0, nr33 = 0, nr34 = 0;
		std::array<uint8_t, 16> ram = {};   // 0xFF30-0xFF3F, two 4-bit samples per byte

		bool enabled = false;
		bool dacEnabled = false;

		int freqTimer = 0;
		uint8_t samplePos = 0;     // 0-31

		bool lengthEnabled = false;
		uint16_t lengthCounter = 0;

		uint16_t frequency() const { return static_cast<uint16_t>(((nr34 & 0x07) << 8) | nr33); }

		void trigger(int frameSeqStep);
		void tickFrequency();
		void clockLength();
		uint8_t output() const;    // digital 0-15
	};

	// The noise channel clocks a linear-feedback shift register.
	struct NoiseChannel
	{
		uint8_t nr41 = 0, nr42 = 0, nr43 = 0, nr44 = 0;

		bool enabled = false;
		bool dacEnabled = false;

		int freqTimer = 0;
		uint16_t lfsr = 0x7FFF;

		bool lengthEnabled = false;
		uint16_t lengthCounter = 0;

		uint8_t volume = 0;
		uint8_t envPeriod = 0;
		uint8_t envTimer = 0;
		bool envAdd = false;

		void trigger(int frameSeqStep);
		void tickFrequency();
		void clockLength();
		void clockEnvelope();
		uint8_t output() const;    // digital 0-15
	};

	/** Runs one of the frame sequencer's eight steps (length/sweep/envelope). */
	void stepFrameSequencer();

	/** Mixes the four channels into one stereo frame and appends it to the buffer. */
	void generateSample();

	/** Hands the accumulated sample block to SDL, dropping it if the queue is over its latency cap. */
	void pushSamples();

	/** Queues `bytes` of silence to SDL to establish/rebuild the latency cushion. No-op without a stream. */
	void pushSilence(int bytes);

	/** Clears every channel and control register to power-on state (APU power off / init). */
	void powerOff();

	SquareChannel m_ch1;   // NR10-NR14, with sweep
	SquareChannel m_ch2;   // NR21-NR24
	WaveChannel   m_ch3;   // NR30-NR34 + wave RAM
	NoiseChannel  m_ch4;   // NR41-NR44

	bool m_powered = false;
	uint8_t m_nr50 = 0;    // master volume + VIN panning
	uint8_t m_nr51 = 0;    // channel L/R panning

	int m_frameSeqCounter = 0;   // T-cycles toward the next 512 Hz step
	int m_frameSeqStep = 0;      // 0-7 position in the frame sequencer

	int m_sampleCounter = 0;     // Bresenham accumulator: +SAMPLE_RATE per tick, emits on CPU_FREQ

	// One-pole DC-blocking high-pass state per side. The DMG DAC sits a channel
	// at -1.0 when its DAC is on but output is 0, so the raw mix carries a large
	// DC offset; this removes it (as the real hardware's coupling capacitor does)
	// to stop clipping and enable/disable clicks.
	float m_hpPrevInL = 0.0f, m_hpPrevOutL = 0.0f;
	float m_hpPrevInR = 0.0f, m_hpPrevOutR = 0.0f;

	std::vector<float> m_sampleBuffer;         // interleaved L,R awaiting the next SDL push
	std::atomic<float> m_masterVolume{0.75f};  // user slider, 0.0-1.0; UI thread writes, emu thread reads

	SDL_AudioStream* m_stream = nullptr;
};

#endif //GAMEBOYEMULATOR_APU_H
