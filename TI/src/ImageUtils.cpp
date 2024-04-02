#include "ImageUtils.hpp"

namespace TiM
{
    uint64_t palette[256] = { 
        0x000000, 0x002008, 0x004010, 0x006018, 0x008020, 0x00a028, 0x00c030, 0x00e038, 0x080040, 0x082048, 0x084050, 0x086058, 0x088060, 0x08a068, 0x08c070, 0x08e078,
        0x100080, 0x102088, 0x104090, 0x106098, 0x1080a0, 0x10a0a8, 0x10c0b0, 0x10e0b8, 0x1800c0, 0x1820c8, 0x1840d0, 0x1860d8, 0x1880e0, 0x18a0e8, 0x18c0f0, 0x18e0f8,
        0x200400, 0x202408, 0x204410, 0x206418, 0x208420, 0x20a428, 0x20c430, 0x20e438, 0x280440, 0x282448, 0x284450, 0x286458, 0x288460, 0x28a468, 0x28c470, 0x28e478,
        0x300480, 0x302488, 0x304490, 0x306498, 0x3084a0, 0x30a4a8, 0x30c4b0, 0x30e4b8, 0x3804c0, 0x3824c8, 0x3844d0, 0x3864d8, 0x3884e0, 0x38a4e8, 0x38c4f0, 0x38e4f8,
        0x400800, 0x402808, 0x404810, 0x406818, 0x408820, 0x40a828, 0x40c830, 0x40e838, 0x480840, 0x482848, 0x484850, 0x486858, 0x488860, 0x48a868, 0x48c870, 0x48e878,
        0x500880, 0x502888, 0x504890, 0x506898, 0x5088a0, 0x50a8a8, 0x50c8b0, 0x50e8b8, 0x5808c0, 0x5828c8, 0x5848d0, 0x5868d8, 0x5888e0, 0x58a8e8, 0x58c8f0, 0x58e8f8,
        0x600c00, 0x602c08, 0x604c10, 0x606c18, 0x608c20, 0x60ac28, 0x60cc30, 0x60ec38, 0x680c40, 0x682c48, 0x684c50, 0x686c58, 0x688c60, 0x68ac68, 0x68cc70, 0x68ec78,
        0x700c80, 0x702c88, 0x704c90, 0x706c98, 0x708ca0, 0x70aca8, 0x70cca8, 0x70eca8, 0x78ccc0, 0x78ccc8, 0x78ccd0, 0x78ccd8, 0x78cce0, 0x78cce8, 0x78ccf0, 0x78ccf8,
        0x801000, 0x803008, 0x805010, 0x807018, 0x809020, 0x80a028, 0x80c030, 0x80e038, 0x881040, 0x883048, 0x885050, 0x887058, 0x889060, 0x88a068, 0x88c070, 0x88e078,
        0x901080, 0x903088, 0x905090, 0x907098, 0x9090a0, 0x90a0a8, 0x90c0b0, 0x90e0b8, 0x9810c0, 0x9830c8, 0x9850d0, 0x9870d8, 0x9890e0, 0x98a0e8, 0x98c0f0, 0x98e0f8,
        0xa01400, 0xa03408, 0xa05410, 0xa07418, 0xa09420, 0xa0b428, 0xa0d430, 0xa0f438, 0xa81440, 0xa83448, 0xa85450, 0xa87458, 0xa89460, 0xa8b468, 0xa8d470, 0xa8f478,
        0xb01480, 0xb03488, 0xb05490, 0xb07498, 0xb094a0, 0xb0b4a8, 0xb0d4b0, 0xb0f4b8, 0xb814c0, 0xb834c8, 0xb854d0, 0xb874d8, 0xb894e0, 0xb8b4e8, 0xb8d4f0, 0xb8f4f8,
        0xc01800, 0xc03808, 0xc05810, 0xc07818, 0xc09820, 0xc0b828, 0xc0d830, 0xc0f838, 0xc81840, 0xc83848, 0xc85850, 0xc87858, 0xc89860, 0xc8b868, 0xc8d870, 0xc8f878,
        0xd01880, 0xd03888, 0xd05890, 0xd07898, 0xd098a0, 0xd0b8a8, 0xd0d8b0, 0xd0f8b8, 0xd818c0, 0xd838c8, 0xd858d0, 0xd878d8, 0xd898e0, 0xd8b8e8, 0xd8d8f0, 0xd8f8f8,
        0xe01c00, 0xe03c08, 0xe05c10, 0xe07c18, 0xe09c20, 0xe0bc28, 0xe0dc30, 0xe0fc38, 0xe81c40, 0xe83c48, 0xe85c50, 0xe87c58, 0xe89c60, 0xe8bc68, 0xe8dc70, 0xe8fc78,
        0xf01c80, 0xf03c88, 0xf05c90, 0xf07c98, 0xf09ca0, 0xf0bca8, 0xf0dcb0, 0xf0fcb8, 0xf81cc0, 0xf83cc8, 0xf85cd0, 0xf87cd8, 0xf89ce0, 0xf8bce8, 0xf8dcf0, 0xf8fcf8
    };

    uint8_t* changeResolution(uint8_t* pixels, int width, int height, int newWidth, int newHeight)
    {
        uint8_t* newPixels = (uint8_t*)malloc(newWidth * newHeight * 3);

        //interpolate

        for (int x = 0; x < newWidth; x++)
        {
            for (int y = 0; y < newHeight; y++)
            {
                int px = (int)((float)x / (float)newWidth * (float)width);
                int py = (int)((float)y / (float)newHeight * (float)height);

                newPixels[(y * newWidth + x) * 3 + 0] = pixels[(py * width + px) * 3 + 0];
                newPixels[(y * newWidth + x) * 3 + 1] = pixels[(py * width + px) * 3 + 1];
                newPixels[(y * newWidth + x) * 3 + 2] = pixels[(py * width + px) * 3 + 2];
            }
        }

        return newPixels;
    }

    uint8_t* resizeImage(uint8_t* pixels, int width, int height, int newWidth, int newHeight)
    {
        uint8_t* newPixels = (uint8_t*)malloc(newWidth * newHeight * 3);
        
        // bilinear resampling
        float xRatio = (float)width / (float)newWidth;
        float yRatio = (float)height / (float)newHeight;

        for (int x = 0; x < newWidth; x++)
        {
            for (int y = 0; y < newHeight; y++)
            {
                float gx = (float)x / (float)newWidth * (float)width;
                float gy = (float)y / (float)newHeight * (float)height;

                int gxi = (int)gx;
                int gyi = (int)gy;

                float dx = gx - gxi;
                float dy = gy - gyi;

                float r = 0;
                float g = 0;
                float b = 0;

                for (int i = 0; i < 2; i++)
                {
                    for (int j = 0; j < 2; j++)
                    {
                        int px = gxi + i;
                        int py = gyi + j;

                        if (px < 0 || py < 0 || px >= width || py >= height)
                        {
                            continue;
                        }

                        float weight = (1 - abs(px - gx)) * (1 - abs(py - gy));

                        r += pixels[(py * width + px) * 3 + 0] * weight;
                        g += pixels[(py * width + px) * 3 + 1] * weight;
                        b += pixels[(py * width + px) * 3 + 2] * weight;
                    }
                }

                newPixels[(y * newWidth + x) * 3 + 0] = (uint8_t)r;
                newPixels[(y * newWidth + x) * 3 + 1] = (uint8_t)g;
                newPixels[(y * newWidth + x) * 3 + 2] = (uint8_t)b;
            }
        }

        return newPixels;
    }

    uint32_t* combineChannels(uint8_t* pixels, int width, int height)
    {
        uint32_t* pixelsCombined = new uint32_t[width * height];

        for (int i = 0; i < width * height; i++)
        {
            pixelsCombined[i] = (pixels[i * 3 + 0] << 16) | (pixels[i * 3 + 1] << 8) | (pixels[i * 3 + 2]);
        }

        return pixelsCombined;
    }

    uint16_t RGBToPalletteIndex(uint8_t r, uint8_t g, uint8_t b)
    {
        uint16_t closest = 0;
        uint64_t closestDist = 0xFFFFFFFFFFFFFFFF;
        for (size_t i = 0; i < 256; i++)
        {
            uint64_t dist = (r - ((palette[i] >> 16) & 0xFF)) * (r - ((palette[i] >> 16) & 0xFF)) + 
                            (g - ((palette[i] >> 8 ) & 0xFF)) * (g - ((palette[i] >> 8 ) & 0xFF)) + 
                            (b - ((palette[i] >> 0 ) & 0xFF)) * (b - ((palette[i] >> 0 ) & 0xFF));
    
            if (dist < closestDist)
            {
                closestDist = dist;
                closest = i;
            }
        }

        return closest;
    }

    //uint16_t  RGBToBGR565_(uint8_t r, uint8_t g, uint8_t b, uint32_t width, uint32_t height)
    //{
    //    uint16_t* pixelsBGR565 = new uint16_t[width * height];
//
    //    for (int i = 0; i < width * height; i++)
    //    {
    //        int r = (pixels[i] >> 0) & 0xFF;
    //        int g = (pixels[i] >> 8) & 0xFF;
    //        int b = (pixels[i] >> 16) & 0xFF;
//
    //        //pixelsBGR565[i] = RGBToPalletteIndex(r, g, b) << 8 | RGBToPalletteIndex(r, g, b);
    //        pixelsBGR565[i] = r >> 3 << 11 | g >> 2 << 5 | b >> 3;
    //    }
//
    //    return pixelsBGR565;
    //}

    
    uint16_t* RGBToBGR565(uint32_t* pixels, int width, int height)
    {
        uint16_t* pixelsBGR565 = new uint16_t[width * height];

        for (int i = 0; i < width * height; i++)
        {
            int r = (pixels[i] >> 0) & 0xFF;
            int g = (pixels[i] >> 8) & 0xFF;
            int b = (pixels[i] >> 16) & 0xFF;

            //pixelsBGR565[i] = RGBToPalletteIndex(r, g, b) << 8 | RGBToPalletteIndex(r, g, b);
            pixelsBGR565[i] = r >> 3 << 11 | g >> 2 << 5 | b >> 3;
        }

        return pixelsBGR565;
    }
}
