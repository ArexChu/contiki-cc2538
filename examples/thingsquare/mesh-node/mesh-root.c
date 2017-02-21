#include "thsq.h"

/*---------------------------------------------------------------------------*/
PROCESS(mesh_root_process, "Mesh root");
AUTOSTART_PROCESSES(&mesh_root_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(mesh_root_process, ev, data)
{
  PROCESS_BEGIN();

  /* Set us up as a RPL root root. */
  simple_rpl_init_dag();

  while(1) {
    PROCESS_WAIT_EVENT();
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
