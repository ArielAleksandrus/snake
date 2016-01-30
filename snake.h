#ifndef SNAKE_H
#define SNAKE_H

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "list.h"
#include "types.h"
#include "level.h"
#include "utils.h"
#include "spawner.h"


void		buildBox(int** map, int height, int width);

int 		canUpdateDirection(Snake* s);
int 		collide(Level* l, Snake* s, Player* p, GameControl* gc, Position prevHead);
#endif