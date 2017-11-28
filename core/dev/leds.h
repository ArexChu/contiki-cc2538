/*
 * Copyright (c) 2005, Swedish Institute of Computer Science
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
 *
 */
/**
 * \addtogroup dev
 * @{
 */

/**
 * \defgroup leds LEDs API
 *
 * The LEDs API defines a set of functions for accessing LEDs for
 * Contiki plaforms with LEDs.
 *
 * A platform with LED support must implement this API.
 * @{
 */

#ifndef LEDS_H_
#define LEDS_H_

/* Allow platform to override LED numbering */
#include "contiki-conf.h"

void leds_init(void);

/**
 * Blink all LEDs.
 */
void leds_blink(void);

#ifndef LEDS_GREEN
#define LEDS_GREEN  1
#endif /* LEDS_GREEN */
#ifndef LEDS_YELLOW
#define LEDS_YELLOW  2
#endif /* LEDS_YELLOW */
#ifndef LEDS_RED
#define LEDS_RED  4
#endif /* LEDS_RED */
#ifndef LEDS_BLUE
#define LEDS_BLUE  LEDS_YELLOW
#endif /* LEDS_BLUE */

#ifdef LEDS_CONF_ALL
#define LEDS_ALL    LEDS_CONF_ALL
#else /* LEDS_CONF_ALL */
#define LEDS_ALL    7
#endif /* LEDS_CONF_ALL */

#define LEDS_B_ALL    0xff
#define LEDS_D_ALL    0x0f

#define LEDS_1  1
#define LEDS_2  2
#define LEDS_3  4
#define LEDS_4  8
#define LEDS_5  0x10
#define LEDS_6  0x20
#define LEDS_7  0x40 
#define LEDS_8  0x80

/**
 * Returns the current status of all leds
 */
unsigned char leds_get(void);
void leds_set(unsigned char leds);
void leds_on(unsigned char leds);
void leds_off(unsigned char leds);
void leds_toggle(unsigned char leds);

unsigned char leds_b_get(void);
void leds_b_set(unsigned char leds);
void leds_b_on(unsigned char leds);
void leds_b_off(unsigned char leds);

unsigned char leds_d_get(void);
void leds_d_set(unsigned char leds);
void leds_d_on(unsigned char leds);
void leds_d_off(unsigned char leds);

/**
 * Leds implementation
 */
void leds_arch_init(void);
unsigned char leds_arch_get(void);
void leds_arch_set(unsigned char leds);

void leds_b_arch_init(void);
unsigned char leds_b_arch_get(void);
void leds_b_arch_set(unsigned char leds);

void leds_d_arch_init(void);
unsigned char leds_d_arch_get(void);
void leds_d_arch_set(unsigned char leds);

#endif /* LEDS_H_ */

/** @} */
/** @} */
