#pragma once

#include <BHW/utils/console/Console.hpp>
#include <BHW/utils/event/EventSystem.hpp>

#include <TC/gapi/impl/GLFW_OpenGL_DearImGui.hpp>
#include <TC/TeaCup.hpp>

#include "EventSubscriber.hpp"
#include "MirrorGui.hpp"

namespace TiM
{
    class GUI : public TC::TeaCup<TC::GLFW_OpenGL_DearImGui, EventSubscriber, GUI>
    {
    public:
        inline GUI() : TeaCup() 
        { 
            this-> template SubscribeEventSystem<MirrorGui>();
        }
    };
}