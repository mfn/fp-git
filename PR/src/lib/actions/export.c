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
extract.c: Princed Resources : DAT Extractor
���������
 Copyright 2003 Princed Development Team
  Created: 24 Aug 2003

  Author: Enrique Calot <ecalot.cod@princed.com.ar>
  Version: 1.01 (2003-Oct-23)

 Note:
  DO NOT remove this copyright notice
*/

#include <stdio.h>
#include <string.h>

#include "pr.h"

#include "extract.h"
#include "disk.h"
#include "memory.h"

#include "bmp.h"
#include "wav.h"
#include "pal.h"
#include "plv.h"
#include "mid.h"

/***************************************************************\
|                  I M P L E M E N T A T I O N                  |
\***************************************************************/




/***************************************************************\
|                     M A I N   E X T R A C T                   |
\***************************************************************/

/*
	Extracts a dat file
	For parameter documentation, see pr.c
*/

int extract(const char* vFiledat,const char* vDirExt, tResource* r[], int task, const char* vDatFileName, const char* vDatAuthor) {

	//Variables
	char          vFileext[260];
	FILE*         fp;
	int           ok,pop1;

	if (ok=((fp=fopen(vFiledat,"rb"))!=NULL)) {
		//loop variables
		unsigned long  int indexOffset;
		unsigned short int indexSize,numberOfItems;
		unsigned char*     index;
		int                ofk=0;
		int                k;

		//if header ok, new variables
		char               recordSize;
		char               type=0;
		tImage             image; //this is used to make a persistent palette
		unsigned char*     data;
		unsigned long  int size,offset;
		unsigned short int id;
		unsigned short int paletteId=0;

		//verify dat format
		ok    = fread(&indexOffset,4,1,fp);
		ok=ok&& fread(&indexSize,2,1,fp);
		ok=ok&& !fseek(fp,indexOffset,SEEK_SET);
		ok=ok&& fread(&numberOfItems,2,1,fp);
		pop1=((numberOfItems*8+2)==indexSize);

		if (!pop1) { //verify if pop2
			ofk=numberOfItems*6+2+(numberOfItems-2)*13;
			numberOfItems=((indexSize-6-(numberOfItems*6)-((numberOfItems-2)*13))/11);
		}
		recordSize=pop1?8:11;
		if (!ok) {
			fclose(fp);
			return -3; //this is not a valid prince dat file
		}
		if ((index=getMemory(indexSize-2))==NULL) {
			fclose(fp);
			return -2; //no memory
		}

		ok=fread(index,indexSize-2,1,fp);

		//Initializes the palette list
		for (id=0;id<MAX_RES_COUNT;id++) {
			if (r[id]!=NULL) {
				r[id]->palAux=NULL;
			}
		}

		//main loop
		for (k=0;ok&&(k<numberOfItems);k++) {
			//for each archived file the index is read
			id=index[ofk+k*recordSize]+256*index[ofk+k*recordSize+1];
			offset=index[ofk+k*recordSize+2]+256*index[ofk+k*recordSize+3]+256*256*index[ofk+k*recordSize+4]+256*256*256*index[ofk+k*recordSize+5];
			size=index[ofk+k*recordSize+6]+256*index[ofk+k*recordSize+7]+1;
			if (!pop1) {
				ok=ok&&(index[ofk+k*recordSize+8]==0x40)&&(!index[ofk+k*recordSize+9])&&(!index[ofk+k*recordSize+10]);
			}
			ok=ok&&((data=getMemory(size))!=NULL);
			ok=ok&&(!fseek(fp,offset,SEEK_SET));
			ok=ok&&fread(data,size,1,fp);
			if (!ok) return -3;
			//End of index reading

			//set resource information on this index entry
			if (r[id]==NULL) {
				r[id]=(tResource*)malloc(sizeof(tResource));
				if (r[id]==NULL) return -2; //no memory
				r[id]->path=NULL;
				r[id]->palAux=NULL;
				r[id]->desc=NULL;
				r[id]->title=NULL;
				r[id]->palette=0;
				r[id]->number=0;
				r[id]->size=(unsigned short int)size;
				r[id]->offset=(unsigned short)offset;
				r[id]->type=verifyHeader(data,(unsigned short int)size);
			} else { //If resource type is invalid or 0, the type will be decided by PR
				if (!(r[id]->type)) r[id]->type=verifyHeader(data,(unsigned short int)size);
			}

			if (!(task&unknown_flag)) { //If unknown flag is set do nothing but generate the unknown.xml file
				//select type
				if (task&raw_flag) (*(r[id])).type=0; //If "extract as raw" is set, type is 0

				//save file
				getFileName(vFileext,vDirExt,r[id],id,vFiledat,vDatFileName);

				if (task&verbose_flag) printf("Extracting '%s'...\n",getFileNameFromPath(vFileext));

				switch (r[id]->type) {
					case 1:
						ok=ok&&mFormatExtractPlv(data,vFileext,size,r[id]->number,vDatFileName,r[id]->title,r[id]->desc,vDatAuthor);
						break;
					case 5:
					case 0: //Raw files
						ok=ok&&writeData(data,1,vFileext,size); //Ignore checksum
						break;
					case 6: //save and remember palette file
						//This will remember the palette for the next images
						r[id]->palAux=(unsigned char*)malloc(size);
						memcpy(r[id]->palAux,data,size);
						if (!paletteId) { //In case there is no loaded palettes, then load immediately the first found palette to clear garbage
							mLoadPalette(data,&image);
							paletteId=id;
						}
						//This will extract the palette
						ok=ok&&mFormatExtractPal(&data,vFileext,size);
						break;
					case 7: //save pcs file
					case 4:	//save midi file
						ok=ok&&mFormatExtractMid(data,vFileext,size);
						break;
					case 3: //save wav file
						ok=ok&&mFormatExtractWav(data,vFileext,size);
						break;
					case 2: //save image
						//Palette handling

						if (r[id]->palette!=paletteId) { //The palette isn't the already loaded
							if (r[id]->palette) { //We need a palette
								/*
									We need a palette and it is not the palette we have loaded in memory
									So a new palette is going to be loaded.
								*/
								if ((r[r[id]->palette]->palAux)!=NULL) { //If this palette wasn't loaded, it becomes loaded
									mLoadPalette(r[r[id]->palette]->palAux,&image);
									paletteId=r[id]->palette; //sets the new palette loaded
								}
							}
						}

						//Extract bitmap
						ok=ok&&mFormatExtractBmp(data,vFileext,size,image);
						break;
				}
			}
			if (data!=NULL) free(data);
		}
		fclose(fp);

		//Free allocated palettes and descriptions
		for (id=0;id<MAX_RES_COUNT;id++) {
			if (r[id]!=NULL) {
				if (r[id]->palAux!=NULL) {
					free (r[id]->palAux);
				}
				if (r[id]->desc!=NULL) {
					free (r[id]->palAux);
				}
			}
		}

		//Close unknownXML
		endUnknownXml();
		return ok-1;
	} else {
		return -1; //file could not be open
	}
}
