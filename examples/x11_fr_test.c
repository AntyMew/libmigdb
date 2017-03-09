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

void print_frames(mi_frames *f, int free_f)
{
 mi_frames *ff=f;

 if (!f)
   {
    printf("Error! empty frames info\n");
    return;
   }
 while (f)
   {
    printf("Level %d, addr %p, func %s, where: %s:%d args? %c\n",f->level,f->addr,
           f->func,f->file,f->line,f->args ? 'y' : 'n');
    f=f->next;
   }
 if (free_f)
    mi_free_frames(ff);
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
    print_frames(sr->frame,0);
    mi_free_stop(sr);
   }
 else
   {
    printf("Error while waiting\n");
    res=0;
   }
 return res;
}

void print_gvar(mi_gvar *v)
{
 if (!v)
   {
    printf("Error! failed to define variable\n");
    return;
   }
 printf("Variable name: '%s', type: '%s', number of children: %d format: %s expression: %s lang: %s editable: %c\n",
        v->name,v->type,v->numchild,mi_format_enum_to_str(v->format),
        v->exp,mi_lang_enum_to_str(v->lang),v->attr & MI_ATTR_EDITABLE ? 'y' : 'n');
}

void print_update(mi_gvar_chg *changed)
{
 printf("List of changed variables:\n");
 while (changed)
   {
    printf("Name: %s\nIn scope: %c\n",changed->name,changed->in_scope ? 'y' : 'n');
    if (changed->in_scope && changed->new_type)
      {
       printf("New type: %s\nNew num children: %d\n",changed->new_type,changed->new_num_children);
      }
    changed=changed->next;
    printf("\n");
   }
}

void print_children(mi_gvar *ch)
{
 int i;
 mi_gvar *s;

 if (!ch->child)
   {
    printf("Error! getting children list\n");
    return;
   }
 printf("\nChildren List (%d):\n",ch->numchild);
 for (i=0, s=ch->child; i<ch->numchild; s++, i++)
    {
     printf("Name: %s Exp: %s Children: %d",s->name,s->exp,s->numchild);
     if (s->type)
        printf(" Type: %s",s->type);
     if (s->value)
        printf(" Value: %s",s->value);
     printf("\n");
    }
 printf("\n");
}


int main(int argc, char *argv[])
{
 mi_aux_term *xterm_tty=NULL;
 mi_bkpt *bk;
 mi_frames *fr;
 /* This is like a file-handle for fopen.
    Here we have all the state of gdb "connection". */
 mi_h *h;
 mi_gvar *gv, *gv2;
 mi_gvar_chg *changed;
 char *value;
 int r_assign;

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
 if (!gmi_set_exec(h,"./target_frames",""))
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
 bk=gmi_break_insert(h,"target_frames.cc",7);
 if (!bk)
   {
    printf("Error setting breakpoint\n");
    mi_disconnect(h);
    return 1;
   }
 printf("Breakpoint %d @ function: %s\n",bk->number,bk->func);

 /* If we no longer need this data we can release it. */
 mi_free_bkpt(bk);

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

 /* Get information about the calling stack. */
 fr=gmi_stack_list_frames(h);
 printf("\nCalling stack:\n\n");
 print_frames(fr,1);
 printf("\n");
 fr=gmi_stack_info_frame(h);
 printf("\nCurrent frame:\n\n");
 print_frames(fr,1);
 printf("\n");
 printf("Stack depth: %d\n",gmi_stack_info_depth_get(h));
 gmi_stack_select_frame(h,1);
 fr=gmi_stack_info_frame(h);
 printf("\nFrame 1:\n\n");
 print_frames(fr,1);
 printf("\n");

 if (0)
   {
    gv=gmi_var_create(h,-1,"v");
    print_gvar(gv);
    gmi_var_show_format(h,gv);
    print_gvar(gv);
    gmi_var_info_num_children(h,gv);
    print_gvar(gv);
    gmi_var_info_type(h,gv);
    print_gvar(gv);
    gmi_var_info_expression(h,gv);
    print_gvar(gv);
    gmi_var_show_attributes(h,gv);
    print_gvar(gv);
   }
 else
   {
    gv=gmi_full_var_create(h,-1,"v");
    print_gvar(gv);
   }
 gv2=gmi_full_var_create(h,-1,"sup");
 print_gvar(gv2);
 gmi_var_set_format(h,gv,fm_hexadecimal);
 print_gvar(gv);

 /* Continue execution. */
 if (!gmi_exec_until(h,"target_frames.cc",21))
   {
    printf("Error in exec until!\n");
    mi_disconnect(h);
    return 1;
   }
 if (!wait_for_stop(h))
   {
    mi_disconnect(h);
    return 1;
   }

 gmi_var_update(h,NULL,&changed);
 print_update(changed);
 mi_free_gvar_chg(changed);

 r_assign=gmi_var_assign(h,gv,"i+5");
 if (r_assign && gv->value)
    printf("\nAssigned v=%s\n\n",gv->value);

 gmi_var_list_children(h,gv2);
 print_children(gv2);

 gmi_var_evaluate_expression(h,gv2);
 printf("\n%s = %s\n\n",gv2->exp,gv2->value);

 value=gmi_data_evaluate_expression(h,gv2->exp);
 printf("\n%s = %s\n\n",gv2->exp,value);
 free(value);

 gmi_var_delete(h,gv);
 mi_free_gvar(gv);
 gmi_var_delete(h,gv2);
 mi_free_gvar(gv2);

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
