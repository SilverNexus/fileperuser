/**
 * @file output.h
 * The file declares the output function to be used at termination.
 */

#ifndef OUTPUT_H
#define OUTPUT_H

/**
 * Outputs the found matches to the file.
 * Used at both normal termination and SIGINT.
 */
void output_matches();

#endif
