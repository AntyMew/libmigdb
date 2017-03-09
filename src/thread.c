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

  Module: Thread commands.
  Comments:
  GDB/MI commands for the "Thread Commands" section.@p

@<pre>
gdb command:              Implemented?
-thread-info              N.A.
-thread-list-all-threads  Yes, implemented as "info threads"
-thread-list-ids          Yes
-thread-select            Yes
@</pre>

***************************************************************************/

#include "mi_gdb.h"

/* Low level versions. */

void mi_thread_list_ids(mi_h *h)
{
 mi_send(h,"-thread-list-ids\n");
}

void mi_thread_select(mi_h *h, int id)
{
 mi_send(h,"-thread-select %d\n",id);
}

void mi_thread_list_all_threads(mi_h *h)
{
 mi_send(h,"info threads\n");
}

/* High level versions. */

/**[txh]********************************************************************

  Description:
  List available thread ids.

  Command: -thread-list-ids
  Return: !=0 OK
  
***************************************************************************/

int gmi_thread_list_ids(mi_h *h, int **list)
{
 mi_thread_list_ids(h);
 return mi_res_thread_ids(h,list);
}

/**[txh]********************************************************************

  Description:
  Select a thread.

  Command: -thread-select
  Return: A new mi_frames or NULL on error.
  
***************************************************************************/

mi_frames *gmi_thread_select(mi_h *h, int id)
{
 mi_thread_select(h,id);
 return mi_res_frame(h);
}

/**[txh]********************************************************************

  Description:
  Get a list of frames for each available thread. Implemented using "info
thread".

  Command: -thread-list-all-threads
  Return: A kist of frames, NULL on error
  
***************************************************************************/

mi_frames *gmi_thread_list_all_threads(mi_h *h)
{
 mi_thread_list_all_threads(h);
 return mi_res_frames_list(h);
}

