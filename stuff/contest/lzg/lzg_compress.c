/* 
 * LZG compression
 * 
 * ---------------------------------------------------------------------------- 
 * 
 * Authors: 
 *   Enrique Calot <ecalot.cod@princed.com.ar>
 *   Diego Essaya <dessaya@fi.uba.ar>
 * 
 * Research: Tammo Jan Dijkemma, Anke Balderer, Enrique Calot
 *
 * ----------------------------------------------------------------------------
 *
 * Copyright (C) 2004, 2005 the Princed Team
 * 
 * This file is part of the Princed project.
 * 
 * Princed is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Princed is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
 * Modus operandi of the compression algorithm
 * -------------------------------------------
 * 
 * For each input byte we take a window containing the 1023 previous bytes.
 * If the window goes out of bounds (ie, when the current input byte is
 * before position 1024), we consider it filled with zeros.
 *
 *     00000000000000000000********************************
 *                         ^                  ^
 *                    input start   current input byte
 *           |--------------------------------|
 *                    window size=1023
 * 
 * The algorithm works as follows:
 *
 * While there is unread input data:
 *     Create a maskbyte.
 *     For each bit in the maskbyte (and there is still unread input data):
 *         Compare the following input bytes with the bytes in the window,
 *         and search the longest pattern that is equal to the next bytes.
 *         If we found a pattern of length n > 2:
 *             Assign 0 to the current bit of the maskbyte.
 *             In the next 2 bytes of the output, specify the relative
 *             position and length of the pattern.
 *             Advance output pointer by 2.
 *             Advance input pointer by n.
 * 	       Else:
 *             Assign 1 to the current bit of the maskbyte.
 * 	           Copy the current input byte in the next output byte.
 *             Advance output pointer by 1.
 *             Advance input pointer by 1.
 */

#include <stdio.h>
#include "lzg_compress.h"

/*#define LZG_REVERSE*/

#ifdef LZG_REVERSE
void *memrchr2(unsigned char *s, int c, size_t n) {
	while (n--) {
		if (s[n]==c) return s+n;
	}
	return NULL;
}
#define memsearch(a,b,c) memrchr2(a,b,c)
#else
#include <string.h>  /* memchr() */
#define memsearch(a,b,c) memchr(a,b,c)
#endif

/* Window size */
#define WIN_SIZE 1024

/* A repetition pattern must have a length of at least MIN_PATTERN_SIZE 
 * to be accepted (ie, to be worth for compressing) */
#define MIN_PATTERN_SIZE 3
#define MAX_PATTERN_SIZE 66

/* search the longest pattern in the window that matches the first bytes
 * of the input */
void search_best_pattern(unsigned char *input, int inputSize, 
                         unsigned char **best_pattern, int *best_pattern_len)
{
	unsigned char *pattern;
	int pattern_len;
	unsigned char *window = input - (WIN_SIZE - 1);
	int window_len = WIN_SIZE - 1;

	*best_pattern_len = 0;

	while ((pattern = (unsigned char *)memsearch(window, *input, window_len)))
	{
		unsigned char *ic = input + 1;
		unsigned char *wc = pattern + 1;
		pattern_len = 1;

		while ( (ic < (input + inputSize)) &&
		        (*ic == *wc) &&
				pattern_len < MAX_PATTERN_SIZE)
		{
			ic++; wc++; pattern_len++; /* increase until the pattern is different */
		}

		if (pattern_len > *best_pattern_len) /* if it is the maximum, save it */
		{
			*best_pattern_len = pattern_len;
			*best_pattern = pattern;
		}

		if (pattern_len == MAX_PATTERN_SIZE) break;

		/* Comment these three lines and uncomment the next two to get
		 * 5% more compression at 4x execution time: */
#ifdef LZG_FASTER
		window_len -= wc - window;
		if (window_len <= 0) break;
		window = wc;
#else
		window_len--;
		window++;
#endif
	}
}

/* Insert the specified bit in the current maskByte. If the maskByte is full,
 * start a new one. */
void pushMaskBit(int b, unsigned char **maskByte, 
                 unsigned char *output, int *outputPos)
{
	static int maskBit=8;
	if ( maskBit == 8 ) /* first time or maskBit is full */
	{
		/* start a new maskByte */
		*maskByte = output + *outputPos;
		(*outputPos)++;
		**maskByte = 0;
		maskBit = 0;
printf("maskbyte i=%d\n", *outputPos - 1);
	}

	**maskByte |= b<<maskBit;
	maskBit++;
}

/* Insert the two bytes describing the pattern repetition to the output. */
void addPattern(unsigned char *input, int inputPos,
                unsigned char *output, int outputPos,
                unsigned char *pattern, int pattern_len)
{
	int loc = (pattern - input) + 1024 - 66;
printf("pattern i=%d o=%d rep=%d loc=%d\n", outputPos, inputPos, pattern_len, inputPos-(pattern-input));
	output[outputPos] = 
		(((pattern_len - 3) << 2) & 0xfc) + ((loc & 0x0300) >> 8);
	output[outputPos + 1] = (loc & 0x00FF);
}

/* Compress using the LZG algorithm */
/* Assume output has been allocated and the size is very big */
void compressLzg(unsigned char* input, int inputSize, 
                 unsigned char* output, int *outputSize)
{
	int inputPos = 0, outputPos = 0;
	unsigned char *maskByte;
	int i;

	/* Create ghost window filled with zeros before input data: */
	for (i = inputSize - 1; i >= 0; i--)
		input[i + WIN_SIZE] = input[i]; /* First move the data WIN_SIZE bytes forward */
	for (i = 0; i < WIN_SIZE; i++)
		input[i] = 0; /* Second fill in with zeros the first WIN_SIZE bytes */
	input += WIN_SIZE; /* Third move the input beginning again to the data start */

	while (inputPos < inputSize)
	{
		unsigned char *best_pattern;
		int best_pattern_len;

		search_best_pattern(input + inputPos, inputSize - inputPos, 
		                    &best_pattern, &best_pattern_len);

		if (best_pattern_len < MIN_PATTERN_SIZE)
		{
			/* No suitable pattern found. Just copy the current byte. */
			pushMaskBit(1, &maskByte, output, &outputPos);
			output[outputPos] = input[inputPos];
printf("copy i=%d o=%d data=%02x\n", outputPos, inputPos, output[outputPos]);
			inputPos++;
			outputPos++;
		}
		else
		{
			/* Can compress. Repeat the best pattern. */
			pushMaskBit(0, &maskByte, output, &outputPos);
			addPattern(input, inputPos, output, outputPos,
			           best_pattern, best_pattern_len);
			inputPos += best_pattern_len;
			outputPos += 2;
		}
	}

	*outputSize = outputPos;
}

