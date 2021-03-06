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
sound.h: Princed Resources : Common sound objects interfaces
�������
 Copyright 2006 Princed Development Team
  Created: 19 Feb 2006

  Author: Enrique Calot <ecalot.cod@princed.com.ar>

 Note:
  DO NOT remove this copyright notice
*/

#ifndef _PR_SOUND_H_
#define _PR_SOUND_H_

#include "binary.h"
#include "types.h"

#include "mid.h"
#include "wav.h"

typedef enum {
	magicMidi=0x02,
	magicPcspeaker=0x00,
	magicWave=0x01
} tSoundType;

int objectSoundWaveWrite(void* o, const char* file, int optionflag, const char* backupExtension);

void* objSoundCreate(tBinary c, int *error);
#define objectSoundWaveCreate(a,b)      objSoundCreate(a,b)
#define objectSoundMidiCreate(a,b)      objSoundCreate(a,b)
#define objectSoundPcspeakerCreate(a,b) objSoundCreate(a,b)

void* objectSoundWaveRead(const char* file, int *result);
#define objectSoundMidiRead(a,b) objectOtherBinaryRead(a,b)
#define objectSoundPcspeakerRead(a,b) objectOtherBinaryRead(a,b)

int objectSoundSet(void* o,tResource* res, tSoundType soundType);
#define objectSoundWaveSet(a,b) objectSoundSet(a,b,magicWave)
#define objectSoundPcspeakerSet(a,b) objectSoundSet(a,b,magicPcspeaker)
#define objectSoundMidiSet(a,b) objectSoundSet(a,b,magicMidi)

#define objectSoundMidiWrite(a,b,c,d) objectBinaryWrite(a,b,c,d)
#define objectSoundPcspeakerWrite(a,b,c,d) objectBinaryWrite(a,b,c,d)

#endif
