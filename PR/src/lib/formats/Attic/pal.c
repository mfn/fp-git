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
pal.c: Princed Resources : JASC PAL files support
�����
 Copyright 2003 Princed Development Team
  Created: 24 Aug 2003

  Author: Enrique Calot <ecalot.cod@princed.com.ar>
  Version: 1.01 (2003-Oct-23)

 Note:
  DO NOT remove this copyright notice
*/

//Includes
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "pal.h"
#include "memory.h"
#include "disk.h"
#include "resources.h"

/***************************************************************\
|                 Jasc Palette handling functions               |
\***************************************************************/

//Public functions
char mFormatExtractPal(unsigned char** data, char *vFileext,unsigned long int size) {
	//Convert palette from POP format to JASC format
	mExportPalette(data,&size);
	//save JASC palette
	return writeData(*data,0,vFileext,size);
}

char mImportPalette(unsigned char** data, unsigned short *size) {

	//declare variables
	unsigned char palh[]=PAL_HEADER;
	unsigned char pals[]=PAL_SAMPLE;
	unsigned char* pal;
	unsigned char* pal2;
	unsigned char* data2;
	//unsigned short int parsed;
	unsigned char r,g,b;
	int i=0;
	int k=16;

	//check size
	if (*size<130) return 0;

	pal=getMemory(100);

	//set palette with sample
	memcpy(pal,pals,100);
	pal2=pal+4;

	//verify jasc pal header
	while (palh[i]==(*data)[i++]);
	if (i!=sizeof(palh)) return 0; //pallete differs with headers

	//set current values
	data2=strtok(*data+sizeof(palh),"\r\n");
	while (k--) {
		if (!sscanf(data2,"%d %d %d",&r,&g,&b)) return 0;
		*(pal2++)=(r+2)>>2;
		*(pal2++)=(g+2)>>2;
		*(pal2++)=(b+2)>>2;
		data2=strtok(NULL,"\r\n");
	}

	//free old data and set new
	free(*data);
	*data=pal;
	*size=100;
	return 1;
}

void mExportPalette(unsigned char** data, unsigned long int *size) {
	unsigned char* pal=getMemory(240);
	unsigned char* aux=getMemory(240);
	unsigned char i;

	sprintf(pal,PAL_HEADER);

	for (i=0;i<16;i++) {
		sprintf(aux,pal);
		sprintf(pal,"%s%d %d %d\r\n",aux,(*data)[(i*3)+5]<<2,(*data)[(i*3)+6]<<2,(*data)[(i*3)+7]<<2);
	}
	for (i=0;pal[i];i++);
	free(*data);
	free(aux);
	(*data)=pal;
	*size=i-1;
}

//TODO: use a macro
void mLoadPalette(char* array,tImage *image) {
	memcpy(image->pal,array+5,16*3);
}
