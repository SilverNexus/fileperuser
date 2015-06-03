/***************************************************************************/
/*                                                                         */
/*                               ErrorLog.c                                */
/* Original code written by Daniel Hawkins. Last modified on 2015-06-03.   */
/*                                                                         */
/* The file defines the functions for logging errors.                      */
/*                                                                         */
/***************************************************************************/

#include "ErrorLog.h"
#include "settings.h"
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <stdlib.h>

const char *ERROR_TYPE_CHARS[] = {"DEBUG", "INFO", "WARNING", "ERROR", "FATAL"};
const char *MONTH[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

/**
 * Logs a message that is printed and, if urgent enough, printed to the log file.
 *
 * @param err The urgency of the message. Referred to as the log level in most cases.
 *
 * @param msg The message to be written
 *
 * @retval 0 Log operation successful.
 *
 * @retval 1 Could not open the log file.
 *
 * @warning If errorType err is FATAL, will exit the program.
 */
void log_event(enum errorType err, const char *msg, ...){
       // If message is too low of importance to print or log, just skip it all
       if (err < settings.min_log_level && err < settings.min_print_level)
           return;
       char fnMsg[10000];
       va_list ap;
       va_start(ap, msg);
       vsnprintf(fnMsg, sizeof(fnMsg), msg, ap);
       va_end(ap);
       if (err >= settings.min_print_level){
           // Print the message
           fprintf(stderr, "%s: %s\n", ERROR_TYPE_CHARS[err], fnMsg);
       }
       // If major enough, log to the file
       if (err >= settings.min_log_level){
           time_t theTime = time(0);
           struct tm *date = localtime(&theTime);
           // Create output file stream object
           FILE *ErrorFile = fopen(settings.log_file, "a");
           if (ErrorFile)
                fprintf(ErrorFile, "%2i %s %4i %02i:%02i:%02i: %s: %s\n", date->tm_mday, MONTH[date->tm_mon], date->tm_year+1900, date->tm_hour, date->tm_min, date->tm_sec, ERROR_TYPE_CHARS[err], fnMsg);
           else{
                fputs("ERROR: Could not open log file. Errors will not be logged.\n", stderr);
                return;
           }
           fclose(ErrorFile);
       }
       if (err == FATAL)
           exit(EXIT_FAILURE);
       return;
}
