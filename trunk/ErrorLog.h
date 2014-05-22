/***************************************************************************/
/*                                                                         */
/*                               ErrorLog.h                                */
/* Original code written by Daniel Hawkins. Last modified on 2014-05-19.   */
/*                                                                         */
/* The file defines the enumeration for the type of error message, and     */
/* has the function prototype for the errorlogging function.               */
/*                                                                         */
/***************************************************************************/

#ifndef ERROR_H
#define ERROR_H
/******************************
 *                            *
 * Uses of the enumeration:   *
 *                            *
 * BUG - a path in the code   *
 *   that should never, or    *
 *   the program otherwise    *
 *   malfunctioning           *
 * DEBUG - debugging output   *
 * INFO - general info        *
 *   probably for use with    *
 *   debugging                *
 * ERROR - files not loading, *
 *   or other major problems  *
 *                            *
 ******************************/
enum errorType{BUG, DEBUG, INFO, ERROR};

int logError(enum errorType, const char *, ...);
#endif
