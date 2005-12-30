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
unknown.c: Princed Resources : Unknown resources handler
ŻŻŻŻŻŻŻŻŻŻ
 Copyright 2003 Princed Development Team
  Created: 24 Aug 2003

  Author: Enrique Calot <ecalot.cod@princed.com.ar>
  Version: 1.01 (2003-Oct-23)
  Version: 1.50 (2005-Dec-19)

 Note:
  DO NOT remove this copyright notice
*/

/***************************************************************\
|                  I M P L E M E N T A T I O N                  |
\***************************************************************/

/* Includes */
#include <stdio.h>
#include "tree.h"
#include "common.h"
#include "disk.h"
#include "unknown.h"
#include "memory.h"
#include "stringformat.h"
#include "translate.h"

/***************************************************************\
|                     Unknown.xml primitives                    |
\***************************************************************/

/* XML generation defines */

#define RES_XML_UNKNOWN_FILES "%t%03n.%e"
#define XML_TAG_RESOURCES     "resources"
#define XML_ATTRV_VERSION     "generated"

tUnknownFile unknownFile;

/***************************************************************\
|                           Logging Layer                       |
\***************************************************************/

int unknownLogStart (const char* file,int optionflag, const char* backupExtension) {
	if (unknownFile.fd) return PR_RESULT_ERR_XML_ALREADY_OPEN; /* File already open */

	/* Use default filename if file is NULL */
	if (!file) file=RES_XML_UNKNOWN_XML;

	/* Remember optionflag and backupExtension */
	unknownFile.optionflag=optionflag;
	unknownFile.backupExtension=strallocandcopy(backupExtension);
	unknownFile.currentDat=NULL;

	/* Read the previous file if exists */
	{
		int error;

		unknownFile.status.folderFirst=NULL;
		unknownFile.status.folderCursor=NULL;
		unknownFile.tree=parseXmlFile(file,&error);
		if (error) {
			unknownFile.tree=malloc(sizeof(tTag));
			memset(unknownFile.tree,0,sizeof(tTag));
			unknownFile.tree->version=strallocandcopy(XML_ATTRV_VERSION);
			unknownFile.tree->tag=strallocandcopy(XML_TAG_RESOURCES);
		}
	}
	
	/* Open the file */
	if (!writeOpen(file,&unknownFile.fd,optionflag)) return PR_RESULT_ERR_XML_FILE; /* file not open */

	return PR_RESULT_SUCCESS; /* Ok */
}

int unknownLogStop () {
	int i;
	tTag* t;

	if (!unknownFile.fd) return PR_RESULT_ERR_XML_NOT_OPEN; /* File not open */

	/* it is time to fix the inheritances */
	unknown_fixtreeinheritances();
	
	/* now we'll add the new generated part of the tree at the end of the second level (resources id the first) */
	if (unknownFile.tree) {
		if (unknownFile.tree->child) {
			for (t=unknownFile.tree->child;t->next;t=t->next);
			t->next=unknownFile.status.folderFirst; /* the first folder of the new tree */
		} else {
			unknownFile.tree->child=unknownFile.status.folderFirst; /* the first folder of the new tree */
		}
	}

	/* TODO: create common factor tree reducing function */
	
	/* generate the xml file */
	generateXML(0,unknownFile.tree);

	/* it's time to free the tree */
	freeParsedStructure (&unknownFile.tree);
	
	/* and close the file */
	writeCloseOk(unknownFile.fd,unknownFile.optionflag,unknownFile.backupExtension);

	/* Free structures */
	freeAllocation(unknownFile.currentDat);
	freeAllocation(unknownFile.backupExtension);
	unknownFile.backupExtension=NULL;
	unknownFile.fd=NULL;
	for (i=0;i<RES_TYPECOUNT;i++) unknownFile.typeCount[i]=0; /* re-initialize in 0 for next file processing */

	return PR_RESULT_SUCCESS; /* Ok */
}

int unknownLogAppend(const char* vFiledat,tResourceId id,const char* ext,tResourceType type,const char* vDirExt,tResourceId pal,const char* vFiledatWithPath,int optionflag,int count, unsigned long int flags,const char* filename) {
	if (!unknownFile.fd) return PR_RESULT_ERR_XML_NOT_OPEN; /* File not open, logging is off, just a warning */

	if (!unknownFile.currentDat) { /* this is the beginning of the file */
		unknown_folder(vFiledatWithPath,vFiledat,pal.value,translateInt2Ext(toLower(pal.index)),&unknownFile.status);
		unknownFile.currentDat=strallocandcopy(vFiledat);
		/* TODO: move here the read-parsing-loading and write-opening */
		unknown_deletetreefile(vFiledat);
	} else if (!equalsIgnoreCase(unknownFile.currentDat,vFiledat)) {
		int i;
		unknown_folder(vFiledatWithPath,vFiledat,pal.value,translateInt2Ext(toLower(pal.index)),&unknownFile.status);
		freeAllocation(unknownFile.currentDat);
		unknownFile.currentDat=strallocandcopy(vFiledat);
		unknown_deletetreefile(vFiledat);
		for (i=0;i<RES_TYPECOUNT;i++) unknownFile.typeCount[i]=0; /* re-initialize in 0 for next file processing */
	}

	unknown_item(id.value,translateInt2Ext(toLower(id.index)),filename,getExtDesc(type),flags,getExtDesc(type),count,&unknownFile.status);

	return PR_RESULT_SUCCESS;
}

/***************************************************************\
|                     Middle layer function                     |
\***************************************************************/

void getFileName(char* vFileext,const char* vDirExt,const tResource* r,const char* vFiledat, const char* vDatFileName,int optionflag, const char* backupExtension,const char* format) {
	static const char* extarray[]=RES_FILE_EXTENSIONS;
	const char* filename;
	int pos;

	if (r->path==NULL) {
		pos=((r->type<RES_TYPECOUNT)&&(r->type>=0))?r->type:eResTypeBinary;
		unknownFile.typeCount[pos]++;

		/* set filename */
		if (!format) format=RES_XML_UNKNOWN_FILES;
		filename=parseformat(format,r->id.value,r->id.index,getExtDesc(pos),extarray[pos],unknownFile.typeCount[pos],r->id.order,r->desc);

		sprintf(vFileext,"%s/"RES_XML_UNKNOWN_PATH"/%s/%s",vDirExt,vDatFileName,filename);
		unknownLogAppend(vDatFileName,r->id,extarray[pos],r->type,vDirExt,r->palette,vFiledat,optionflag,unknownFile.typeCount[pos],r->flags,filename);
	} else {
		/* set filename */
		sprintf(vFileext,"%s/%s",vDirExt,r->path);
	}
}

