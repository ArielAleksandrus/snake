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
