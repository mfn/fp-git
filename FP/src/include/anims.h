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
anims.h: FreePrince : Animation functions
��������
 Copyright 2005 Princed Development Team
  Created: 5 Jan 2005

  Author: Enrique Calot <ecalot.cod@princed.com.ar>

 Note:
  DO NOT remove this copyright notice
*/

#ifndef _PR_ANIMS_H_
#define _PR_ANIMS_H_

#include "res_conf.h"    /* RES_FILE_*    */
#include "states_conf.h" /* STATES_MARK_* */
#include "anims_conf.h"

/* table types */
typedef struct {
	unsigned short frame;
	unsigned long  res;
	unsigned short duration;
	unsigned char  layer;
	unsigned short x;
	unsigned short y;
} animImage;

typedef struct {
	unsigned short frame;
	unsigned long  res;
	unsigned short duration;
	unsigned short state;
	unsigned short location;
	unsigned char  floor;
	unsigned char  cacheMirror;
} animObject;

typedef struct {
	unsigned short frame;
	unsigned long res;
	enum {anims_enum_midi,anims_enum_wav,anims_enum_speaker} type;
} animSound;

void animStart(int animId,int *qt,int *qf,int *qo);
int animGetFrame(int* qf,int* qt,int* qo, animImage** f,animObject** t, animSound** o);

#endif

