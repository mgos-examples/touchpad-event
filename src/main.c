
#include "mgos.h"
#include "driver/touch_pad.h"
#include "mgos_system.h"
#include "mgos_time.h"
#include "mgos_event.h"

/* data structure */
static uint16_t poll_interval = 100;        //ms
static uint16_t long_touch_duration = 3000; //ms
static int64_t last_touch_event = 0;  // us
static int64_t last_release_event = 0;  // us

// lazy way of creating events, though it is more elegant to pass the touchpad number via evdata
#define TPAD_EVT_BASE MGOS_EVENT_BASE('T', 'P', 'E')
enum tpad_event
{
  TOUCH9 = TPAD_EVT_BASE,
  LONG_TOUCH9,
  UNTOUCH9,
  DOUBLE_TOUCH9
  /* add TOUCH8 ...etc for additional used pins */
};

static void poll_touchpad_cb(void *arg)
{
  static uint16_t touch_value;
  static uint16_t touch_duration = 0; // in ms
  static bool touched = false;
  static bool touch_emitted = false;
  static bool long_touch_emitted = false;
  int64_t now = mgos_uptime_micros();

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
        mgos_event_trigger(LONG_TOUCH9, NULL);
        last_touch_event = now;
      }
    }
    else
    {
      // emit touch event
      if (!touch_emitted)
      {
        touch_emitted = true;
        LOG(LL_INFO, ("emit touch event"));
        mgos_event_trigger(TOUCH9, NULL);
        last_touch_event = now;
      }
    }
  }
  else
  {
    LOG(LL_DEBUG, ("!t"));
    if (touched)
    { // release
      LOG(LL_INFO, ("released"));
      mgos_event_trigger(UNTOUCH9, NULL);
      touched = false;
      touch_duration = 0;
      touch_emitted = false;
      long_touch_emitted = false;
      if ((now - last_release_event) < 1000000 ) {        
        mgos_event_trigger(DOUBLE_TOUCH9, NULL);
        LOG(LL_INFO, ("double touch"));
        last_release_event = 0;
      } else {
        last_release_event = now;
      }
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

  mgos_event_register_base(TPAD_EVT_BASE, "touchpad module");
  mgos_set_timer(poll_interval, MGOS_TIMER_REPEAT, poll_touchpad_cb, NULL);

  return MGOS_APP_INIT_SUCCESS;
}
