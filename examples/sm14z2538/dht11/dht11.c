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
#include "apps/simple-rpl/simple-rpl.h"
#include "tcp-socket.h"
#include "udp-socket.h"
//#include "websocket.h"
#include "http-socket/http-socket.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dev/DHT11-arch.h"
#include "dev/oled.h"
#define PORT 12345

static struct udp_socket s;
//static uip_ipaddr_t;// addr;
static struct uip_ds6_notification n;
//static uint8_t i=0; 

#define SEND_INTERVAL		(2 * CLOCK_SECOND)
static struct etimer periodic_timer;// send_timer;

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

  leds_on(LEDS_ALL);

  uip_ds6_notification_add(&n, route_callback);

  /* Register UDP socket callback */
  udp_socket_register(&s, NULL, receiver);

  /* Bind UDP socket to local port */
  udp_socket_bind(&s, PORT);

  /* Connect UDP socket to remote port */
  udp_socket_connect(&s, NULL, PORT);

  //etimer_set(&et, CLOCK_SECOND*2); 

  while(1) {

    /* Set up two timers, one for keeping track of the send interval,
       which is periodic, and one for setting up a randomized send time
       within that interval. */
    etimer_set(&periodic_timer, SEND_INTERVAL);
    //etimer_set(&send_timer, (random_rand() % SEND_INTERVAL));

    PROCESS_WAIT_UNTIL(etimer_expired(&periodic_timer));
    unsigned char  temp[7]="Tem:", humidity[7]="Hun:";
    uip_ipaddr(&ip4addr, 192,168,18,86);

    ip64_addr_4to6(&ip4addr, &ip6addr);
    DHT11_Inint();
        //temp="Tem:";
        temp[4] = Tem_dec+0x30;
	temp[5] = Tem_uni+0x30;
	temp[6] = '\0';
        //humidity="Hum:";
	humidity[4] = Hum_dec+0x30;
	humidity[5] = Hum_uni+0x30;
	humidity[6] = '\0';
    if((Tem_data_H!=0)&&(RH_data_H!=0))
    {
        printf("Tem: %d \r\n",Tem_data_H);
	printf("Hum: %d \r\n",RH_data_H);
	OLED_Display_On1();
        OLED_P8x16Str(20,0,"DaLian");
        OLED_P8x16Str(20,2,"Sanmu-Link");
        OLED_P8x16Str(20,4,temp);
        OLED_P8x16Str(20,6,humidity);
    }
    //printf("Sending unicast %d\n",i);
    //i++;
    udp_socket_sendto(&s,
                      temp, 7,
                      &ip6addr, PORT);
    udp_socket_sendto(&s,
                      humidity, 7,
                      &ip6addr, PORT);

    //PROCESS_WAIT_UNTIL(etimer_expired(&periodic_timer));
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
