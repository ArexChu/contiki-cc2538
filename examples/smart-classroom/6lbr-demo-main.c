#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "rest-engine.h"

#if PLATFORM_HAS_BUTTON
#include "dev/button-sensor.h"
#endif

#define DEBUG 0
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

#if SHELL
#include "shell.h"
#include "serial-shell.h"
#include "dev/serial-line.h"
#ifdef CONTIKI_TARGET_Z1
#include "dev/uart0.h"
#else
#include "dev/uart1.h"
#endif
#include "shell-6lbr.h"
#endif

#ifdef CONTIKI_TARGET_ECONOTAG
#include "maca.h"
#endif

#ifdef CONTIKI_TARGET_CC2538DK
#include "i2c.h"
#endif

#if WITH_TINYDTLS
#include "dtls.h"
#endif

#if WITH_DTLS_ECHO
#include "dtls-echo.h"
#endif

#if WITH_CETIC_6LN_NVM
#include "nvm-config.h"
#endif

#if WITH_COAPSERVER
#include "coap-server.h"
#endif

#if WITH_IPSO_APP_FW
#include "ipso-app-fw.h"
#endif

#if WITH_LWM2M
#include "lwm2m.h"
#endif

/*
 * Resources to be activated need to be imported through the extern keyword.
 * The build system automatically compiles the resources in the corresponding sub-directory.
 */
extern resource_t
  res_hello,
  res_mirror,
  res_chunks,
  res_separate,
  res_push,
  res_event,
  res_sub,
  res_b1_sep_b2,
  res_occupy;
#if PLATFORM_HAS_LEDS
extern resource_t res_leds, res_toggle, res_status, res_dimmed;
#endif
#if PLATFORM_HAS_LIGHT
#include "dev/tsl256x.h"
extern resource_t res_light;
extern resource_t res_human;
#endif
#if PLATFORM_HAS_BATTERY
extern resource_t res_battery;
#endif
#if PLATFORM_HAS_RADIO
extern resource_t res_radio;
#endif
#if PLATFORM_HAS_SHT11
extern resource_t res_sht11;
#endif

PROCESS(er_example_server, "Erbium Example Server");
PROCESS(demo_6lbr_process, "6LBR Demo");

#if WEBSERVER
PROCESS_NAME(web_sense_process);
PROCESS_NAME(webserver_nogui_process);
#endif
#if UDPCLIENT
PROCESS_NAME(udp_client_process);
#endif

/*---------------------------------------------------------------------------*/
#define INIT_USER_MOD(module) \
  extern void module##_init(); \
  module##_init();

/*---------------------------------------------------------------------------*/
void
light_interrupt_callback(uint8_t value)
{
  printf("* Light sensor interrupt!\n");
  //leds_toggle(LEDS_GREEN);
}
/*---------------------------------------------------------------------------*/

void
start_apps(void)
{
#if UDPCLIENT
  process_start(&udp_client_process, NULL);
#endif

#if WEBSERVER
  process_start(&web_sense_process, NULL);
  process_start(&webserver_nogui_process, NULL);
#endif

#if WITH_COAPSERVER
  coap_server_init();
#endif

#if WITH_IPSO_APP_FW
  ipso_app_fw_init();
#endif

#if WITH_LWM2M
  lwm2m_init();
#endif

#if !IGNORE_CETIC_CONTIKI_PLATFORM && WITH_COAPSERVER
  extern void contiki_platform_resources_init();
  contiki_platform_resources_init();
#endif

#if defined CETIC_6LN_PLATFORM_CONF && WITH_COAPSERVER
  extern void platform_resources_init();
  platform_resources_init();
#endif

#if WITH_DTLS_ECHO
  process_start(&dtls_echo_server_process, NULL);
#endif

#ifdef USER_MODULES
  USER_MODULES;
#endif
}

PROCESS_THREAD(demo_6lbr_process, ev, data)
{
  PROCESS_BEGIN();

#if WITH_CETIC_6LN_NVM
  load_nvm_config();
#endif

#ifdef CONTIKI_TARGET_ECONOTAG
  set_channel(RF_CHANNEL - 11);
#endif

#ifdef CONTIKI_TARGET_CC2538DK
#if ENABLE_I2C
  i2c_init(I2C_SDA_PORT, I2C_SDA_PIN, I2C_SCL_PORT, I2C_SCL_PIN, I2C_SCL_FAST_BUS_SPEED);
#endif
#endif

  #if WITH_TINYDTLS
  dtls_init();
#endif

#if SHELL
#ifdef CONTIKI_TARGET_Z1
  uart0_set_input(serial_line_input_byte);
#elif defined CONTIKI_TARGET_COOJA
  rs232_set_input(serial_line_input_byte);
#else
  uart1_set_input(serial_line_input_byte);
#endif
  serial_line_init();

  serial_shell_init();
  shell_ping_init();
  shell_6lbr_init();
#endif
#if !WITH_DELAY_IP
  start_apps();
#endif

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(er_example_server, ev, data)
{
  PROCESS_BEGIN();

  PROCESS_PAUSE();

  PRINTF("Starting Erbium Example Server\n");

#ifdef RF_CHANNEL
  PRINTF("RF channel: %u\n", RF_CHANNEL);
#endif
#ifdef IEEE802154_PANID
  PRINTF("PAN ID: 0x%04X\n", IEEE802154_PANID);
#endif

  PRINTF("uIP buffer: %u\n", UIP_BUFSIZE);
  PRINTF("LL header: %u\n", UIP_LLH_LEN);
  PRINTF("IP+UDP header: %u\n", UIP_IPUDPH_LEN);
  PRINTF("REST max chunk: %u\n", REST_MAX_CHUNK_SIZE);

  /* Initialize the REST engine. */
  rest_init_engine();

  /*
   * Bind the resources to their Uri-Path.
   * WARNING: Activating twice only means alternate path, not two instances!
   * All static variables are the same for each URI path.
   */
//  rest_activate_resource(&res_hello, "test/hello");
//  rest_activate_resource(&res_occupy, "test/occupy");
/*  rest_activate_resource(&res_mirror, "debug/mirror"); */
/*  rest_activate_resource(&res_chunks, "test/chunks"); */
/*  rest_activate_resource(&res_separate, "test/separate"); */
//  rest_activate_resource(&res_push, "test/push");
/*  rest_activate_resource(&res_event, "sensors/button"); */
/*  rest_activate_resource(&res_sub, "test/sub"); */
/*  rest_activate_resource(&res_b1_sep_b2, "test/b1sepb2"); */
#if PLATFORM_HAS_LEDS
  rest_activate_resource(&res_leds, "actuators/leds");
  rest_activate_resource(&res_status, "status/leds");
//  rest_activate_resource(&res_dimmed, "dimmed/leds");
//  rest_activate_resource(&res_toggle, "actuators/toggle");
#endif
#if PLATFORM_HAS_LIGHT
  rest_activate_resource(&res_light, "sensors/light"); 
  rest_activate_resource(&res_human, "sensors/human");
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
#endif

//#if PLATFORM_HAS_BATTERY
//  rest_activate_resource(&res_battery, "sensors/battery");  
//  SENSORS_ACTIVATE(battery_sensor);  
//#endif
//#if PLATFORM_HAS_RADIO
//  rest_activate_resource(&res_radio, "sensors/radio");  
//  SENSORS_ACTIVATE(radio_sensor);  
//#endif
//#if PLATFORM_HAS_SHT11
//  rest_activate_resource(&res_sht11, "sensors/sht11");  
//  SENSORS_ACTIVATE(sht11_sensor);  
//#endif


  /* Define application-specific events here. */
  while(1) {
    PROCESS_WAIT_EVENT();
#if PLATFORM_HAS_BUTTON
    if(ev == sensors_event && data == &button_sensor) {
      PRINTF("*******BUTTON*******\n");

      /* Call the event_handler for this application-specific event. */
      res_event.trigger();

      /* Also call the separate response example handler. */
      res_separate.resume();
    }
#endif /* PLATFORM_HAS_BUTTON */
  }                             /* while (1) */

  PROCESS_END();
}
