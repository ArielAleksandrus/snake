#ifndef TYPES_H
#define TYPES_H

#include <pthread.h>
#include "list.h"

#define SNAKE_HEAD 	1
#define SNAKE_BODY 	2
#define SNAKE_TAIL 	3
#define FOOD				4
#define BONUS				5
#define BLOCK				6
#define BLANK				7

//////////// AVAILABLE BONUSES //////////////
#define EXTRA_POINTS 	10
#define EXTRA_SIZE		20
#define DOUBLE_POINTS	30
#define REDUCED_SPEED	40
#define INVINCIBLE 		50
#define EXTRA_LIFE 		60
#define HALF_SIZE		70
/////////////////////////////////////////////

#define LEFT 	1
#define UP		2
#define RIGHT	3
#define DOWN	4


#define BLACK			1
#define WHITE			2
#define RED				3
#define GREEN			4
#define YELLOW		5

#define BASE_SPEED 100

#define REFRESH_RATE 100
#define FOOD_THREAD_RATE 500
#define BONUS_THREAD_RATE 1000
#define SNAKE_MOVE_THREAD_RATE BASE_SPEED * (5.5 - snake->speed / 2)
#define GAME_CONTROL_THREAD_RATE 200


typedef struct Stats{
	int food_eaten;
	int bonus_eaten;
	int max_snake_size;
	int times_died;
	int times_played;
}Stats;

typedef struct Score{
	int total;
	int extraModifier;
	int multiplier;
}Score;
typedef struct Player{
	Score score;
	int lives;
	Stats curStats;
	Stats lifetimeStats;
}Player;

typedef struct Position{
	int x;
	int y;
	int direction;
}Position;

typedef struct Snake{
	Position head;
	Position tail;
	List* bodyPositions;
	List* activeBonuses;

	int direction;
	int newDirection;
	float speed; // 0 for the slowest possible, 10 for the fastest possible.
	
	int invincible;
}Snake;

typedef struct Bonus{
	Position position;
	int type; // see "Available Bonuses" constants.
	int duration; // duration of effect in ms.
	int rarity; // 0 is the most common. 6 is the most rare.
	int timeout; // timeout to be removed from the map if not caught.
}Bonus;

typedef struct SpawnData{
	int frequency; // frequency in number of seconds.
	int generosity; // 0 for min generosity. 3 for max generosity.
	int itemsAtOnce;
	int expiring_in; // millisseconds until the bonus fades if not caught.
	int maxItemsAtOnce;
	List* whatCanBeSpawned;
}SpawnData;

typedef struct Level{
	int levelNumber;

	Position spawnPoint;
	float initialSpeed; // 0 for the slowest possible, 10 for the fastest possible.
	int initialDirection;

	int** map;
	int width;
	int height;
	
	int targetScore;

	int curFoodQty;
	List* bonuses;
	SpawnData bonusSpawnRules;
}Level;

typedef struct GameControl{
	int paused;
	int restartLevel;
	int advanceLevel;
}GameControl;

typedef struct BonusThreads{
	pthread_t extraSizeThread;
}BonusThreads;

typedef struct PrintMapData{
	Level* level;
	Player* player;
	Snake* snake;
}PrintMapData;
typedef struct SpawnFoodData{
	Level* level;
}SpawnFoodData;
typedef struct SpawnBonusData{
	Level* level;
	SpawnData spawnData;
}SpawnBonusData;
typedef struct HandleSnakeData{
	Snake* snake;
	Level* level;
	Player* player;
	GameControl* gameControl;
	BonusThreads* bonusThreads;
}HandleSnakeData;
typedef struct DirectionInputData{
	GameControl* gameControl;
	Snake* snake;
}DirectionInputData;
typedef struct Threads{
	pthread_t gameControlThread;
	pthread_t directionInputThread;
	pthread_t handleSnakeThread;
	pthread_t printMapThread;
	pthread_t spawnFoodThread;
	pthread_t spawnBonusThread;
	BonusThreads bonusThreads;
	List* args; // arguments provided for these threads (except gameControlThread
							// and directionInputThread) so they can be freed.
}Threads;
typedef struct GameControlData{
	GameControl* gameControl;
	Threads* threads;
	Level* level;
	Player* player;
	Snake* snake;
}GameControlData;
#endif