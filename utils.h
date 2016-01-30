#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <time.h>
#include "types.h"
#include "level.h"

void mapSnake(int** map, int height, int width, List* snakeBodyParts);
void printHeader(Player* p);
void printMap(int** map, int height, int width);
void printMapThreadFunction(PrintMapData* pmd);

Player*	newPlayer();
Stats 	newStats();
GameControl* newGameControl();
Stats 	loadLifetimeStats();

void 		directionInput(Snake* s);
void 		directionInputThreadFunction(Snake* s);
void		moveSnake(Snake* s, Level* l, Player* p, GameControl* gc);
void		handleBonus(Snake* s, Player* p);
void		handleSnake(Snake* s, Level* l, Player* p, GameControl* gc);
void		handleSnakeThreadFunction(HandleSnakeData* msd);
void		gameControlThreadFunction(GameControlData* gcd);
int			collide(Level* l, Snake* s, Player* p, GameControl* gc, Position prevHead);
void		playerDie(GameControl* gc, Player* p);

struct timespec toTimespec(unsigned int millisseconds);
#endif