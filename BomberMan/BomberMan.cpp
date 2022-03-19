#include <fstream>
#include <iostream>
#include <string>
using namespace std;
//Program made my Colin Fang 3-16-2022 to 2-18-2022 for Tactic Studios Programming Test (BomberMan)
//Classes: Actor, Board
//Functions: runGame, readInput, fireWeapon, moveActor, enemyDeath, checkWon, playerLost, findInBoard
//Note: I am not very experienced with pointers especially with destroy method and smart pointers. 
//I would love to have the chance to learn from your team!

//Actor represents either the Player, Enemy, Empty or Rock
class Actor {
	public:
		char type;
		char weapon;
		char index;
		Actor(char x = 'O', char y = ' ', char z = ' ') : type(x), weapon(y), index(z) {
			type = x;
			weapon = y;
			index = z;
		}
};
//Board is a 2D array that will hold pointers to Actors and represent the game board
class Board {
	public:
		//std::array<std::array<int, 20>, 20> grid; //attempt at using std::array for memory management (failed)
		Actor* grid[20][20];
		//initialize the board with 1 type of actor
		Board(Actor* actor) {
			for (int i = 0; i < 20; i++) {
				for (int j = 0; j < 20; j++) {
					grid[i][j] = actor;
				}
			}
		}
		void PrintBoard() {
			for (int k = 0; k < 20; k++) {
				for (int l = 0; l < 20; l++) {
					std::cout << grid[k][l]->type << grid[k][l]->weapon << grid[k][l]->index;
				}
				std::cout << "\n";
			}
		}
};

//Receives pointers to coordinates and changes them once target is found;
void findInBoard(Board* board, string inString, int* hor, int* vert) {
	for (int i = 0; i < 20; i++) {
		for (int j = 0; j < 20; j++) {
			if (board->grid[i][j]->type == inString[0]) {
				if (board->grid[i][j]->type == 'P' || board->grid[i][j]->type == 'O') {
					*hor = j;
					*vert = i;
					break;
				}
				else if (board->grid[i][j]->type == 'E') {
					if (board->grid[i][j]->index == inString[1]) { //extra check for index of Enemy
						*hor = j;
						*vert = i;
						break;
					}
				}
			}
		}
	}
}
//Called when player is killed, prints the board and ends the program
void playerLost(Board* board, int vert, int hor) {
	board->grid[vert][hor] = new Actor('X', ' ', ' ');
	cout << "PLAYER_LOST \n";
	board->PrintBoard();
	exit(0);
}
//Called when an enemy is killed, checks for remaining enemies
//prints the board and ends the program if there are no remaining enemies;
void checkWon(Board* board, Actor actors[]) {
	for (int i = 0; i < 11; i++) {
		if (actors[i].type == 'O' || actors[i].type == 'P') {
			if (i >= 10) {
				cout << "PLAYER_WON \n";
				board->PrintBoard();
				exit(0);
			}
			else {
				continue;
			}
		}

		else {
			break;
		}
	}
}
//Called when an enemy is killed
//redirects pointer away from enemy actor to empty actor
//in the array of actors, changes enemy actor to an empty actor
void enemyDeath(Board* board, int hor, int vert, Actor actors[]) {
	//find an empty actor to point to later
	int emptyHor;
	int emptyVert;
	int index = board->grid[vert][hor]->index - '0';

	findInBoard(board, "O  ", &emptyHor, &emptyVert);
	actors[index] = *board->grid[emptyVert][emptyHor];
	board->grid[vert][hor] = board->grid[emptyVert][emptyHor];
	checkWon(board, actors);

}
//Checks if can move
//redirects the target location's pointer to point to the actor who wants to move
//original location's pointer points to empty actor
void moveActor(Board* board, int hor, int vert, char dir) {
	Actor *tempPointer;//this will hold our target location
	int h = hor;
	int v = vert;
	switch (dir) {
		case 'U':
			if (vert <= 0) {//out of bounds
				break;
			}
			h = hor;
			v = vert - 1;
			break;
		case 'D':
			if (vert >= 19) {//out of bounds
				break;
			}
			h = hor;
			v = vert + 1;
			break;
		case 'L':
			if (hor <= 0) {//out of bounds
				break;
			}
			h = hor - 1;
			v = vert;
			break;
		case 'R':
			if (hor >= 19) {//out of bounds
				break;
			}
			h = hor + 1;
			v = vert;
			break;
	}
	if (board->grid[v][h]->type == 'O') {
		tempPointer = board->grid[v][h];//holds the empty actor of our target location
		board->grid[v][h] = board->grid[vert][hor];//both target and original location hold the actor who wants to move
		board->grid[vert][hor] = tempPointer;}//the original location points to empty actor
	else {
		std::cout << "the actor could not move because the adjacent space was not empty \n";
	}
	
}
//searches for any enemy or player actor in hitbox and kills them
void fireWeapon(Board* board, int hor, int vert, Actor actors[]) {
	char wType = board->grid[vert][hor]->weapon;//weapon type
	switch (wType) {
		case 'A'://AOE 3x3 square
			for (int i = -1; i <= 1; i++) {
				for (int j = -1; j <= 1; j++) {
					if ((i == 0 && j == 0) || 19 < vert + i || 19 < hor + j || vert + i < 0 || hor + j < 0) {
						continue;
					}
					else if (board->grid[vert + i][hor + j]->type == 'E') {//if hit enemy
						enemyDeath(board, hor + j, vert + i, actors);
					}
					else if (board->grid[vert + i][hor + j]->type == 'P') {//if hit player
						playerLost(board, vert + i, hor + j);
					}
				}
			}break;
		case 'H'://Horizontal Line
			for (int i = hor + 1; i < 20; i++) {
				if (board->grid[vert][i]->type == 'R') {//if hit rock then stop
					break;
				}else if (board->grid[vert][i]->type == 'E') {//if hit enemy
					enemyDeath(board, i, vert, actors);
				}else if (board->grid[vert][i]->type == 'P') {//if hit player
					playerLost(board, vert, i);
				}
			}
			for (int i = hor - 1; i >= 0; i--) {
				if (board->grid[vert][i]->type == 'R') {//if hit rock then stop
					break;
				}
				else if (board->grid[vert][i]->type == 'E') {//if hit enemy
					enemyDeath(board, i, vert, actors);
				}else if (board->grid[vert][i]->type == 'P') {//if hit player
					playerLost(board, vert, i);
				}
			}break;
		case 'V'://Vertical Line
			for (int i = vert + 1; i < 20; i++) {
				if (board->grid[i][hor]->type == 'R') {//if hit rock then stop
					break;
				}
				else if (board->grid[i][hor]->type == 'E') {//if hit enemy
					enemyDeath(board, hor, i, actors);
				}else if (board->grid[i][hor]->type == 'P') {//if hit player
					playerLost(board, i, hor);
				}
			}
			for (int i = vert - 1; i >= 0; i--) {
				if (board->grid[i][hor]->type == 'R') {//if hit rock then stop
					break;
				}
				else if (board->grid[i][hor]->type == 'E') {//if hit enemy
					enemyDeath(board, hor, i, actors);
				}else if (board->grid[i][hor]->type == 'P') {//if hit player
					playerLost(board, i, hor);
				}
			}break;
	}
}
//after initializing the board, this will interpret the enemy and player actions
void readInput(Board *board, string inString, Actor actors[]) {
	//initialize coordinates as out of bounds
	int hor = -1;
	int vert = -1;
	if (inString[0] == 'P') {//player action
		findInBoard(board, inString, &hor, &vert);//find the player
		if (hor == -1) {//if player doesn't exist
			playerLost(board, 0, 0);
		}
		if (inString[1] == 'F') {
			fireWeapon(board, hor, vert, actors);
		}
		else if (inString[1] == 'U' || inString[1] == 'D' || inString[1] == 'L' || inString[1] == 'R') {
			moveActor(board, hor, vert, inString[1]);
		}
		else {
			std::cout << "the input is invalid";
		}
	}
	else if (inString[0] == 'E') {
		findInBoard(board, inString, &hor, &vert);
		if (hor == -1) {//if enemy doesn't exist
			checkWon(board, actors);
		}
		if (inString[2] == 'F') {
			fireWeapon(board, hor, vert, actors);
		}
		else if (inString[2] == 'U' || inString[2] == 'D' || inString[2] == 'L' || inString[2] == 'R') {
			moveActor(board, hor, vert, inString[2]);
		}
		else {
			std::cout << "the input is invalid";
		}

	}
}
//parses the text file to initialize the board and set inputs to readInput()
void runGame() {
	//Create the empty actor, rock actor, and empty array of actors;
	
	//unique_ptr<Actor> emptyActor(new Actor('O', ' ', ' '));
	Actor emptyActor('O', ' ', ' ');
	Actor rock('R', ' ', ' ');
	Actor actors[11];//this will hold 10 enemies(0-9) and 1 player(10)
	Board board(&emptyActor);//Use the empty actor to instantiate the grid

	ifstream infile("input.txt");
	char inchar;
	char enchar;//extra character used for enemy actor
	bool playerExists = false;//only read one player actor
	try {//error handling
		//double for loop to loop through the initial 400 actors
		for (int i = 0; i < 20; i++) {
			for (int j = 0; j < 20; j++) {
				if (infile.is_open()) {
					inchar = infile.get();
					switch (inchar) {
						case 'O':
							board.grid[i][j] = &emptyActor;
							//skip 3 spaces
							inchar = infile.get();
							inchar = infile.get();
							inchar = infile.get();
							break;
						case 'R':
							board.grid[i][j] = &rock;
							//skip 3 spaces
							inchar = infile.get();
							inchar = infile.get();
							inchar = infile.get();
							break;
						case 'P':
							if (!playerExists) {
								playerExists = true;
								inchar = infile.get();
								actors[10].type = 'P';
								actors[10].weapon = inchar;
								board.grid[i][j] = &actors[10];
								//skip 2 spaces
								inchar = infile.get();
								inchar = infile.get();
								break;
							}else{//player already exists initialize with empty actor instead
								board.grid[i][j] = &emptyActor;
								//skip 3 spaces
								inchar = infile.get();
								inchar = infile.get();
								inchar = infile.get();
								break;
							}
						
						case 'E':
							enchar = infile.get();
							inchar = infile.get();
							int index = inchar - '0';
							actors[index].type = 'E';
							actors[index].weapon = enchar;
							actors[index].index = inchar;
							inchar = infile.get();//skip 1 space
							board.grid[i][j] = &actors[index];
							break;
						}
						if (j == 19) {//at the end of the line, skip the \r and \n
							inchar = infile.get();
							inchar = infile.get();
						}
				}
				else {
					std::cout << "file could not be opened";
				}
			}
			std::cout << "\n";
		}
	}
	catch (...) {
		std::cout << "ERROR";
		std::exit(EXIT_FAILURE);
	}
	//end of building the array and beginning of reading inputs
	string input;
	if (infile.is_open()) {
		while (infile) {
			std::getline(infile, input);
			readInput(&board, input, actors);
			//board.PrintBoard();
		}
	}
	std::cout << "STALEMATE \n";
	board.PrintBoard();
	std::exit(0);
}
int main() {
	runGame();
	return 0;
}