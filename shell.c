#include <stdio.h>

int main(){
char *in1 = "view ";
char *in2 = "execute ";
char *in3 = "delete ";
char *in5 = "copy ";
char *in6 = "create ";
char *in7 = "kill " ;
char buffer[90];
char returnedbuffer[13312];
char inputbuffer[6];
char inputbuffer2[7];
char content[400];
int i;
int j;
int viewFlag=1;
int executeFlag=1;
int deleteFlag=1;
int copyFlag=1;
int createFlag=1;
int killFlag=1;
char *bad = "Bad command!\0";

// interrupt(0x21, 0, "entered shell\n\0", 0, 0);

while(1){
interrupt(0x21, 0,"shell$ \0",0,0);
interrupt(0x21, 1, buffer, 0, 0);

// change all the i to j

	for(j=0; j<5; j++){
	   if(in1[j]!=buffer[j]){
			viewFlag = 0;
			break;
	  }
	}

	for(j=0; j<8; j++){
	   if(in2[j]!=buffer[j]){
			executeFlag = 0;
			break;
	  }
	}

	for(j=0; j<7 ;j++){
		if(in3[j]!=buffer[j]){
			deleteFlag = 0;
			break;
		}
	}

	for(j=0; j<5 ;j++){
		if(in5[j]!=buffer[j]){
			copyFlag = 0;
			break;
		}
	}

	for(j=0; j<7 ;j++){
		if(in6[j]!=buffer[j]){
			createFlag = 0;
			break;
		}
	}

	for(j=0; j<5; j++){
	   if(in7[j]!=buffer[j]){
			killFlag = 0;
			break;
	  }
	}

	if (viewFlag == 0 && executeFlag == 0 && deleteFlag == 0 && copyFlag == 0 && createFlag == 0&&killFlag==0) {
		interrupt(0x21, 0, bad, 0, 0);
		interrupt(0x10, 0xE*256+0xa , 0, 0, 0);
		interrupt(0x10, 0xE*256+0xd , 0, 0, 0);
		// viewFlag = 1;
		// executeFlag = 1;
		// deleteFlag = 1;
		// interrupt(0x21, 5, "tstprg\0", 0x2000, 0);
	}

if (killFlag){

if(buffer[5]=='0'){
	interrupt(0x21,9,0,0, 0);
			interrupt(0x21, 5, "tstprg\0", 0x2000, 0);
			interrupt(0x10, 0xE*256+0xa , 0, 0, 0);
			interrupt(0x10, 0xE*256+0xd , 0, 0, 0);
}else{
	if(buffer[5]=='1'){
	 interrupt(0x21,9,1, 0, 0);
	 		interrupt(0x21, 5, "tstprg\0", 0x2000, 0);
			interrupt(0x10, 0xE*256+0xa , 0, 0, 0);
			interrupt(0x10, 0xE*256+0xd , 0, 0, 0);

	}else{
		if(buffer[5]=='2'){
			interrupt(0x21,9,2,0,0);
					interrupt(0x21, 5, "tstprg\0", 0x2000, 0);

		}else{
			if(buffer[5]=='3'){
			interrupt(0x21,9,3,0,0);
				interrupt(0x21, 5, "tstprg\0", 0x2000, 0);

			}else{
				if(buffer[5]=='4'){
				interrupt(0x21,9,4,0,0);
						interrupt(0x21, 5, "tstprg\0", 0x2000, 0);

				}else{
					if(buffer[5]=='5'){
				interrupt(0x21,9,5,0,0);
						interrupt(0x21, 5, "tstprg\0", 0x2000, 0);

				}else{
					if(buffer[5]=='6'){
				interrupt(0x21,9,6,0,0);
						interrupt(0x21, 5, "tstprg\0", 0x2000, 0);

				}else{
					if(buffer[5]=='7'){
				interrupt(0x21,9,5,0,0);
						interrupt(0x21, 5, "tstprg\0", 0x2000, 0);

				}else{
				interrupt(0x21, 0,"invalid process number!\0",0,0);
						// interrupt(0x21, 5, "tstprg\0", 0x2000, 0);
						interrupt(0x10, 0xE*256+0xa , 0, 0, 0);
						interrupt(0x10, 0xE*256+0xd , 0, 0, 0);

				}
				}
				}
				}
			}
		}
	}
}

}


	if(viewFlag){
		 for(j=5; j<=10;j++){ // file names are 6 bytes
			inputbuffer[j-5]=buffer[j];
		 }

		interrupt(0x21,3 , inputbuffer, returnedbuffer, 0);
		interrupt(0x21, 0,returnedbuffer,0,0);
		returnedbuffer[0] = '\0';
		// viewFlag = 1;
		// executeFlag = 1;
		// deleteFlag = 1;
		// interrupt(0x21, 5, "tstprg\n\0", 0x2000, 0);
	}

	if(executeFlag) {
		 for(j=8; j<=13;j++){ // file names are 6 bytes
			inputbuffer[j-8]=buffer[j];
		 }
		interrupt(0x21, 4, inputbuffer, 0x2000, 0);
		// returnedbuffer[0] = '\0';

		//
		// interrupt(0x21, 5, "tstprg\0", 0x2000, 0);
	}

	if(createFlag) {
		 for(j=7; j<=12;j++){ // file names are 6 bytes
			inputbuffer[j-7]=buffer[j];
		 }

		 interrupt(0x21, 1, content, 0, 0);
		 interrupt(0x21,8 , inputbuffer, content, 2);

		// viewFlag = 1;
		// executeFlag = 1;
		// deleteFlag = 1;
		// interrupt(0x21, 5, "tstprg\0", 0x2000, 0);
	}

	if(copyFlag) {
		for (j = 5; j <= 10; j++) {
			inputbuffer[j - 5] = buffer[j];
		}
		for (j = 12; j <= 17; j++) {
			 inputbuffer2[j - 12] = buffer[j];
		}

		interrupt(0x21,3 , inputbuffer, returnedbuffer, 0);
		interrupt(0x21,8 , inputbuffer2, returnedbuffer, 2);
		// viewFlag = 1;
		// executeFlag = 1;
		// deleteFlag = 1;
		// interrupt(0x21, 5, "tstprg\0", 0x2000, 0);
	}

	if(deleteFlag) {
			for (j=7; j<=12; j++) {
				inputbuffer[j-7]=buffer[j];
			}
			interrupt(0x21,3 , inputbuffer, returnedbuffer, 0);

			if(returnedbuffer[0]==0x00){
					interrupt(0x21, 0,"file not found!!\n\0",0,0);
			}
			else{
				interrupt(0x21, 7, inputbuffer, 0, 0);
				interrupt(0x21, 0,"file deleted!!\n\0",0,0);
			}
			// viewFlag = 1;
			// executeFlag = 1;
			// deleteFlag = 1;
			// interrupt(0x21, 5, "tstprg\0", 0x2000, 0);
	}
	viewFlag=1;
	executeFlag=1;
	deleteFlag=1;
	copyFlag=1;
	createFlag=1;
	killFlag=1;
}

}
