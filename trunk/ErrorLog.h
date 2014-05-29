/***************************************************************************/
/*                                                                         */
/*                               ErrorLog.h                                */
/* Original code written by Daniel Hawkins. Last modified on 2014-05-25.   */
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
 * DEBUG - debugging output   *
 * INFO - general info        *
 *   probably for use with    *
 *   debugging                *
 * WARNING - issues that may  *
 *   interfere with normal    *
 *   operation of the program *
 * ERROR - files not loading, *
 *   or other major problems  *
 * FATAL - an error that can  *
 *   not be readily recovered *
 *   from, so the program     *
 *   should exit afterward.   *
 *                            *
 ******************************/
enum errorType{DEBUG, INFO, WARNING, ERROR, FATAL};

int logError(enum errorType, const char *, ...);
#endif