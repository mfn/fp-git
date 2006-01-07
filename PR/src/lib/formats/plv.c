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
plv.c: Princed Resources : PLV prince level files support
�����
 Copyright 2003 Princed Development Team
  Created: 29 Nov 2003

  Author: Enrique Calot <ecalot.cod@princed.com.ar>
  Version: 1.00 (2003-Nov-29)

 PLV file format:
  Defined: 28 Nov 2003

  Authors:
   Brendon James <roomshaker@princed.com.ar>
   Enrique Calot <ecalot.cod@princed.com.ar>
  Version: 1.00 (2003-Nov-28)

 Note:
  DO NOT remove this copyright notice
*/

/* Includes */
#include "plv.h"
#include "disk.h"
#include "dat.h"
#include <string.h>
#include <time.h>

/* Private function to get the currect date/time */
char* getDate() {
	/* Code taken from RDR 1.4.1a coded by Enrique Calot */

	/* Declare variables */
#ifdef PLV_FULL_FORMAT
	static char weeks   []   = DATE_WEEKDAYS;
	static char months  []   = DATE_MONTHS;
#endif
	static char formated [37];
	time_t      datet;
	struct tm*  date;

	/* get GMT time from the system clock */
	time(&datet);
	date=gmtime(&datet);

#ifdef PLV_FULL_FORMAT
	/* Format: "Tue, 26 Nov 2002 22:16:39" */
	sprintf(formated,"%s, %d %s %.4d %.2d:%.2d:%.2d",
		weeks+4*(date->tm_wday),
		date->tm_mday,
		months+4*(date->tm_mon),
		date->tm_year+1900,
		date->tm_hour,
		date->tm_min,
		date->tm_sec
	);
#else
	/* Format: "2002-11-26 22:16:39" */
	sprintf(formated,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d",
		date->tm_year+1900,
		date->tm_mon+1,
		date->tm_mday,
		date->tm_hour,
		date->tm_min,
		date->tm_sec
	);
#endif
	return formated;
}

int mFormatExportPlv(const unsigned char* data, const char *vFileext,unsigned long int size,unsigned char level, const char* filename, const char* desc, const char* title, const char* vDatAuthor,int optionflag,const char* backupExtension) {
	/* Plv files are saved as raw except you must ignore the checksum and add the plv constant file header */

	/* Variables */
	FILE* target;
	int ok;
	unsigned char sizeOfNow;
	char* now;
	char levelnum[10];
	const char* nullString="";
	static const char* author=PLV_DEFAULT_AUTHOR;
	unsigned long int block2size;
	const unsigned long int numberOfFieldPairs=10;

	/* Get current time */
	now=getDate();
	sizeOfNow=(unsigned char)(strlen(now)+1);

	/* Ignore checksum */
	/* size--; */

	/* Validate null strings when no description is set */
	if (desc==NULL) desc=nullString;
	if (title==NULL) title=nullString;
	if (vDatAuthor==NULL) vDatAuthor=author;
	sprintf(levelnum,"%d",level);

	/* Writing file */

	/* Safe open for writing mode */
	ok=writeOpen(vFileext,&target,optionflag);

	/* Write headers */
	ok=ok&&fwrite(PLV_HEADER_A,PLV_HEADER_A_SIZE,1,target);
	ok=ok&&fwritechar(&level,target);
	ok=ok&&fwritelong(&numberOfFieldPairs,target);
	ok=ok&&fwritelong(&size,target);

	/* Write block 1: raw data without ignoring checksum */
	ok=ok&&fwrite(data,size,1,target);

	/* Write footers */
	block2size=(
		sizeof(PLV_FOOT_EDITOR)+	     strlen(vDatAuthor)+1+
		sizeof(PLV_FOOT_TITLE)+	       strlen(title)+1+
		sizeof(PLV_FOOT_DESC)+	       strlen(desc)+1+
		sizeof(PLV_FOOT_TCREAT)+	     sizeOfNow+
		sizeof(PLV_FOOT_TMODIF)+	     sizeOfNow+
		sizeof(PLV_FOOT_ORIG_FILE)+	   strlen(filename)+1,
		sizeof(PLV_FOOT_LEV_NUM_ORIG)+ strlen(levelnum)+1
	);

	ok=ok&&fwritelong(&block2size,target);

	/* Write block 2 */
	ok=ok&&fwrite(PLV_FOOT_EDITOR,sizeof(PLV_FOOT_EDITOR),1,target);
	ok=ok&&fwrite(vDatAuthor,strlen(vDatAuthor)+1,1,target);
	ok=ok&&fwrite(PLV_FOOT_TITLE,sizeof(PLV_FOOT_TITLE),1,target);
	ok=ok&&fwrite(title,strlen(title)+1,1,target);
	ok=ok&&fwrite(PLV_FOOT_DESC,sizeof(PLV_FOOT_DESC),1,target);
	ok=ok&&fwrite(desc,strlen(desc)+1,1,target);
	ok=ok&&fwrite(PLV_FOOT_ORIG_FILE,sizeof(PLV_FOOT_ORIG_FILE),1,target);
	ok=ok&&fwrite(filename,strlen(filename)+1,1,target);
	ok=ok&&fwrite(PLV_FOOT_TCREAT,sizeof(PLV_FOOT_TCREAT),1,target);
	ok=ok&&fwrite(now,sizeOfNow,1,target);
	ok=ok&&fwrite(PLV_FOOT_TMODIF,sizeof(PLV_FOOT_TMODIF),1,target);
	ok=ok&&fwrite(now,sizeOfNow,1,target);
	ok=ok&&fwrite(PLV_FOOT_LEV_NUM_ORIG,sizeof(PLV_FOOT_LEV_NUM_ORIG),1,target);
	ok=ok&&fwrite(levelnum,strlen(levelnum)+1,1,target);

	/* Close file and return */
	ok=ok&&(!writeCloseOk(target,optionflag,backupExtension));
	return ok;
}

extern FILE* outputStream;

int mFormatImportPlv(tResource *res) {
	/* declare variables */
	unsigned char* pos;
	unsigned char* posAux;
	unsigned long int oldSize=res->size;

	/* integrity check 1 */
	if (oldSize<=PLV_HEADER_A_SIZE+1+PLV_HEADER_B_SIZE) return 0; /* false */
	if (memcmp(res->data,PLV_HEADER_A,PLV_HEADER_A_SIZE)) return 0; /* false */

	/* jump to size */
	pos=res->data+PLV_HEADER_A_SIZE+1+PLV_HEADER_B_SIZE;

	/* read size and jump to data */
	res->size=array2long(pos);pos+=4;

	/* integrity check 2 */
	if (oldSize<=PLV_HEADER_A_SIZE+1+PLV_HEADER_B_SIZE+res->size) return 0; /* false */

	/* validate checksum */
	if (!checkSum(pos,res->size))
		fprintf(outputStream,PR_TEXT_IMPORT_PLV_WARN);

	/* save data */
	posAux=res->data;
	res->data=pos;
	mWriteFileInDatFileIgnoreChecksum(res);
	res->data=posAux;

	return 1; /* true */
}
