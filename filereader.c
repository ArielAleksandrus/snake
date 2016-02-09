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
	int i, total = 6;
	
	char* tags[total];
	tags[MAP] = ":map:\n";
	tags[TARGET_SCORE] = ":targetScore:\n";
	tags[INITIAL_SPEED] = ":initialSpeed:\n";
	tags[INITIAL_DIRECTION] = ":initialDirection:\n";
	tags[SPAWN_POINT] = ":spawnPoint:\n";
	tags[SPAWN_DATA] = ":spawnData:\n";
	
	for(i = 0; i < total; i++)
		if(strcmp(tag, tags[i]) == 0)
			return i;
	
	return NOT_LEVEL_TAG;
}
int getBonusRelatedTag(char* tag){
	char buff[32];
	sscanf(tag, "%s", buff);
	switch(buff[1]){
		case 'b':{
			return BBONUS;
		}
		case 'd':{
			return BDURATION;
		}
		case 'e':{
			return BEXPIRING_IN;
		}
		case 'f':{
			return BFREQUENCY;
		}
		case 'g':{
			return BGENEROSITY;
		}
		case 'i':{
			return BITEMS_AT_ONCE;
		}
		case 'm':{
			return BMAX_ITEMS_AT_ONCE;
		}
		case 'r':{
			return BRARITY;
		}
		case 't':{
			return BTYPE;
		}
		case 'w':{
			return BWHAT_CAN_BE_SPAWNED;
		}
	}
	return NOT_BONUS_TAG;
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
	else if(strcmp(constString, "EXTRA_POINTS") == 0)
		return EXTRA_POINTS;
	else if(strcmp(constString, "EXTRA_SIZE") == 0)
		return EXTRA_SIZE;
	else if(strcmp(constString, "HALF_SIZE") == 0)
		return HALF_SIZE;
	else if(strcmp(constString, "DOUBLE_POINTS") == 0)
		return DOUBLE_POINTS;
	else if(strcmp(constString, "REDUCED_SPEED") == 0)
		return REDUCED_SPEED;
	else if(strcmp(constString, "INVINCIBLE") == 0)
		return INVINCIBLE;
	else if(strcmp(constString, "EXTRA_LIFE") == 0)
		return EXTRA_LIFE;
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
void getWhatCanBeSpawned(FILE* in, SpawnData* sd){
	int lbs = 64, wbs = 32;
	char lineBuffer[lbs], wordBuffer[wbs];
	int tag;
	clearBuffer(lineBuffer, lbs);
	clearBuffer(wordBuffer, wbs);
	
	Bonus* b = NULL;
	while(readLine(in, lineBuffer, lbs)){
		tag = getBonusRelatedTag(lineBuffer);
		clearBuffer(lineBuffer, lbs);
		switch(tag){
			case BBONUS:{
				if(b != NULL)
					lappend(sd->whatCanBeSpawned, b);
				b = newDefaultBonus();
				break;
			}
			case BDURATION:{
				clearBuffer(lineBuffer, lbs);
				readLine(in, lineBuffer, lbs);
				sscanf(lineBuffer, "%d", &b->duration);
				break;
			}
			case BTYPE:{
				clearBuffer(lineBuffer, lbs);
				clearBuffer(wordBuffer, lbs);
				readLine(in, lineBuffer, lbs);
				readWord(lineBuffer, wordBuffer, 0);
				sscanf(lineBuffer, "%s", wordBuffer);
				b->type = toConstant(wordBuffer);
				break;
			}
			case BRARITY:{
				clearBuffer(lineBuffer, lbs);
				readLine(in, lineBuffer, lbs);
				sscanf(lineBuffer, "%d", &b->rarity);
				break;
			}
			case NOT_BONUS_TAG:{
				lappend(sd->whatCanBeSpawned, b);
				fseek(in, strlen(lineBuffer) * -1, SEEK_CUR);
				return;
			}
		}
	}
	
}
void getSpawnData(FILE* in, Level* l){
	int lbs = 64;
	char lineBuffer[lbs];
	int tag;
	clearBuffer(lineBuffer, lbs);
	
	l->bonusSpawnRules = newDefaultSpawnData();
	
	while(readLine(in, lineBuffer, lbs)){
		tag = getBonusRelatedTag(lineBuffer);
		switch(tag){
			case BEXPIRING_IN:{
				clearBuffer(lineBuffer, lbs);
				readLine(in, lineBuffer, lbs);
				sscanf(lineBuffer, "%d", &l->bonusSpawnRules.expiring_in);
				break;
			}
			case BFREQUENCY:{
				clearBuffer(lineBuffer, lbs);
				readLine(in, lineBuffer, lbs);
				sscanf(lineBuffer, "%d", &l->bonusSpawnRules.frequency);
				break;
			}
			case BGENEROSITY:{
				clearBuffer(lineBuffer, lbs);
				readLine(in, lineBuffer, lbs);
				sscanf(lineBuffer, "%d", &l->bonusSpawnRules.generosity);
				break;
			}
			case BITEMS_AT_ONCE:{
				clearBuffer(lineBuffer, lbs);
				readLine(in, lineBuffer, lbs);
				sscanf(lineBuffer, "%d", &l->bonusSpawnRules.itemsAtOnce);
				break;
			}
			case BMAX_ITEMS_AT_ONCE:{
				clearBuffer(lineBuffer, lbs);
				readLine(in, lineBuffer, lbs);
				sscanf(lineBuffer, "%d", &l->bonusSpawnRules.maxItemsAtOnce);
				break;
			}
			case BWHAT_CAN_BE_SPAWNED:{
				getWhatCanBeSpawned(in, &l->bonusSpawnRules);
				break;
			}
			case NOT_BONUS_TAG:{
				fseek(in, strlen(lineBuffer) * -1, SEEK_CUR);
				return;
			}
			clearBuffer(lineBuffer, lbs);
		}
	}
}
Level* loadLevelFromFile(Snake* s, const char* fileName){
	Level* l = malloc(sizeof(Level));
	l->initialDirection = -1;
	l->curFoodQty = 0;
	l->bonuses = newList();
	s->bodyPositions = newList();
	
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
				case SPAWN_DATA:{
					getSpawnData(pFile, l);
					break;
				}
			}
		}
	}
	
	if(s == NULL)
			return l;
	
	s->head = l->spawnPoint;
	Position* headPos = malloc(sizeof(Position));
	*headPos = s->head;
	lappend(s->bodyPositions, headPos);
	Position tail = l->spawnPoint;
	int d = l->initialDirection;
	tail.x = l->spawnPoint.x + (d == RIGHT) - (d == LEFT);
	tail.y = l->spawnPoint.y + (d == DOWN) - (d == UP);
	if(l->map[tail.x][tail.y] == BLANK){
		Position* auxTail = malloc(sizeof(Position));
		*auxTail = tail;
		lappend(s->bodyPositions, auxTail);
		s->tail = *auxTail;
	} else {
		s->tail = l->spawnPoint;
	}
	s->direction = l->initialDirection;
	s->newDirection = l->initialDirection;
	s->speed = l->initialSpeed;
	s->invincible = 0;
	
	fclose(pFile);
	return l;
}

void clearBuffer(char* buff, int size){
	while(size >= 0)
		buff[--size] = 0;
}

Bonus* newDefaultBonus(){
	Bonus* b = calloc(1, sizeof(Bonus));
	b->duration = BONUS_DEFAULT_DURATION;
	b->rarity = 0;
	b->type = BONUS_DEFAULT_TYPE;
	return b;
}
SpawnData newDefaultSpawnData(){
	SpawnData sd;
	sd.expiring_in = SPAWN_DATA_DEFAULT_EXPIRATION;
	sd.frequency = SPAWN_DATA_DEFAULT_FREQUENCY;
	sd.generosity = SPAWN_DATA_DEFAULT_GENEROSITY;
	sd.itemsAtOnce = SPAWN_DATA_DEFAULT_IAO;
	sd.maxItemsAtOnce = SPAWN_DATA_DEFAULT_MAX_IAO;
	sd.whatCanBeSpawned = newList();
	return sd;
}