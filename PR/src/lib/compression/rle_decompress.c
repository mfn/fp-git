/*  Princed V3 - Prince of Persia Level Editor for PC Version
    Copyright (C) 2003 Princed Development Team

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

    The authors of this program may be contacted at http://forum.princed.com.ar
*/

/*
rle_decompress.c: Princed Resources : Image Compression Library
ŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻ
 Copyright 2003, 2004 Princed Development Team
  Created: 24 Aug 2003

  Author: Enrique Calot <ecalot.cod@princed.com.ar>
  Version: 1.01 (2003-Oct-23)
  Version: 2.00 (2004-Mar-07)

 Note:
  DO NOT remove this copyright notice
*/

#include <stdio.h>
#include <stdlib.h>
#include "compress.h"

/* Expands RLE algorithm */
int expandRle(tBinary input, unsigned char** output, int *outputSize) {
	register signed char rep=1;
	int oCursor=0;
	int iCursor=0;

	/* reserve memory */
	if ((*output=malloc(40000))==NULL) return PR_RESULT_F_COMPRESS_RESULT_FATAL;

	/* main loop */
	while (iCursor<input.size) {
		rep=(signed char)(input.data[iCursor++]);
		if (rep<0) {
			/* Negative */
			while (rep++) (*output)[oCursor++]=input.data[iCursor];
			iCursor++;
		} else {
			/* Positive */
			rep=~rep;
			while ((rep++)&&(iCursor<input.size)) (*output)[oCursor++]=input.data[iCursor++];
		}
	}

	*outputSize=oCursor;
	return (rep==1)?PR_RESULT_SUCCESS:PR_RESULT_W_COMPRESS_RESULT_WARNING;
}
