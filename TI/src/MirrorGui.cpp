#pragma once

#include "ImageUtils.hpp"
#include "Screen.hpp"

#include "MirrorGui.hpp"

#include "GUI.hpp"

#include <Windows.h>

#include <future>

namespace TiM
{
    void MirrorGui::Start(GUI& gui)
    {

        bool done = false;
        bool stop = false;

        int calcState = 0;

        m_width  = GetSystemMetrics(SM_CXSCREEN);
        m_height = GetSystemMetrics(SM_CYSCREEN);
    }

    void MirrorGui::End(GUI& gui)
    {
	    libusb_exit(NULL);

        if (m_devHandle != NULL)
        {
            libusb_release_interface(m_devHandle, 0);
            libusb_close(m_devHandle);
        }

        delete[] pixels;
        delete[] resizedPixelsRGB;
        delete[] pixelsrgb;
        delete[] pixelsRGB565;
    }

    void MirrorGui::FixedUpdate(GUI& gui)
    {
        static bool once = true;

        if (once)
        {
            libusb_init_context(NULL, NULL, 0);
            once = false;
        }

        if (!TryConnect()) return;

        pixels = GetPixelsFromScreen(m_width, m_height);

        resizedPixelsRGB = resizeImage((uint8_t*)pixels, m_width, m_height, 320, 240);

        pixelsrgb = combineChannels(resizedPixelsRGB, 320, 240);

        pixelsRGB565 = RGBToBGR565(pixelsrgb, 320, 240);

        for (int i = 0; i < 240; i++)
        {
            data_pixel[0] = i;

            memcpy(data_pixel + 1, pixelsRGB565 + i * 320, 320 * 2);

            libusb_bulk_transfer(m_devHandle, 2, data_pixel, 1 + 320 * 2, NULL, 0);
        }
    }

    bool MirrorGui::TryConnect()
    {
		while (m_devHandle == NULL)
		{
			m_devHandle = libusb_open_device_with_vid_pid(NULL, 0x0451, 0xE009);

			if (m_devHandle == NULL)
			{
                std::cout << "Device not found" << std::endl;
			}

			Sleep(100);
		}

        if (m_devHandle == NULL)
        {
            std::cout << "Device not found" << std::endl;

            return false;
        }

		libusb_set_auto_detach_kernel_driver(m_devHandle, 1);
		libusb_claim_interface(m_devHandle, 0);

        return true;
    }


    void MirrorGui::Render(GUI& gui)
    {
        ImGui::Begin("Mirror");

        ImGui::InputInt("Width" , (int*)&m_width );
        ImGui::InputInt("Height", (int*)&m_height);

        ImGui::End();
    }
}