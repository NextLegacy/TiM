#include "GUI.hpp"

#include "libusb.h"
#include <BHW/utils/threads/Thread.hpp>

namespace TiM
{
    class TiM : public GUI::EventSubscriber
    {
    public:
        TiM();

        void Start(GUI& gui) override;

        void USBTransferThread(BHW::Thread& thread);

        void USBReceiveThread(BHW::Thread& thread);
        void USBSendThread   (BHW::Thread& thread);

        void Render(GUI& gui) override;

        void RenderPCScreen(GUI& gui);

        void RenderTIScreen(GUI& gui);

        void CreateFramebuffer();

        void CreateShader(GLuint& shader, const char* vertexPath, const char* fragmentPath);

        void CreateTexture(unsigned int& texture);

        void RenderDockspace(GUI& gui);
        void SetupDockspace(GUI& gui, unsigned int& dockspaceID);
    
    private:
    
        uint32_t* pixels;
        uint8_t* pixels8;
        uint32_t* resizedPixels;
        uint8_t* processedPixels;

        unsigned int m_paletteTexture = 0;

        unsigned int m_textureRaw = 0;
        unsigned int m_textureProcessed = 0;
        unsigned int m_framebuffer = 0;
        unsigned int m_rbo = 0;
        unsigned int m_vao = 0;
        unsigned int m_vbo = 0;

        uint32_t m_textureProcessedPixels[320 * 240];
 
        GLuint m_shader = 0;

        uint8_t m_usbBuffer[1024];

        libusb_device_handle* m_usbDeviceHandle;

        BHW::Thread m_usbTransferThread;
        BHW::Thread m_usbReceiveThread;
        BHW::Thread m_usbSendThread;
    };
}