// GeneticAlgorithm.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "stdlib.h"
#include <iostream>
#include <time.h>

#define GENS		100

#define MAX_COLS    5
#define MAX_ROWS    5
#define CHROMOSOMES 25
#define GENOME_SIZE 50
#define NUM_AGENTS  10
#define CROSSOVER   0.7
#define MUTATION    0.1
#define GOAL_X		4
#define GOAL_Y		2

#define UP_ACTION	 "00"
#define RIGHT_ACTION "01"
#define DOWN_ACTION	 "10"
#define LEFT_ACTION	 "11"

#define UP			0
#define RIGHT		1
#define DOWN		2
#define	LEFT		3

#define RANDOM_NUM  ((float)rand()/(RAND_MAX+1))

/* Class Agent
* Represents the agent that moves according to the genome
*/
class Agent {
public:
	char *genome;
	int  x;
	int  y;
	int  score;
	int  steps;

	Agent();
	~Agent() {delete genome;};
	void setGenome(char*);
	void setLocation(int, int);
	void generateGenome();
	int goalCheck();
	void crossWith(Agent*);
	void mutate();
	void move();
	void status();
};

/* Function Prototypes */
char* crossover(Agent *, Agent *);
void cycle(Agent **);
void newPopulation(Agent **);
void printBoard(Agent **);

Agent::Agent() {
	genome = new char[GENOME_SIZE + 1]();
	score = 0;
	x = 0;
	y = 0;
	steps = 0;
}

void Agent::setGenome(char *newGenome) {
	delete[] genome;
	genome = newGenome;
}

void Agent::setLocation(int new_x, int new_y) {
	x = new_x;
	y = new_y;
}

void Agent::generateGenome() {

	for(int i = 0; i < GENOME_SIZE; i++) {
		char temp = '0' + rand() % 2;
		genome[i] = temp;
	}
}

/*
* goalCheck()
* PRECONDITION:		Agent is valid and is at some x, y
* POSTCONDITION:	0 is returned if Agent is at goal, 1 otherwise.
*/
int Agent::goalCheck() {
	if(x == GOAL_X && y == GOAL_Y) {
		return 0;
	}

	return 1;
}

void Agent::mutate() {
	for(int i = 0; i < GENOME_SIZE; i++) {
		if(RANDOM_NUM < MUTATION) {
			if(genome[i] == '1')
				genome[i] = '0';
			else
				genome[i] = '1';
		}
	}
}

void Agent::crossWith(Agent *newAgent) {

	if(RANDOM_NUM < CROSSOVER) {
		int crossoverPoint = (int) (RANDOM_NUM * CHROMOSOMES);

		char t1[GENOME_SIZE];
		char t2[GENOME_SIZE];

		for (int i = 0; i < crossoverPoint; i++) {
			t1[i] = genome[i];
			t2[i] = newAgent->genome[i];
		}

		for (int i = crossoverPoint; i < GENOME_SIZE; i++) {
			t1[i] = newAgent->genome[i];
			t2[i] = genome[i];
		}
		std::cout << sizeof(genome);
		strncpy_s(genome, (GENOME_SIZE + 1)*sizeof(char), t1, GENOME_SIZE);
		newAgent->genome = t2;
	}
}

/*
* Agent::move
* PRECONDITION:  Agent exists and is at some location
* POSTCONDITION: Agent is moved to a new location depending on genome
*/
void Agent::move() {
	// local variables
	int location = 0;
	int action   = 0;

	// Check if agent is at goal
	if(!goalCheck())
		return;

	// Using current location, convert to place in genome
	location = (x * MAX_COLS + y) * 2;

	// Get chromosome and determine action
	char actionArray[2] = { genome[location], genome[location+1] };

	if(!strncmp(actionArray, UP_ACTION, 2)) {
		action = UP;
	} else if(!strncmp(actionArray, RIGHT_ACTION, 2)) {
		action = RIGHT;
	} else if(!strncmp(actionArray, DOWN_ACTION, 2)) {
		action = DOWN;
	} else {
		action = LEFT;
	}

	// Update position based on action
	switch(action) {
	case UP:
		if(y != 0)
			y--;
		break;
	case RIGHT:
		if(x != MAX_COLS - 1)
			x++;
		break;
	case DOWN:
		if(y != MAX_ROWS - 1)
			y++;
		break;
	case LEFT:
		if(x != 0)
			x--;
		break;
	default:
		break;
	}

	// Increment the steps by 1
	steps++;
}

/*
* Agent::status
* PRECONDITION:		Agent exists
* POSTCONDITION:	Information about the agent is printed to console
*/
void Agent::status() {
	std::cout << "Location: " << x << ", " << y << std::endl;
	std::cout << "Steps:    " << steps << std::endl;
	std::cout << std::endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	// Generate random agents
	Agent *agents[NUM_AGENTS];

	// Set random seed
	time_t seconds;

	// Get time
	time(&seconds);

	srand((unsigned int) seconds);

	for(int i = 0; i < NUM_AGENTS; i++) {
		agents[i] = new Agent();
		agents[i]->setLocation(0, 0);
		agents[i]->generateGenome();
	}

	newPopulation(agents);

	for(int i = 0; i < GENS; i++) {
		for(int j = 0; j < MAX_COLS * MAX_ROWS; j++) {
			//std::cout << "Cycle " << j << std::endl;
			cycle(agents);
		}

		printBoard(agents);

		// Calculate fitness for each
		for(int j = 0; j < NUM_AGENTS; j++) {
			// Agent must reach the goal to get a score
			//agents[j]->score = (MAX_COLS * MAX_ROWS) - agents[j]->steps;

			// Manhattan Distance
			agents[j]->score = (MAX_COLS + MAX_ROWS) - ((int)abs(agents[j]->x - GOAL_X) + (int)abs(agents[j]->y - GOAL_Y));
		}

		// Breed new offspring
		newPopulation(agents);
	}

	return 0;
}

/*
* crossover
* PRECONDITION:	Two valid agents are given
* POSTCONDITION:	A new genome is returned based on the genomes of the two parents
*/
char* crossover(Agent *a1, Agent *a2) {
	int crossoverPoint = (int) (RANDOM_NUM * CHROMOSOMES);

	char *t1 = new char[GENOME_SIZE + 1]();

	// Take the first portion from the first parent
	for (int i = 0; i < crossoverPoint; i++) {
		t1[i] = a1->genome[i];
	}

	// Take the second portion from the second parent
	for (int i = crossoverPoint; i < GENOME_SIZE; i++) {
		t1[i] = a2->genome[i];
	}

	return t1;
}

/*
* cycle
* PRECONDITION:	Agents are valid
* POSTCONDITION:	Agents are moved and positions are updated
*/
void cycle(Agent **agents) {
	// Move all of the agents
	for(int i = 0; i < NUM_AGENTS; i++) {
		agents[i]->move();

		//agents[i]->status();
	}
}

/*
* newPopulation
* PRECONDITION:	A valid population exists
* POSTCONDITION:	Generates a new population based on crossover and mutation rates
*/
void newPopulation(Agent **agents) {
	// Select two parents based on fitness
	int p1 = 0, p2 = 0;
	int max = 0;

	for(int i = 0; i < NUM_AGENTS; i++) {
		if(agents[i]->score > max) {
			p1 = i;
			p2 = p1;  // Second highest
			max = agents[i]->score;
		}
		// Set default values
		agents[i]->steps = 0;
		agents[i]->setLocation(0, 0);
	}

	// Printout
	std::cout << "First parent score:  " << agents[p1]->score << std::endl;
	std::cout << "Second parent score: " << agents[p2]->score << std::endl;

	// Crossover to form new offspring
	for(int i = 0; i < NUM_AGENTS; i++) {
		if(RANDOM_NUM < CROSSOVER && i != p1 && i != p2) {
			agents[i]->setGenome(crossover(agents[p1], agents[p2]));
		}
	}

	// Mutate new offspring at each locus
	for(int i = 0; i < NUM_AGENTS; i++) {
		if(RANDOM_NUM < MUTATION) {
			std::cout << "Genome before mutation: " << agents[i]->genome << std::endl;
			agents[i]->mutate();
			std::cout << "Genome after mutation:  " << agents[i]->genome << std::endl;
		}
	}
}

void printBoard(Agent **agents) {
	char *board = new char[MAX_ROWS * MAX_COLS]();

	// Set board with blank spaces
	memset(board, ' ', MAX_ROWS*MAX_COLS);

	// Fill with agents
	for(int i = 0; i < NUM_AGENTS; i++) {
		int location = agents[i]->x * MAX_COLS + agents[i]->y;
		board[location] = '*';
	}

	// Fill with goal
	int goal = GOAL_X * MAX_COLS + GOAL_Y;
	board[goal] = 'G';

	// Print board
	for(int i = 0; i < MAX_COLS; i++) {
		for(int j = 0; j < MAX_ROWS; j++) {
			std::cout << board[i*MAX_COLS+j];
		}
		std::cout << std::endl;
	}
}