#pragma once

#include <Windows.h>
#include <cstdint>

namespace TiM
{
    //HBITMAP GetScreenBmp(HDC hdc, uint32_t width, uint32_t height);
    //uint32_t* GetPixelsFromScreen(uint32_t width, uint32_t height);

    void GetPixelsFromScreen(uint32_t* pixels, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
}
