#include <tice.h>
#include <keypadc.h>
#include <usbdrvce.h>
#include <graphx.h>
#include "string.h"
#include <sys/lcd.h>
#include <debug.h>
#include <sys/power.h>

bool IsRunning()
{
    return !kb_IsDown(kb_Key5);
}

#define TIMER_FREQ      32768 // Frequency of timer in Hz
#define KEY_RATE      (TIMER_FREQ /16)

usb_device_descriptor_t device_descriptor = {
    .bLength = 0x12,
    .bDescriptorType = USB_DEVICE_DESCRIPTOR,
    .bcdUSB = 0x200,
    .bDeviceClass = 0,
    .bDeviceSubClass = 0,
    .bDeviceProtocol = 0,
    .bMaxPacketSize0 = 0x40,
    .idVendor = 0x0451,
    .idProduct = 0xE009,
    .bcdDevice = 0x0240,
    .iManufacturer = 0x00,
    .iProduct = 0x00,
    .iSerialNumber = 0x00,
    .bNumConfigurations = 0x01
};

uint8_t config_descriptors_array[] = {
    0x09, 0x02, 0x20, 0x00, 0x01, 0x01, 0x00, 
    0x80, 0xFA, 0x09, 0x04, 0x00, 0x00, 0x02, 
    0xFF, 0x01, 0x01, 0x00, 0x07, 0x05, 0x81, 
    0x02, 0x40, 0x00, 0x00, 0x07, 0x05, 0x02,
    0x02, 0x40, 0x00, 0x00
};

const usb_configuration_descriptor_t* config_descriptors[] = {
    (usb_configuration_descriptor_t*)config_descriptors_array
};

usb_standard_descriptors_t standard_descriptors = {
    .device = &device_descriptor,
    .configurations = config_descriptors,
	.langids = NULL,
	.numStrings = 0,
	.strings = NULL,
};

bool connected = false;
bool transfer_scheduled = false;
usb_endpoint_t out_endpoint = NULL;

usb_error_t eventHandler(usb_event_t event, void *eventData, void *context)
{
    if (event == USB_HOST_CONFIGURE_EVENT)
	{
		usb_device_t host_device = usb_FindDevice(NULL, NULL, USB_SKIP_HUBS);

		if (!host_device)
		{
			return USB_ERROR_NO_DEVICE;
		}

		out_endpoint = usb_GetDeviceEndpoint(host_device, 0x02);

		connected = true;
	}

	return USB_SUCCESS;
}

#define BUFFER_SIZE 1 + 320 * 2

uint8_t dataBuffer[BUFFER_SIZE];

uint32_t request_size = 1 + 320 * 2;

extern void lcd_Configure();
extern void lcd_Reset();

usb_error_t transfer_callback(usb_endpoint_t endpoint, usb_transfer_status_t status, size_t transferred, usb_transfer_data_t* data)
{
    if (status == USB_TRANSFER_COMPLETED)
    {
        memcpy((uint8_t*)lcd_Ram + dataBuffer[0] * 320 * 2, (uint8_t*)data + 1, 320 * 2);
    }

    transfer_scheduled = false;

    return USB_SUCCESS;
}

int main(void)
{
    boot_Set48MHzMode();

    if (usb_Init(eventHandler, NULL, &standard_descriptors, USB_DEFAULT_INIT_FLAGS))
    {
        //printf("USB initialized\n");
    }

    os_ClrHome();

    timer_Disable(1);

	timer_Set(1, KEY_RATE);
	timer_SetReload(1, KEY_RATE);

	timer_Enable(1, TIMER_32K, TIMER_0INT, TIMER_DOWN);

	kb_EnableOnLatch();
	kb_ClearOnLatch();

	//lcd_Control = (uint24_t)0b00000100000100111; // 8bpp palette mode

	memset(lcd_Ram, 0, 320 * 240 * 2);
    //memset(lcd_Palette, 0, 256 * 2);

    //lcd_Configure();
    
    while (IsRunning())
    {
        if (usb_HandleEvents() != USB_SUCCESS) break;

        if (connected)
        {
            if (!transfer_scheduled)
            {
                usb_ScheduleTransfer(out_endpoint, &dataBuffer, request_size, transfer_callback, &dataBuffer);

                transfer_scheduled = true;
            }
        }
    }

    //lcd_Reset();
    usb_Cleanup();

	//lcd_Control = 0b00000100100101101; // back to rgb 565 mode

    return 0;
}