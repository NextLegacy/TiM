#pragma once

#include <cstdint>
#include <unordered_map>

namespace TiM
{
    extern uint64_t palette[256];

    uint8_t*  changeResolution  (uint8_t* pixels, int width, int height, int newWidth, int newHeight);
    uint8_t*  resizeImage       (uint8_t* pixels, int width, int height, int newWidth, int newHeight);
    uint32_t* combineChannels   (uint8_t* pixels, int width, int height);

    uint16_t  RGBToPalletteIndex(uint8_t r, uint8_t g, uint8_t b);
    //uint16_t  RGBToBGR565_      (uint8_t r, uint8_t g, uint8_t b);
    //
    uint16_t* RGBToBGR565       (uint32_t* pixels, int width, int height);
}
