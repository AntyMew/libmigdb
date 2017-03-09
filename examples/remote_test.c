/**[txh]********************************************************************

  GDB/MI interface library
  Copyright (c) 2004 by Salvador E. Tropea.
 
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Comment:
  Remote example/test of the libmigdb.
  To run it you must:

1) Copy "test_target" to the "remote" host (it could be the same machine if
you want).
2) In the "remote" end run "gdbserver :6000 ./test_target 1 2 3"
3) Run this example in the "local" end.

You should edit the remote IP and port before compiling.
  
***************************************************************************/

#include <stdio.h>
#include <unistd.h> //usleep
#include "mi_gdb.h"

// Example: "192.168.1.3:5500"
#define REMOTE_MACHINE ":6000"

void cb_console(const char *str, void *data)
{
 printf("CONSOLE> %s\n",str);
}

/* Note that unlike what's documented in gdb docs it isn't usable. */
void cb_target(const char *str, void *data)
{
 printf("TARGET> %s\n",str);
}

void cb_log(const char *str, void *data)
{
 printf("LOG> %s\n",str);
}

void cb_to(const char *str, void *data)
{
 printf(">> %s",str);
}

void cb_from(const char *str, void *data)
{
 printf("<< %s\n",str);
}

volatile int async_c=0;

void cb_async(mi_output *o, void *data)
{
 printf("ASYNC\n");
 async_c++;
}

int wait_for_stop(mi_h *h)
{
 mi_output *o;
 int res=1;
 char *aux;

 while (!mi_get_response(h))
    usleep(1000);
 /* The end of the async. */
 o=mi_retire_response(h);
 if (mi_get_async_stop_reason(o,&aux))
    printf("Stopped, reason: %s\n",aux);
 else
   {
    printf("Error while waiting: %s\n",aux);
    res=0;
   }
 mi_free_output(o);
 return res;
}

int main(int argc, char *argv[])
{
 mi_bkpt *bk;
 /* This is like a file-handle for fopen.
    Here we have all the state of gdb "connection". */
 mi_h *h;

 /* You can use any gdb you want: */
 /*mi_set_gdb_exe("/usr/src/gdb-6.1.1/gdb/gdb");*/
 /* You can use a terminal different than xterm: */
 /*mi_set_xterm_exe("/usr/bin/Eterm");*/

 /* Connect to gdb child. */
 h=mi_connect_local();
 if (!h)
   {
    printf("Connect failed\n");
    return 1;
   }
 printf("Connected to gdb!\n");

 /* Set all callbacks. */
 mi_set_console_cb(h,cb_console,NULL);
 mi_set_target_cb(h,cb_target,NULL);
 mi_set_log_cb(h,cb_log,NULL);
 mi_set_async_cb(h,cb_async,NULL);
 mi_set_to_gdb_cb(h,cb_to,NULL);
 mi_set_from_gdb_cb(h,cb_from,NULL);

 /* Tell gdb to load symbols from the local copy. */
 if (!gmi_file_symbol_file(h,"./test_target"))
   {
    printf("Error loading symbols\n");
    mi_disconnect(h);
    return 1;
   }

 /* Connect to remote machine using TCP/IP. */
 if (!gmi_target_select(h,"extended-remote",REMOTE_MACHINE))
   {
    printf("Error connecting to gdb server\n");
    mi_disconnect(h);
    return 1;
   }

 /* Set a breakpoint. */
 bk=gmi_break_insert(h,"test_target.cc",12);
 if (!bk)
   {
    printf("Error setting breakpoint\n");
    mi_disconnect(h);
    return 1;
   }
 printf("Breakpoint %d @ function: %s\n",bk->number,bk->func);

 /* You can do things like:
 gmi_break_delete(h,bk->number);
 gmi_break_set_times(h,bk->number,2);
 gmi_break_set_condition(h,bk->number,"1");
 gmi_break_state(h,bk->number,0);*/
 /* If we no longer need this data we can release it. */
 mi_free_bkpt(bk);

 /* Run the program. */
 /* Note that remote targets starts running and we must use continue! */
 if (!gmi_exec_continue(h))
   {
    printf("Error in continue!\n");
    mi_disconnect(h);
    return 1;
   }
 /* Here we should be stopped at the breakpoint. */
 if (!wait_for_stop(h))
   {
    mi_disconnect(h);
    return 1;
   }

 /* Continue execution. */
 if (!gmi_exec_continue(h))
   {
    printf("Error in continue!\n");
    mi_disconnect(h);
    return 1;
   }
 /* Here we should be terminated. */
 if (!wait_for_stop(h))
   {
    mi_disconnect(h);
    return 1;
   }

 /* Exit from gdb. */
 gmi_gdb_exit(h);
 /* Close the connection. */
 mi_disconnect(h);

 return 0;
}
