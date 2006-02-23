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
sound.h: Princed Resources : 
�������
 Copyright 2006 Princed Development Team
  Created: 19 Feb 2006

  Author: Enrique Calot <ecalot.cod@princed.com.ar>

 Note:
  DO NOT remove this copyright notice
*/

#ifndef _SOUND_H_
#define _SOUND_H_

#include "binary.h"
#include "reslist.h"

#include "mid.h"
#include "wav.h"

int objSoundWrite(void* o, const char* file, int write(const char* file,tBinary* data,int optionflag, const char* backupExtension), int optionflag, const char* backupExtension);
void* objSoundCreate(tBinary c, int *error);


#define objWaveWrite(a,b,c,d) objSoundWrite(a,b,writeWav,c,d)
#define objMidiWrite(a,b,c,d) objSoundWrite(a,b,writeMid,c,d)
#define objPcspeakerWrite(a,b,c,d) objBinaryWrite(a,b,c,d)

#define objWaveCreate(a,b) objSoundCreate(a,b)
#define objMidiCreate(a,b) objSoundCreate(a,b)
#define objPcspeakerCreate(a,b) objSoundCreate(a,b)

/*#define objWaveRead(a,b,c,d,e) objSoundRead(a,readWav,b,c,d,e)*/
void* objSoundRead(const char* file, int read(const char* file, tBinary* c, int *pchannels, long *psamplerate, long *pbps), int *result);

void* objWaveRead(const char* file, int *result);

int objWaveSet(void* o,tResource* res);

#endif
