/* Concatenate variable number of strings.
   Copyright (C) 1991, 1994, 2001 Free Software Foundation, Inc.
   Written by Fred Fish @ Cygnus Support

This file is part of the libiberty library.
Libiberty is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

Libiberty is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with libiberty; see the file COPYING.LIB.  If
not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#include <sys/types.h>		/* size_t */
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#define VA_START(VA_LIST, VAR)	va_start(VA_LIST, VAR)

#define VA_OPEN(AP, VAR)	{ va_list AP; va_start(AP, VAR); { struct Qdmy
#define VA_CLOSE(AP)		} va_end(AP); }
#define VA_FIXEDARG(AP, T, N)	struct Qdmy

char * concat(const char *first, ...) {
  register size_t length;
  register char *newstr;
  register char *end;
  register const char *arg;

  /* First compute the size of the result and get sufficient memory.  */
  VA_OPEN (args, first);
  VA_FIXEDARG (args, const char *, first);
  
  length = 0;
  for (arg = first; arg ; arg = va_arg (args, const char *))
    length += strlen (arg);

  VA_CLOSE (args);

  newstr = (char *)malloc (length + 1);

  /* Now copy the individual pieces to the result string. */
  VA_OPEN (args, first);
  VA_FIXEDARG (args, const char *, first);

  end = newstr;
  for (arg = first; arg ; arg = va_arg (args, const char *))
    {
      length = strlen (arg);
      memcpy (end, arg, length);
      end += length;
    }
  *end = '\0';
  VA_CLOSE (args);

  return newstr;
}

size_t concat_len(const char *first, ...) {
  register size_t length;
  register char *newstr;
  register char *end;
  register const char *arg;

  /* First compute the size of the result and get sufficient memory.  */
  VA_OPEN (args, first);
  VA_FIXEDARG (args, const char *, first);
  
  length = 0;
  for (arg = first; arg ; arg = va_arg (args, const char *))
    length += strlen (arg);

  VA_CLOSE (args);

  return length;
}
