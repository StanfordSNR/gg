/* Output to stdout / stderr for GNU make
Copyright (C) 2013-2016 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.  */

struct output
  {
    int out;
    int err;
    unsigned int syncout:1;     /* True if we want to synchronize output.  */
 };

extern struct output *output_context;
extern unsigned int stdio_traced;

#define OUTPUT_SET(_new)    do{ output_context = (_new)->syncout ? (_new) : NULL; }while(0)
#define OUTPUT_UNSET()      do{ output_context = NULL; }while(0)

#define OUTPUT_TRACED()     do{ stdio_traced = 1; }while(0)
#define OUTPUT_IS_TRACED()  (!!stdio_traced)

FILE *output_tmpfile (char **, const char *);

/* Initialize and close a child output structure: if NULL do this program's
   output (this should only be done once).  */
void output_init (struct output *out);
void output_close (struct output *out);

/* In situations where output may be about to be displayed but we're not
   sure if we've set it up yet, call this.  */
void output_start (void);

/* Show a message on stdout or stderr.  Will start the output if needed.  */
void outputs (int is_err, const char *msg);

#ifndef NO_OUTPUT_SYNC
int output_tmpfd (void);
/* Dump any child output content to stdout, and reset it.  */
void output_dump (struct output *out);
#endif
