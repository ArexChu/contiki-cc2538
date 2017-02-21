#include "thsq.h"

#define PORT 12345

static struct udp_socket s;
static uip_ipaddr_t addr;
static struct uip_ds6_notification n;
static uint8_t i=0; 

#define SEND_INTERVAL		(10 * CLOCK_SECOND)
static struct etimer periodic_timer, send_timer;

/*---------------------------------------------------------------------------*/
PROCESS(unicast_example_process, "Link local unicast example process");
AUTOSTART_PROCESSES(&unicast_example_process);


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

/*---------------------------------------------------------------------------*/
static void
receiver(struct udp_socket *c,
         void *ptr,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
  printf("Data received on port %d from port %d with length %d, '%s'\n",
         receiver_port, sender_port, datalen, data);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(unicast_example_process, ev, data)
{
  uip_ip6addr_t ip6addr;
  uip_ip4addr_t ip4addr;

  PROCESS_BEGIN();
#if 0
  /* Create a linkl-local multicast addresses. */
  uip_ip6addr(&addr, 0xff02, 0, 0, 0, 0, 0, 0x1337, 0x0001);

  /* Join local group. */
  if(uip_ds6_maddr_add(&addr) == NULL) {
    printf("Error: could not join local multicast group.\n");
  }
#endif

  leds_on(LEDS_ALL);

  uip_ds6_notification_add(&n, route_callback);

  /* Register UDP socket callback */
  udp_socket_register(&s, NULL, receiver);

  /* Bind UDP socket to local port */
  udp_socket_bind(&s, PORT);

  /* Connect UDP socket to remote port */
  udp_socket_connect(&s, NULL, PORT);

  while(1) {

    /* Set up two timers, one for keeping track of the send interval,
       which is periodic, and one for setting up a randomized send time
       within that interval. */
    etimer_set(&periodic_timer, SEND_INTERVAL);
    //etimer_set(&send_timer, (random_rand() % SEND_INTERVAL));

    PROCESS_WAIT_UNTIL(etimer_expired(&periodic_timer));

    uip_ipaddr(&ip4addr, 192,168,18,82);

    ip64_addr_4to6(&ip4addr, &ip6addr);

    printf("Sending unicast %d\n",i);
    i++;
    udp_socket_sendto(&s,
                      &i, 1,
                      &ip6addr, PORT);

    //PROCESS_WAIT_UNTIL(etimer_expired(&periodic_timer));
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
