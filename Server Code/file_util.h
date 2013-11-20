#ifndef FILE_UTIL_H
#define FILE_UTIL_H
 
#include <openssl/md5.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include "data_structs.h"

#define LOGNAME "log.txt"
#define ITUNES_XML_FILEPATH "it.xml"

typedef unsigned char byte;

int alphasort(const struct dirent ** a, const struct dirent **b);
int update_files(filestate* state);
void free_files(filestate* state);
void delta(filestate* receiver, filestate* sender, filestate* res);
void get_capped_diff(int max_bytes, filestate* diff, filestate* res);
void save_file(byte* fileBuffer, int fileSize, char* filename);
void set_playcount_mappings(filestate* diff, int numFiles);
int compare_music_files(music_file* a, music_file* b);
void TEST_PARSING_LOGIC();

byte* load_file(char fileName[], off_t fileSize) 
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
            fileList[i].fileSize = fileAttributes.st_size;
            fileList[i].playCount = 0; 
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

void get_capped_diff(int max_bytes, filestate* diff, filestate* res)
{
    int numFiles = diff->numFiles;                                                                                 
    set_playcount_mappings(diff, numFiles);
    qsort(diff->music_files, numFiles, sizeof(music_file),(*compare_music_files));

    int capped_bytes = 0;
    int capped_files = 0;
    int i;
    for(i = 0; i < numFiles; i++){
        if(capped_bytes + diff->music_files[i].fileSize < max_bytes){
            capped_bytes+= diff->music_files[i].fileSize;
            capped_files++;
        }
        else{
            break;
        }

    }

    music_file *capped_file_list;
    fileList=(music_file*) malloc(capped_files * sizeof(music_file));
    for(i = 0; i < capped_files; i++){
        capped_file_list[i] = diff->music_files[i];
    }
    
    res->numFiles = capped_files;
    res->music_files = diff->capped_file_list;//find out how to remove end part of this array
}

//first compare by playCount, then compare by fileSize
int compare_music_files(music_file* a, music_file* b)
{
    if (a->playCount > b->playCount)
        return 1;
    else if (a->playCount < b->playCount)
        return -1;
    else{
        if (a->fileSize > b->fileSize)
            return 1;
        else if (a->fileSize < b->fileSize)
            return -1;
        else
            return 0;
    }
}

void TEST_PARSING_LOGIC(){

    printf("Testing parsing logic\n");

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    int play_count;
    int files_mapped = 0; 
    int lookForFilename = 0;
    char * raw_filename;

    int i;
    printf("TSetup done\n");

    fp = fopen("it.xml", "r");
    printf("Attempting to open file\n");
    if (fp == NULL)
        printf("Unable to open iTunes Music Library.xml\n");
        //return;
        //exit(EXIT_FAILURE);

    printf("File opened \n");
    while ((read = getline(&line, &len, fp)) != -1) {
        
        //if a Play Count line is found
        if(!strncmp(line, "\t\t\t<key>Play C", 14)){ 
            //extract the play count from the line
            //printf("%s\n", line);
            //printf("%s\n", &line[33]);
            play_count = atoi(&line[33]);
            //printf("%d\n", play_count);
            lookForFilename = 1;
        }

        if(lookForFilename && !strncmp(line, "\t\t\t<key>Location", 16)){
            //extract filename        
            //printf("found a location line\n");
            raw_filename = line;
            char *p = strtok(line, "/");
            while( p!= NULL && strncmp(p,"string>",7)){
                raw_filename = p;
                //printf("%s\n", raw_filename);
                //printf("NEXT\n");
                p = strtok(NULL, "/");//NULL paramater means use last string that was being tokenized
            }

            //printf("raw_filename: %s\n",raw_filename);
            int charIdx = 0;
            int spaces = 0;
            //cleanup: strip "%20" substrings and remove "<" char that is always at the end
            //printf("%s\n",raw_filename);
            while(charIdx < (strlen(raw_filename) -1)){ 
                //technically this could go out of bounds, but all well formed files should end in a file extension 
                if(raw_filename[charIdx] == '%' && raw_filename[charIdx+1] == '2' && raw_filename[charIdx+2] == '0'){
                    //printf("%d\n",charIdx);
                    spaces++;
                    charIdx+=3;
                }
                else{
                    charIdx++;
                }
            }
            //printf("RF: %s\n",raw_filename);
            ///printf("Spaces found: %d\n",spaces);

            //strcpy(raw_filename, filename);
            char filename[strlen(raw_filename) - 1 - (spaces*2)];
            //filename = (*char) malloc(sizeof(char) * (strlen(raw_filename) - 1 - (spaces*2))); //replace 3 chars with 1 char
            //printf("Spaces found: %s\n",filename);


            charIdx = 0;
            i = 0;
            //cleanup: strip "%20" substrings and remove "<" char that is always at the end
            while(charIdx < (strlen(raw_filename) - 1)){ 
                //technically this could go out of bounds, but all well formed files should end in a file extension 
                if(raw_filename[charIdx] == '%' && raw_filename[charIdx+1] == '2' && raw_filename[charIdx+2] == '0'){
                    filename[i++] = ' ';
                    charIdx+=3;
                    //printf("Spaces found: %d\n",charIdx);

                }
                else{
                    filename[i++] = raw_filename[charIdx++];  
                     //printf("Spaces found: %d\n",charIdx);
 
                }
                //printf("filename: %s\n", filename);
            }
            //add null char... do I actually need to do this?
            //filename[strlen(filename)] ='\0';
            //printf("done\n");
            printf("filename: %s\n", filename);
            printf("play_count: %d\n", play_count);

            //now check for a matching music file and add its play count if found
            //for(i = 0; i < numFiles; i++){
             //   if(!strcmp(filename, diff->music_files[i].filename)){
            //        diff->music_files[i].playCount = play_count;
            //        files_mapped++;
            //        break;
            //    }
            //}
            lookForFilename = 0;

        }
        
    }
    printf("Ending Testing parsing logic\n");

    if (line)
        free(line);
    exit(EXIT_SUCCESS);
}


void set_playcount_mappings(filestate* diff, int numFiles){
    printf("Setting playcount mappings from itunes xml file\n");  

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    int play_count;
    int files_mapped = 0; 
    int playCountFound = 0;
    char * raw_filename;

    int i;
    
    fp = fopen(ITUNES_XML_FILEPATH, "r");
    if (fp == NULL)
        printf("Unable to open iTunes Music Library.xml. Mappings will not be added. Make sure it is in the working directory\n");
        return;
        //exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        
        //If we've mapped every file, break
        if(files_mapped == numFiles){
            break;
        }

        //if a Play Count line is found, exrtact play count from line
        if(!strncmp(line, "\t\t\t<key>Play C", 14)){ 
            play_count = atoi(&line[33]);
            playCountFound = 1;
        }

        //if playCountFound, and Location line found, extract the filename
        if(playCountFound && !strncmp(line, "\t\t\t<key>Location", 16)){
            raw_filename = line;
            char *p = strtok(line, "/");
            while( p!= NULL && strncmp(p,"string>",7)){
                raw_filename = p;
                p = strtok(NULL, "/");//NULL paramater means use last string that was being tokenized
            }

            int charIdx = 0;
            int spaces = 0;
            //cleanup: strip "%20" substrings and remove "<" char that is always at the end.
            //First find what the length of the new string will be after cleanup 
            while(charIdx < (strlen(raw_filename) -1)){ 
                //technically this could go out of bounds, but all well formed files should end in a file extension 
                if(raw_filename[charIdx] == '%' && raw_filename[charIdx+1] == '2' && raw_filename[charIdx+2] == '0'){
                    spaces++;
                    charIdx+=3;
                }
                else{
                    charIdx++;
                }
            }

            char filename[strlen(raw_filename) - 1 - (spaces*2)];
            charIdx = 0;
            i = 0;
            while(charIdx < (strlen(raw_filename) - 1)){ 
                if(raw_filename[charIdx] == '%' && raw_filename[charIdx+1] == '2' && raw_filename[charIdx+2] == '0'){
                    filename[i++] = ' ';
                    charIdx+=3;
                }
                else{
                    filename[i++] = raw_filename[charIdx++];  
                }
            }
            printf("PC: %d, filename: %s\n", play_count, filename);

            //now check for a matching music file and add its play count if found
            for(i = 0; i < numFiles; i++){
                if(!strcmp(filename, diff->music_files[i].filename)){
                    diff->music_files[i].playCount = play_count;
                    files_mapped++;
                    break;
                }
            }
            playCountFound = 0; 
        }
        
    }
 
    printf("%d files were mapped to playcounts\n", files_mapped);

    if (line)
        free(line);
    exit(EXIT_SUCCESS);
}

#endif