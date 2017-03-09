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

  Module: Symbol query.
  Comments:
  GDB/MI commands for the "Symbol Query" section.@p

@<pre>
gdb command:              Implemented?
-symbol-info-address      N.A. (info address, human readable)
-symbol-info-file         N.A.
-symbol-info-function     N.A.
-symbol-info-line         N.A. (info line, human readable)
-symbol-info-symbol       N.A. (info symbol, human readable)
-symbol-list-functions    N.A. (info functions, human readable)
-symbol-list-types        N.A. (info types, human readable)
-symbol-list-variables    N.A. (info variables, human readable)
-symbol-list-lines        No (gdb 6.x)
-symbol-locate            N.A.
-symbol-type              N.A. (ptype, human readable)
@</pre>

Note:@p

Only one is implemented and not in gdb 5.x.@p

***************************************************************************/

#include "mi_gdb.h"

