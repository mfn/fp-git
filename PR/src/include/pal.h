
/***************************************************************\
|                 Jasc Palette handling functions               |
\***************************************************************/

#ifndef _PAL_H_
#define _PAL_H_

#include "compress.h"

#define PAL_HEADER "JASC-PAL\r\n0100\r\n16\r\n"
#define PAL_SAMPLE {0x97, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x03, 0x08, 0x0F, 0x07, 0x0C, 0x13, 0x0C, 0x11, 0x16, 0x11, 0x17, 0x1C, 0x1E, 0x21, 0x25, 0x2F, 0x31, 0x33, 0x3F, 0x3F, 0x3F, 0x2A, 0x2A, 0x2A, 0x15, 0x15, 0x15, 0x17, 0x1C, 0x20, 0x07, 0x0C, 0x13, 0x0C, 0x12, 0x17, 0x15, 0x1A, 0x1F, 0x00, 0x1D, 0x13, 0x00, 0x29, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

char mFormatExtractPal(unsigned char** data, char *vFileext,unsigned long int size);
char mImportPalette(unsigned char** data, unsigned short *size);
void mExportPalette(unsigned char** data, unsigned long int *size);

//This function will be obsolete
void mLoadPalette(char* array,tImage *image);

#endif
