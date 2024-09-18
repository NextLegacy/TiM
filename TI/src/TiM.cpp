#pragma once

#include "ImageUtils.hpp"
#include "Screen.hpp"

#include "TiM.hpp"

#include <Windows.h>

#include <future>

#include <BHW/utils/io/FileHandler.hpp>

static const float rectangleVertices[] =
{
	// Coords    // texCoords
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f,

	 1.0f,  1.0f,  1.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f
};

bool connected = false;

namespace TiM
{
    TiM::TiM() :
        m_usbTransferThread(std::bind(&TiM::USBTransferThread, this, std::placeholders::_1)),
        m_usbReceiveThread (std::bind(&TiM::USBReceiveThread , this, std::placeholders::_1)),
        m_usbSendThread    (std::bind(&TiM::USBSendThread    , this, std::placeholders::_1)),
        m_usbBuffer        { 0 }
    {
        
    }

    void TiM::Start(GUI& gui)
    {
        m_usbTransferThread.Activate();
    }

    void TiM::USBTransferThread(BHW::Thread& thread)
    {
        libusb_init_context(NULL, NULL, 0);
        
        while (!thread.ShouldStop())
        {
            connected = false;

            m_usbDeviceHandle = NULL;

            while (m_usbDeviceHandle == NULL && !thread.ShouldStop())
            {
                m_usbDeviceHandle = libusb_open_device_with_vid_pid(NULL, 0x0451, 0xE009);

                if (m_usbDeviceHandle != NULL)
                {
                    connected;
                }

                Sleep(100);
            }

            if (thread.ShouldStop())
            {
                libusb_exit(NULL);
                return;
            }

            libusb_set_auto_detach_kernel_driver(m_usbDeviceHandle, 1);
            libusb_claim_interface(m_usbDeviceHandle, 0);

            m_usbReceiveThread.Activate();
            m_usbSendThread   .Activate();

            m_usbReceiveThread.Join();
            m_usbSendThread   .Join();

            libusb_close(m_usbDeviceHandle);
        }
    }

    void TiM::USBReceiveThread(BHW::Thread& thread)
    {
    }

    void TiM::USBSendThread(BHW::Thread& thread)
    {
        libusb_bulk_transfer(m_usbDeviceHandle, 0x02, (unsigned char*)"", 0, NULL, 0);
    }

    void TiM::Render(GUI& gui)
    {
        if (m_shader == 0)
        {
            CreateShader(m_shader, "./shaders/vertex.glsl", "./shaders/fragment.glsl");
        }

        if (m_textureRaw == 0)
        {
            CreateTexture(m_textureRaw);
        }

        if (m_framebuffer == 0)
        {
            CreateFramebuffer();
        }

        if (m_paletteTexture == 0)
        {
            glGenTextures(1, &m_paletteTexture);
            glBindTexture(GL_TEXTURE_1D, m_paletteTexture);

            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, palette);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Prevents edge bleeding

            glBindTexture(GL_TEXTURE_1D, 0);
            std::cout << "Created Palette Texture" << std::endl;
            uint8_t* data8 = new uint8_t[256];

            glTexImage1D(GL_TEXTURE_1D, 0, GL_RED, 256, 0, GL_RED, GL_UNSIGNED_BYTE, data8);
        }

        RenderDockspace(gui);

        ImGui::Begin("Controls");

        ImGui::Text("Hello, World!");

        ImGui::End();

        ImGui::Begin("PC Screen");

        RenderPCScreen(gui);

        ImGui::Image((void*)(intptr_t)m_textureRaw, ImVec2(320, 240));

        ImGui::End();

        ImGui::Begin("TI-84 Screen");

        RenderTIScreen(gui);

        ImGui::Image((void*)(intptr_t)m_textureProcessed, ImVec2(320, 240));

        ImGui::End();
    }

    void TiM::RenderPCScreen(GUI& gui)
    {
        int x = 0;
        int y = 0;
        int width  = GetSystemMetrics(SM_CXSCREEN);
        int height = GetSystemMetrics(SM_CYSCREEN);

        static bool once = true;

        if (once)
        {
            once = false;
            pixels = new uint32_t[width * height];
            pixels8 = new uint8_t[width * height * 3];
            resizedPixels = new uint32_t[320 * 240];
        }

        GetPixelsFromScreen(pixels, x, y, width, height);

        pixels8 = resizeImage((uint8_t*)pixels, width, height, 320, 240);

        glBindTexture(GL_TEXTURE_2D, m_textureRaw);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 320, 240, GL_BGR, GL_UNSIGNED_BYTE, pixels8);
    }
    
    void TiM::RenderTIScreen(GUI& gui)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
        glViewport(0, 0, 320, 240);
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glUseProgram(m_shader);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_1D, m_paletteTexture);
        glUniform1i(glGetUniformLocation(m_shader, "palette"), 1);

        glBindVertexArray(m_vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glUseProgram(0);
    }

    void TiM::RenderDockspace(GUI& gui)
    {
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        ImGuiViewport*   viewport     = ImGui::GetMainViewport();

        ImGui::SetNextWindowPos     (viewport->Pos );
        ImGui::SetNextWindowSize    (viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID  );

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding  , 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        window_flags |= ImGuiWindowFlags_NoTitleBar            | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        
        ImGui::Begin("Dockspace", nullptr, window_flags);

        ImGui::PopStyleVar();
        ImGui::PopStyleVar(2);

        if (gui.GetIO()->ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspaceID = ImGui::GetID("MyDockspace");

            ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspace_flags);

            static auto first_time = true;
            if (first_time)
            {
                first_time = false;

                ImGui::DockBuilderRemoveNode (dockspaceID); // clear any previous layout
                ImGui::DockBuilderAddNode    (dockspaceID, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
                ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->Size);

                SetupDockspace(gui, dockspaceID);

                ImGui::DockBuilderFinish(dockspaceID);
            }
        }

        ImGui::End();
    }

    void TiM::SetupDockspace(GUI& gui, unsigned int& dockspaceID)
    {
        auto dock_id_left     = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Left , 0.5f, nullptr, &dockspaceID);
        auto dock_id_Right    = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Right, 0.75f, nullptr, &dockspaceID);
        auto dock_id_leftDown = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Down , 0.5f, nullptr, &dock_id_left);

        ImGui::DockBuilderDockWindow("Controls", dock_id_Right);
        ImGui::DockBuilderDockWindow("PC Screen", dock_id_left);
        ImGui::DockBuilderDockWindow("TI-84 Screen", dock_id_leftDown);
    }

    void TiM::CreateShader(unsigned int& shader, const char* vertexPath, const char* fragmentPath)
    {
        shader = glCreateProgram();

        std::string vertexCode = BHW::ReadFile(vertexPath);
        std::string fragmentCode = BHW::ReadFile(fragmentPath);

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        GLuint vertex, fragment;
        GLint success;
        GLchar infoLog[512];

        // Vertex Shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        // Fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        // Shader Program
        GLuint shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertex);
        glAttachShader(shaderProgram, fragment);
        glLinkProgram(shaderProgram);
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        glDeleteShader(vertex);
        glDeleteShader(fragment);

        shader = shaderProgram;
    }
       
    void TiM::CreateTexture(unsigned int& texture)
    {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 320, 240, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Prevents edge bleeding

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void TiM::CreateFramebuffer()
    {
        glGenFramebuffers(1, &m_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

        // Create Framebuffer Texture
        CreateTexture(m_textureProcessed);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureProcessed, 0);

        // Create Render Buffer Object
        glGenRenderbuffers(1, &m_rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 320, 240);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

        // Create Vertex Array Object
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);

        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}