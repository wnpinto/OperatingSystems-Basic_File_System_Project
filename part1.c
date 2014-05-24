#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void initFiles();
void listFiles();
void createFile();
void readStruct();

struct file{
	int fileNum;
	int blockSize;
	int size;

	struct file *next;
};


struct file *fileTableHead = (struct file*)NULL;
struct file *fileTableCurrent = (struct file *)NULL;
struct file *fileTable_ptr = (struct file *)NULL;

int main(int argc, char *argv[]){


	if(argc > 1){
		if(0 == strcmp(argv[1], "-r"))
			listFiles();
		else if(0 == strcmp(argv[1], "-c"))
			createFile();
		else if(0 == strcmp(argv[1], "-init"))
			initFiles();
		else if(0 == strcmp(argv[1], "-d"))
			deleteFile(argv[2]);
		else if(0 == strcmp(argv[1], "-rn"))
                        renameFile(argv[2], argv[3]);
		else if(0 == strcmp(argv[1], "-rs"))
			readStruct();

	}
	else{
		printf("Not enough Arguments.\n");
	}

exit(0);
}

void initFiles(){
        srand(time(NULL));
        /*generate 10 files with random size. 1 block = 512 bytes. 1 file 1-40 blocks*/
        FILE *file;
        file = fopen("filelist.txt", "w+");
       int i = 0;
	

	printf("sssssssssssssssssssssss");
        /*Create new file table head*/
        fileTable_ptr = (struct file*)malloc(sizeof(struct file));
        fileTable_ptr->fileNum=0;
        fileTable_ptr->blockSize=0;
        fileTable_ptr->size=0;

        fileTableHead = fileTable_ptr;
        fileTableCurrent = fileTableHead;

	
	for(i = 0; i < 10; i++){

                int size = rand() % 40 + 1;     /*Generate size file blocks 1-40*/
                int bytes = size*512;           /*calculate bytes per block*/
	        char buffer[20];
        	char buff2[10];

       		strcpy(buffer, "file_");
        	sprintf(buff2, "%d", i);
        	strcpy(buffer, buff2);

		/*make new file node*/
		struct file *newFile_ptr = (struct file*)malloc(sizeof(struct file));

		newFile_ptr->fileNum=i;
		newFile_ptr->blockSize=size;
		newFile_ptr->size=bytes;
			
		fileTableCurrent->next = newFile_ptr;
		fileTableCurrent = fileTableCurrent->next;	
	
                fprintf(file,"%d %d %d %s\n", i, size, bytes, buffer);
        }
      fclose(file);


	fileTableHead = fileTableHead->next;
	fileTableCurrent = fileTableHead;
	while(fileTableCurrent->next != NULL){
		printf("NODE: %d", fileTableCurrent->fileNum);
		fileTableCurrent = fileTableCurrent->next;
	}
}

void listFiles(){
	FILE *file;
	file = fopen("filelist.txt", "r");
	
	int *f_num, *f_block, *f_size; 
	char *f_name;


	while(fscanf(file, "%d %d %d %s", &f_num, &f_block, &f_size, &f_name) != EOF){
		printf("file: %d\n", f_num);
	}
	fclose(file);
}



void createFile(){
	srand(time(NULL));
	FILE *file;
        file = fopen("filelist.txt", "r");

	int *file_num=0;
	int *file_block, *file_size;
	char *file_name;
	int i=0;

       	while(fscanf(file, "%d %d %d %s", &file_num, &file_block, &file_size, &file_name) != EOF){i++;}
	fclose(file);
        file = fopen("filelist.txt", "a+");

	int size = rand() % 40 + 1;     /*Generate size file blocks 1-40*/
        int bytes = size*512;           /*calculate bytes per block*/
        
	 char buffer[20];
         char buff2[10];
         strcpy(buffer, "file_");
         sprintf(buff2, "%d", i);
         strcpy(buffer, buff2);

	fprintf(file,"%d %d %d %s\n", i, size, bytes,buffer);
        
        fclose(file);
}


void renameFile(char *file_name, char *replace){
        FILE *file;
        file = fopen("filelist.txt", "r");
        int *file_num=0;
        int *file_block, *file_size;
        int i=0;
	char *f_name;

        while(fscanf(file, "%d %d %d %s", &file_num, &file_block, &file_size, &f_name) != EOF){
		i++;
		if(0==strcmp(&f_name, file_name))
			break;
	}
        fclose(file);
        
	FILE *ff,*ff2;
	ff = fopen("filelist.txt", "r");
	ff2 = fopen("filelist2.txt", "w");

	 while(fscanf(ff, "%d %d %d %s", &file_num, &file_block, &file_size, &f_name) != EOF){
		if(file_num == (i-1))
			 fprintf(ff2, "%d %d %d %s\n", file_num, file_block, file_size, replace);
		else
			fprintf(ff2, "%d %d %d %s\n", file_num, file_block, file_size, &f_name);
        }
	fclose(ff);
	fclose(ff2);
	
	system("mv filelist2.txt filelist.txt");
}

void deleteFile(char *file_name){
        FILE *file;
        file = fopen("filelist.txt", "r");
        int *file_num=0;
        int *file_block, *file_size;
        int i=0;
        char *f_name;

        while(fscanf(file, "%d %d %d %s", &file_num, &file_block, &file_size, &f_name) != EOF){
                i++;
                if(0==strcmp(&f_name, file_name))
                        break;
        }
        fclose(file);
        
        FILE *ff,*ff2;
        ff = fopen("filelist.txt", "r");
        ff2 = fopen("filelist2.txt", "w");

         while(fscanf(ff, "%d %d %d %s", &file_num, &file_block, &file_size, &f_name) != EOF){
                if(file_num == (i-1)){
		}
                else
                        fprintf(ff2, "%d %d %d %s\n", file_num, file_block, file_size, &f_name);
        }
        fclose(ff);
        fclose(ff2);

        system("mv filelist2.txt filelist.txt");

}

void readStruct(){

	/*reset current pointer to head of table*/
	fileTableCurrent = fileTableHead;
	printf("PASS1: %d\n", fileTableCurrent->fileNum);
	while(fileTableCurrent != NULL){
	printf("PASS2\n");
		printf("STRUCT FILE: %d", fileTableCurrent->fileNum);
		printf("pass3\n");
		fileTableCurrent = fileTableCurrent->next;
		printf("pass4\n");
	}

}
