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
palette.h: Princed Resources : Palette object support headers
���������
 Copyright 2003 Princed Development Team
  Created: 24 Aug 2003

  Author: Enrique Calot <ecalot.cod@princed.com.ar>
  Version: 1.01 (2003-Oct-23)

 Note:
  DO NOT remove this copyright notice
*/

#ifndef _PR_PALETTE_H_
#define _PR_PALETTE_H_

#include "image.h" /* tPalette, tImage */
#include "binary.h"

#define SAMPLE_PAL16 {\
	{0x00,0x00,0x00},\
	{0x00,0x00,0xa0},\
	{0x00,0xa7,0x00},\
	{0x00,0xa7,0xa0},\
	{0xa0,0x00,0x00},\
	{0xa0,0x00,0xa0},\
	{0xa0,0x50,0x00},\
	{0xa0,0xa7,0xa0},\
	{0x50,0x50,0x50},\
	{0x50,0x50,0xff},\
	{0x50,0xf8,0x50},\
	{0x50,0xf8,0xff},\
	{0xff,0x50,0x50},\
	{0xff,0x50,0xff},\
	{0xff,0xf8,0x50},\
	{0xff,0xff,0xff}}

void* objPalette_pop2_4bitsCreate(tBinary c, int *error);
int objPalette_pop2_4bitsWrite(void* o, const char* file, int optionflag, const char* backupExtension);
void* objectPalettePop1_16Create(tBinary c, int *error);
int objectPalettePop1_16Write(void* o, const char* file, int optionflag, const char* backupExtension);
void* objectPalettePop1_16Read(const char* file,int *result);
int objectPalettePop1_16Set(void* o,tResource* res);
void* objectPalettePop2_NColorsCreate(tBinary cont, int *error);
int objectPalettePop2_NColorsWrite(void* o, const char* file, int optionflag, const char* backupExtension);
void* objPop2PaletteNColorsRead(const char* file,int *result);
int objPop2PaletteNColorsSet(void* o,tResource* res);

tColor* objectPalettePop1_16GetColorArray(void* o);
tColor* objectPalettePop2_NColorsGetColorArray(void* o);

/* middle layer */
#define to8bits_A(a) (((a)<<2)|((a)>>4))
#define to8bits_B(a) (((a)<<2)         )
#define to8bits_C(a) (((a)<<2)+2       )

#define convert24to18(x) (unsigned char)((x+2)>>2);

/* Hooks */

#define PAL_COLORS_eResTypePalettePop2_NColors objectPalettePop2_NColorsGetColorCount(object.obj)
#define PAL_COLORS_eResTypePalettePop1_16      16
#define PAL_COLORS_eResTypePalettePop1_Mono    2

int objectPalettePop2_NColorsGetColorCount(void* o);
int objectPaletteGetBitRate(tObject pal);
tColor* objectPaletteGetColorArray(tObject pal);

#endif

