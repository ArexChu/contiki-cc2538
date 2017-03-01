#include "contiki.h"
#include "leds.h"
/*---------------------------------------------------------------------------*/
PROCESS(blink_process, "Blink");
PROCESS(hello_world_process, "hello world process");
AUTOSTART_PROCESSES(&blink_process, &hello_world_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(blink_process, ev, data)
{
  PROCESS_EXITHANDLER(leds_off(LEDS_ALL););
  PROCESS_BEGIN();

  static struct etimer et;
  etimer_set(&et, CLOCK_SECOND);

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    etimer_reset(&et);
    leds_on(LEDS_ALL);

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    etimer_reset(&et);
    leds_off(LEDS_ALL);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{
  PROCESS_BEGIN();

  static struct etimer et;
  etimer_set(&et, CLOCK_SECOND);
  
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    etimer_reset(&et);

    printf("Hello world\n");
  }
  PROCESS_END();
}
