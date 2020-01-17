
#include "mgos.h"
#include "driver/touch_pad.h"
//#include "esp32_touchpad.h"
#include "mgos_system.h"
#include "mgos_time.h"

static uint16_t poll_interval = 200;         //ms
static uint16_t long_touch_duration = 3000; //ms
static int64_t last_touch_event = 0;

static void poll_touchpad_cb(void *arg)
{
  static uint16_t touch_value;
  static uint16_t touch_duration = 0; // in ms
  static bool touched = false;
  static bool touch_emitted = false;
  static bool long_touch_emitted = false;

  //touch_value = esp32_touch_pad_read(9);
  touch_pad_read(TOUCH_PAD_NUM9, &touch_value);
  LOG(LL_DEBUG, ("[%4d]", touch_value));

  if (touch_value < 500)
  {
    LOG(LL_INFO, ("touched"));
    touched = true;
    touch_duration += poll_interval;
    if (touch_duration >= long_touch_duration)
    {
      // emit long touch
      if (!long_touch_emitted)
      {
        long_touch_emitted = true;
        LOG(LL_INFO, ("emit looooong touch event"));
        // emit code here ...
        last_touch_event = mgos_uptime_micros();
      }
    }
    else
    {
      // emit touch event
      if (!touch_emitted)
      {
        touch_emitted = true;
        LOG(LL_INFO, ("emit touch event"));
        // emit code here ...
      }
    }
  }
  else
  {
    LOG(LL_DEBUG, ("!t"));
    if (touched)
    { // release
      LOG(LL_INFO, ("released"));
      touched = false;
      touch_duration = 0;
      touch_emitted = false;
      long_touch_emitted = false;
    }
  }

  (void)arg;
}

enum mgos_app_init_result mgos_app_init(void)
{

  // Initialize touch pad peripheral.
  // The default fsm mode is software trigger mode.
  touch_pad_init();
  // Set reference voltage for charging/discharging
  // In this case, the high reference valtage will be 2.7V - 1V = 1.7V
  // The low reference voltage will be 0.5
  // The larger the range, the larger the pulse count value.
  touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
  touch_pad_config(TOUCH_PAD_NUM9, 0);

  mgos_set_timer(poll_interval, MGOS_TIMER_REPEAT, poll_touchpad_cb, NULL);

  return MGOS_APP_INIT_SUCCESS;
}
