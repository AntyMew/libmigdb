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

int wait_for_stop(MIDebugger &d)
{
 int res=1;
 mi_stop *sr;

 while (!d.Poll(sr))
    usleep(1000);
 /* The end of the async. */
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

int DoTryRun(int res, MIDebugger &d)
{
 if (!res)
   {
    printf("Error in executing!\n");
    return 0;
   }
 if (!wait_for_stop(d))
    return 0;
 return 1;
}

#define TryRun(a,b) if (!DoTryRun(a,b)) return 1

int main(int argc, char *argv[])
{
 mi_bkpt *bk;
 mi_wp *wp;

 // Debugging object, used as an auto var.
 MIDebugger d;

 // You can use any gdb you want:
 //MIDebugger::SetGDBExe("/usr/src/gdb-6.1.1/gdb/gdb");
 // You can use a terminal different than xterm:
 //MIDebugger::SetXTerm("/usr/bin/Eterm");
 // You can specify commands for gdb
 MIDebugger::SetGDBStartFile("cmds.txt");

 // Connect to gdb child.
 if (!d.Connect())
   {
    printf("Connect failed\n");
    return 1;
   }
 printf("Connected to gdb!\n");

 /* Set all callbacks. */
 d.SetConsoleCB(cb_console);
 d.SetTargetCB(cb_target);
 d.SetLogCB(cb_log);
 d.SetAsyncCB(cb_async);
 d.SetToGDBCB(cb_to);
 d.SetFromGDBCB(cb_from);

 // Set the name of the child and the command line aguments.
 // It also opens the xterm.
 if (!d.SelectTargetX11("./test_target","prb1 2 prb3"))
   {
    printf("Error setting exec y args\n");
    return 1;
   }

 /* Set a breakpoint. */
 bk=d.Breakpoint("test_target.cc",15);
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
 wp=d.Watchpoint(wm_write,"v");
 if (!wp)
   {
    printf("Error al setting watchpoint\n");
    return 1;
   }
 printf("Watchpoint %d for expression: %s\n",wp->number,wp->exp);
 mi_free_wp(wp);

 TryRun(d.StepOver(),d);
 /* Run the program. */
 TryRun(d.RunOrContinue(),d);
 /* Here we should be stopped at the breakpoint. */

 /* Continue execution. */
 TryRun(d.RunOrContinue(),d);
 /* Here we should be terminated. */

 /* Wait 5 seconds and close the auxiliar terminal. */
 printf("Waiting 5 seconds\n");
 sleep(5);

 return 0;
}
