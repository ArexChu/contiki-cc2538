#include "contiki.h"

/*---------------------------------------------------------------------------*/

PROCESS_NAME(demo_6lbr_process);
PROCESS_NAME(er_example_server);

AUTOSTART_PROCESSES(&demo_6lbr_process, &er_example_server);

/*---------------------------------------------------------------------------*/
