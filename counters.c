
/* counters.c - Routines to access TeX variables that contain TeX counters

Copyright (C) 2001-2002 The Free Software Foundation

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

This file is available from http://sourceforge.net/projects/latex2rtf/
*/

#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "utils.h"
#include "counters.h"
#include "definitions.h"

#define MAX_COUNTERS 500

typedef struct {
    char *name;
    int number;
    char *parent;  /*added for parent support*/
} counter_type;

counter_type Counters[MAX_COUNTERS];

static int iCounterCount = 0;

static int existsCounter(char *s)

/**************************************************************************
     purpose: checks to see if a named TeX counter exists
     returns: the array index of the named TeX counter
**************************************************************************/
{
    int i = 0;
  
    while (i < iCounterCount && strcmp(Counters[i].name, s) != 0)
        i++;

    if (i == iCounterCount)
        return -1;
    else
        return i;
}

static int nextChild(char *s, int i)

/**************************************************************************
     purpose: checks to see if a named TeX counter exists
     returns: the array index of the named TeX counter
**************************************************************************/
{
    if (s == NULL)
      return -1;
  
    while (i < iCounterCount) {
      if (Counters[i].parent != NULL)
	if (strcmp(Counters[i].parent, s) == 0)
	  break;
      i++;
    }

    if (i < iCounterCount)
        return i;
    else
        return -1;
}

static void newCounter(char *s, int n, char* p)

/**************************************************************************
     purpose: allocates and initializes a named TeX counter 
**************************************************************************/
{
    if (iCounterCount == MAX_COUNTERS) {
        diagnostics(WARNING, "Too many counters, ignoring %s", s);
        return;
    }

    Counters[iCounterCount].number = n;
    Counters[iCounterCount].name = strdup(s);
    if (p != NULL)
      Counters[iCounterCount].parent = strdup(p);
    else
      Counters[iCounterCount].parent = NULL;

    if (Counters[iCounterCount].name == NULL) {
        diagnostics(WARNING, "\nCannot allocate name for counter \\%s\n", s);
        exit(1);
    }

    iCounterCount++;
    
    /* provide standard \thecounter command */
    char *cmd, arg[50];
    cmd = strdup_together("the", s);
    if (p)
        snprintf(arg, 50, "\\the%s.\\arabic{%s}", p, s);
    else
        snprintf(arg, 50, "\\arabic{%s}", s);
    
    newDefinition(cmd, NULL, arg, 0);
    diagnostics(5, "thecounter <\\%s>, defined as <%s>", cmd, arg);
    free(cmd);
}

void incrementCounter(char *s)

/**************************************************************************
     purpose: increments a TeX counter (or initializes to 1) 
**************************************************************************/
{
    int i, c;
    
    i = existsCounter(s);
    
    if (i < 0)
        newCounter(s, 1, NULL);
    else {
        Counters[i].number++;
	c = nextChild(s, 0);
	diagnostics(3, "Resetting children for counter %s ....", s);
	/* reset to zero all children */
	while (c != -1) {
	   diagnostics(3, "Zeroing counter %s", Counters[c].name);
	   Counters[c].number = 0;
	   c = nextChild(s, c+1);
	}
	diagnostics(3, "Done for counter %s", s);
    }
}

void setCounter(char *s, int n)
{
    return setCounterParent(s, n, NULL);
}

void setCounterParent(char *s, int n, char *p)

/**************************************************************************
     purpose: allocates (if necessary) and sets a named TeX counter 
**************************************************************************/
{
    int i = existsCounter(s);
    if (i < 0)
        newCounter(s, n, p);
    else
        Counters[i].number = n;
}


int getCounter(char *s)

/**************************************************************************
     purpose: retrieves a named TeX counter 
**************************************************************************/
{
    int i;

    i = existsCounter(s);

    if (i < 0) {
        diagnostics(2, "No counter of type <%s>", s);
        return 0;
    }

    return Counters[i].number;
}

void zeroKeyCounters(char *key)

/**************************************************************************
     purpose: zeros all the acronym counters
**************************************************************************/
{    
    int i;
    for (i=0; i < iCounterCount; i++) {
        if (strstr(Counters[i].name,key)!=Counters[i].name)
            Counters[i].number = 0;
    }
}
