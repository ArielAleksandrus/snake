#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <time.h>
#include "types.h"
#include "level.h"

void mapSnake(int** map, List* snakeBodyParts);
void printHeader(Level* l, Player* p);
void printMap(int** map, int height, int width);
void printMapThreadFunction(PrintMapData* pmd);

Player*	newPlayer();
Stats 	newStats();
GameControl* newGameControl();
Stats 	loadLifetimeStats();


int 		canUpdateDirection(Snake* s);
void 		directionInput(GameControl* gc, Snake* s);
void 		directionInputThreadFunction(DirectionInputData* did);
void		moveSnake(Snake* s, Level* l, Player* p, GameControl* gc, BonusThreads* bt);
void		handleBonus(Level* l, Snake* s, Player* p, int sleepTime, BonusThreads* bt);
void		handleSnake(Snake* s, Level* l, Player* p, GameControl* gc, int sleepTime, BonusThreads* bt);
void		handleSnakeThreadFunction(HandleSnakeData* msd);
void		gameControlThreadFunction(GameControlData* gcd);
int 		collide(Level* l, Snake* s, Player* p, GameControl* gc, BonusThreads* bt);
void		playerDie(GameControl* gc, Player* p);

struct timespec toTimespec(unsigned int millisseconds);
#endif