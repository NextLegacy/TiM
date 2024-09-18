#include <sys/lcd.h>
#include <ti/getcsc.h>
#include <string.h>
#include <usbdrvce.h>
#include <ti/screen.h>
#include <stdint.h>
#include <keypadc.h>

bool connected_to_device = false;

usb_endpoint_t endpoint_in ;
usb_endpoint_t endpoint_out;

uint8_t dataBuffer[1 + 320 * 2];

bool transfer_scheduled = false;

static usb_error_t event_handler(usb_event_t event, void* event_data, usb_callback_data_t* callback_data)
{
    if (event == USB_HOST_CONFIGURE_EVENT)
    {
        usb_device_t host = usb_FindDevice(NULL, NULL, USB_SKIP_ATTACHED);
        
        if (!host) return USB_ERROR_NO_DEVICE;

        endpoint_in  = usb_GetDeviceEndpoint(host, 0x81);
        endpoint_out = usb_GetDeviceEndpoint(host, 0x02);

        connected_to_device = true;
    }

    if (event == USB_HOST_INTERRUPT)
    {
        connected_to_device = false;
    }

    return USB_SUCCESS;
}

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
    if (!usb_Init(event_handler, NULL, NULL, USB_DEFAULT_INIT_FLAGS)) 
    {
        return 1;
    }

    kb_EnableOnLatch();
	kb_ClearOnLatch();

    memset(lcd_Palette, 0, 512);
    memset(lcd_Ram, 0, 320 * 240 * 2);

    while (true)
    {
        if (kb_On) break;

        if (connected_to_device)
        {
            if (!transfer_scheduled)
            {
                usb_ScheduleTransfer(endpoint_out, &dataBuffer, 1 + 1 * 320 * 2, transfer_callback, &dataBuffer);
            }
        }

        if (!usb_HandleEvents()) break;
    }

    lcd_UpBase = (uint24_t)lcd_Ram;
	lcd_Control = 0b00000100100101101;

    return 0;
}