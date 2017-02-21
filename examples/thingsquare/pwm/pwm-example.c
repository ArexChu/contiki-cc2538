/**
 * \addtogroup remote-examples
 * @{
 *
 * \defgroup remote-test-pwm Test the CC2538 PWM driver
 *
 * Demonstrates the use of the CC2538 PWM driver for the Zolertia's Zoul boards
 * The PWM timer is stopped when dropping below PM0, alternatively you can set
 * LPM_CONF_MAX_PM to zero, or call lpm_max_pm(0).  In this example is not 
 * needed as we disable RDC in the Makefile, and the CC2538 never drops below
 * PM0
 *
 * @{
 *
 * \file
 *         A quick program for testing the CC2538 PWM driver
 */
#include "contiki.h"
#include "cpu.h"
#include "dev/leds.h"
#include "dev/watchdog.h"
#include "dev/sys-ctrl.h"
#include "pwm.h"
#include "systick.h"
#include "lpm.h"
#include "dev/ioc.h"
#include <stdio.h>
#include <stdint.h>
#include "dev/cc2538-sensors.h"
#include "dev/button-sensor.h"
/*---------------------------------------------------------------------------*/
#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
/*---------------------------------------------------------------------------*/
typedef struct {
  uint8_t timer;
  uint8_t ab;
  uint8_t port;
  uint8_t pin;
  uint8_t duty;
  uint8_t off_state;
  uint32_t freq;
} pwm_config_t;

static uint8_t R_duty=99, G_duty=99, B_duty=99;
/*---------------------------------------------------------------------------*/
#define MAX_PWM  4
static  pwm_config_t pwm_num[MAX_PWM] = {
  {
    .timer = PWM_TIMER_1,
    .ab = PWM_TIMER_A,
    .port = GPIO_C_NUM,
    .pin = 1,
    .duty = 50,
    .freq = 1000,
    .off_state = PWM_OFF_WHEN_STOP,
  }, {
    .timer = PWM_TIMER_1,
    .ab = PWM_TIMER_B,
    .port = GPIO_C_NUM,
    .pin = 2,
    .duty = 1,
    .freq = 1000,
    .off_state = PWM_ON_WHEN_STOP,
  }, {
    .timer = PWM_TIMER_2,
    .ab = PWM_TIMER_A,
    .port = GPIO_C_NUM,
    .pin = 3,
    .duty = 1,
    .freq = 1000,
    .off_state = PWM_OFF_WHEN_STOP,
  }, {
    .timer = PWM_TIMER_2,
    .ab = PWM_TIMER_B,
    .port = GPIO_C_NUM,
    .pin = 2,
    .duty = 10,
    .freq = 160000,
    .off_state = PWM_ON_WHEN_STOP,
  }
};
static uint8_t pwm_en[MAX_PWM];
/*---------------------------------------------------------------------------*/
#if DEBUG
static const char *
gpt_name(uint8_t timer)
{
  switch(timer) {
  case PWM_TIMER_0:
    return "PWM TIMER 0";
  case PWM_TIMER_1:
    return "PWM TIMER 1";
  case PWM_TIMER_2:
    return "PWM TIMER 2";
  case PWM_TIMER_3:
    return "PWM TIMER 3";
  default:
    return "Unknown";
  }
}
#endif
/*---------------------------------------------------------------------------*/
static struct etimer et;
/*---------------------------------------------------------------------------*/
PROCESS(cc2538_pwm_test, "cc2538 pwm test");
AUTOSTART_PROCESSES(&cc2538_pwm_test);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(cc2538_pwm_test, ev, data)
{
  PROCESS_BEGIN();

  uint8_t i;
  uint16_t sys_clk_freq;
  memset(pwm_en, 0, MAX_PWM);

  PRINTF("\nStarting the test\n");
  sys_clk_freq =sys_ctrl_get_sys_clock();
  PRINTF("sys clk freq: %d \n", sys_clk_freq);
  sys_clk_freq =sys_ctrl_get_io_clock();
  PRINTF("io clk freq: %d \n", sys_clk_freq);
  for(i = 0; i < MAX_PWM; i++) {
    if(pwm_enable(pwm_num[i].freq, pwm_num[i].duty,
                  pwm_num[i].timer, pwm_num[i].ab) == PWM_SUCCESS) {
      pwm_en[i] = 1;
      PRINTF("%s (%u) configuration OK\n", gpt_name(pwm_num[i].timer),
             pwm_num[i].ab);
    }
  }

  while(1) {
   PROCESS_WAIT_EVENT();
   if(ev == sensors_event){
     if(data == &button_left_sensor){
	B_duty = B_duty -5;
	 if(B_duty < 5) B_duty=100;
     }
/*     else if(data == &button_right_sensor ){
       R_duty = R_duty -5;
         if(R_duty < 5) R_duty=100;
      }
     else if(data == &button_down_sensor){
	G_duty = G_duty -5;
	 if(G_duty < 5) G_duty=100;
      }
*/
    }
    for(i = 0; i < MAX_PWM; i++) {
      if((pwm_en[i]) &&
         (pwm_stop(pwm_num[i].timer, pwm_num[i].ab,
                   pwm_num[i].port, pwm_num[i].pin,
                   pwm_num[i].off_state) != PWM_SUCCESS)) {
        pwm_en[i] = 0;
      }
      PRINTF("success stop the PWM timer.\n");
    }
	pwm_num[0].duty = R_duty;
	pwm_num[1].duty = G_duty;
	pwm_num[2].duty = B_duty;

  for(i = 0; i < MAX_PWM; i++) {
    if(pwm_enable(pwm_num[i].freq, pwm_num[i].duty,
                  pwm_num[i].timer, pwm_num[i].ab) == PWM_SUCCESS) {
      pwm_en[i] = 1;
      PRINTF("%s (%u) configuration OK\n", gpt_name(pwm_num[i].timer),
             pwm_num[i].ab);
    }
  }
    for(i = 0; i < MAX_PWM; i++) {
      if((pwm_en[i]) &&
         (pwm_start(pwm_num[i].timer, pwm_num[i].ab,
                    pwm_num[i].port, pwm_num[i].pin) != PWM_SUCCESS)) {
        pwm_en[i] = 0;
      }
      PRINTF("success enable the PWM timer.\n");
      printf("success R:%3u G:%3u B:%3u \n",pwm_num[0].duty,pwm_num[1].duty,pwm_num[2].duty);
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 */
