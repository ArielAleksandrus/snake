#include "level.h"
Level* loadLevel(Snake* s, int levelNumber){
	char filename[16] = "maps";
	char buff[4];
	sprintf(buff, "%d", levelNumber);
	
	strcat(filename, buff);
	strcat(filename, ".txt");
	
	Level* l = loadLevelFromFile(s, filename);
	if(l == NULL)
		return NULL;
	
	l->levelNumber = levelNumber;
	s->activeBonuses = newList();
	
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

	lclear(l->bonusSpawnRules.whatCanBeSpawned);
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