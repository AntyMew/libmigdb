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
  Example/test of pseudo terminals.
  Note: Contributed by Greg Watson (gwatson lanl gov)

***************************************************************************/

#include <stdio.h>
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

void cb_async(mi_output *o, void *data)
{
 printf("ASYNC\n");
}

int main(int argc, char *argv[])
{
 mi_pty *pty=NULL;
 mi_h *h;
 fd_set rfds;
 char buf[BUFSIZ];

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

 /* Look for a free pseudo terminal. */
 pty=gmi_look_for_free_pty();
 if (!pty) 
   {
    printf("Error opening pseudo terminal.\n");
    return 1;
   }

 printf("Free pty slave = %s, master on %d\n",pty->slave,pty->master);

 /* Tell gdb to attach the terminal. */
 if (!gmi_target_terminal(h,pty->slave))
   {
    printf("Error selecting target terminal\n");
    mi_disconnect(h);
    return 1;
   }

 /* Set the name of the child and the command line aguments. */
 if (!gmi_set_exec(h,"./test_target","prb1 2 prb3"))
   {
    printf("Error setting exec y args\n");
    mi_disconnect(h);
    return 1;
   }

 /* Run the program. */
 if (!gmi_exec_run(h))
   {
    printf("Error in run!\n");
    mi_disconnect(h);
    return 1;
   }

 for (;;)
   {
    FD_ZERO(&rfds);
    FD_SET(pty->master,&rfds);
    FD_SET(0,&rfds);
    
    if (select(pty->master+1,&rfds,NULL,NULL,NULL)<0)
      {
       perror("select");
       mi_disconnect(h);
       return 1;
      }

    if (FD_ISSET(pty->master,&rfds))
      {
       int n=read(pty->master,buf,BUFSIZ);
       if (n<=0)
          break;
       write(1,buf,n);
      }

    if (FD_ISSET(0,&rfds))
      {
       int n=read(0,buf,BUFSIZ);
       if (n<=0)
          break;
       write(pty->master,buf,n);
      }
   }

 /* Exit from gdb. */
 gmi_gdb_exit(h);
 /* Close the connection. */
 mi_disconnect(h);
 gmi_end_pty(pty);

 return 0;
}
