/*
 * files.h
 *
 *  Created on: 18.06.2012
 *      Author: Martin Köhler, metraTec GmbH
 */

#ifndef FILES_H_
#define FILES_H_

unsigned char *build_output_filename (unsigned char *buffer, unsigned char Hardware);
int read_file (unsigned char* inbuf, unsigned int length, char* filename);
int write_file (unsigned char* inbuf, unsigned int length, const char* filename);

#endif /* FILES_H_ */
