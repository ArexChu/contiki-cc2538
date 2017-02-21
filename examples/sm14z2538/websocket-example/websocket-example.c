#include "contiki.h"
#include "contiki-net.h"
#include "contiki-lib.h"
#include "lib/random.h"
#include "sys/ctimer.h"
#include "sys/etimer.h"
#include "net/rpl/rpl.h"
#include "net/ip/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "dev/leds.h"

#include "aes.h"
#include "ip64-addr.h"
#include "apps/mdns/mdns.h"
#include "simple-rpl.h"
#include "tcp-socket.h"
#include "udp-socket.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "websocket.h"

static struct websocket s;

static void callback(struct websocket *s, websocket_result r,
                     uint8_t *data, uint16_t datalen);

#define RECONNECT_INTERVAL 10 * CLOCK_SECOND
static struct ctimer reconnect_timer;

/*---------------------------------------------------------------------------*/
PROCESS(websocket_example_process, "Websocket Example");
AUTOSTART_PROCESSES(&websocket_example_process);
/*---------------------------------------------------------------------------*/
static void
reconnect_callback(void *ptr)
{
  websocket_open(&s, "ws://192.168.1.101:8080/",
                 "thingsquare mist", callback);
}
/*---------------------------------------------------------------------------*/
static void
callback(struct websocket *s, websocket_result r,
         uint8_t *data, uint16_t datalen)
{
  if(r == WEBSOCKET_CLOSED ||
     r == WEBSOCKET_RESET ||
     r == WEBSOCKET_HOSTNAME_NOT_FOUND ||
     r == WEBSOCKET_TIMEDOUT) {
    ctimer_set(&reconnect_timer, RECONNECT_INTERVAL, reconnect_callback, s);
  } else if(r == WEBSOCKET_CONNECTED) {
    websocket_send_str(s, "Connected");
  } else if(r == WEBSOCKET_DATA) {
    printf("websocket-example: Received data '%.*s' (len %d)\n", datalen,
           data, datalen);
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(websocket_example_process, ev, data)
{
  static struct etimer et;
  PROCESS_BEGIN();

  ctimer_set(&reconnect_timer, RECONNECT_INTERVAL, reconnect_callback, &s);

  while(1) {
    etimer_set(&et, CLOCK_SECOND / 8);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    //    websocket_send_str(&s, "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef");
    char buf[] = "012345678";
    static int count;
    buf[0] = (count % 9) + '0';
    count++;
    websocket_send_str(&s, buf);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
