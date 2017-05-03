#include <stdio.h>

// Mile 1

void printString(char*);// task 1
void readString(char*); // task 2
void readSector(char* , int );// task 3
void writeSector(char* , int );// task 3
int mod (int, int);
int div(int, int);
void handleInterrupt21(int, int, int, int);
void terminate();
void deleteFile(char*);

// Mile 2

void readFile(char* , char*) ;
void executeProgram(char* name);
//mile 3
void writeFile(char* name, char* buffer, int secNum);

// mile 4
void handleTimerInterrupt(int, int);
void killProcess(int );

// 8 processes
int active[8] ; // if(active[i]==1)it is currently running
int sp[8] ; // sp[i] = current pos in this process
int currentProcess;// the process currently running
int timer;

int main () {
  // initializing the table array
  int i = 0 ;
  currentProcess = -1 ;
  while(i<8){
    active[i] = 0 ;
    i = i+1 ;
  }

  i = 0  ;
  while(i<8){
    sp[i] = 0xFF00   ;
    i = i+1 ;
  }
  // initializing done

  makeInterrupt21();
  makeTimerInterrupt();
  // interrupt(0x21, 4, "hello1\0", 0, 0);
  interrupt(0x21, 4, "shell\0", 0x2000, 0);
  // interrupt(0x21, 4, "hello2\0", 0, 0);
  // printString("entered kernel\n\0");
  while(1);
}

// Mile 4


void handleTimerInterrupt(int segment, int stackPointer){
	// char s[4];
  // s[0] = 'T';
  // s[1] = 'i';
  // s[2] = 'c';
  // s[3] = '\0';
	// printString(s);
  int i;
  int j;
  int seg;
  timer++;
  if(timer == 100 ) {
    timer=0;
    setKernelDataSegment();
    sp[currentProcess]=stackPointer;
    for (i = currentProcess+1; i < 8; i++) {
      if(active[i]) {
          currentProcess = i;
          seg = (i+2)*0x1000;
          restoreDataSegment();
          returnFromTimer(seg,sp[i]);
      } else if( i<7 && j<8){
        i=-1;
      } else {
        break;
      }
      j++;
    }
    returnFromTimer(segment,stackPointer);
  }
	returnFromTimer(segment,stackPointer);
	// return;
}

// Mile 3

void writeFile(char* name, char* buffer, int secNum){
	char mapSector[512];
	char dirSector[512];
	char emptySectors[26];
	int iMap;
	int iFile;
	int iPerFile;
	int iSec;

	readSector(mapSector, 1);
	readSector(dirSector, 2);

	if( secNum > 26 ) {
	 interrupt(0x21, 0, "ERROR 0x00: FileSystem Cannot Support >26 Sectors Files.\n\0", 0, 0);
		 return;
		}

	iSec = 0; iMap = 0;
	while( iSec<secNum ){

		while( iMap<512 && *(mapSector+iMap)!=0x00 )
			iMap++;
		if( iMap==512 ) {
			interrupt(0x21, 0, "ERROR 0x01: No Disk Space for your file.\n\0", 0, 0);
			 return;
			}
		else {
		 *(emptySectors + iSec) = (iMap);
			 iSec++;
			iMap++;
		 }
	}

	iFile = 0;
	while( iFile<512 && *(dirSector + iFile)!=0x00 )
		iFile += 32;
	if( iFile >= 512 ) {
	 interrupt(0x21, 0, "ERROR 0x02: Current FileSystem Cannot support more files (>13).\n\0", 0, 0);
		 return;
		}
	iPerFile = 0;
	while ( iPerFile<6 && *(name+iPerFile)!='\0')
		*((dirSector+iFile) + iPerFile++) = *(name + iPerFile);
	while ( iPerFile<6 )
		*((dirSector+iFile) + iPerFile++) = 0x00;
	iSec = 0;
	while( (iPerFile-6) < secNum ) {
		*((dirSector + iFile) + iPerFile) = *(emptySectors + iSec);
		*(mapSector + ((*(emptySectors + iSec)) - 1)) = 0xFF;
		writeSector(buffer, *(emptySectors + iSec));
		iPerFile++;
		iSec++;
	}
	writeSector(mapSector, 1);
	writeSector(dirSector, 2);
}
void deleteFile(char* name){
// printString("here\n\0") ;
	int i  = 0 , j = 0 ;
	// 1 - load directory from sector 2
	char directory[512] ;
	readSector(directory , 2 ) ; // read into array  directory from sector 2

	// 2 - find a matching file name in the directory
	while(i < 512){
		char c0 = directory[i]   , c1 = directory[i+1] ; // first 6 bytes are the name
		char c2 = directory[i+2] , c3 = directory[i+3] ; // you didnt handle that the name is less than 6
		char c4 = directory[i+4] , c5 = directory[i+5] ;
			if(c0 == *(name) && c1 == *(name+1)  && c2 == *(name+2) && c3 == *(name+3)
				&& c4 == *(name+4) && c5 == *(name+5) ){// you found a matching name
					directory[i] = 0x00 ;
					for (j = i+6 ; j < i+32 ; j ++){ // loop on the remaining 26 bytes  for sector number
						directory[j] = 0x00;
					}
					break ; // return
				}

		i = i+32 ; // next entry
	}
	writeSector(directory , 2) ;
}

void writeSector(char* buffer, int sector){

	int relSector = mod(sector, 18) + 1 ;
	int head = mod(div(sector, 18), 2);
	int track = div(sector, 36) ;
	int AH = 3;
	int AL = 1;
	int DL = 0;

	int AX = AH * 256 + AL ;
	int CX = track * 256 + relSector ; // CH*256+CL
	int DX = head  *256 + DL ;// DH*256+DL

// interrup takes 5 parameters (opcode , ax  , bx , cx , dx )
	interrupt(0x13,  AX , buffer , CX , DX );
}

// Mile 2 and 4
void executeProgram(char* name){
	char buffer[13312];
	int ptr;
  // Mile 4 part
  int i = 0 ;
  int segment = 0 ; // in mile 2 this was taken as parameter
  setKernelDataSegment();
  for(i = 0 ; i < 8 ; i ++){
    if(!active[i]){
      active[i] = 1 ;
      restoreDataSegment();
      segment = (i+2) * 0x1000;
      break ;
    }
  }
	for(ptr=0; ptr<13312; ptr++)
    buffer[ptr] = 0x0;
	readFile(name,buffer);

	 ptr=0;
	while( ptr<13312 ) putInMemory(segment, ptr++, buffer[ptr]);

	initializeProgram(segment);
}

void readFile(char* fileName, char* buffer){

	int i  = 0 , j = 0 ;
	char* tmp  =  buffer ; // as to return the array as it is
	// 1 - load directory from sector 2
	char directory[512] ;
	readSector(directory , 2 ) ; // read into array  directory from sector 2
	// 2 - find a matching file name in the directory
	// 16 entries each 32 bytes ( 16*32 = 512)
	while(i < 512){ // this loop check if we found the name in directory
		char c0 = directory[i]   , c1 = directory[i+1] ; // first 6 bytes are the name
		char c2 = directory[i+2] , c3 = directory[i+3] ; // you didnt handle that the name is less than 6
		char c4 = directory[i+4] , c5 = directory[i+5] ;
		if(c0 == *(fileName) && c1 == *(fileName+1)  && c2 == *(fileName+2) && c3 == *(fileName+3)
			&& c4 == *(fileName+4) && c5 == *(fileName+5) ){// you found a matching name

			for (j = i+6 ; j < i+32 ; j ++){ // loop on the remaining 26 bytes and check for sector number
				int x = directory[j] ; // current sector numbeer
				if(x==0 )break ; // no more sectors
				readSector(tmp , x);
				tmp = tmp+512 ; // thats why use tmp instead of buffer
			}
			break ; // return
		}

		i = i+32 ; // next entry
	}
}





void readString(char *chars) {

	int i = 0;
	char r;

	while((r = interrupt(0x16,0, 0, 0, 0)) != 0xd) {
		if(r != 0x8) {
			/*
				Here we allow the User to enter maximum 78 Characters
				to leave 2 free spaces at the end of the array to the
				(line feed) & (end of string) characters.
			*/
			if (i == 78) continue;
			chars[i++] = r;
			interrupt(0x10, 0xE*256+r, 0, 0, 0);
		}
		else {
			if (i != 0) {
				interrupt(0x10, 0xE*256+r, 0, 0, 0);
				interrupt(0x10, 0xE*256+' ', 0, 0, 0);
				interrupt(0x10, 0xE*256+r, 0, 0, 0);
				chars[--i] = ' ';
			}
		}
	}

	chars[i++] = 0xa;
	interrupt(0x10, 0xE*256+0xa, 0, 0, 0);
	chars[i] = 0xd;
	interrupt(0x10, 0xE*256+0xd, 0, 0, 0);
}



// Mile 1
void printString(char* chars) {
	char *c;
	for (c = chars; *c != '\0'; c++) {
	  	interrupt(0x10, 0xE*256+(*c) , 0, 0, 0);
	}
}

int div (int x, int y) {

	int res = 0;
	while (x >= y) {
		x -= y;
		res++;
	}

	if (x < 0) res--;
	return res;
}

int mod(int x, int y) {
    int result = x;
    while (result >= y)
        result -=y;

    return result;
}

void readSector(char* buffer, int sector){ // buffer : a predefined character array of 512 bytes or bigger,
											//  and a sector number to read.
	int relSector = mod(sector, 18) + 1 ;
	int head = mod(div(sector, 18), 2);
	int track = div(sector, 36) ;
	int AH = 2;
	int AL = 1;
	int DL = 0;

	int AX = AH * 256 + AL ;
	int CX = track * 256 + relSector ; // CH*256+CL
	int DX = head  *256 + DL ;// DH*256+DL

// interrup takes 5 parameters (opcode , ax  , bx , cx , dx )
	interrupt(0x13,  AX , buffer , CX , DX );
}

void terminate() {
 // char shell[6];
 //  shell[0] = 's';
 //  shell[1] = 'h';
 //  shell[2] = 'e';
 //  shell[3] = 'l';
 //  shell[4] = 'l';
 //  shell[5] = 0x0;
 //
 //  interrupt(0x21, 4, shell, 0x2000, 0);
 // interrupt(0x21, 4, "shell\0", 0x2000, 0);
 // printString("here\n\0");
 setKernelDataSegment();
 active[currentProcess] = 0;
 restoreDataSegment();
 while(1){
  //  printString("stuck here\n\0") ;
 }
}
void killProcess(int process){
  active[process] = 0 ;
}
void handleInterrupt21(int AX, int BX, int CX, int DX) {
	if (AX == 0) printString (BX);
	else if (AX == 1) readString (BX);
	else if (AX == 2) readSector (BX, CX);
	else if (AX == 3) readFile(BX, CX) ; // STEP 1
	else if (AX == 4) executeProgram(BX);
	else if (AX == 5)	terminate();
	else if (AX == 6) writeSector(BX,CX);
	else if (AX==7 ) deleteFile(BX);
	else if(AX==8) writeFile(BX,CX,DX);
  else if(AX==9)killProcess(BX) ;
	else{
		char errMessage[8];
		errMessage[0] = 'E';
		errMessage[1] = 'R';
		errMessage[2] = 'R';
		errMessage[3] = 'O';
		errMessage[4] = 'R';
		errMessage[5] = '!';
		errMessage[6] = 0xa;
		errMessage[7] =  0xa;
		printString(errMessage);
	}
}
