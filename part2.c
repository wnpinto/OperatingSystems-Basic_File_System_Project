#define _REENTRANT
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include "semaphore.c"
#define MAX  10
#define MAX_READS 2

void readT();
void printT();
void readFileList();
void * threadRequest();
void performRequests();
void readF(int fn);
void writeF(int fNum, int newBlockSize);
void deleteF(int fNum);

typedef int semaphore;
typedef int rwd_sempahore;
int sem1;
int reads;
int sem2;
int space;

struct file {
	int fileNum;
	int blockSize;
	int size;
	int blockNum;
	struct file *next;
};

struct file *fileHead = NULL;
struct file *fileCurrent = NULL;

void main(int argc, char *argv[]){
	
	int *option;
	readT();
	
	while(option != -1){

	printf("[1] Perform Requests\n");
	printf("[2] Print File List\n");
	printf("[-1] EXIT\n");

	scanf("%d", &option);
		if(option == 1) 
			performRequests();
		else if(option == 2)
			printT();
		else if(option == -1)
			printf("Program exiting...\n");
		else
			printf("Invalid input.");
	}
}

void readT(){
// printf("PASSwwwwwS1");

	FILE *f;
	f = fopen("filelist.txt", "r");

	int *fnum, *fbsize, *fsize;
	char *f_name;

	struct file *headFile = (struct file *)malloc(sizeof(struct file));
	headFile->fileNum = 0;
	headFile->blockSize = 0;
	headFile->size = 0;
	headFile->blockNum = 0;
	headFile->next = NULL;

//	printf("22222222");
	fileHead = headFile;
	fileCurrent = fileHead;	
//	printf("wwwwwwwwwwwwwwww");	
	int bNum = 0;

	while(fscanf(f, "%d %d %d %s", &fnum, &fbsize, &fsize, &f_name) != EOF){
	
		bNum = fileCurrent->blockNum + fileCurrent->blockSize;	
		struct file *newFile = (struct file *)malloc(sizeof(struct file));
		newFile->fileNum = fnum;
		newFile->blockSize = fbsize;
		newFile->size = fsize;
		newFile->blockNum = bNum;
 		space+=(newFile->blockSize);
		printf("HDD Space: %d\n", space);
		fileCurrent->next = newFile;
		fileCurrent = fileCurrent->next;
//	 printf("PASSS3");

	}
}

void printT(){
	fileCurrent = fileHead;
	space = 0;

	while(fileCurrent->next != NULL){
		printf("FILE: %d [%d block #] [%d blocks] [%d size]\n", fileCurrent->fileNum, fileCurrent->blockNum, fileCurrent->blockSize, fileCurrent->size);
		space+=(fileCurrent->blockSize);
		fileCurrent = fileCurrent->next;
	}	

	printf("HDD Space: %d\n", space);

}

void readFileList(){
		fileCurrent = fileHead;

		while(fileCurrent != NULL){
			printf("%d", fileCurrent->fileNum);
			fileCurrent = fileCurrent->next;
		}
}

void performRequests(){

	pthread_t thread_id[MAX];
	int status, *p_status = &status;

	srand(time(NULL));
	int i;
	int argc = 1;
	sem1 = createSem();
	
	for(i = 0; i < MAX; i++){
		if(pthread_create(&thread_id[i], NULL, threadRequest, NULL) > 0)
		printf("THREAD FAILED TO CREATE\n");
	}

	for(i = 0; i < MAX; i++){
                if(pthread_join(thread_id[i], (void **) p_status) > 0)
        	printf("THREAD FAILED TO CREATE\n");

	}

	printf("Thread returned %d returns %d\n", thread_id[i], status);
}

void * threadRequest(){
	 down(sem1);
	int randFile = rand() % 8 + 1;
        int randReq = rand() % 3 + 1;
        char op;
        
	//enter critical region 
	FILE *f;
        f = fopen("fileoperations.txt", "a+");
                
        if(randReq == 1){
             op = 'R';
	     //run read thread.
		
	     readF(randFile);
	}
        else if(randReq == 2){
             op = 'W';
	     int randNewBlocks = rand() % 40 + 1;
	     int randNewBytes = randNewBlocks * 512;
	//	if((totalSpace+randNewBlocks) < 400){
			printf("**WRITING TO FILE %d [+%d blocks]\n**", randFile, randNewBlocks);
			writeF(randFile, randNewBytes);
	//	}
	//	else
	//		printf("WRITE ERROR: Not enough hard disk space. MUST DELETE FILES\n");
	}
        else{
             op = 'D';
	      printf("DELETE FILE %d\n", randFile);
		deleteF(randFile);
	}
	fprintf(f, "%d %c\n", randFile, op);
	fclose(f);
	//exit critical region
	up(sem1);
	return (void *) NULL;
}

/*Thread that handles read statements*/
void readF(int fn){
	FILE *f;
	f = fopen("fileLog.txt", "a+");


	fileCurrent = fileHead;
	int fileReadStatus = 0;

	while(fileCurrent->next != NULL){
		if(fn == fileCurrent->fileNum){
			fileReadStatus = 1;
			fprintf(f, "%d\t R\t [%d blocks] [%d size] Success\n", fileCurrent->fileNum, fileCurrent->blockSize, fileCurrent->size);
			break;
		}
		
		fileCurrent = fileCurrent->next;

	}
	if(fileReadStatus == 0)
		 fprintf(f, "%d\t R\t [%d blocks] [%d size] FAILED - file not found\n", fileCurrent->fileNum, fileCurrent->blockSize, fileCurrent->size);

	fclose(f);
}

void writeF(int fNum, int newFbytes){
	int newFblocks = newFbytes / 512;
	int bNum = 0;
	fileCurrent = fileHead;
        int fileExists = 0;
	int bn = 0;
	FILE *f;
        f = fopen("fileLog.txt", "a+");
	space = 0;
	while(fileCurrent->next != NULL){
		space+=(fileCurrent->blockSize);
	
		if(space >= 400){
			 fprintf(f, "%d\t W\t [%d blocks] [%d size] FAILED - HDD FULL.\n", fNum, newFblocks, newFbytes);

			break;
		}
                else if(fNum == fileCurrent->fileNum){
                	fileExists = 1;
		        int currBlock = fileCurrent->blockSize + newFblocks;
			
			/*test whether the current file has all its blocks 
			occupied when adding new blocks. If it exceeds 40, 
			add new block to the end of file.
			Add pointer of current block to the end block.*/
			
			int blockRemaining = 40 - fileCurrent->blockSize;
			
				if((fileCurrent->blockSize + newFblocks) > 40){
					//create new block at end of stucture if not enough space.
					struct file *newFile = (struct file *)malloc(sizeof(struct file));
			                newFile->fileNum = fNum;
                			newFile->blockSize =newFblocks;
                			newFile->size = newFblocks*512;
					newFile->blockNum = space;
					fprintf(f, "%d\t W\t [%d blocks] [%d size] Success - New node created.\n", fileCurrent->fileNum, fileCurrent->blockSize, fileCurrent->size);

					struct file *temp = fileCurrent->next;
					fileCurrent->next = newFile;
					fileCurrent = fileCurrent->next;
					fileCurrent->next = temp;
					break;
				} 
				
				else if (blockRemaining > 0 && newFblocks <= blockRemaining){
					//add new block at current node if enough space.
					printf("adding new blocks...%d\n", newFblocks);	
					fileCurrent->blockSize = currBlock;
					fileCurrent->size = currBlock*512;
					fileCurrent->blockNum = space; 
					fprintf(f, "%d\t W\t [%d blocks] [%d size] Success\n", fileCurrent->fileNum, fileCurrent->blockSize, fileCurrent->size);
					break;
				}
	
                }
			bn = fileCurrent->blockNum + fileCurrent->blockSize;
			fileCurrent = fileCurrent->next;	
        }

	if(fileExists == 0 && (space + newFblocks)< 400){
		struct file *newFile = (struct file *)malloc(sizeof(struct file));
                newFile->fileNum = fNum;
                newFile->blockSize =newFblocks;
                newFile->size = newFblocks*512;
                newFile->blockNum = space;
		fileCurrent->next = newFile;

		fprintf(f, "%d\t W\t [%d blocks] [%d size] FAILED - creating new file...\n", fileCurrent->next->fileNum, fileCurrent->next->blockSize, fileCurrent->next->size);

		fprintf(f, "%d\t W\t [%d blocks] [%d size] Success - created new file.\n", fileCurrent->next->fileNum, fileCurrent->next->blockSize, fileCurrent->next->size);

	}
	fclose(f);
}


/*delete a file completely*/
void deleteF(int fNum){
	FILE *f;
        f = fopen("fileLog.txt", "a+");
	int fileDeleteStatus = 0;
	int fileDeleted = 0;
	fileCurrent = fileHead->next->next;
	
	while(fileCurrent->next != NULL){
		if(fileCurrent->next->next != NULL){
			if(fileCurrent->next->fileNum == fNum){
				fileDeleteStatus = 1;
				fprintf(f, "%d\t D\t [%d blocks] [%d size] Success\n", fileCurrent->next->fileNum, fileCurrent->next->blockSize, fileCurrent->next->size);		
				fileCurrent->next = fileCurrent->next->next;
				fileDeleted = 0;
			}
			
		}
	if(fileDeleted == 1)
		fileDeleted = 0;
	else
		fileCurrent = fileCurrent->next;
	
	}

	if(fileDeleteStatus == 0)
		 fprintf(f, "%d\t D\t [0] [0] FAILED - No such file...\n", fNum);

	fclose(f);
}

void logFiles(char operation, char status){

}
