#include "utils.h"

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

void mapSnake(int** map, int height, int width, List* snakeBodyParts){
	int i;

	for(i = 0; i < snakeBodyParts->size; i++){
		int part = SNAKE_BODY;
		if(i == snakeBodyParts->size - 1)
			part = SNAKE_TAIL;
		if(i == 0)
			part = SNAKE_HEAD;
		map[((Position*) lAt(snakeBodyParts, i))->x][((Position*) lAt(snakeBodyParts, i))->y] = part;
	}
}

void printHeader(Level* l, Player* p){
	printw("Lives: %d\tScore: %d/%d\n", p->lives, p->score.total, l->targetScore);
}
void printFooter(Snake* s){
	printw("Snake speed %.2f\n", s->speed);
	printw("Snake size: %d", s->bodyPositions->size);
	int i;
	
	Bonus* b;
	char* str;
	
	printw("Active bonuses: ");
	for(i = 0; i < s->activeBonuses->size; i++){
		b = lAt(s->activeBonuses, i);
		switch(b->type){
			case EXTRA_POINTS:{
				str = "Extra points";
				break;
			}
			case EXTRA_SIZE:{
				str = "Extra size";
				break;
			}
			case DOUBLE_POINTS:{
				str = "Double points";
				break;
			}
			case REDUCED_SPEED:{
				str = "Reduced speed";
				break;
			}
			case INVINCIBLE:{
				str = "Invincible";
				break;
			}
			case EXTRA_LIFE:{
				str = "Extra life";
				break;
			}
			case RESET_SIZE:{
				str = "Reset size";
			}
		}
		printw("%s", str);
		if(i != s->activeBonuses->size - 1)
			printw(" | ");
	}
	printw("\n");
}
void printMap(int** map, int width, int height){
	int i, j, color;
	for(i = 0; i < height; i++){
		for(j = 0; j < width; j++){
			char* c;
			switch(map[j][i]){
				case BLANK:
					color = BLACK;
					c = " ";
					break;
				case BLOCK:
					color = WHITE;
					c = " ";
					break;
				case BONUS:
					color = WHITE;
					c = "b";
					break;
				case FOOD:
					color = WHITE;
					c = ".";
					break;
				case SNAKE_HEAD:
					color = RED;
					c = "'";
					break;
				case SNAKE_BODY:
				case SNAKE_TAIL:
					color = GREEN;
					c = " ";
					break;
				default:
					color = WHITE;
					c = "?";
					break;
			}
			attron(COLOR_PAIR(color));
			printw("%s", c);
			attroff(COLOR_PAIR(color));
		}
		printw("\n");
	}
}

void printMapThreadFunction(PrintMapData* pmd){
	struct timespec spec, spec2;
	
	while(1){
  	clear();
  	move(0,0);
  	printHeader(pmd->level, pmd->player);
		printMap(pmd->level->map, pmd->level->width, pmd->level->height);
		printFooter(pmd->snake);
		//INCOMPLETE! - printStats(pmd->level, pmd->player);
  	refresh();

		spec.tv_sec = 0;
		spec.tv_nsec = REFRESH_RATE * 1.0e6;
		nanosleep(&spec, &spec2);
	}
}


Player* newPlayer(){
	Player* p = malloc(sizeof(Player));
	Score s;
	s.extraModifier = 0;
	s.total = 0;
	s.multiplier = 1;
	p->score = s;
	p->lives = 3;
	p->curStats = newStats();
	p->lifetimeStats = loadLifetimeStats();

	return p;
}

Stats newStats(){
	Stats s;

	s.food_eaten = 0;
	s.bonus_eaten = 0;
	s.max_snake_size = 0;
	s.times_died = 0;
	s.times_played = 0;

	return s;
}

Stats loadLifetimeStats(){
	// INCOMPLETE!! -  take care about it later.
	return newStats();
}

GameControl* newGameControl(){
	GameControl* gc = malloc(sizeof(GameControl));
	gc->paused = 0;
	gc->restartLevel = 0;
	gc->advanceLevel = 0;

	return gc;
}
void moveSnake(Snake* s, Level* l, Player* p, GameControl* gc){
	int i;
	Position* head = (Position*) lAt(s->bodyPositions, 0);
	Position prevVal = *head;
	int cud = canUpdateDirection(s);
	if(cud){
		s->direction = s->newDirection;
	}

	switch(s->direction){
		case UP:
			head->y--;
			break;
		case DOWN:
			head->y++;
			break;
		case LEFT:
			head->x--;
			break;
		case RIGHT:
			head->x++;
			break;
	}
	
	// INCOMPLETE!! -- Detect collisions.
	int collision = 0;
	if(l->map[head->x][head->y] != BLANK)
		collision = collide(l, s, p, gc, prevVal);

	if(collision == BLOCK){
		*head = prevVal;
		return;
	}

	l->map[head->x][head->y] = SNAKE_HEAD;

	if(collision != FOOD){
		l->map[prevVal.x][prevVal.y] = BLANK;
		for(i = 1; i < s->bodyPositions->size; i++){
			Position* part = (Position*) lAt(s->bodyPositions, i);
			l->map[part->x][part->y] = BLANK;
			Position aux = *part;

			*part = prevVal;
			l->map[prevVal.x][prevVal.y] = SNAKE_BODY;
			prevVal = aux;
		}
		Position* tail = NULL;
		if(s->bodyPositions->size > 1)
			tail = (Position*) lAt(s->bodyPositions, s->bodyPositions->size - 1);

		if(tail != NULL)
			l->map[tail->x][tail->y] = SNAKE_TAIL;
	}
}
void handleBonus(Snake* s, Player* p){
	if(s->activeBonuses->size == 0)
		return;
	
	int i;
	Bonus* bonus;
	for(i = 0; i < s->activeBonuses->size; i++){
		bonus = lAt(s->activeBonuses, i);
		bonus->duration -= BASE_SPEED * s->speed;
		if(bonus->duration <= 0){
			setBonusEffect(s, p, bonus, 0);
			lremoveAt(s->activeBonuses, i);
			i--;
		}
	}
}
void handleSnake(Snake* s, Level* l, Player* p, GameControl* gc){
	handleBonus(s, p);
	moveSnake(s, l, p, gc);
}

void directionInput(GameControl* gc, Snake* s){
	//////// CAPTURING USER INPUT ////////
  while(1){
  	if(s == NULL){
  		return;
  	}
		char c = getch();
    if (c == '\033') {    // if the first value is esc
      getch();                // skip the [
      switch(getch()) {       // the real value
        case 'A':
          // code for arrow up
          s->newDirection = UP;
          break;
        case 'B':
          // code for arrow down
          s->newDirection = DOWN;
          break;
        case 'C':
          // code for arrow right
          s->newDirection = RIGHT;
          break;
        case 'D':
          // code for arrow left
          s->newDirection = LEFT;
          break;
      }
    } else {
      // INCOMPLETE!! - q for quit, p for pause, etc..
			if(c == 'n' || c == 'N')
				gc->advanceLevel = 1;
    }
  }
  ////////////////////////////////////////
}
void directionInputThreadFunction(DirectionInputData* did){
	directionInput(did->gameControl, did->snake);
}
void handleSnakeThreadFunction(HandleSnakeData* msd){
	struct timespec spec, spec2;
	while(1){
		handleSnake(msd->snake, msd->level, msd->player, msd->gameControl);
		spec = toTimespec(BASE_SPEED * (5.5 - msd->snake->speed / 2));
		nanosleep(&spec, &spec2);
	}
}
void gameControlThreadFunction(GameControlData* gcd){
	struct timespec spec, spec2;
	GameControl* gc = gcd->gameControl;
	while(1){
		// activated when player dies.
		if(gc->restartLevel == 1){
			if(gcd->player->lives <= 0){
				// INCOMPLETE! - update stats.
				endwin();
				system("clear");
				printf("Você perdeu!\n\n");
				exit(0);
				// INCOMPLETE! - print score and stats.
			}
			int levelNumber = gcd->level->levelNumber;
			unloadLevel(gcd->level, gcd->snake, gcd->threads);

			gcd->snake = malloc(sizeof(Snake));
			gcd->level = loadLevel(gcd->snake, levelNumber);
			
			startLevel(gcd->level, gcd->snake, gcd->player, gcd->gameControl, gcd->threads);
			
			gc->restartLevel = 0;
		}

		// player wants to advance to the next level.
		if(gcd->gameControl->advanceLevel == 1){
			// does he/she have enough points?
			if(gcd->player->score.total >= gcd->level->targetScore){
				gcd->gameControl->advanceLevel = 0;
				int levelNumber = gcd->level->levelNumber;
				unloadLevel(gcd->level, gcd->snake, gcd->threads);
				
				gcd->snake = malloc(sizeof(Snake));
				gcd->level = loadLevel(gcd->snake, levelNumber + 1);

				startLevel(gcd->level, gcd->snake, gcd->player, gcd->gameControl, gcd->threads);
			} else {
				gcd->gameControl->advanceLevel = 0;
			}
		}
		spec.tv_sec = 0;
		spec.tv_nsec = 200 * 1.0e6;
		nanosleep(&spec, &spec2);
	}
}

int collide(Level* l, Snake* s, Player* p, GameControl* gc, Position prevHead){
	Position* head = (Position*) lAt(s->bodyPositions, 0);
	switch(l->map[head->x][head->y]){
		case FOOD:{
			p->score.total += (10 + p->score.extraModifier) * p->score.multiplier;
			Position* neck = malloc(sizeof(Position));
			*neck = prevHead;
			linsertAt(s->bodyPositions, neck, 1);
			l->map[neck->x][neck->y] = SNAKE_BODY;
			l->curFoodQty--;
			return FOOD;
		}
		case BONUS:{
			Bonus* b = findBonus(l, *head);
			
			void* aux;
			aux = b;
			int i = lsearchIndexByRef(l->bonuses, aux);
			
			if(i >= 0)
				lremoveNode(l->bonuses, i);
			else{
				endwin();
				printf("Error! Bonus could not be retrieved!\n");
				exit(1);
			}
			setBonusEffect(s, p, b, 1);
			lappend(s->activeBonuses, b);
			break;
		}
		case BLOCK:{
			if(s->invincible)
				return BLOCK;
			
			playerDie(gc, p);
			return BLOCK;
		}
		case SNAKE_BODY:{
			if(s->invincible)
				return SNAKE_BODY;
			
			playerDie(gc, p);
			break;
		}
		case SNAKE_TAIL:{
			if(s->invincible)
				return SNAKE_TAIL;
			
			playerDie(gc, p);
			break;
		}
	}
	return BLANK;
}
void playerDie(GameControl* gc, Player* p){
	if(gc->restartLevel == 0){
		p->lives--;
		p->score.total -= 100;
		if(p->score.total < 0)
			p->score.total = 0;
		
		// reset score modifiers.
		p->score.extraModifier = 0;
		p->score.multiplier = 1;
		gc->restartLevel = 1;
	}
}
struct timespec toTimespec(unsigned int millisseconds){
	struct timespec spec;
	
	spec.tv_sec = millisseconds / 1000;
	spec.tv_nsec = (millisseconds % 1000) * 1.0e6;
	
	return spec;
}