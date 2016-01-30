#ifndef LEVEL_H
#define LEVEL_H

#include <stdlib.h>
#include <pthread.h>
#include <curses.h>
#include "types.h"
#include "utils.h"
#include "spawner.h"
#include "list.h"

Level*	loadLevel1(Snake* s);
void		startLevel(Level* l, Snake* s, Player* p, GameControl* gc, Threads* threads);
void		unloadLevel(Level* l, Snake* s, Threads* threads);
Bonus*	findBonus(Level* l, Position p);
#endif