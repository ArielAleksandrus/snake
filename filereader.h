#ifndef FILEREADER_H
#define FILEREADER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "types.h"

#define MAP 0
#define TARGET_SCORE 1
#define INITIAL_SPEED 2
#define INITIAL_DIRECTION 3
#define SPAWN_POINT 4
#define NOT_LEVEL_TAG -1

int readWord(char* str, char* buffer, int start);
int readLine(FILE* in, char* buffer, size_t max);
void clearBuffer(char* buff, int size);

int toConstant(char* constString);
int getTag(char* tag);

void getMap(FILE* in, Level* l);
Level* loadLevelFromFile(const char* fileName);

#endif /* FILEREADER_H */

