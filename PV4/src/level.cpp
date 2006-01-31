#include <iostream>
#include "endiansafestream.h"

#include "levelFormat.h"
#include "pop1levelFormat.h"
#include "level.h"
#include "guard.h"
#include "tile.h"
#include "outertile.h"

using namespace std;
/*
                   Table 6.1: PLV blocks
                   ~~~~~~~~~~~~~~~~~~~~~

   Size Offset Description                  Type   Content
   ~~~~ ~~~~~~ ~~~~~~~~~~~                  ~~~~   ~~~~~~~
      7      0 Magic identifier             text   "POP_LVL"
      1      7 POP version                  UC     0x01
      1      8 PLV version                  UC     0x01
      1      9 Level Number                 UC
      4     10 Number of fields             UL
      4     14 Block 1: Level size (B1)     UL     2306/2305
     B1     18 Block 1: Level code          -
      4  18+B1 Block 2: User data size (B2) UL
     B2  22+B1 Block 2: User data           -
*/
typedef char* fieldPointer;

Level::Level(const char* file) { //open
	//open the file
  iesstream stream(file);

	//check the magic
	char magic[7];
	stream.read(magic,7);
	if (strncmp(magic,"POP_LVL",7)) throw -3;

	//read pop version
	unsigned char popVersion;
	stream.read(popVersion);

	//read plv version
	unsigned char plvVersion;
	stream.read(plvVersion);

	//level number
	unsigned char level;
	stream.read(level);

	//number of fields
	unsigned long nf;
	stream.read(nf);

	//level size
	unsigned long b1;
	stream.read(b1);

	//level code
	switch (popVersion) {
		case 1:
			this->level=new Pop1LevelFormat(stream,b1);
		case 2:
//			this->level=new Pop2LevelFormat(stream,b1);
		default:
			throw -2;
	}

	//user data size
	unsigned long b2;
	stream.read(b2);

	//alloc user data (TODO: use integrity checks here)
	char* ud=new char[b2];
	stream.read(ud,b2);

	//process user data
	fieldPointer* fields=new fieldPointer[nf*2];
	int currentField=1;

	fields[0]=ud;
	for (int i=0;i<b2&&currentField<nf*2;i++) {
		if (!ud[i]) fields[currentField++]=ud+i;
	}

	if (currentField!=nf*2-1||ud[b2-1]!=0) throw -2;

	//TODO: generate a hash table with this values

	//remember the file name
	this->fileName=new string(file);

	//Finally arrange the rooms
	this->arrangeRooms();
}

Level::Level(int popVersion,int LevelNumber){} // new

void Level::save(){}
void Level::save(const char* file){}

Level::~Level(){}

/*
plvInfo Level::getInfo()
Level::setInfo(plvInfo i)
*/

void Level::linkRecurse(int x, int y, int room) {
 if (matrix(x,y)==-1) {
  matrix(x,y)=room;
  if (room) {
   int up,down,left,right;
   this->level->getRoomLink(room,left,right,up,down);
   linkRecurse(x+1,y,right);
   linkRecurse(x-1,y,left);
   linkRecurse(x,y+1,down);
   linkRecurse(x,y-1,up);
  }
 } else {
  if (matrix(x,y)!=room) throw -1;
 }
}

void Level::arrangeRooms() {
 //initialize matrix
 for (int i=0;i<SIZE_OF_MATRIX;i++)
  this->screenMatrix[i]=-1;

 int startScreen,junk;
 Direction junk2;

 //Fill in the matrix with the rooms
 getStartPosition(startScreen,junk,junk2);
 linkRecurse(MATRIX_CENTER_X,MATRIX_CENTER_Y,startScreen);

 //Get the matrix limits
 for (int i=0;i<MATRIX_HEIGHT;i++) {
  for (int j=0;j<MATRIX_WIDTH;j++) {
   if (!matrix(i,j)) {
    this->rs=i;
    i=j=MATRIX_WIDTH;
   }
  }
 }

 for (int i=MATRIX_HEIGHT;i--;) {
  for (int j=0;j<MATRIX_WIDTH;j++) {
   if (!matrix(i,j)) {
    this->re=i;
    i=0;
    j=MATRIX_WIDTH;
   }
  }
 }

 for (int i=0;i<MATRIX_WIDTH;i++) {
  for (int j=0;j<MATRIX_HEIGHT;j++) {
   if (!matrix(j,i)) {
    this->cs=i;
    i=j=MATRIX_WIDTH;
   }
  }
 }

 for (int i=MATRIX_WIDTH;i--;) {
  for (int j=0;i<MATRIX_HEIGHT;j++) {
   if (!matrix(j,i)) {
    this->ce=i;
    i=0;
    j=MATRIX_WIDTH;
   }
  }
 }

	//Now it's time to add all screens in the screens array
	this->screens=new int[this->level->countMax()];
	for (int i=0;i<this->level->countMax();i++)
		this->screens[i]=0;

	for (int i=this->rs;i<this->re;i++) {
		for (int j=this->cs;i<this->ce;i++) {
			if (matrix(i,j)>0) {
				if (matrix(i,j)>this->level->countMax()) throw -13;
				this->screens[matrix(i,j)]=MATRIX_WIDTH*j+i;
			}
		}
	}
}

int Level::getHeight(){
 return (this->rs-this->re-2)*3+2;
}


int Level::getWidth(){
 return (this->cs-this->ce-2)*10+2;
}

int Level::countRooms(){}

bool Level::addGuard(int floor,int col,Guard g){}
bool Level::delGuard(int floor,int col){}
bool Level::moveGuard(int floor,int col,int nfloor,int ncol){}
bool Level::getGuard(int floor,int col,Guard &g){}
/*vector <floor,col> getGuards()*/


void Level::setTile(int floor,int col,Tile* tile) {
	int screen;
	int location;

	this->abstractToFormat(floor,col,screen,location);

	//check if the screen exists
	if (screen<1) {
		if (screen==-1) throw -80; //room is too far and cannot be displayed in the level

		//the screen doesn't exist, but it's near, we'll try to create it
		int x,y;
		this->floorColToXY(floor,col,x,y);

		screen=this->addScreen(x,y);
	}

	//and now set the tile

}

int Level::addScreen(int x, int y) {
	int result=-1;
	for (int i=0;i<this->level->countMax() && result==-1;i++) {
		if (!this->screens[i]) result=i;
	}
	if (result==-1) throw -87; //level full

	//Link the new screen
	int left,right,up,down;
	int screen;

	screen=matrix(x-1,y); //left
	if (screen>0) {
		this->level->getRoomLink(screen,left,right,up,down);
		this->level->setRoomLink(screen,left,result,up,down);
	}

	screen=matrix(x+1,y); //right
	if (screen>0) {
		this->level->getRoomLink(screen,left,right,up,down);
		this->level->setRoomLink(screen,result,right,up,down);
	}

	screen=matrix(x,y+1); //down
	if (screen>0) {
		this->level->getRoomLink(screen,left,right,up,down);
		this->level->setRoomLink(screen,left,right,result,down);
	}

	screen=matrix(x,y-1); //up
	if (screen>0) {
		this->level->getRoomLink(screen,left,right,up,down);
		this->level->setRoomLink(screen,left,right,up,result);
	}

	this->level->setRoomLink(result,matrix(x-1,y),matrix(x+1,y),matrix(x,y-1),matrix(x,y+1));

	//recalculate
	this->arrangeRooms();
}

//TODO: delScreen

Tile* Level::getTile(int floor,int col) {
	int screen;
	int location;

	this->abstractToFormat(floor,col,screen,location);

	if (screen<1) {
		int r=0;
		if (!screen) { //only screens 0 (means near to a level)
			//calculate x and y of the screen
			int x,y;
			this->floorColToXY(floor,col,x,y);
			if (col%10==0  && matrix(x-1,y)>0) r|=DL;
			if (col%10==9  && matrix(x+1,y)>0) r|=DR;
			if (floor%3==0 && matrix(x,y-1)>0) r|=DU;
			if (floor%3==2 && matrix(x,y+1)>0) r|=DD;
		}
		return (Tile*)new OuterTile(r);
	} else {
		return this->level->getTile(screen,location);
	}
}

void copyTiles(int sfloor,int scol,int efloor,int ecol) {}

/*
bool addTrigger(int triggerfloor,int triggercol,int targetfloor,int targetcol)
bool delTrigger(int triggerfloor,int triggercol,int targetfloor,int targetcol)
vector <floor,col> getTargets(int triggerfloor,int triggercol)
vector <floor,col> getTriggers(int targetfloor,int targetcol)
*/
void Level::getStartPosition(int &floor,int &col,Direction &direction){
	int screen;
	int location;
	this->level->getStartPosition(screen,location,direction);
	this->formatToAbstract(floor,col,screen,location);
}

void Level::setStartPosition(int floor,int col,Direction direction){
	int screen;
	int location;
	this->abstractToFormat(floor,col,screen,location);
	if (screen<1) throw -90;
	this->level->setStartPosition(screen,location,direction);
}

bool Level::getDebugPosition(int &floor,int &col,Direction &direction){
	int screen;
	int location;
	this->level->getDebugPosition(screen,location,direction);
	this->formatToAbstract(floor,col,screen,location);
	return screen!=0;
}

void Level::setDebugPosition(int floor,int col,Direction direction){
	int screen;
	int location;
	this->abstractToFormat(floor,col,screen,location);
	if (screen<1) throw -90;
	this->level->setDebugPosition(screen,location,direction);
}

void Level::clearDebugPosition(){
	this->level->clearDebugPosition();
}

void Level::switchPositions(){
	int dscreen;
	int dlocation;
	Direction ddirection;

	this->level->getDebugPosition(dscreen,dlocation,ddirection);

	int sscreen;
	int slocation;
	Direction sdirection;

	this->level->getStartPosition(sscreen,slocation,sdirection);

	if (!dscreen) throw -91; //no debug position set

	this->level->setStartPosition(dscreen,dlocation,ddirection);
	this->level->setDebugPosition(sscreen,slocation,sdirection);
}

/*
bool undo();
bool redo();
*/

//Functions
void Level::floorColToXY(int floor,int col, int &x, int &y){
	x=this->rs+1+col/10;
	y=this->cs+1+floor/3;
}

void Level::abstractToFormat(int floor,int col, int &screen, int &location){

	//ignore the first col and row
	col--;
	floor--;

	//calculate x and y of the screen
	int x,y;
	floorColToXY(floor,col,x,y);

	//return values
	screen=matrix(x,y);
	location=col%10+(floor%3)*10;
}

void Level::formatToAbstract(int &floor,int &col, int screen, int location){
	if (screen<1||screen>this->level->countMax()) throw -14;

	int pos=this->screens[screen];

	int y=pos/MATRIX_WIDTH;
	int x=pos%MATRIX_WIDTH;

	y-=this->rs;
	x-=this->cs;

	col=x*10+1;
	floor=y*3+1;
}
