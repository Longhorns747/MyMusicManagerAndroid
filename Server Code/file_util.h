#ifndef FILE_UTIL_H
#define FILE_UTIL_H
 
#include <openssl/md5.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include "data_structs.h"

#define LOGNAME "log.txt"
#define ITUNES_XML_FILEPATH "test.xml"

typedef unsigned char byte;

int alphasort(const struct dirent ** a, const struct dirent **b);
int update_files(filestate* state);
void free_files(filestate* state);
void delta(filestate* receiver, filestate* sender, filestate* res);
void get_capped_diff(int max_bytes, filestate* diff, filestate* res);
void save_file(byte* fileBuffer, int fileSize, char* filename);
void set_playcount_mappings(filestate* diff, int numFiles);

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
    set_playcount_mappings(diff,numFiles);
    filestate* sorted_diff;
    //sort(&diff, &sorted_diff);

    int capped_bytes = 0;
    int i;
    for(i = 0; i < numFiles; i++){
        if(capped_bytes + sorted_diff->music_files[i].fileSize < max_bytes){
            capped_bytes+= sorted_diff->music_files[i].fileSize;

            //then add to res
        }
        else{
            break;
        }

    }

    //for now, just return the same thing. Later, implement code the actually
    //finds the top files in this diff that is passed in by parsing the itunes xml stuff
    res->numFiles = diff->numFiles;
    res->music_files = diff->music_files;
}

void set_playcount_mappings(filestate* diff, int numFiles){
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    int play_count;
    int files_mapped = 0; 
    int lookForFilename = 0;
    char * filename;
    char * raw_filename;

    int i;

    fp = fopen(ITUNES_XML_FILEPATH, "r");
    if (fp == NULL)
        printf("Unable to open iTunes Music Library.xml\n");
        //exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        //if we've already matched every file with a play_count, then break
        if(files_mapped == numFiles){
            break;
        }
        //if a Play Count line is found
        if(strncmp(line, "\t\t\t<key>Play Count</key><integer>", 2) == 0)
            //extract the play count from the line
            play_count = atoi(line);
            lookForFilename = 1;

        if(lookForFilename && !strncmp(line, "\t\t\t<key>Location</key>", 2)){
            //extract filename        
            raw_filename = line;
            char *p = strtok(line, "/");
            while(p != "string>") {
                raw_filename = p;
                p = strtok(NULL, "/");//NULL paramater means use last string that was being tokenized
            }

            int charIdx = 0;
            //cleanup: strip "%20" substrings and remove "<" char that is always at the end
            while(charIdx < (sizeof(raw_filename) -1)){ 
                //technically this could go out of bounds, but all well formed files should end in a file extension 
                if(raw_filename[i] == '%' && raw_filename[i+1] == '2' && raw_filename[i] == '0'){
                    filename[strlen(filename)] = " ";
                    charIdx+=3;
                }
                else{
                    filename[strlen(filename)] = raw_filename[i];
                    charIdx++;
                }
            }

            printf("%s %s", play_count, filename);

            //now check for a matching music file and add it's play count if found
            for(i = 0; i < numFiles; i++){
                if(!strcmp(filename, diff->music_files[i].filename)){
                    diff->music_files[i].playCount = play_count;
                    files_mapped++;
                    break;
                }
            }
            lookForFilename = 0;

        printf("%s", line);
        }
    }

    if (line)
        free(line);
    exit(EXIT_SUCCESS);

}

#endif