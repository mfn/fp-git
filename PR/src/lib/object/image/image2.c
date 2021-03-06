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
image2.c: Princed Resources : Image Compression Library
��������
 Copyright 2003, 2004, 2005, 2006 Princed Development Team
  Created: 24 Aug 2003

  Author: Enrique Calot <ecalot.cod@princed.com.ar>
  Version: 1.01 (2003-Oct-23)
  Version: 2.00 (2004-Mar-07)

 Note:
  DO NOT remove this copyright notice
*/

#include <stdio.h>
#include <string.h>
#include "common.h"
#include "compress.h"
#include "dat.h"
#include "disk.h" /* array2short */
#include "image.h"
#include "memory.h"
#include "object.h" /* paletteGet* */

#include "palette.h" /* getColorArrayByColors */

#include "bitmap.h"

/* Compression level explanation:
 *
 * Definitions:
 *  no compression is called RAW
 *  there are 2 algorithms types: RLE and LZG
 *  we can use the modifier: not transposed and transposed (t)
 *  we can use the LZG modifier: higher (checks more extensively the LZG window
 *   without ignoring less probable patterns) (+)
 *
 *  So the possible compression algorithm variants are:
 *   RAW, RLE, RLEt, LZG, LZGt, LZG+, LZGt+
 *
 *  It is known that LZG+ always compresses better or equal than LZG
 *
 * Depending on the compression level, the compressor will compress with
 * all the algorithms specified and keep only the smaller result using
 * the following table
 *
 * Level  Algorithms
 *   1    RAW
 *   2    RAW, RLE
 *   3    RAW, RLE, RLEt
 *   4    RAW, RLE, RLEt, LZG
 *   5    RAW, RLE, RLEt, LZG, LZGt
 *   6    RAW, RLE, RLEt, LZG+, LZGt
 *   7    RAW, RLE, RLEt, LZG+, LZGt+
 *
 * The default level used in PR will be 3.
 *
 * In images with big entropy that generates DAT files bigger than 64kb, using
 * a better compression is a must. The POP1 DAT file format has this limitation
 * and the only way to get through with it is improving the compression.
 *
 * For testing DAT files that are not for distribution compression 3 is highly
 * recommended because is much faster and you perform compressions more often.
 *
 * When you release a DAT file a compression level 7 is the best you can use.
 * You'll have to wait some time to get the importing, but the decompression
 * is as faster as the decompression in other levels. The game supports it and
 * decompresses the files very fast. Another advantage is that it is better to
 * distribute smaller DAT files.
 *
 */

/***************************************************************\
|                  I M P L E M E N T A T I O N                  |
\***************************************************************/

tColor* objPalette_pop1_monoGetColors() {
	static tColor c[2]={{0,0,0},{255,255,255}};
	return c;
}

extern FILE* outputStream;

void* objectImage2Create(tBinary cont, int *error) {

	/*
	 * This function will expand the data into an image structure,
	 * then the bitmap structure will be saved to disk
	 *
	 * Note: The old structure is passed by parameters in order to
	 *       keep the right palette.
	 */

	tImage* image;
	image=(tImage*)malloc(sizeof(tImage));

	/* Expand graphic and check results */
	*error=mExpandGraphic(cont,image);

	if (*error==PR_RESULT_F_COMPRESS_RESULT_FATAL) {
		free(image);
		return NULL;
	}

	if (getCarry(image->type) != 1 ) printf("error, monochrome palette fromat wrong\n"); /* TODO: add a warning */
	
	image->colorCount=2;

	return (void*)image;
}

int objectImage2Write(void* img,const char* file,int optionflag,const char* backupExtension) {
	tImage* i=img;
	tColor* colorArray=objPalette_pop1_monoGetColors();

	/* Write bitmap */
	return mWriteBmp(file,i->pix,i->width,i->height,1,2,colorArray,i->widthInBytes,optionflag,backupExtension);
}

void* objectImage2Read(const char* file, int *result) {
	tImage* image=(tImage*)malloc(sizeof(tImage));
	tColor* colorArray;
	int colors;

	*result=readBmp(file,&(image->pix),&(image->height),&(image->width),&(image->bits),&colors,&colorArray,&(image->widthInBytes));
	/* check if image was succesfully read loaded */
	if (*result!=PR_RESULT_SUCCESS) {
		free(image);
		return NULL;
	}

	free(colorArray);

	/* check the palette information */
	if (colors!=2) {
		*result=PR_RESULT_F_PAL_UNSUPPORTED_SIZE; /* TODO: convert in warning and perform a palette conversion */
		free(image->pix);
		free(image);
		return NULL;
	}
	image->colorCount=2;

	return (void*)image;
}

int objectImage2Set(void* o,tResource* res) {
	tImage* img=o;
	tBinary decompressed,compressed;
	int algorithm;

	decompressed.data=img->pix;
	decompressed.size=img->widthInBytes*img->height;

	algorithm=mCompressGraphic(&decompressed,&compressed,6,img->widthInBytes,img->height);

	/*
	 * Write header
	 */
/* TODO: merge with objectImage16Set */
	/* (16 bits)height (Intel short int format) */
	short2array(compressed.data,img->height);
	short2array(compressed.data+2,img->width);
#if 0
	compressed.data[0]=img->height; /* TODO: use shorttoarray */
	compressed.data[1]=img->height>>8;
	/* (16 bits)width (Intel short int format) */
	compressed.data[2]=img->width;
	compressed.data[3]=img->width>>8;
#endif
	/* (8 bits)00000000+(4 bits)palette type+(4 bits)algorithm */
	compressed.data[4]=0;
	compressed.data[5]=algorithm;

	res->content=compressed;
	mWriteFileInDatFile(res);
	return PR_RESULT_SUCCESS;
}

