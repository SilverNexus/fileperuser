/***************************************************************************/
/*                                                                         */
/*                               ErrorLog.c                                */
/* Original code written by Daniel Hawkins. Last modified on 2014-05-21.   */
/*                                                                         */
/* The file defines the functions for logging errors.                      */
/*                                                                         */
/***************************************************************************/

#include "ErrorLog.h"
#include <stdio.h>
#include <time.h>
#include <stdarg.h>

const char *ERROR_TYPE_CHARS[4] = {"BUG", "DEBUG", "INFO", "ERROR"};
const char *MONTH[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

int logError(enum errorType err, const char *msg, ...){
       char fnMsg[10000];
       va_list ap;
       va_start(ap, msg);
       vsnprintf(fnMsg, sizeof(fnMsg), msg, ap);
       va_end(ap);
       // Print the message
       fprintf(stderr, "%s: %s\n", ERROR_TYPE_CHARS[err], fnMsg);
       time_t theTime = time(0);
       struct tm *date = localtime(&theTime);
       // Create output file stream object
       FILE *ErrorFile;
       if (err == INFO)
            ErrorFile = fopen("InfoLog.txt", "a");
       else
            ErrorFile = fopen("ErrorLog.txt", "a");
       if (ErrorFile)
            fprintf(ErrorFile, "%2i %s %4i %02i:%02i:%02i: %s: %s\n", date->tm_mday, MONTH[date->tm_mon], date->tm_year+1900, date->tm_hour, date->tm_min, date->tm_sec, ERROR_TYPE_CHARS[err], fnMsg);
       else{
            puts("ERROR: Could not open log file. Errors will not be logged.");
            return 1;
       }
       fclose(ErrorFile);
       return 0;
}
