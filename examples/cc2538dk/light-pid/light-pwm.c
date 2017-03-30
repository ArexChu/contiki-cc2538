/*
 * Copyright (c) 2015, Zolertia - http://www.zolertia.com
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
 */
/*---------------------------------------------------------------------------*/
/**
 * \addtogroup zoul-examples
 * @{
 *
 * \defgroup zoul-tsl256x-test TSL256X light sensor test (TSL2561/TSL2563)
 *
 * Demonstrates the use of the TSL256X digital ambient light sensor
 * @{
 *
 * \file
 *  Test file for the external TSL256X light sensor
 *
 * \author
 *         Antonio Lignan <alinan@zolertia.com>
 *         Toni Lozano <tlozano@zolertia.com>
 */
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include "contiki.h"
#include "dev/i2c.h"
#include "dev/leds.h"
#include "dev/tsl256x.h"
#include "PID.h"
#include "pwm.h"
#include "dev/cc2538-sensors.h"
#include "dev/button-sensor.h"
/*---------------------------------------------------------------------------*/
/* Default sensor's integration cycle is 402ms */
#define SENSOR_READ_INTERVAL (CLOCK_SECOND)
/*---------------------------------------------------------------------------*/
PROCESS(remote_tsl256x_process, "TSL256X test process");
PROCESS(pid_process, "PID process");
PROCESS(test_process, "test process");
AUTOSTART_PROCESSES(&remote_tsl256x_process, &pid_process, &test_process);

// Structure to strore PID data and pointer to PID structure
struct pid_controller ctrldata;
pid_ty pid;

// Control loop input,output and setpoint variables
float input = 0, output = 0;
float setpoint = 12;

// Control loop gains
float kp = 4.0, ki = 3.0, kd = 0;

static struct etimer et_pid;

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
    .port = GPIO_B_NUM,
    .pin = 4,
    .duty = 1,
    .freq = 1000,
    .off_state = PWM_OFF_WHEN_STOP,
  }, {
    .timer = PWM_TIMER_1,
    .ab = PWM_TIMER_B,
    .port = GPIO_B_NUM,
    .pin = 3,
    .duty = 1,
    .freq = 1000,
    .off_state = PWM_ON_WHEN_STOP,
  }, {
    .timer = PWM_TIMER_2,
    .ab = PWM_TIMER_A,
    .port = GPIO_B_NUM,
    .pin = 2,
    .duty = 1,
    .freq = 1000,
    .off_state = PWM_OFF_WHEN_STOP,
  }, {
    .timer = PWM_TIMER_2,
    .ab = PWM_TIMER_B,
    .port = GPIO_B_NUM,
    .pin = 1,
    .duty = 1,
    .freq = 1000,
    .off_state = PWM_ON_WHEN_STOP,
  }
};
static uint8_t pwm_en[MAX_PWM];
/*---------------------------------------------------------------------------*/
static uint8_t light;
static struct etimer et;
/*---------------------------------------------------------------------------*/
void
light_interrupt_callback(uint8_t value)
{
  printf("* Light sensor interrupt!\n");
  //leds_toggle(LEDS_GREEN);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(remote_tsl256x_process, ev, data)
{
  PROCESS_BEGIN();

  /* Print the sensor used, teh default is the TSL2561 (from Grove) */
  if(TSL256X_REF == TSL2561_SENSOR_REF) {
    printf("Light sensor test --> TSL2561\n");
  } else if(TSL256X_REF == TSL2563_SENSOR_REF) {
    printf("Light sensor test --> TSL2563\n");
  } else {
    printf("Unknown light sensor reference, aborting\n");
    PROCESS_EXIT();
  }

  /* Use Contiki's sensor macro to enable the sensor */
  SENSORS_ACTIVATE(tsl256x);

  /* Default integration time is 402ms with 1x gain, use the below call to
   * change the gain and timming, see tsl2563.h for more options
   */
  /* tsl256x.configure(TSL256X_TIMMING_CFG, TSL256X_G16X_402MS); */

  /* Register the interrupt handler */
  TSL256X_REGISTER_INT(light_interrupt_callback);

  /* Enable the interrupt source for values over the threshold.  The sensor
   * compares against the value of CH0, one way to find out the required
   * threshold for a given lux quantity is to enable the DEBUG flag and see
   * the CH0 value for a given measurement.  The other is to reverse the
   * calculations done in the calculate_lux() function.  The below value roughly
   * represents a 2500 lux threshold, same as pointing a flashlight directly
   */
  tsl256x.configure(TSL256X_INT_OVER, 0x15B8);

  /* And periodically poll the sensor */

  while(1) {
    etimer_set(&et, SENSOR_READ_INTERVAL);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    light = tsl256x.value(TSL256X_VAL_READ);
    if(light != TSL256X_ERROR) {
      printf("Light = %d\n", (uint8_t)light);
    } else {
      printf("Error, enable the DEBUG flag in the tsl256x driver for info, ");
      printf("or check if the sensor is properly connected\n");
      PROCESS_EXIT();
    }
  }
  PROCESS_END();
}

PROCESS_THREAD(pid_process, ev, data)
{
	PROCESS_BEGIN();

	uint8_t i;
	memset(pwm_en, 0, MAX_PWM);

	// Prepare PID controller for operation
	pid = pid_create(&ctrldata, &input, &output, &setpoint, kp, ki, kd);
	// Set controler output limits from 0 to 200
	pid_limits(pid, 1, 99);
	// Allow PID to compute and change output
	pid_auto(pid);

	while(1)
	{
		// Check if need to compute PID
		if (pid_need_compute(pid)) {
			// Read process feedback
			input = (float)light;
			//printf("input = %d\n", (int)input);
			// Compute new PID output value
			pid_compute(pid);
			//Change actuator value
			for(i = 0; i < MAX_PWM; i++)
			{
				pwm_num[i].duty = (int)output;
			}
			printf("output = %d\n", (int)output);

			for(i = 0; i < MAX_PWM; i++) {
				if(pwm_enable(pwm_num[i].freq, pwm_num[i].duty, 0,
							pwm_num[i].timer, pwm_num[i].ab) == PWM_SUCCESS) {
					pwm_en[i] = 1;
				}
			}

			for(i = 0; i < MAX_PWM; i++) {
				if((pwm_en[i]) &&
						(pwm_set_direction(pwm_num[i].timer, pwm_num[i].ab,									PWM_SIGNAL_INVERTED) != PWM_SUCCESS)) {
				}
			}

			for(i = 0; i < MAX_PWM; i++) {
				if((pwm_en[i]) &&
						(pwm_start(pwm_num[i].timer, pwm_num[i].ab,
								   pwm_num[i].port, pwm_num[i].pin) != PWM_SUCCESS)) {
					pwm_en[i] = 0;
				}
			}
		}

		etimer_set(&et_pid, CLOCK_SECOND);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et_pid));

	}

	PROCESS_END();
}

PROCESS_THREAD(test_process, ev, data)
{
	PROCESS_BEGIN();
	while(1)
	{
		PROCESS_WAIT_EVENT();

		if(ev == sensors_event){
			if(data == &button_up_sensor ){
				kp = kp + 0.1;
				if(kp > 6.0) 
					kp = 0;
				printf("kp = %d\n", (int)(kp*10));
			}else if(data == &button_down_sensor){
				ki = ki + 0.1;
				if(ki > 5.0) 
					ki = 0;
				printf("ki = %d\n", (int)(ki*10));
			}else if(data == &button_left_sensor){
				kd = kd + 0.1;
				if(kd > 5.0) 
					kd = 0;
				printf("kd = %d\n", (int)(kd*10));
			}else if(data == &button_right_sensor){
				setpoint = setpoint + 1;
				if(setpoint > 30.0)
					setpoint = 0;
				printf("setpoint = %d\n", (int)setpoint);
			}
		}

		pid_tune(pid, kp, ki, kd);
	}
	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 */

