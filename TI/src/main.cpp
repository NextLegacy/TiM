#include "main.hpp"

#include "GUI.hpp"

#include <iostream>

int BHW::EntryPoint(std::vector<std::string> args)
{
    TiM::GUI gui;

    gui.Run();

    std::cout << "Hello, World!" << std::endl;

    return 0;
}