#include "thsq.h"

#include "http-socket.h"

static struct http_socket s;

#define SEND_INTERVAL		(60 * CLOCK_SECOND)

/*---------------------------------------------------------------------------*/
PROCESS(mesh_node_process, "Mesh node");
AUTOSTART_PROCESSES(&mesh_node_process);
/*---------------------------------------------------------------------------*/
static void
route_callback(int event, uip_ipaddr_t *route, uip_ipaddr_t *ipaddr,
               int numroutes)
{
  if(event == UIP_DS6_NOTIFICATION_DEFRT_ADD) {
    leds_off(LEDS_ALL);
    printf("Got a RPL route\n");
  }
}
/*---------------------------------------------------------------------------*/static void
callback_http(struct http_socket *s, void *ptr,
         http_socket_event_t e,
         const uint8_t *data, uint16_t datalen)
{
  printf("HTTP call done,callback e %d datalen %d data '%s'\n", e, datalen, data);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(mesh_node_process, ev, data)
{
  static struct uip_ds6_notification n;
  static struct etimer periodic_timer;
  static uip_ip4addr_t v4addr;
  static uip_ip6addr_t v6addr;
  PROCESS_BEGIN();

  leds_on(LEDS_ALL);
  uip_ds6_notification_add(&n, route_callback);
  //mdns_init();

  /* Turn off AES */
  netstack_aes_set_active(0);

  etimer_set(&periodic_timer, SEND_INTERVAL);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

  while(1) {
    etimer_set(&periodic_timer, SEND_INTERVAL);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

    uip_ipaddr(&v4addr, 192,168,18,82);
    ip64_addr_4to6(&v4addr, &v6addr);
  
    printf("Doing GET http://192.168.18.82/\n");
    http_socket_get(&s, "http://192.168.18.82/", callback_http, NULL);

  }


  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
