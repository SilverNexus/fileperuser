/***************************************************************************/
/*                                                                         */
/*                              parseArgs.h                                */
/* Original code written by Daniel Hawkins. Last modified on 2014-06-03.   */
/*                                                                         */
/* The file defines the argument parsing function prototypes.              */
/*                                                                         */
/***************************************************************************/

#ifndef PARSEARGS_H
#define PARSEARGS_H

// TODO: Should these be reversed for consistancy with main?
int parseArgs(char **, int);

/* The help message should be its own function, so it can be called as necessary */
void help_message();

#endif
