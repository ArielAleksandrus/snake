#include "snake.h"

int main(){

	////////////////// SET NCURSES //////////////////////
  initscr();      // init ncurses screen
  noecho();       // user input will not be shown
  curs_set(0);    // cursor will not be shown
  start_color();	// start color mode
  refresh();      // so the map will be properly shown

	init_pair(BLACK, COLOR_BLACK, COLOR_BLACK);
	init_pair(WHITE, COLOR_BLACK, COLOR_WHITE);
	init_pair(RED, COLOR_BLACK, COLOR_RED);
	init_pair(GREEN, COLOR_BLACK, COLOR_GREEN);
	/////////////////////////////////////////////////////

  Player* p = newPlayer();
  GameControl* gc = newGameControl();
  Threads threads;
	threads.args = newList();
	////////////////// LOAD LEVEL 1 /////////////////////
	Snake* s = malloc(sizeof(Snake));
  Level* l1 = loadLevel1(s);

  GameControlData gcd;
  gcd.threads = &threads;
  gcd.gameControl = gc;
  gcd.player = p;
  gcd.snake = s;
  gcd.level = l1;
  int rc = pthread_create(&threads.gameControlThread, NULL, (void*) gameControlThreadFunction, &gcd);
  if(rc != 0){
  	endwin();
  	printf("\nError creating gameControlThread\n");
  	exit(1);
  }
  
  startLevel(l1, s, p, gc, &threads);
	/////////////////////////////////////////////////////
  while(1){
		sleep(1);
	}
	///////////////// UNSET NCURSES /////////////////////
  endwin();
	/////////////////////////////////////////////////////
	return 0;
}


void buildBox(int** map, int height, int width){
	int i, j; // iterators.

	for(i = 0; i < width; i++){
		for(j = 0; j < height; j++){
			// vertical extremities.
			if(i == 0 || i == height-1)
				map[i][j] = BLOCK;
			// horizontal extremities.
			if(j == 0 || j == width-1)
				map[i][j] = BLOCK;

			if(map[i][j] != BLOCK)
				map[i][j] = BLANK;
		}
	}
}

int canUpdateDirection(Snake* s){
	Position* head = (Position*) lAt(s->bodyPositions, 0);
	Position* neck = (Position*) lAt(s->bodyPositions, 1);
	if(neck == NULL)
		return 1;

	int xDiff = head->x - neck->x;
	int yDiff = head->y - neck->y;

	if((xDiff == 1 || xDiff == -1) && s->newDirection != LEFT && s->newDirection != RIGHT)
		return 1;
	if((yDiff == 1 || yDiff == -1) && s->newDirection != UP && s->newDirection != DOWN)
		return 1;
	return 0;
}
