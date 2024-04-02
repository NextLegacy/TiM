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

        pixels = GetPixelsFromScreen(m_width, m_height);

        resizedPixelsRGB = resizeImage((uint8_t*)pixels, m_width, m_height, 320, 240);

        pixelsrgb = combineChannels(resizedPixelsRGB, 320, 240);

        pixelsRGB565 = RGBToBGR565(pixelsrgb, 320, 240);
        
        if (!TryConnect(gui)) return;

        for (int i = 0; i < 240; i++)
        {
            data_pixel[0] = i;

            memcpy(data_pixel + 1, pixelsRGB565 + i * 320, 320 * 2);

            libusb_bulk_transfer(m_devHandle, 2, data_pixel, 1 + 320 * 2, NULL, 0);
        }

    }

    bool MirrorGui::TryConnect(GUI& gui)
    {
        if (m_devHandle != NULL) return true;

        static double timer = 0;

        timer += gui.GetFixedDeltaTime();

        if (timer < 1.0f) return false;
        timer = 0;

        m_devHandle = libusb_open_device_with_vid_pid(NULL, 0x0451, 0xE009);

        if (m_devHandle == NULL)
        {
            std::cout << "Device not found" << std::endl;

            return false;
        }

		libusb_set_auto_detach_kernel_driver(m_devHandle, 1);
		libusb_claim_interface(m_devHandle, 0);

        return true;
    }
    
    void MirrorGui::LoadFBO(unsigned int* fbo, unsigned int* texture)
    {
        //glGenFramebuffers(1, fbo);
        //glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

        glGenTextures(1, texture);
        glBindTexture(GL_TEXTURE_2D, *texture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 320, 240, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, pixelsRGB565);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *texture, 0);

        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //glBindTexture(GL_TEXTURE_2D, 0);
    }

    void MirrorGui::UpdateFBO(unsigned int fbo, unsigned int texture)
    {
        //glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 320, 240, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, pixelsRGB565);

        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void MirrorGui::Render(GUI& gui)
    {
        static unsigned int fbo;
        static unsigned int texture;

        if (fbo == 0)
        {
            LoadFBO(&fbo, &texture);
        }

        UpdateFBO(fbo, texture);

        ImGui::Begin("Mirror");

        ImGui::InputInt("Width" , (int*)&m_width );
        ImGui::InputInt("Height", (int*)&m_height);

        ImGui::End();

        ImGui::Begin("Screen");
        // render framebuffer
        ImGui::Image((void*)(intptr_t)texture, ImVec2(320 * 3, 240 * 3));
        ImGui::End();
    }
}