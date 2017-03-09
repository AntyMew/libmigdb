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
  X11 example/test of the libmigdb.
  Run it from an X11 terminal (xterm, Eterm, etc.).
  
***************************************************************************/

#include <stdio.h>
#include <unistd.h> //usleep
#include "mi_gdb.h"

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
 int res=1;
 mi_stop *sr;

 while (!mi_get_response(h))
    usleep(1000);
 /* The end of the async. */
 sr=mi_res_stop(h);
 if (sr)
   {
    printf("Stopped, reason: %s\n",mi_reason_enum_to_str(sr->reason));
    mi_free_stop(sr);
   }
 else
   {
    printf("Error while waiting\n");
    printf("mi_error: %d\nmi_error_from_gdb: %s\n",mi_error,mi_error_from_gdb);
    res=0;
   }
 return res;
}

int main(int argc, char *argv[])
{
 mi_aux_term *xterm_tty=NULL;
 mi_bkpt *bk;
 mi_wp *wp;
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

 /* Set the name of the child and the command line aguments. */
 if (!gmi_set_exec(h,"./test_target","prb1 2 prb3"))
   {
    printf("Error setting exec y args\n");
    mi_disconnect(h);
    return 1;
   }

 /* Open an xterm to be used as terminal for the debuggy. */
 xterm_tty=gmi_start_xterm();
 if (!xterm_tty)
    printf("Error opening auxiliar terminal, we'll use current one.\n");
 else
    printf("XTerm opened @ %s\n",xterm_tty->tty);

 /* Tell gdb to attach the child to a terminal. */
 if (!gmi_target_terminal(h,xterm_tty ? xterm_tty->tty : ttyname(STDIN_FILENO)))
   {
    printf("Error selecting target terminal\n");
    mi_disconnect(h);
    return 1;
   }

 /* Set a breakpoint. */
 bk=gmi_break_insert(h,"test_target.cc",12);
 if (!bk)
   {
    printf("Error setting breakpoint\n");
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

 /* Set a watchpoint, that's a data breakpoint. */
 wp=gmi_break_watch(h,wm_write,"v");
 if (!wp)
   {
    printf("Error al setting watchpoint\n");
    return 1;
   }
 printf("Watchpoint %d for expression: %s\n",wp->number,wp->exp);
 mi_free_wp(wp);

 /* Run the program. */
 if (!gmi_exec_run(h))
   {
    printf("Error in run!\n");
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
 /* Wait 5 seconds and close the auxiliar terminal. */
 printf("Waiting 5 seconds\n");
 sleep(5);
 gmi_end_aux_term(xterm_tty);

 return 0;
}
