#include "spawner.h"
#include <ncurses.h>
#include <stdio.h>
void spawnSnake(Level* l, Snake* s){
	mapSnake(l->map, s->bodyPositions);
}
void spawnFood(Level* l){
	if(l->curFoodQty > 0)
		return;

	int randX = rand() % l->width;
	int randY = rand() % l->height;

	while(l->map[randX][randY] != BLANK){
		randX = rand() % l->width;
		randY = rand() % l->height;
	}
	l->map[randX][randY] = FOOD;
	l->curFoodQty++;
}
void spawnBonus(Level* l, SpawnData sd){
	if(l->bonuses->size >= sd.maxItemsAtOnce)
		return;
	
	Bonus* b = malloc(sizeof(Bonus));
	
	int randomRarity = rand() % 7;
	if(randomRarity != 0)
		randomRarity = rand() % randomRarity;
	
	int randomGenerosityFactor = 0;
	if(sd.generosity > 0)
		randomGenerosityFactor = rand() % sd.generosity;
	
	randomRarity += randomGenerosityFactor;
	if(randomRarity > 6)
		randomRarity = 6;
	
	*b = *(getBonus(sd.whatCanBeSpawned, randomRarity));
	b->timeout = sd.expiring_in;
	b->position.x = rand() % l->width;
	b->position.y = rand() % l->height;
	
	while(l->map[b->position.x][b->position.y] != BLANK){
		b->position.x = rand() % l->width;
		b->position.y = rand() % l->height;
	}
	
	l->map[b->position.x][b->position.y] = BONUS;
	
	lappend(l->bonuses, b);
}
void spawnFoodThreadFunction(SpawnFoodData* sfd){
	srand(time(NULL));
	struct timespec spec, spec2;
	
	while(1){

		spawnFood(sfd->level);

		spec.tv_sec = 0;
		spec.tv_nsec = FOOD_THREAD_RATE * 1.0e6;
		nanosleep(&spec, &spec2);
	}
}
void spawnBonusThreadFunction(SpawnBonusData* sbd){
	srand(time(NULL));
	struct timespec spec, spec2;
	while(1){
		
		spawnBonus(sbd->level, sbd->spawnData);
		
		spec = toTimespec(BONUS_THREAD_RATE * sbd->spawnData.frequency);
		nanosleep(&spec, &spec2);
	}
}

Bonus* getBonus(List* whatCanBeSpawned, int rarity){
	Bonus* res = NULL;
	Bonus* aux;
	List* possibilities = newList();
	int i;
	int maxLowestRarity = 0;
	int minHigherRarity = 6;
	for(i = 0; i < whatCanBeSpawned->size; i++){
		aux = lAt(whatCanBeSpawned, i);
		if(aux->rarity > rarity){
			if(aux->rarity < minHigherRarity)
				minHigherRarity = aux->rarity;
			continue;
		}
		
		if(maxLowestRarity < aux->rarity){
			maxLowestRarity = aux->rarity;
		}
	}
	for(i = 0; i < whatCanBeSpawned->size; i++){
		aux = lAt(whatCanBeSpawned, i);
		if(aux->rarity == maxLowestRarity)
			lappend(possibilities, aux);
	}
	if(possibilities->size == 0){
		for(i = 0; i < whatCanBeSpawned->size; i++){
			aux = lAt(whatCanBeSpawned, i);
			if(aux->rarity == minHigherRarity)
				lappend(possibilities, aux);
		}
	}
	
	if(possibilities->size == 0)
		return NULL;
	
	int randBonus = rand() % possibilities->size;
	
	res = lAt(possibilities, randBonus);	
	lfree(possibilities);
	
	return res;
}

int hasBonus(List* bonuses, int type){
	int i;
	Bonus* b;
	for(i = 0; i < bonuses->size; i++){
		b = lAt(bonuses, i);
		if(b->type == type)
			return 1;
	}
	return 0;
}
void setBonusEffect(Level* l, Snake* s, Player* p, Bonus* b, int apply, BonusThreads* bt){
	switch(b->type){
		case EXTRA_POINTS:{
			if(apply)
				p->score.extraModifier += 5;
			else
				p->score.extraModifier -= 5;
			break;
		}
		case EXTRA_SIZE:{
			if(apply){
				ExtraSizeBonusData* esbd = malloc(sizeof(ExtraSizeBonusData));
				esbd->bonus = b;
				esbd->snake = s;
				esbd->player = p;
				if(bt->extraSizeThread)
					pthread_cancel(bt->extraSizeThread);
				int rc = pthread_create(&bt->extraSizeThread, NULL, (void*) extraSizeBonusThreadFunction, esbd);
				if(rc != 0){
					endwin();
					printf("\nError creating extraSizeBonusThread\n");
					exit(1);
				}
			}
			break;
		}
		case DOUBLE_POINTS:{
			if(apply)
				p->score.multiplier *= 2;
			else
				p->score.multiplier /= 2;
			break;
		}
		case REDUCED_SPEED:{
			if(apply)
				s->speed *= 2;
			else
				s->speed /= 2;
			break;
		}
		case INVINCIBLE:{
			if(apply)
				s->invincible++;
			else
				s->invincible--;
			break;
		}
		case EXTRA_LIFE:{
			if(apply)
				p->lives++;
			break;
		}
		case HALF_SIZE:{
			if(!apply)
				break;

			if(bt->extraSizeThread)
				pthread_cancel(bt->extraSizeThread);
			int size = s->bodyPositions->size;
			int i;
			Position* part;
			for(i = 2; i < (size / 2) + 2; i++){
				part = lAt(s->bodyPositions, (size/2));
				l->map[part->x][part->y] = BLANK;
				lremoveAt(s->bodyPositions, (size/2));
			}
			if(s->bodyPositions->size > 1){
				part = lAt(s->bodyPositions, s->bodyPositions->size - 1);
				s->tail = *part;
			} else {
				s->tail = s->head;
			}
			break;
		}
	}
}

void extraSizeBonusThreadFunction(ExtraSizeBonusData* esbd){
	float i;
	Snake* snake = esbd->snake;
	float smtr = SNAKE_MOVE_THREAD_RATE;
	struct timespec spec, spec2;
	int dur = esbd->bonus->duration;
	
	for(i = 0; i < dur / smtr; i++){
		Position* newTail = malloc(sizeof(Position));
			
		*newTail = snake->tail;
		
		lappend(snake->bodyPositions, newTail);
		int extra = esbd->player->score.extraModifier;
		int multiplier = esbd->player->score.multiplier;
		esbd->player->score.total += (BASE_SCORE + extra) * multiplier;
		spec = toTimespec(SNAKE_MOVE_THREAD_RATE);
		nanosleep(&spec, &spec2);
	}
	free(esbd);
}