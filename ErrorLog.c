/*
    FilePeruser, a recursive file search utility.
    Copyright (C) 2014-2016  SilverNexus

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
*/

/**
 * @file ErrorLog.c
 * Last modified on 2016-05-01 by Daniel Hawkins.
 *
 * The file defines the functions for logging errors.
 */

#include "ErrorLog.h"
#include "settings.h"
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <stdlib.h>

const char * const ERROR_TYPE_CHARS[] = {"DEBUG", "INFO", "WARNING", "ERROR", "FATAL"};
const char * const MONTH[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

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
       va_list ap;
       if (err >= settings.min_print_level){
           // Print the message
	   fprintf(stderr, "%s: ", ERROR_TYPE_CHARS[err]);
	   va_start(ap, msg);
	   vfprintf(stderr, msg, ap);
	   va_end(ap);
	   fputs("\n", stderr);
       }
       // If major enough, log to the file
       if (err >= settings.min_log_level){
           time_t theTime = time(0);
           struct tm *date = localtime(&theTime);
           // Create output file stream object
           FILE *ErrorFile = fopen(settings.log_file, "a");
           if (ErrorFile){
		fprintf(ErrorFile, "%2i %s %4i %02i:%02i:%02i: %s: ", date->tm_mday, MONTH[date->tm_mon], date->tm_year+1900, date->tm_hour, date->tm_min, date->tm_sec, ERROR_TYPE_CHARS[err]);
		va_start(ap, msg);
		vfprintf(ErrorFile, msg, ap);
		va_end(ap);
		fputs("\n", ErrorFile);
	   }
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
