#pragma once

struct DDSImage {
    struct DDS_PIXELFORMAT {
        uint32_t dwSize;
        uint32_t dwFlags;
        uint32_t dwFourCC;
        uint32_t dwRGBBitCount;
        uint32_t dwRBitMask;
        uint32_t dwGBitMask;
        uint32_t dwBBitMask;
        uint32_t dwABitMask;
    };
    
    struct DDSHeader{
        uint32_t           dwSize;
        uint32_t           dwFlags;
        uint32_t           dwHeight;
        uint32_t           dwWidth;
        uint32_t           dwPitchOrLinearSize;
        uint32_t           dwDepth;
        uint32_t           dwMipMapCount;
        uint32_t           dwReserved1[11];
        DDS_PIXELFORMAT    ddspf;
        uint32_t           dwCaps;
        uint32_t           dwCaps2;
        uint32_t           dwCaps3;
        uint32_t           dwCaps4;
        uint32_t           dwReserved2;
    };

    DDSHeader header;
    std::vector<uint8_t> data;

    template<typename T>
    static void read(std::ifstream &input, T &val) {
        input.read(reinterpret_cast<char *>(&val), sizeof(T));
    }

    DDSImage(const std::filesystem::path &filepath) {
        std::ifstream input(filepath, std::ios::in | std::ios::binary);

        uint32_t magicWord;
        read(input, magicWord);

        if (magicWord != 0x20534444) {
            throw std::runtime_error("Can't open DDS file: invalid header");
        }

        read(input, header);

        std::cout << header.dwWidth << " " << header.dwHeight << std::endl;
        std::cout << header.dwSize << std::endl;

        data.resize(header.dwPitchOrLinearSize);
        input.read(reinterpret_cast<char *>(data.data()), data.size());
    }
};