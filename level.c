#include "level.h"

Level* loadCurrentLevel(Snake* s, int playerScore){
	if(playerScore < LEVEL1_TARGET_SCORE)
		return loadLevel(s, 1);
	if(playerScore < LEVEL2_TARGET_SCORE)
		return loadLevel(s, 2);
	return NULL;
}
Level* loadLevel(Snake* s, int levelNumber){
	switch(levelNumber){
		case 1:
			return loadLevel1(s);
		case 2:
			return loadLevel2(s);
	}
	return NULL;
}
void setSnakeProps(Level* l, Snake* s){
	s->head = l->spawnPoint;
	s->tail.x = l->spawnPoint.x - 1;
	s->tail.y = l->spawnPoint.y;

	s->bodyPositions = newList();
	s->activeBonuses = newList();

	Position* auxPos = malloc(sizeof(Position));
	*auxPos = s->head;
	lappend(s->bodyPositions, auxPos);
	auxPos = malloc(sizeof(Position));
	*auxPos = s->tail;
	lappend(s->bodyPositions, auxPos);

	s->direction = l->initialDirection;
	s->newDirection = l->initialDirection;
	s->speed = l->initialSpeed;

	spawnSnake(l, s);
}
Level* loadLevel1(Snake* s){
	int i; // iterator.
	Level* l = malloc(sizeof(Level));

	/////////////////// BUILD MAP //////////////////////
	l->width = 40;
	l->height = 40;
	l->map = malloc(l->width * sizeof(int*));
	for(i = 0; i < l->width; i++)
		l->map[i] = malloc(l->height * sizeof(int));

	buildBox(l->map, l->height, l->width);
	////////////////////////////////////////////////////


	///////////////// SET MAP PROPS ////////////////////
	l->levelNumber = 1;
	l->targetScore = LEVEL1_TARGET_SCORE;
	l->spawnPoint.x = l->width / 2;
	l->spawnPoint.y = l->height / 2;

	l->initialDirection = RIGHT;
	l->initialSpeed = 1;

	l->curFoodQty = 0;
	l->bonuses = newList();
	
	SpawnData bsr;
	bsr.frequency = 1;
	bsr.generosity = 1;
	bsr.itemsAtOnce = 1;
	bsr.maxItemsAtOnce = 1;
	bsr.whatCanBeSpawned = newList();
	Bonus* bonusesAvailable = malloc(4*sizeof(Bonus));
	bonusesAvailable[0].type = EXTRA_POINTS;
	bonusesAvailable[0].duration = 10000;
	bonusesAvailable[0].rarity = 0;
	bonusesAvailable[1].type = DOUBLE_POINTS;
	bonusesAvailable[1].duration = 10000;
	bonusesAvailable[1].rarity = 2;
	bonusesAvailable[2].type = REDUCED_SPEED;
	bonusesAvailable[2].duration = 12000;
	bonusesAvailable[2].rarity = 4;
	bonusesAvailable[3].type = INVINCIBLE;
	bonusesAvailable[3].duration = 8000;
	bonusesAvailable[3].rarity = 3;
	
	
	lappend(bsr.whatCanBeSpawned, &bonusesAvailable[0]);
	lappend(bsr.whatCanBeSpawned, &bonusesAvailable[1]);
	lappend(bsr.whatCanBeSpawned, &bonusesAvailable[2]);
	lappend(bsr.whatCanBeSpawned, &bonusesAvailable[3]);
	
	l->bonusSpawnRules = bsr;
	////////////////////////////////////////////////////


	//////////////// SET SNAKE PROPS ///////////////////
	setSnakeProps(l, s);
	////////////////////////////////////////////////////
	return l;
}
Level* loadLevel2(Snake* s){
	int i; // iterator.
	Level* l = malloc(sizeof(Level));

	/////////////////// BUILD MAP //////////////////////
	l->width = 20;
	l->height = 10;
	l->map = malloc(l->width * sizeof(int*));
	for(i = 0; i < l->width; i++)
		l->map[i] = malloc(l->height * sizeof(int));

	buildBox(l->map, l->height, l->width);
	////////////////////////////////////////////////////


	///////////////// SET MAP PROPS ////////////////////
	l->levelNumber = 2;
	l->targetScore = LEVEL2_TARGET_SCORE;
	l->spawnPoint.x = l->width / 2;
	l->spawnPoint.y = l->height / 2;

	l->initialDirection = RIGHT;
	l->initialSpeed = 1;

	l->curFoodQty = 0;
	l->bonuses = newList();
	
	SpawnData bsr;
	bsr.frequency = 1;
	bsr.generosity = 1;
	bsr.itemsAtOnce = 1;
	bsr.maxItemsAtOnce = 1;
	bsr.whatCanBeSpawned = newList();
	Bonus* bonusesAvailable = malloc(4*sizeof(Bonus));
	bonusesAvailable[0].type = EXTRA_POINTS;
	bonusesAvailable[0].duration = 10000;
	bonusesAvailable[0].rarity = 0;
	bonusesAvailable[1].type = DOUBLE_POINTS;
	bonusesAvailable[1].duration = 10000;
	bonusesAvailable[1].rarity = 2;
	bonusesAvailable[2].type = REDUCED_SPEED;
	bonusesAvailable[2].duration = 12000;
	bonusesAvailable[2].rarity = 4;
	bonusesAvailable[3].type = INVINCIBLE;
	bonusesAvailable[3].duration = 8000;
	bonusesAvailable[3].rarity = 3;
	
	
	lappend(bsr.whatCanBeSpawned, &bonusesAvailable[0]);
	lappend(bsr.whatCanBeSpawned, &bonusesAvailable[1]);
	lappend(bsr.whatCanBeSpawned, &bonusesAvailable[2]);
	lappend(bsr.whatCanBeSpawned, &bonusesAvailable[3]);
	
	l->bonusSpawnRules = bsr;
	////////////////////////////////////////////////////


	//////////////// SET SNAKE PROPS ///////////////////
	setSnakeProps(l, s);
	////////////////////////////////////////////////////
	return l;
}
void startLevel(Level* l, Snake* s, Player* p, GameControl* gc, Threads* threads){
	////////////////////////// PREPARE SNAKE DATA ////////////////////////////////////
	s->invincible = 0;
	//////////////////////////////////////////////////////////////////////////////////
	
 	////////////////////////// PREPARE THREAD DATA ///////////////////////////////////
  PrintMapData* pmd = malloc(sizeof(PrintMapData));
  pmd->level = l;
  pmd->player = p;
	pmd->snake = s;

  HandleSnakeData* hsd = malloc(sizeof(HandleSnakeData));
  hsd->level = l;
  hsd->snake = s;
  hsd->player = p;
  hsd->gameControl = gc;

  SpawnFoodData* sfd = malloc(sizeof(SpawnFoodData));
  sfd->level = l;
	
	SpawnBonusData* sbd = malloc(sizeof(SpawnBonusData));
	sbd->level = l;
	sbd->spawnData = l->bonusSpawnRules;
	
	DirectionInputData* did = malloc(sizeof(DirectionInputData));
	did->gameControl = gc;
	did->snake = s;
 	//////////////////////////////////////////////////////////////////////////////////

 	//////////////////////////////// START THREADS ///////////////////////////////////
  int rc;
  rc = pthread_create(&threads->printMapThread, NULL, (void*) printMapThreadFunction, pmd);
	lappend(threads->args, pmd);
  if(rc != 0){
  	endwin();
  	printf("\nError creating printMapThread\n");
  	exit(1);
  }
  rc = pthread_create(&threads->handleSnakeThread, NULL, (void*) handleSnakeThreadFunction, hsd);
	lappend(threads->args, hsd);
  if(rc != 0){
  	endwin();
  	printf("\nError creating handleSnakeThread\n");
  	exit(1);
  }
  rc = pthread_create(&threads->spawnFoodThread, NULL, (void*) spawnFoodThreadFunction, sfd);
	lappend(threads->args, sfd);
  if(rc != 0){
  	endwin();
  	printf("\nError creating spawnFoodThread\n");
  	exit(1);
  }
  rc = pthread_create(&threads->spawnBonusThread, NULL, (void*) spawnBonusThreadFunction, sbd);
	lappend(threads->args, sbd);
  if(rc != 0){
  	endwin();
  	printf("\nError creating spawnBonusThread\n");
  	exit(1);
  }
  rc = pthread_create(&threads->directionInputThread, NULL, (void*) directionInputThreadFunction, did);
	lappend(threads->args, did);
  if(rc != 0){
  	endwin();
  	printf("\nError creating directionInputThread\n");
  	exit(1);
  }
 	//////////////////////////////////////////////////////////////////////////////////
}

void unloadLevel(Level* l, Snake* s, Threads* threads){
	int i;
	
	//////////////// CANCEL ALL THREADS ///////////////////
	pthread_cancel(threads->printMapThread);
	pthread_cancel(threads->handleSnakeThread);
	pthread_cancel(threads->spawnFoodThread);
	pthread_cancel(threads->spawnBonusThread);
	pthread_cancel(threads->directionInputThread);
	///////////////////////////////////////////////////////

	//////////////// FREE ALLOCATED RESOURCES /////////////
	for(i = 0; i < l->width; i++)
		free(l->map[i]);

	Bonus* bonusArray = lAt(l->bonusSpawnRules.whatCanBeSpawned, 0);
	free(bonusArray);
	lfree(l->bonusSpawnRules.whatCanBeSpawned);
	lclear(l->bonuses);
	free(l);
	lclear(s->bodyPositions);
	lclear(s->activeBonuses);
	free(s);
	lclear(threads->args);
	threads->args = newList();
	///////////////////////////////////////////////////////
}

Bonus* findBonus(Level* l, Position p){
	int i;
	for(i = 0; i < l->bonuses->size; i++){
		Bonus* b = lAt(l->bonuses, i);
		if(b->position.x == p.x && b->position.y == p.y)
			return b;
	}
	return NULL;
}

void buildBox(int** map, int height, int width){
	int i, j; // iterators.

	// clear map to avoid memory garbage affecting the construction.
	for(i = 0; i < width; i++)
		for(j = 0; j < height; j++)
			map[i][j] = BLANK;
	
	for(i = 0; i < width; i++){
		for(j = 0; j < height; j++){
			// vertical extremities.
			if(i == 0 || i == width - 1)
				map[i][j] = BLOCK;
			// horizontal extremities.
			if(j == 0 || j == height - 1)
				map[i][j] = BLOCK;

			if(map[i][j] != BLOCK)
				map[i][j] = BLANK;
		}
	}
}