/*
 * Copyright (c) 2013, Institute for Pervasive Computing, ETH Zurich
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
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 */

/**
 * \file
 *      Example resource
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 */

#include "contiki.h"

#if PLATFORM_HAS_LEDS

#include <string.h>
#include "rest-engine.h"
#include "dev/leds.h"
#include <stdlib.h>
#include "pwm.h"

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]", (lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3], (lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

typedef struct {
  uint8_t timer;
  uint8_t ab;
  uint8_t port;
  uint8_t pin;
  uint8_t duty;
  uint8_t off_state;
  uint32_t freq;
} pwm_config_t;

/*---------------------------------------------------------------------------*/
#define MAX_PWM  4
static pwm_config_t pwm_num[MAX_PWM] = {
  {
    .timer = PWM_TIMER_1,
    .ab = PWM_TIMER_A,
    .port = GPIO_C_NUM,
    .pin = 0,
    .duty = 0,
    .freq = 1000,
    .off_state = PWM_OFF_WHEN_STOP,
  }, {
    .timer = PWM_TIMER_1,
    .ab = PWM_TIMER_B,
    .port = GPIO_C_NUM,
    .pin = 1,
    .duty = 0,
    .freq = 1000,
    .off_state = PWM_OFF_WHEN_STOP,
  }, {
    .timer = PWM_TIMER_2,
    .ab = PWM_TIMER_A,
    .port = GPIO_C_NUM,
    .pin = 2,
    .duty = 0,
    .freq = 1000,
    .off_state = PWM_OFF_WHEN_STOP,
  }, {
    .timer = PWM_TIMER_2,
    .ab = PWM_TIMER_B,
    .port = GPIO_C_NUM,
    .pin = 3,
    .duty = 0,
    .freq = 1000,
    .off_state = PWM_OFF_WHEN_STOP,
  }
};
static uint8_t pwm_en[MAX_PWM];

static void res_post_put_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

/*A simple actuator example, depending on the color query parameter and post variable dimmed, corresponding led is activated or deactivated*/
RESOURCE(res_dimmed,
         "title=\"LEDs: ?color=r|g|b, POST/PUT dimmed\";rt=\"Control\"",
         NULL,
         res_post_put_handler,
         res_post_put_handler,
         NULL);

static void
res_post_put_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  size_t len = 0;
  const char *color = NULL;
  const char *dimmed = NULL;
  uint8_t i = 0;
  int success = 1;

  if((len = REST.get_query_variable(request, "color", &color))) {
    PRINTF("color %.*s\n", len, color);

    if(strncmp(color, "r", len) == 0) {
      i = 0;
    } else if(strncmp(color, "g", len) == 0) {
      i = 2;
    } else if(strncmp(color, "b", len) == 0) {
      i = 3;
    } else {
      success = 0;
    }
  } else {
    success = 0;
  } if(success && (len = REST.get_post_variable(request, "dimmed", &dimmed))) {
    PRINTF("dimmed %s\n", dimmed);

	int percent = atoi(dimmed);

	memset(pwm_en, 0, MAX_PWM);

	pwm_num[i].duty = percent;

	if(pwm_enable(pwm_num[i].freq, pwm_num[i].duty, 0,
				pwm_num[i].timer, pwm_num[i].ab) == PWM_SUCCESS) {
		pwm_en[i] = 1;
	}

	if((pwm_en[i]) &&
			(pwm_set_direction(pwm_num[i].timer, pwm_num[i].ab,									PWM_SIGNAL_INVERTED) != PWM_SUCCESS)) {
	}

	if((pwm_en[i]) &&
			(pwm_start(pwm_num[i].timer, pwm_num[i].ab,
					   pwm_num[i].port, pwm_num[i].pin) != PWM_SUCCESS)) {
		pwm_en[i] = 0;
	}

  } else {
    success = 0;
  } if(!success) {
    REST.set_response_status(response, REST.status.BAD_REQUEST);
  }
}
#endif /* PLATFORM_HAS_LEDS */
