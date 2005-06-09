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
resources.h: Princed Resources : Resource Handler headers
ŻŻŻŻŻŻŻŻŻŻŻ
 Copyright 2003 Princed Development Team
  Created: 24 Aug 2003

  Author: Enrique Calot <ecalot.cod@princed.com.ar>
  Version: 1.01 (2003-Oct-23)

 Note:
  DO NOT remove this copyright notice
*/

#ifndef _RESOURCES_H_
#define _RESOURCES_H_

/* XML generation defines */

#define RES_XML_UNKNOWN_PATH  "%s/unknown/%s/"
#define RES_XML_UNKNOWN_FILES "%s%03d.%s"
#define RES_XML_UNKNOWN_START "<?xml version=\"1.0\" ?>\n<resources version=\"generated\">\n <folder name=\"unknown\" path=\"%s\" file=\"%s\" palette=\"%d\" paletteindex=\"%s\">\n"
#define RES_XML_UNKNOWN_ITEM  "  <item value=\"%d\" index=\"%s\" path=\""RES_XML_UNKNOWN_FILES"\" itemtype=\"%s\">Unknown %s %d</item>\n"
#define RES_XML_UNKNOWN_END   " </folder>\n</resources>\n"

/* File extensions */

#define RES_FILE_EXTENSIONS   {"raw","plv","bmp","wav","mid","bin","pal","pcs"}
#define RES_FILE_TYPES        {"autodetect","level","image","wave","midi","binary","palette","pcspeaker"}

/* Item Types */

#define RES_TYPE_RAW          0
#define RES_TYPE_LEVEL        1
#define RES_TYPE_IMAGE        2
#define RES_TYPE_WAVE         3
#define RES_TYPE_MIDI         4
#define RES_TYPE_BINARY       5
#define RES_TYPE_PALETTE      6
#define RES_TYPE_PCSPEAKER    7

#define RES_TYPECOUNT         8


/* Id list for partial manipulation */
typedef enum {eString,eValue}tResIdType;

typedef struct {
  tResIdType idType;
  union {
    char*      text;
    long       number;
  } value;
}tResIdListItem;

typedef struct {
	int             count;
	tResIdListItem* list;
}tResIdList;

void parseGivenPath(char* path);
int  partialListActive();
int  isInThePartialList(const char* file, int value);
void freePartialList();

/* Includes */
#include <stdio.h>
#include "xmlparse.h"

/* Verify  header */
int verifyHeader(const unsigned char* array, int size);

/*
	Headers may be:
	 01 Levels
	 02 Bitmaps
	 03 Waves
	 04 Midis
	 05 Binary
	 06 Palette
	 07 Internal Speaker Sounds
*/

#include "dat.h" /* tResource */
#include "reslist.h" /* tResourceList */

/* CheckSum verification */
int checkSum(const unsigned char* data,int size);

/* Resources extras */
void getFileName(char* vFileext,const char* vDirExt,const tResource* r,const char* vFiledat, const char* vDatFileName,int optionflag,const char* backupExtension);
void getUpperFolder(char* aux, char* vFiledat);

const char* getExtDesc(int type);

/* parse xml file */
int parseFile(const char* vFile, const char* datFile, tResourceList *r);

/* In case there are unknown resources it closes the unknown XML output */
void endUnknownXml();

/* Search files for the Import feature */
int importDir(const char* directory, const char* vResFile, int pOption, const char* backupExtension,const char* vDirectory,FILE* output);
int isntADatFile(const char* testFile, const char* vResFile);

#endif
