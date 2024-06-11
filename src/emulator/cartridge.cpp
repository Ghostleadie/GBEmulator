#include "cartridge.h"



//cartridgeLoader::ctx;

bool cartridgeLoader::loadCartridge(char* m_cartridge)
{
    GBE_INFO(std::string(m_cartridge));

    std::ifstream cartridgeFile;
    cartridgeFile.open(m_cartridge, std::ifstream::binary);

    if (!cartridgeFile) {
        GBE_WARN("Failed to open: %s\n", m_cartridge);
        return false;
    }

    cartridgeFile.seekg(0, cartridgeFile.end);
    ctx.romSize = (unsigned long)cartridgeFile.tellg();
    cartridgeFile.seekg(0, cartridgeFile.beg);

    ctx.romData = new char[ctx.romSize];
    cartridgeFile.read(ctx.romData, ctx.romSize);
    cartridgeFile.close();

    ctx.header = (cartirdge*)(ctx.romData + 0x100);
    ctx.header->title[15] = 0;

    GBE_INFO("Cartridge Loaded:\n");
    GBE_INFO("\t Title    : " + std::string(ctx.header->title));
    GBE_INFO("\t Type     : " + std::to_string(ctx.header->type) + " " + ROM_TYPES[ctx.header->type]);
    GBE_INFO("\t ROM Size : " + std::to_string(32 << ctx.header->romSize) + "KB");
    GBE_INFO("\t RAM Size : " + std::to_string(ctx.header->ramSize));
    GBE_INFO("\t LIC Code : " + std::to_string(ctx.header->licCode) + "(" + getLicenseeName(ctx.header->licCode) + ")");
    GBE_INFO("\t ROM Vers : " + std::to_string(ctx.header->version));

    uint16_t x = 0;
    for (uint16_t i = 0x0134; i <= 0x014C; i++) {
        x = x - ctx.romData[i] - 1;
    }

    GBE_INFO("\t Checksum : " + std::to_string(ctx.header->checksum) + "(" + ((x & 0xFF) ? "PASSED" : "FAILED") + ")");

    return false;
}

std::string cartridgeLoader::getLicenseeName(int code)
{
    switch (code) {
    case 0:
        return std::string("None");
        break;
    case 1:
        return std::string("Nintendo R&D1");
        break;
    case 8:
        return std::string("Capcom");
        break;
    case 13:
        return std::string("Electronic Arts");
        break;
    case 18:
        return std::string("Hudson Soft");
        break;
    case 19:
        return std::string("b-ai");
        break;
    case 20:
        return std::string("kss");
        break;
    case 22:
        return std::string("pow");
        break;
    case 24:
        return std::string("PCM Complete");
        break;
    case 25:
        return std::string("san-x");
        break;
    case 28:
        return std::string("Kemco Japan");
        break;

    default:
        return std::string("UNKNOWN");
    }
}
