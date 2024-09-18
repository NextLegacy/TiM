#include "main.hpp"

#include "GUI.hpp"
#include "TiM.hpp"

#include <iostream>

#include <libusb.h>

int BHW::EntryPoint(std::vector<std::string> args)
{
    TiM::GUI gui;
    
    gui.SubscribeEventSystem<TiM::TiM>();

    gui.Run();

    std::cout << "Hello, World!" << std::endl;

    /*


    bool done = false;
    bool stop = false;

    int calcState = 0;

	while (!done)
	{
		calcState = 0;

		libusb_device_handle* devHandle = NULL;
		while (devHandle == NULL && !done)
		{
			devHandle = libusb_open_device_with_vid_pid(NULL, 0x0451, 0xE009);

			if (devHandle == NULL)
			{
                std::cout << "Device not found" << std::endl;
			}
			else
			{
				calcState = 3;
				break;
			}

			Sleep(100);
		}

		if (done)
		{
			libusb_exit(NULL);
			return 0;
		}

		libusb_set_auto_detach_kernel_driver(devHandle, 1);
		libusb_claim_interface(devHandle, 0);

        std::thread sendThread([&] {
            while (!stop)
            {
                //libusb_bulk_transfer(devHandle, 2, dataToSend + i * (320 + 1), 320 + 1, &sentLen, 0);
            }

            //ReleaseDC(NULL, hdc);
        });
        
        sendThread.join();

		libusb_close(devHandle);

		stop = false;
	}

	libusb_exit(NULL);
    */
    return 0;
}