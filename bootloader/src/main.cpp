#include "ch.h"
#include "hal.h"
#include "stdlib.h"
#include "string.h"
#include "stm32f4xx.h"
#include "stm32f4xx_iwdg.h"
#include "stdbool.h"
#include "peripherals.h"
#include "comms.h"
#include "constants.h"
#include "helper.h"

namespace motor_driver {

static systime_t last_comms_activity_time = 0;

static void comms_activity_callback() {
  last_comms_activity_time = chTimeNow();
}

/*
 * LED blinker thread
 */

static WORKING_AREA(blinker_thread_wa, 512);
static msg_t blinkerThreadRun(void *arg) {
  (void)arg;

  chRegSetThreadName("blinker");

  int t = 0;

  peripherals::setCommsActivityLED(false);

  while (true) {
    uint8_t g = ::abs(t - 255);
    peripherals::setStatusLEDColor(0, 0, g);

    systime_t time_now = chTimeNow();

    peripherals::setCommsActivityLED(time_now - last_comms_activity_time < MS2ST(consts::comms_activity_led_duration) &&
                        last_comms_activity_time != 0);

    t = (t + 10) % 510;
    chThdSleepMilliseconds(10);
  }

  return CH_SUCCESS; // Should never get here
}

/*
 * Communications thread
 */

static WORKING_AREA(comms_thread_wa, 512);
static msg_t commsThreadRun(void *arg) {
  (void)arg;

  chRegSetThreadName("comms");

  comms::startComms();

  while (true) {
    comms::runComms();
  }

  return CH_SUCCESS; // Should never get here
}

int main(void) {
  // Start RTOS
  halInit();
  chSysInit();

  if (RCC->CSR & RCC_CSR_WDGRSTF) {
    flashJumpApplication((uint32_t)consts::firmware_ptr);
  }

  // Start peripherals
  peripherals::startPeripherals();

  // Set comms activity callback
  comms::comms_protocol_fsm.setActivityCallback(&comms_activity_callback);

  // Start threads
  chThdCreateStatic(blinker_thread_wa, sizeof(blinker_thread_wa), LOWPRIO, blinkerThreadRun, NULL);
  chThdCreateStatic(comms_thread_wa, sizeof(comms_thread_wa), NORMALPRIO, commsThreadRun, NULL);

  // Wait forever
  while (true) {
    chThdSleepMilliseconds(1000);
  }

  return CH_SUCCESS; // Should never get here
}

} // namespace motor_driver

// FIXME: hack
int main(void) {
  return motor_driver::main();
}
