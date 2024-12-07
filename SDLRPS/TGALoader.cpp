#include "TGALoader.h"

SDL_Surface* TGALoader::LoadTGASurface(const char* filename) {
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        SDL_Log("Failed to open TGA file: %s", filename);
        return nullptr;
    }

    TGAHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(TGAHeader));

    SDL_Log("TGA Info - Type: %d, BPP: %d, Width: %d, Height: %d",
        header.imageType, header.bitsPerPixel, header.width, header.height);

    if (header.imageType != 2 && header.imageType != 10) {
        SDL_Log("Unsupported TGA type: %d", header.imageType);
        return nullptr;
    }

    if (header.bitsPerPixel != 24 && header.bitsPerPixel != 32) {
        SDL_Log("Unsupported TGA bit depth: %d", header.bitsPerPixel);
        return nullptr;
    }

    if (header.idLength > 0) {
        file.seekg(header.idLength, std::ios::cur);
    }

    if (header.colorMapType == 1) {
        file.seekg(header.colorMapLength * (header.colorMapDepth / 8), std::ios::cur);
    }

    const int bytesPerPixel = header.bitsPerPixel / 8;
    const int imageSize = header.width * header.height * bytesPerPixel;
    std::vector<uint8_t> pixels(imageSize);

    if (header.imageType == 2) { // Uncompressed
        file.read(reinterpret_cast<char*>(pixels.data()), imageSize);
    }
    else if (header.imageType == 10) { // RLE Compressed
        int pixelCount = 0;
        while (pixelCount < imageSize) {
            uint8_t packHeader;
            file.read(reinterpret_cast<char*>(&packHeader), 1);

            const bool isRLE = (packHeader & 0x80) != 0;
            const int count = (packHeader & 0x7F) + 1;

            if (isRLE) {
                uint8_t pixel[4];
                file.read(reinterpret_cast<char*>(pixel), bytesPerPixel);
                for (int i = 0; i < count && pixelCount < imageSize; ++i) {
                    memcpy(&pixels[pixelCount], pixel, bytesPerPixel);
                    pixelCount += bytesPerPixel;
                }
            }
            else {
                const int byteCount = count * bytesPerPixel;
                file.read(reinterpret_cast<char*>(&pixels[pixelCount]), byteCount);
                pixelCount += byteCount;
            }
        }
    }

    // Convert BGR(A) to RGB(A)
    for (int i = 0; i < imageSize; i += bytesPerPixel) {
        std::swap(pixels[i], pixels[i + 2]);
    }

    uint32_t rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xFF000000;
    gmask = 0x00FF0000;
    bmask = 0x0000FF00;
    amask = 0x000000FF;
#else
    rmask = 0x000000FF;
    gmask = 0x0000FF00;
    bmask = 0x00FF0000;
    amask = 0xFF000000;
#endif

    SDL_Surface* surface = SDL_CreateRGBSurface(0, header.width, header.height,
        header.bitsPerPixel, rmask, gmask, bmask, amask);

    if (!surface) {
        SDL_Log("Failed to create surface: %s", SDL_GetError());
        return nullptr;
    }

    SDL_LockSurface(surface);
    memcpy(surface->pixels, pixels.data(), imageSize);
    SDL_UnlockSurface(surface);

    // Flip vertically if needed
    if ((header.imageDescriptor & 0x20) == 0) {
        uint8_t* tempRow = new uint8_t[surface->pitch];
        uint8_t* pixels = (uint8_t*)surface->pixels;

        for (int i = 0; i < surface->h / 2; ++i) {
            memcpy(tempRow, &pixels[i * surface->pitch], surface->pitch);
            memcpy(&pixels[i * surface->pitch],
                &pixels[(surface->h - 1 - i) * surface->pitch],
                surface->pitch);
            memcpy(&pixels[(surface->h - 1 - i) * surface->pitch],
                tempRow, surface->pitch);
        }

        delete[] tempRow;
    }

    return surface;
}