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
reslist.c: Princed Resources : Resource list layer implementation
���������
 Copyright 2005 Princed Development Team
  Created: 04 Jun 2005

  Author: Enrique Calot <ecalot.cod@princed.com.ar>
  Version: 1.00 (2005-Jun-04)

 Note:
  DO NOT remove this copyright notice
*/

/* Defines */
#include "memory.h" /* freeAllocation */
#include "reslist.h"
#include <stdio.h> /* debug */
#include <string.h> /* strncmp */

/* resource list layer (that uses the abstract list layer primitives) */

int resourceListCompareId(const tResourceId a,const tResourceId b) {
	/* the index has the priority */
	int c=strncmp(a.index,b.index,5);
	if (c>0) return GT;
	if (c<0) return LT;

	/* at this point, the indexes are the same, so let's compare the order */
	if (a.order>b.order) return GT;
	if (a.order<b.order) return LT;

	/* at this point, indexes and order are the same, but this is not enough, we'll use value to be sure it's unique */
	if (a.value>b.value) return GT;
	if (a.value<b.value) return LT;

	return EQ;
}

int reslist_compare(const void* a,const void* b) {
	return resourceListCompareId(((tResource*)a)->id,((tResource*)b)->id);
}

void freeResource(void* a) {
	tResource* res=a;
	freeAllocation(res->desc);
	freeAllocation(res->name);
	freeAllocation(res->path);
}

const tResource* resourceListGetElement(tResourceList* r) {
	const tResource* ret=list_getCursor(r);
	if (ret) list_nextCursor(r);
	return ret;
}

tResourceList resourceListCreate(int isCopy) {
	return list_create(sizeof(tResource),reslist_compare,isCopy?NULL:freeResource);
}

void resourceListAdd(tResourceList* r,tResource* res) {
	/* first try to detect if the resource exists */
/*	res->id.order=0;
	while (list_moveCursor(r,res)) {
		printf("id=(%d,%s,%d)\n",res->id.value,res->id.index,res->id.order);
					res->id.order++; * increase order until the id is unique *
	}*/
	list_insert(r,res);
}

#ifdef DEBUG_RESPRINT
void printr(const tResource* record) {
		printf("id=(%d,%s,%d)\n",record->id.value,record->id.index,record->id.order);
		printf("palette=(%d,%s)\n",record->palette.value,record->palette.index);
		printf("size=%ld offset=%lu\n",record->size,record->offset);
		printf("number=%d type=%d\n",record->number,record->type);
		printf("path='%s' name='%s' desc='%s'\n\n",record->path,record->name,record->desc);
}

void resourceListDebugPrint(tResourceList* r) {
	const tResource* record;
	list_firstCursor(r);
	while ((record=list_getCursor(r))) {
		printr(record);
		list_nextCursor(r);
	}
	list_firstCursor(r);
}
#endif

void resourceListAddInfo(tResourceList* r,tResource* res) {
	if (!list_moveCursor(r,res)) {
		res->path=NULL;
		res->desc=NULL;
		res->name=NULL;
		res->palette.value=0;
		res->palette.order=0;
		memset(res->palette.index,0,5);
		res->number=0;
		res->type=0;
	} else {
		const tResource* resInfo=list_getCursor(r);
		/* copy only the list information */
		res->path=resInfo->path;
		res->desc=resInfo->desc;
		res->name=resInfo->name;
		res->palette=resInfo->palette;
		res->number=resInfo->number;
		/* If resource type is invalid or 0, the type will be decided by PR */
		res->type=resInfo->type;
	}
}

