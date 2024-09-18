#include "Screen.hpp"

namespace TiM
{
    /*
    HBITMAP GetScreenBmp(HDC hdc, uint32_t width, uint32_t height)
    {
        // Get screen dimensions
        int nScreenWidth = width; //GetSystemMetrics(SM_CXSCREEN);
        int nScreenHeight = height; //GetSystemMetrics(SM_CYSCREEN);

        // Create compatible DC, create a compatible bitmap and copy the screen using BitBlt()
        HDC hCaptureDC  = CreateCompatibleDC(hdc);
        HBITMAP hBitmap = CreateCompatibleBitmap(hdc, nScreenWidth, nScreenHeight);
        HGDIOBJ hOld = SelectObject(hCaptureDC, hBitmap); 
        BOOL bOK = BitBlt(hCaptureDC,0,0,nScreenWidth, nScreenHeight, hdc,0,0,SRCCOPY|CAPTUREBLT); 

        SelectObject(hCaptureDC, hOld); // always select the previously selected object once done
        DeleteDC(hCaptureDC);
        return hBitmap;
    }

    uint32_t* GetPixelsFromScreen(uint32_t width, uint32_t height)
    {
        int x1, y1, x2, y2, w, h;

        // get screen dimensions
        x1  = GetSystemMetrics(SM_XVIRTUALSCREEN);
        y1  = GetSystemMetrics(SM_YVIRTUALSCREEN);
        x2  = width; //GetSystemMetrics(SM_CXSCREEN);
        y2  = height; //GetSystemMetrics(SM_CYSCREEN);

        w   = x2 - x1;
        h   = y2 - y1;

        // copy screen to bitmap
        HDC     hScreen = GetDC(NULL);
        HDC     hDC     = CreateCompatibleDC(hScreen);
        HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
        HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
        BOOL    bRet    = BitBlt(hDC, 0, 0, w, h, hScreen, x1, y1, SRCCOPY);

        uint32_t* pixels = (uint32_t*)malloc(w * h * 3);

        if (!bRet)
        {
            //std::cout << "error" << std::endl;
        }
        else
        {
            BITMAPINFOHEADER bmi = {0};
            bmi.biSize = sizeof(bmi);
            bmi.biPlanes = 1;
            bmi.biBitCount = 24;
            bmi.biWidth = w;
            bmi.biHeight = -h;
            bmi.biCompression = BI_RGB;
            bmi.biSizeImage = 0;
            bmi.biXPelsPerMeter = 0;
            bmi.biYPelsPerMeter = 0;
            bmi.biClrUsed = 0;
            bmi.biClrImportant = 0;

            GetDIBits(hDC, hBitmap, 0, h, pixels, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);
        }

        // clean up
        SelectObject(hDC, old_obj);
        DeleteDC(hDC);
        ReleaseDC(NULL, hScreen);
        DeleteObject(hBitmap);

        return pixels;
    }
    */

    void GetPixelsFromScreen(uint32_t* pixels, uint32_t x1, uint32_t y1, uint32_t width, uint32_t height)
    {

        HDC hScreen = GetDC(NULL);
        HDC hDC = CreateCompatibleDC(hScreen);
        HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, width, height);
        HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
        BOOL bRet = BitBlt(hDC, 0, 0, width, height, hScreen, x1, y1, SRCCOPY);

        if (!bRet)
        {
            //std::cout << "error" << std::endl;
        }
        else
        {
            BITMAPINFOHEADER bmi = {0};
            bmi.biSize = sizeof(bmi);
            bmi.biPlanes = 1;
            bmi.biBitCount = 24;
            bmi.biWidth = width;
            bmi.biHeight = -height;
            bmi.biCompression = BI_RGB;
            bmi.biSizeImage = 0;
            bmi.biXPelsPerMeter = 0;
            bmi.biYPelsPerMeter = 0;
            bmi.biClrUsed = 0;
            bmi.biClrImportant = 0;

            GetDIBits(hDC, hBitmap, 0, height, pixels, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);
        }

        // clean up
        SelectObject(hDC, old_obj);
        DeleteDC(hDC);
        ReleaseDC(NULL, hScreen);
        DeleteObject(hBitmap);
    }
}