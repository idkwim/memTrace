/**
 * @file lib_tls.c
 *
 * Copyright (c) 2011 ETH Zurich
 * @author Tobias Hartmann <tobi.hartmann@gmail.com>
 * @author Mathias Payer <mathias.payer@nebelwelt.net>
 *
 * $Date: 2011-12-30 14:24:05 +0100 (ven, 30 dic 2011) $
 * $LastChangedDate: 2011-12-30 14:24:05 +0100 (ven, 30 dic 2011) $
 * $LastChangedBy: payerm $
 * $Revision: 1134 $
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 */
/**
 *  Tests the Initial Executable (IE) and General Dynamic (GD) access models.
 *  
 *  We do not need to test Local Dynamic (LD) because this produces just a
 *  R_386_TLS_DTPMOD32 relocation which is tested with the GD model. Also we
 *  do not need to test the Local Executable (LE) model, because this is
 *  resolved by static linking.
 */

#include <stdio.h>
#include <string.h>

__thread int i_init=66;
__thread int i_init2=23;
__thread int i;
static __thread int static_1 = 123;
static __thread int *pointer;

void _init() {
  int *ip1, *ip2;
  int p = 42;
  pointer = &p;

  if (*pointer != 42)
    return;
  
  /* This adds a R_386_TLS_DTPMOD32 without symbol information */
  if (static_1 != 123)
    return;

  /* This adds a R_386_TLS_TPOFF without symbol information */
  asm ("movl %%gs:0,%0;"
       "addl static_2@gotntpoff(%%ebx),%0;"
       : "=r" (ip1));
  /* This adds a R_386_TLS_TPOFF32 without symbol information */
  asm ("movl %%gs:0,%0;"
       "subl static_3@gottpoff(%%ebx),%0;"
       : "=r" (ip2));  
  if (*ip1 != 456 || *ip2 != 789)
    return;

  
  /* General Dynamic (GD) */ 
  /* This adds R_386_TLS_DTPMOD32 and R_386_TLS_DTPOFF32 relocations */
  if (i_init2==23)
    i = 42;
}


int get_data () {
  int *ip1, *ip2;

  /* Initial Executable (LE) */
  /* This adds a R_386_TLS_TPOFF32 relocation */
  asm ("movl %%gs:0,%0;"
       "subl i_init@gottpoff(%%ebx),%0;"
       : "=r" (ip1));  
  /* This adds a R_386_TLS_TPOFF relocation */
  asm ("movl %%gs:0,%0;"
       "addl i_init@gotntpoff(%%ebx),%0;"
       : "=r" (ip2));
  
  if (*ip1 == 66 && *ip2 == 66)
    return i;
  else
    return 0;
}
