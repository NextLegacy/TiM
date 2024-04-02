#pragma once

#include "EventSubscriber.hpp"

#include "libusb.h"

namespace TiM
{
    class MirrorGui : public EventSubscriber
    {
    public:
        inline MirrorGui() { }

        void Start(GUI& gui) override;
        void End(GUI& gui) override;
        void Render(GUI& gui) override;
        void FixedUpdate(GUI& gui) override;

        bool TryConnect(GUI& gui);

        void LoadFBO(unsigned int* fbo, unsigned int* texture);
        void UpdateFBO(unsigned int fbo, unsigned int texture);

    private:
        uint32_t m_width;
        uint32_t m_height;

        bool stop;
        unsigned int texture;

        libusb_device_handle* m_devHandle = NULL;

        uint32_t* pixels;

        uint8_t* resizedPixelsRGB;

        uint32_t* pixelsrgb;

        uint16_t* pixelsRGB565;

        uint8_t data_pixel[1 + 320 * 2];
    };
}