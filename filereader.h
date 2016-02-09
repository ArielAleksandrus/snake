#ifndef FILEREADER_H
#define FILEREADER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "types.h"
#include "defaults.h"

#define MAP 0
#define TARGET_SCORE 1
#define INITIAL_SPEED 2
#define INITIAL_DIRECTION 3
#define SPAWN_POINT 4
#define SPAWN_DATA 5
#define NOT_LEVEL_TAG -1

#define BRARITY 0
#define BFREQUENCY 4
#define BGENEROSITY 8
#define BITEMS_AT_ONCE 12
#define BEXPIRING_IN 14
#define BMAX_ITEMS_AT_ONCE 16
#define BWHAT_CAN_BE_SPAWNED 20
#define BBONUS 24
#define BTYPE 28
#define BDURATION 32
#define NOT_BONUS_TAG -1

int readWord(char* str, char* buffer, int start);
int readLine(FILE* in, char* buffer, size_t max);
void clearBuffer(char* buff, int size);

int toConstant(char* constString);
int getTag(char* tag);
int getBonusRelatedTag(char* tag);

void getMap(FILE* in, Level* l);
void getSpawnData(FILE* in, Level* l);
void getWhatCanBeSpawned(FILE* in, SpawnData* sd);
Level* loadLevelFromFile(Snake* s, const char* fileName);

Bonus* newDefaultBonus();
SpawnData newDefaultSpawnData();
#endif /* FILEREADER_H */

