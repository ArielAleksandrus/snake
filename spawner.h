#ifndef SPAWNER_H
#define SPAWNER_H

#include <time.h>
#include <math.h>
#include <stdlib.h>
#include "list.h"
#include "types.h"
#include "utils.h"

void spawnSnake(Level* l, Snake* s);
void spawnFood(Level* l);
void spawnBonus(Level* l, SpawnData sd);
void spawnFoodThreadFunction(SpawnFoodData* sfd);
void spawnBonusThreadFunction(SpawnBonusData* sbd);

// if apply is true, it activates the bonus effect. else, it deactivates.
void setBonusEffect(Snake* s, Player* p, Bonus* b, int apply);
Bonus* getBonus(List* whatCanBeSpawned, int rarity);
int hasBonus(List* bonuses, int type);
#endif