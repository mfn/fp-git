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
binary.c: Princed Resources : Binary object implementation
��������
 Copyright 2006 Princed Development Team
  Created: 17 Feb 2006

  Author: Enrique Calot <ecalot.cod@princed.com.ar>
  Version: 1.01 (2006-Feb-09)

 Note:
  DO NOT remove this copyright notice
*/

/***************************************************************\
|                  I M P L E M E N T A T I O N                  |
\***************************************************************/

#include "binary.h" /* tBinary */
#include "common.h"
#include "dat.h" /* WriteDat */
#include "types.h" /* tResources */
#include "disk.h" /* we are skiping the format layer here */
#include <stdlib.h>

/***************************************************************\
|                         Binary Object                         |
\***************************************************************/

void* objectBinaryCreate(tBinary cont, int *error) { /* use get like main.c */
	tBinary* r;
	*error=PR_RESULT_SUCCESS;

	r=(tBinary*)malloc(sizeof(tBinary));
	*r=cont;
	r->isCopy=1;
	return (void*)r;
}

int objectBinaryWrite(void* o, const char* file, int optionflag, const char* backupExtension) {
	tBinary* b=o;
	return writeData(b->data,0,file,b->size,optionflag,backupExtension)?PR_RESULT_SUCCESS:PR_RESULT_F_FILE_NOT_WRITE_ACCESS;
}

void* objectOtherBinaryRead(const char* file,int *result) {
	tBinary o=mLoadFileArray(file);
	if (o.size<0) {
		*result=o.size;
		return NULL;
	}
	return objectBinaryCreate(o,result); /* TODO: isCopy=0 */
}

int objectOtherBinarySet(void* o,tResource* res) {
	tBinary* bin=o;
	res->content=*bin;
	mWriteFileInDatFile(res);
	return PR_RESULT_SUCCESS;
}

