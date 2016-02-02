#include "filereader.h"
int readWord(char* str, char* buffer, int start){
	if(start < 0){
		perror("start cannot be less than 0.\n");
		exit(1);
	}
	int i = start, j = 0;
	while(str[i] == '\t' || str[i] == '\0' || str[i] == ' ')
		i++;
	while(str[i] != '\n' && str[i] != '\t' && str[i] != '\0' && str[i] != ' '){
		buffer[j++] = str[i++];
	}
	
	buffer[j] = '\0';
	if(str[i] == '\n'){
		return -1;
	}
	
	return i;
}
int readLine(FILE* in, char* buffer, size_t max){
	return fgets(buffer, max, in) == buffer;
}

int getTag(char* tag){
	int i, total = 5;
	
	char* tags[total];
	tags[MAP] = ":map:\n";
	tags[TARGET_SCORE] = ":targetScore:\n";
	tags[INITIAL_SPEED] = ":initialSpeed:\n";
	tags[INITIAL_DIRECTION] = ":initialDirection:\n";
	tags[SPAWN_POINT] = ":spawnPoint:\n";
	
	for(i = 0; i < total; i++){
		if(strcmp(tag, tags[i]) == 0)
			return i;
	}
	return NOT_LEVEL_TAG;
}
int toConstant(char* constString){
	if(strcmp(constString, "BLANK") == 0)
		return BLANK;
	else if(strcmp(constString, "BLOCK") == 0)
		return BLOCK;
	else if(strcmp(constString, "RIGHT") == 0)
		return RIGHT;
	else if(strcmp(constString, "DOWN") == 0)
		return DOWN;
	else if(strcmp(constString, "LEFT") == 0)
		return LEFT;
	else if(strcmp(constString, "UP") == 0)
		return UP;
	else
		return -1;
}
void getMap(FILE* in, Level* l){
	int i, j;
	int start = 0, endedRead = 0;
	char lineBuffer[256];
	char wordBuffer[16];
	char auxWordBuffer[17];
	
	List* cols = NULL; // list of integers.
	List* rows = newList(); // list of lists of cols.
	
	int* number;
	
	while(!endedRead){
		cols = newList();
		clearBuffer(lineBuffer, 256);
		readLine(in, lineBuffer, 256);
		start = 0;
		while(start != -1){
			clearBuffer(wordBuffer, 16);
			clearBuffer(auxWordBuffer, 17);
			start = readWord(lineBuffer, wordBuffer, start);
			strcpy(auxWordBuffer, wordBuffer);
			// if we find another tag, then we have read the whole map.
			if(getTag(strcat(auxWordBuffer, "\n")) != NOT_LEVEL_TAG){
				endedRead = 1;
				break;
			}
			
			int constant = toConstant(wordBuffer);
			if(constant != -1){
				number = malloc(sizeof(int));
				*number = constant;
				lappend(cols, number);
			}
		}
		
		if(cols->size > 0)
			lappend(rows, cols);
		else
			lclear(cols);
	}
	
	fseek(in, strlen(lineBuffer) * -1, SEEK_CUR);
	l->height = rows->size;
	l->width = ((List*) lAt(rows, 0))->size;
	l->map = malloc(l->width * sizeof(int*));
	
	for(i = 0; i < l->width; i++)
		l->map[i] = malloc(l->height * sizeof(int));
	
	for(i = 0; i < l->height; i++){
		cols = lAt(rows, i);
		for(j = 0; j < l->width; j++)
			l->map[j][i] = *((int*) lAt(cols, j));
		
		lclear(cols);
	}
	lclear(rows);
}
Level* loadLevelFromFile(const char* fileName){
	Level* l = malloc(sizeof(Level));
	char path[32];
	strcpy(path, "maps/");
	strcat(path, fileName);
	
	FILE* pFile = fopen(path, "r");
	
	int buffsize = 256;
	char lineBuffer[buffsize];
	clearBuffer(lineBuffer, buffsize);
	while(readLine(pFile, lineBuffer, buffsize)){
		int tag = getTag(lineBuffer);
		if(tag != NOT_LEVEL_TAG){
			switch(tag){
				case MAP:{
					getMap(pFile, l);
					break;
				}
				case TARGET_SCORE:{
					clearBuffer(lineBuffer, buffsize);
					readLine(pFile, lineBuffer, buffsize);
					sscanf(lineBuffer, "%d", &l->targetScore);
					break;
				}
				case INITIAL_SPEED:{
					clearBuffer(lineBuffer, buffsize);
					readLine(pFile, lineBuffer, buffsize);
					int k = -1;
					sscanf(lineBuffer, "%f", &l->initialSpeed);
					break;
				}
				case INITIAL_DIRECTION:{
					clearBuffer(lineBuffer, buffsize);
					readLine(pFile, lineBuffer, buffsize);
					char word[10];
					readWord(lineBuffer, word, 0);
					l->initialDirection = toConstant(word);
					break;
				}
				case SPAWN_POINT:{
					clearBuffer(lineBuffer, buffsize);
					readLine(pFile, lineBuffer, buffsize);
					sscanf(lineBuffer, "%d %d", &l->spawnPoint.x, &l->spawnPoint.y);
					break;
				}
			}
		}
		clearBuffer(lineBuffer, buffsize);
	}
	
	fclose(pFile);
	return l;
}

void clearBuffer(char* buff, int size){
	while(size >= 0)
		buff[--size] = 0;
}