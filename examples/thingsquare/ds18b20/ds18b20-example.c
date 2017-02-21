#include "contiki.h"
#include "sys/etimer.h"
#include "dev/leds.h"
#include "dev/cc2538-sensors.h"
#include "dev/button-sensor.h"
#include "dev/watchdog.h"
#include "dev/ds18b20.h"

#include <stdio.h>
#include <stdint.h>
/*---------------------------------------------------------------------------*/
#define LOOP_INTERVAL       CLOCK_SECOND
#define BROADCAST_CHANNEL   129
/*---------------------------------------------------------------------------*/
static struct etimer et;
static uint16_t counter;
static uint8_t ch[8];
/*---------------------------------------------------------------------------*/
PROCESS(cc2538_demo_process, "cc2538 demo process");
AUTOSTART_PROCESSES(&cc2538_demo_process);
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(cc2538_demo_process, ev, data)
{

  PROCESS_BEGIN();

  counter = 0;

  etimer_set(&et, CLOCK_SECOND);

  while(1) {

    PROCESS_YIELD();

     if(data == &button_left_sensor) {
	printf("Button K1 pressed.\n");
        leds_toggle(LEDS_RED);
        SET_OUT;
	SET_DQ;
      } else if(data == &button_right_sensor) {
	printf("Button K2 pressed.\n");
        leds_toggle(LEDS_GREEN);
	 ds18b20_read(ch);
         printf("Env Temp:%s\n", (char *)ch);
      }
    
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 * @}
 */
