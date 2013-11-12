#ifndef FILE_UTIL_H
#define FILE_UTIL_H
 
#include <openssl/md5.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include "data_structs.h"

#define LOGNAME "log.txt"

typedef unsigned char byte;

int alphasort(const struct dirent ** a, const struct dirent **b);
int update_files(filestate* state);
void free_files(filestate* state);
void delta(filestate* receiver, filestate* sender, filestate* res);
void save_file(byte* fileBuffer, int fileSize, char* filename);

byte* load_file(char fileName[], off_t fileSize) //how do I know what the filesize is?
{
	//Open an I/O stream to the file
	FILE* fileStream;
	fileStream = fopen(fileName, "rb");
	byte* fileBuf = (byte *)malloc(sizeof(byte)*fileSize);

	fread(fileBuf, sizeof(byte), fileSize, fileStream); 
        
	fclose(fileStream);
	return fileBuf;
}

void save_file(byte* fileBuffer, int fileSize, char* filename)
{
    FILE* file = fopen(filename, "wb");
    fwrite(fileBuffer, sizeof(byte), fileSize, file);
    fclose(file);
}

byte* get_unique_id(char fileName[], off_t fileSize)
{
	byte* c = (byte *) malloc(sizeof(byte) * MD5_DIGEST_LENGTH);
	byte* payload;
	payload = load_file(fileName, fileSize);

	FILE *inFile = fopen(fileName,"rb");
	MD5_CTX mdContext;
	int bytes;
	byte data[1024];
	
	if(inFile == NULL){
	    exit(1);
	}
	MD5_Init(&mdContext);
	while((bytes = fread(data, 1, 1024, inFile)) != 0){
	    MD5_Update(&mdContext, data, bytes);
	}
	MD5_Final(c, &mdContext);
	return c;
}

//Needed for the scandir function used below
int findMusic(struct dirent * file)
{
	struct stat fileAttributes;
    int namelength = strlen(file->d_name);

    char extension[4];
    memcpy(extension, &file->d_name[namelength - 3], 3);
    extension[3] = '\0';
    char mp3[] = {"mp3"}; 

    return !strcmp(extension, mp3);
}

int update_files(filestate* state)
{
    struct dirent **files;

    int numFiles = scandir("./", &files, findMusic, alphasort);

    music_file *fileList;
    fileList=(music_file*) malloc(numFiles * sizeof(music_file));
  
    if(numFiles >= 0)
    {
    	for(int i = 0; i < numFiles; i++)
    	{
    		struct stat fileAttributes;
    		stat(files[i]->d_name, &fileAttributes);
    		fileList[i].filename = files[i]->d_name;
    		fileList[i].ID = get_unique_id(files[i]->d_name, fileAttributes.st_size);
    	}
    }
    else
    {
    	return 0;
    }

    state->music_files = fileList;
    state->numFiles = numFiles;
    return numFiles;
}

void free_files(filestate* state)
{
    free(state->music_files);
}

//Returns the filestate of files different in sender from receiver  (sender - receiver )
void delta(filestate* receiver, filestate* sender, filestate* res)
{
    int senderLength = sender->numFiles;
    int receiverLength = receiver->numFiles;

    //if the receiver has no files, go ahead and send everything
    if(receiverLength == 0){
	res->numFiles = senderLength;
        res->music_files = sender->music_files;
    }

    //if the sender has no files, nothing can be sent
    if(senderLength == 0){
    	res->numFiles = 0;
	//Nothing to add to res->music_files
    }

    int fileCount = 0;

    music_file* fileList; 
    fileList = (music_file*) malloc(sizeof(music_file));  
    int i;
    int j;
    int found = 0;

    for(i = 0; i < senderLength; i++)
    {
	    found = 0;
        
        for(j = 0; j < receiverLength; j++)
    	{
	        found = 0;
       	    if(!memcmp(sender->music_files[i].ID, receiver->music_files[j].ID, sizeof(sender->music_files[i].ID))){
	            found = 1;
	            break;	
	        }
	    }

	    if(!found){
	        fileList = (music_file*) realloc(fileList, sizeof(music_file)*(++fileCount));
            fileList[fileCount-1] = sender->music_files[i];
	    }
    }	
    
    for(int i = 0; i < fileCount; i++){
        printf("Diff %d: %s\n", i, fileList[i].filename);
    }

    res->numFiles = fileCount;
    res->music_files = fileList;
}


#endif
