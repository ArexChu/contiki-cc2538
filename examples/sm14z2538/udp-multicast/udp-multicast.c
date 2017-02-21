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
//#include "apps/mdns/mdns.h"
//#include "simple-rpl.h"
#include "tcp-socket.h"
#include "udp-socket.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PORT 12345

static struct udp_socket s;
static uip_ipaddr_t addr;
static uip_ipaddr_t addr1;

#define SEND_INTERVAL		(30 * CLOCK_SECOND)
static struct etimer periodic_timer, send_timer;

/*---------------------------------------------------------------------------*/
PROCESS(multicast_example_process, "Link local multicast example process");
AUTOSTART_PROCESSES(&multicast_example_process);
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
PROCESS_THREAD(multicast_example_process, ev, data)
{
  PROCESS_BEGIN();

  /* Create a linkl-local multicast addresses. */
  //uip_ip6addr(&addr, 0xfe80, 0, 0, 0, 0x6062, 0x537b, 0xab9f, 0xc79b);
  uip_ip6addr(&addr, 0xfe80, 0, 0, 0, 0x212, 0x4b00, 0x5af, 0x7f61);
  //fe80::212:4b00:5af:7f61
  uip_ip6addr(&addr1, 0xfe80, 0, 0, 0, 0x212, 0x4b00, 0x5af, 0x7f8a);
  //fe80::212:4b00:5af:7f8a

  /* Join local group. */
  if(uip_ds6_maddr_add(&addr) == NULL) {
    printf("Error: could not join local multicast group.\n");
  }

  if(uip_ds6_maddr_add(&addr1) == NULL) {
    printf("Error: could not join local multicast group.\n");
  }

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
    etimer_set(&send_timer, (random_rand() % SEND_INTERVAL));

    PROCESS_WAIT_UNTIL(etimer_expired(&send_timer));

    printf("Sending multicast#1\n");
    udp_socket_sendto(&s,
                      "hello", 6,
                      &addr, PORT);

    printf("Sending multicast#2\n");
    udp_socket_sendto(&s,
                      "world", 6,
                      &addr1, PORT);

    PROCESS_WAIT_UNTIL(etimer_expired(&periodic_timer));
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
