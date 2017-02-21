#include "contiki-net.h"
#include "http-socket.h"
#include "ip64-addr.h"

#include <stdio.h>

static struct http_socket s;
static int bytes_received = 0;

/*---------------------------------------------------------------------------*/
PROCESS(http_example_process, "HTTP Example");
AUTOSTART_PROCESSES(&http_example_process);
/*---------------------------------------------------------------------------*/
static void
callback(struct http_socket *s, void *ptr,
         http_socket_event_t e,
         const uint8_t *data, uint16_t datalen)
{
  printf("callback e %d datalen %d data '%s'\n", e, datalen, data);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(http_example_process, ev, data)
{
  static struct etimer et;
  uip_ip4addr_t ip4addr;
  uip_ip6addr_t ip6addr;

  PROCESS_BEGIN();

  uip_ipaddr(&ip4addr, 192,168,18,209);
  ip64_addr_4to6(&ip4addr, &ip6addr);
  uip_nameserver_update(&ip6addr, UIP_NAMESERVER_INFINITE_LIFETIME);

  etimer_set(&et, CLOCK_SECOND * 60);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

  http_socket_init(&s);
  http_socket_get(&s, "http://192.168.18.209/", 0, 0,
                  callback, NULL);

  etimer_set(&et, CLOCK_SECOND);
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    etimer_reset(&et);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
