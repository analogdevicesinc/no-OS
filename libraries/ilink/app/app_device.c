#include <stdio.h>
#include "osal.h"
#include "osal_log.h"
#include "iolink.h"
#include "app_handler.h"
#include "app_smi.h"
#include "app_device.h"

static void app_generic_run (app_port_ctx_t * app_port)
{
   uint8_t pdata[IOLINK_PD_MAX_SIZE];
   bool valid = false;
   int8_t len;
   int i;

   len = app_smi_pdin (app_port, &valid, pdata);
   if (len <= 0)
   {
      return;
   }

   if (!valid)
   {
      LOG_WARNING (
         LOG_STATE_ON,
         "Port %u: PD invalid\n",
         app_port->portnumber);
      return;
   }

   printf ("Port %u PD [%d]: ", app_port->portnumber, len);
   for (i = 0; i < len; i++)
   {
      printf ("%02X ", pdata[i]);
   }
   printf ("\n");
}

void app_generic_setup (app_port_ctx_t * app_port)
{
   LOG_INFO (
      LOG_STATE_ON,
      "Port %u: Generic device setup\n",
      app_port->portnumber);

   app_port->type           = GENERIC;
   app_port->run_function   = app_generic_run;
   app_port->app_port_state = IOL_STATE_RUNNING;
}
