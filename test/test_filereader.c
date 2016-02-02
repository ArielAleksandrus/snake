#include <stdio.h>
#include <stdlib.h>
#include "../level.h"
#include "../filereader.h"

void printFile(FILE* f){
	char buffer[1024], buffer2[64];
	clearBuffer(buffer, 1024);
	clearBuffer(buffer2, 64);
	int words = 0, line = 0;
	int start;
	// read line by line
	while(readLine(f, buffer, 1024)){
		printf("\n\nLine: %d\n", ++line);
		start = 0;
		while(start != -1){
			start = readWord(buffer, buffer2, start);
			printf("\n\tword %d: '%s'", ++words, buffer2);
		}
	}
	printf("\n\n");
}
void printLevelMap(Level* l){
	printf("MAP:\n\t");
	printf("Size: \n\t\t%d width\n\t\t%d height\n\n\t", l->width, l->height);
	printf("Spawn point: (%d, %d)\n\n\t", l->spawnPoint.x, l->spawnPoint.y);
	printf("Target score: %d\n\n\t", l->targetScore);
	printf("Initial speed: %f\n\n\t", l->initialSpeed);
	printf("Initial direction: %d\n\n\t", l->initialDirection);
	printf("Structure:\n\t\t");
	int i, j;
	for(j = 0; j < l->height; j++){
		for(i = 0; i < l->width; i++){
			printf("%d\t", l->map[i][j]);
		}
		printf("\n\t\t");
	}
}
void testLoadLevelFromFile(){
	Level* l = loadLevelFromFile("maps3.txt");
	printLevelMap(l);
}
int main(){
    char* filename = "maps/maps3.txt";
    FILE* f = fopen(filename, "r");
    //printFile(f);
		fclose(f);
		printf("\n\n============================\n\n");
		/*char* s = "\t\t3\n";
		int x;
		sscanf(s, "%d", &x);
		printf("X eh: %d\n", x);*/
		testLoadLevelFromFile();
		printf("\n\n============================\n\n");
    return 0;
}
