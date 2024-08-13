#include "cartridge.h"



//cartridgeLoader::ctx;

bool cartridgeLoader::loadCartridge(char* m_cartridge)
{
    GBE_INFO(std::string(m_cartridge));
    snprintf(ctx.filename, sizeof(ctx.filename), "%s", m_cartridge);
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

    return true;
}

void cartridgeLoader::writeToCartridge(uint16_t address, uint8_t value)
{
}

uint8_t cartridgeLoader::readCartridge(uint16_t address)
{
    return ctx.romData[address];
}

std::string cartridgeLoader::getLicenseeName(int code)
{
    static const std::unordered_map<int, std::string> licenseeNames = {
            {0, "None"},
            {1, "Nintendo R&D1"},
            {8, "Capcom"},
            {13, "Electronic Arts"},
            {18, "Hudson Soft"},
            {19, "b-ai"},
            {20, "kss"},
            {22, "pow"},
            {24, "PCM Complete"},
            {25, "san-x"},
            {28, "Kemco Japan"},
    };

    auto it = licenseeNames.find(code);
    if (it != licenseeNames.end()) {
        return it->second;
    }
    else {
        return "UNKNOWN";
    }
}
