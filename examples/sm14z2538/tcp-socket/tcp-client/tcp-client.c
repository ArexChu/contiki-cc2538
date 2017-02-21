/*
 * Copyright (c) 2013, Thingsquare, http://www.thingsquare.com/.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

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

#define PORT 80

static struct tcp_socket socket;

#define INPUTBUFSIZE 400
static uint8_t inputbuf[INPUTBUFSIZE];

#define OUTPUTBUFSIZE 400
static uint8_t outputbuf[OUTPUTBUFSIZE];

PROCESS(tcp_client_process, "TCP client example process");
AUTOSTART_PROCESSES(&tcp_client_process);
/*---------------------------------------------------------------------------*/
static int
input(struct tcp_socket *s, void *ptr,
      const uint8_t *inputptr, int inputdatalen)
{
  printf("input %d bytes '%s' - closing socket\n", inputdatalen, inputptr);
  tcp_socket_close(s);
  return 0;
}
/*---------------------------------------------------------------------------*/
static void
event(struct tcp_socket *s, void *ptr,
      tcp_socket_event_t ev)
{
  if(ev == TCP_SOCKET_CONNECTED) {
    printf("Socket connected\n");
    tcp_socket_send_str(s, "Connected\n");
  } else if(ev == TCP_SOCKET_DATA_SENT) {
    printf("Socket data was sent\n");
  } else if(ev == TCP_SOCKET_CLOSED) {
    printf("Socket closed\n");
  } else if(ev == TCP_SOCKET_ABORTED) {
    printf("Socket reset\n");
  } else if(ev == TCP_SOCKET_TIMEDOUT) {
    printf("Socket timedout\n");
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(tcp_client_process, ev, data)
{
  uip_ip6addr_t ip6addr;
  uip_ip4addr_t ip4addr;

  PROCESS_BEGIN();

  //uip_ipaddr(&ip4addr, 192,168,18,33);
  //ip64_addr_4to6(&ip4addr, &ip6addr);
  
  uip_ip6addr(&ip6addr, 0xfe80, 0, 0, 0, 0x212, 0x4b00, 0x42a, 0xeae3); //server's IPv6 address
  tcp_socket_register(&socket, NULL,
                      inputbuf, sizeof(inputbuf),
                      outputbuf, sizeof(outputbuf),
                      input, event);

  tcp_socket_connect(&socket, &ip6addr, PORT);
  while(1) {
    PROCESS_WAIT_EVENT();
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
