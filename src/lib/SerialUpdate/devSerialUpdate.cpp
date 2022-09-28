#include "targets.h"

#if defined(PLATFORM_ESP32) && defined(TARGET_RX)
#include <Update.h>

#include "devSerialUpdate.h"
#include "common.h"
#include "hwTimer.h"
#include "POWERMGNT.h"
#include "devVTXSPI.h"

#include "telemetry.h"

extern void stub_handle_rx_byte(char byte);

static bool running = false;

static void initialize()
{
    running = true;
}

static int event()
{
    if (connectionState == serialUpdate && running)
    {
        running = false;
        hwTimer::stop();
#ifdef HAS_VTX_SPI
        VTxOutputMinimum();
#endif
        POWERMGNT::setPower(MinPower);
        Radio.End();
        return DURATION_IMMEDIATELY;
    }
    return DURATION_IGNORE;
}

static int timeout()
{
    if (connectionState != serialUpdate)
    {
        return DURATION_NEVER;
    }

    while (true)
    {
        uint8_t buf[64];
        int count = Serial.read(buf, sizeof(buf));
        for (int i=0 ; i<count ; i++)
        {
            stub_handle_rx_byte(buf[i]);
        }
    }
    return DURATION_IMMEDIATELY;
}

device_t SerialUpdate_device = {
    .initialize = initialize,
    .start = nullptr,
    .event = event,
    .timeout = timeout
};
#endif