
#ifndef _WAV_H_
#define _WAV_H_

#include <stdio.h>
#if DIR_SEPARATOR=='/'
#include "../resources.h"
#else
#include "resources.h"
#endif

#define WAVE_HEADER {0x52, 0x49, 0x46, 0x46, 0x12, 0x16, 0x00, 0x00, 0x57, 0x41, 0x56, 0x45, 0x66, 0x6D, 0x74, 0x20, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x11, 0x2B, 0x00, 0x00, 0x11, 0x2B, 0x00, 0x00, 0x01, 0x00, 0x08, 0x00, 0x64, 0x61, 0x74, 0x61, 0xA3, 0x15, 0x00, 0x00}

char mFormatExtractWav(unsigned char* data, char *vFileext,unsigned long int size);
char mFormatCompileWav(unsigned char* data, FILE* fp, tResource *res);

#endif
