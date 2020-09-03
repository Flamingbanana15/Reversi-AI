// Reversi Game.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include <sstream>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

class GameBoard {
public:
	int board[8][8];
	const int compyAI[8][8] = { {16,  -6,  7,  7,  7,  7,  -6, 16},
								{-6, -12, -4, -4, -4, -4, -12, -6},
								{ 7,  -4,  2,  0,  0,  2,  -4,  7},
								{ 7,  -4,  0,  1,  1,  0,  -4,  7},
								{ 7,  -4,  0,  1,  1,  0,  -4,  7},
								{ 7,  -4,  2,  0,  0,  2,  -4,  7},
								{-6, -12, -4, -4, -4, -4, -12, -6},
								{16,  -6,  7,  7,  7,  7,  -6, 16} };

public:
	GameBoard();
	void printBoard();
	bool makeMove(int, int, int);
	bool checkPath(int, int, int, int, int, bool);
	bool validateMove(int, int, int);
	bool canMove(int);
	void showMoves(int);
	bool fullGame();
	void clearBoard();
	int scoreBoard(int);
	void copyBoard(GameBoard*);
	int evaluateBoard(int);
	//int stableBoard(int);
	bool evaluateCorners(int, int, int);
	int stablePieces(int, int, int);
	bool makeCompyMove(GameBoard*, int);
	int countNeighbors(int, int);
};

int minValue(GameBoard* oldBoard, int player, int alpha, int beta, int depth, int maxDepth, time_t startingTime);
int maxValue(GameBoard* oldBoard, int player, int alpha, int beta, int depth, int maxDepth, time_t startingTime);
std::pair<int, int> minimax(GameBoard* game, int player);
GameBoard::GameBoard() {
	memset(board, 0, sizeof(board));
	//Need to initilize the starting positions
	//1 is black -1 is white
	board[3][3] = -1;
	board[4][4] = -1;
	board[3][4] = 1;
	board[4][3] = 1;
}

void GameBoard::printBoard() {
	std::stringstream game;
	char tiles[] = { 'w', 'W', '_', 'B', 'b'};
	game << "  0 1 2 3 4 5 6 7" << std::endl;
	for (int r = 0; r < 8; r++) {
		game << r << '|';
		for (int c = 0; c < 8; c++)
			game << tiles[board[r][c] + 2] << '|';
		game << std::endl;
	}
	std::cout << game.str();
}

bool GameBoard::makeMove(int row, int col, int player) {
	if (!(validateMove(row, col, player))) {
		return false;
	}
	board[row][col] = player;
	for (int r = -1; r <= 1; r++) {
		for (int c = -1; c <= 1; c++) {
			checkPath(row, col, r, c, player, true);
		}
	}
	return true;
}

bool GameBoard::checkPath(int row, int col, int r, int c, int player, bool flipTiles) {
	int rowPath = row + r;
	int colPath = col + c;
	
	if (rowPath < 0 || rowPath > 7 || colPath < 0 || colPath > 7) {
		return false;
	}
	else if (board[rowPath][colPath] != -(player)) {
		return false;
	}
	while (true) {
		rowPath += r;
		colPath += c;
		if (rowPath < 0 || rowPath > 7 || colPath < 0 || colPath > 7 || board[rowPath][colPath] == 0 || board[rowPath][colPath] == -2 || board[rowPath][colPath] == 2) {
			return false;
		}
		if (board[rowPath][colPath] == player) {
			if (flipTiles) {
				rowPath = row+ r;
				colPath = col + c;
				while (board[rowPath][colPath] != player) {
					board[rowPath][colPath] = player;
					rowPath += r;
					colPath += c;
				}
			}
			return true;
		}
	}
	return false;
}

bool GameBoard::validateMove(int row, int col, int player) {
	if (row < 0 || row > 7 || col  < 0 || col  > 7) {
		return false;
	}

	if (board[row][col] != 0) {
		return false;
	}

	for (int r = -1; r <= 1; r++)
		for (int c = -1; c <= 1; c++) {
			if (checkPath(row, col, r, c, player, false))
				return true;
		}
	return false;
}

bool GameBoard::canMove(int player) {
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			if (validateMove(row, col, player)) {
				return true;
			}
		}
	}
	return false;
}

void GameBoard::showMoves(int player) {
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			if (validateMove(row, col, player)) {
				board[row][col] = player * 2;
			}
		}
	}
}

bool GameBoard::fullGame() {
	for (int r = 0; r < 8; r++) {
		for (int c = 0; c < 8; c++) {
			if (board[r][c] == 0) {
				return false;
			}
		}
	}
	return true;	
}

void GameBoard::clearBoard() {
	for (int r = 0; r < 8; r++) {
		for (int c = 0; c < 8; c++) {
			if (board[r][c] != -1 && board[r][c] != 1) {
				board[r][c] = 0;
			}
		}
	}
}

int GameBoard::scoreBoard(int player) {
	int sum = 0;
	for (int r = 0; r < 8; r++) {
		for (int c = 0; c < 8; c++) {
			if (board[r][c] == player) {
				sum++;
			}
		}
	}
	return sum;
}

void GameBoard::copyBoard(GameBoard* oldBoard) {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			board[i][j] = oldBoard->board[i][j];
		}
	}
}

int GameBoard::evaluateBoard(int player) {
	int unused = scoreBoard(0);
	int playerMoves = scoreBoard(player);
	int opponentMoves = scoreBoard(-player);
	int score = 0 + rand()&63;
	//Available moves
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (validateMove(i, j, player)) {
				score += 30 * compyAI[i][j];
				if (unused <= 6) {
					score += 30;
				}
				if (unused <= 20) {
					score += -20 * countNeighbors(i, j);
				}
				else {
					score += (60 * playerMoves - opponentMoves) / (playerMoves + opponentMoves + 1);
				}
			}
			if (validateMove(i, j, -player)) {
				score -= 30 * compyAI[i][j];
				if (unused <= 6) {
					score -= 30;
				}
				if (unused <= 20) {
					score -= -20 * countNeighbors(i, j);
				}
				else {
					score -= (60 * playerMoves - opponentMoves) / (playerMoves + opponentMoves + 1);
				}
			}
			if (board[i][j] == player) {
				score += 80 * stablePieces(player, i, j) * compyAI[i][j];
				
			}
			if (board[i][j] == -player) {
				score -= 80 * stablePieces(-player, i, j) * compyAI[i][j];
				
			}
		}
	}
	return (score/5);
}


bool GameBoard::evaluateCorners(int row, int col, int player) {
	if (board[row][col] == player) {
		return true;
	}
	int r = -1, c = -1;
	if (row == 0) {
		r = 1;
	}
	if (col == 0) {
		c = 1;
	}
	for (int i = row; i < row + 8 * r; i++) {
		for (int j = row; j < row + 8 * r; j++) {

		}
	}
	return false;
}
/*
int GameBoard::stableBoard(int player) {
	int tempSpot;
	int tempRow, tempCol;
	int firstEncountered, secondEncountered;

	int opponentStability = 0, playerStability = 0;
	int spotStable;

	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			//std::cout <<row << ", " << col << std::endl;
			spotStable = 2;
			tempSpot = board[row][col];

			// if occupied by no one then skip		
			if (tempSpot == 0) {
				continue;
			}
			// look in all the directions
			for (int rowDirection = -1; rowDirection <= 1; rowDirection++) {
				for (int colDirection = -1; colDirection <= 1; colDirection++) {
					//std::cout << rowDirection << ", " << colDirection << std::endl;
					if (rowDirection == 0 && colDirection == 0) {
						continue;
					}
					tempRow = row + rowDirection;
					tempCol = col + colDirection;

					firstEncountered = tempSpot;
					secondEncountered = tempSpot;

					while (tempRow >= 0 && tempRow < 8 && tempCol >= 0 && tempCol < 8) {
						//std::cout << tempRow << ", " << tempCol << std::endl;
						if (board[tempRow][tempCol] != tempSpot) {
							firstEncountered = board[tempRow][tempCol];
							break;
						}
						tempRow += rowDirection;
						tempCol += colDirection;
					}

					if (firstEncountered == tempSpot) {
						continue;
					}

					tempRow = row - rowDirection;
					tempCol = col - colDirection;

					while (tempRow >= 0 && tempRow < 8 && tempCol >= 0 && tempCol < 8) {
						//std::cout << tempRow << ", " << tempCol << std::endl;
						if (board[tempRow][tempCol] != tempSpot) {
							secondEncountered = board[tempRow][tempCol];
							//std::cout << "brake" << std::endl;
							break;
						}

						tempRow -= rowDirection;
						tempCol -= colDirection;
					}

					if (secondEncountered == tempSpot) {
						continue;
					}
					// if 2 spots of the opponents color are encountered then semi stable
					if (firstEncountered == secondEncountered) {
						if (spotStable > 1)
							spotStable = 1;
					}
					// else unstable as the oppnent can catch it
					else {
						spotStable = 0;
						goto here;
					}
				}
			}
		here:;
			if (tempSpot == player) {
				playerStability += spotStable;
			}
			else if (tempSpot == -player) {
				opponentStability += spotStable;
			}
		}
	}
	//std::cout << "Player: "<<playerStability << std::endl;
	//std::cout << "opponent: " << opponentStability << std::endl;
	return ((100 * (playerStability - opponentStability)) / (playerStability + opponentStability + 1)) + 1;
}
*/


int GameBoard::stablePieces(int player, int row, int col) {
	int tempSpot;
	int tempRow, tempCol;
	int firstEncountered, secondEncountered;
	int spotStable;
	bool breakOut = false;
	spotStable = 2;
	tempSpot = board[row][col];
	// if occupied by no one then skip		
	if (tempSpot == 0) {
		return 1;
	}
	// look in all the directions
	for (int rowDirection = -1; rowDirection <= 1; rowDirection++) {
		for (int colDirection = -1; colDirection <= 1; colDirection++) {
			if (rowDirection == 0 && colDirection == 0) {
				continue;
			}
			tempRow = row + rowDirection;
			tempCol = col + colDirection;

			firstEncountered = tempSpot;
			secondEncountered = tempSpot;

			while (tempRow >= 0 && tempRow < 8 && tempCol >= 0 && tempCol < 8) {

				if (board[tempRow][tempCol] != tempSpot) {
					firstEncountered = board[tempRow][tempCol];
					break;
				}
				tempRow += rowDirection;
				tempCol += colDirection;
			}
			if (firstEncountered == tempSpot) {
				continue;
			}

			tempRow = row - rowDirection;
			tempCol = col - colDirection;

			while (tempRow >= 0 && tempRow < 8 && tempCol >= 0 && tempCol < 8) {
	
				if (board[tempRow][tempCol] != tempSpot) {
					secondEncountered = board[tempRow][tempCol];
					break;
				}

				tempRow -= rowDirection;
				tempCol -= colDirection;
			}

			if (secondEncountered == tempSpot) {
				continue;
			}
			// if 2 spots of the opponents color are encountered then semi stable
			if (firstEncountered == secondEncountered) {
				if (spotStable > 1) {
					spotStable = 1;
				}
			}
			// else unstable as the oppnent can catch it
			else {
				spotStable = 0;
				breakOut = true;
				break;
			}
		}
		if (breakOut) {
			break;
		}
	}
	return (4*spotStable) + 1;
}

int minValue(GameBoard* oldBoard, int player, int alpha, int beta, int depth, int maxDepth, time_t startingTime) {
	std::unique_ptr<GameBoard> tempBoard(new GameBoard());
	tempBoard->copyBoard(oldBoard);

	int maximumValue = alpha;
	int tempValue;
	//Stops when gets to maxDepth or when game is over
	time_t now;
	time(&now);

	if (depth == maxDepth || difftime(now, startingTime) >= 30) {
		return oldBoard->evaluateBoard(player); //Something needs to go here
	}
	if (oldBoard->fullGame() || ((oldBoard->canMove(player)) && (oldBoard->canMove(-player)))) {
		int score = oldBoard->scoreBoard(player);
		int score2 = oldBoard->scoreBoard(-player);
		if (score == score2) {
			return 0;
		}
		else if (score > score2) {
			return 32768;
		}
		else {
			return -32768;
		}
	}

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (oldBoard->validateMove(i, j, -player)) {
				if (oldBoard->makeMove(i, j, -player)) {
					tempValue =   maxValue(oldBoard, player, alpha, beta, depth++, maxDepth, startingTime);
					//Find the min max value
					if (maximumValue > tempValue) {
						maximumValue = tempValue;
					}
					//Reset board
					oldBoard->copyBoard(tempBoard.get());

					//Alpha-beta pruning 
					/*
					if (maximumValue < beta) {
						return beta;
					}
					*/
				}
			}
		}
	}
	return maximumValue;
}
int maxValue(GameBoard* oldBoard, int player, int alpha, int beta, int depth, int maxDepth, time_t startingTime) {
	std::unique_ptr<GameBoard> tempBoard(new GameBoard());
	tempBoard->copyBoard(oldBoard);

	int minimumValue = alpha;
	int tempValue;
	//Stops when gets to maxDepth or when game is over
	time_t now;
	time(&now);
	//30 second limit on turns otherwise the starting game takes a long time
	if (depth == maxDepth || difftime(now, startingTime) >= 30) {
		return oldBoard->evaluateBoard(player); //Something needs to go here
	}
	if (oldBoard->fullGame() || ((oldBoard->canMove(player)) && (oldBoard->canMove(-player)))) {
		int score = oldBoard->scoreBoard(player);
		int score2 = oldBoard->scoreBoard(-player);
		if (score == score2) {
			return 0;
		}
		else if (score > score2) {
			return 32768;
		}
		else {
			return -32768;
		}
	}

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (oldBoard->validateMove(i, j, -player)) {
				if (oldBoard->makeMove(i, j, -player)) {
					tempValue =   minValue(oldBoard, player, alpha, beta, depth++, maxDepth, startingTime);
					//Find the max min value
					if (minimumValue < tempValue) {
						minimumValue = tempValue;
					}
					//Reset board
					oldBoard->copyBoard(tempBoard.get());

					
					//Alpha-beta pruning 
					/*
					if (minimumValue > alpha) {
						return alpha;
					}
					*/
				}
			}
		}
	}
	return minimumValue;
}


std::pair<int, int> minimax(GameBoard* game, int player) {
	GameBoard* tempBoard = new GameBoard();
	tempBoard->copyBoard(game);

	int tempValue, maxX, maxY;
	//Needed something larger that what evaluate can return
	int maximumValue = -32768;

	bool pass = true;
	int maxDepth = 0;

	time_t start;
	time_t now;
	time(&start);
	std::pair<int, int> coords;
	while (true) {
		maxDepth++;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if (tempBoard->validateMove(i, j, player)) {
					if (tempBoard->makeMove(i, j, player)) {

						tempValue = maxValue(tempBoard, player, 32768, -32768, 1, maxDepth, start);
						if (tempValue != 0)
						if (tempValue >= maximumValue && tempValue < 32768) { // found a new minimum max value
							//std::cout << i << ", " << j << ": " << tempValue << std::endl;
							pass = false;
							maxX = i;
							maxY = j;
							maximumValue = tempValue;
						}
						tempBoard->copyBoard(game);
					}
				}
			}
		}
		time(&now);
		if (difftime(now, start) >= 30 || maxDepth > 128*(64-game->scoreBoard(0)))
			break;
	}

	if (pass) {
		pass = false;
		maxX = -1; // just return something so comp can pass
		maxY = -1;
	}

	coords.first = maxX;
	coords.second = maxY;
	return coords;
}

bool GameBoard::makeCompyMove(GameBoard* game, int player) {
	std::pair<int, int> temp = minimax(game, player);
	if (temp.first != -1 && temp.second != -1) {
		if (game->makeMove(temp.first, temp.second, player))
			return true;
	}
	std::cout << "Computer skips" << std::endl;
	return false;
}

int GameBoard::countNeighbors(int x, int y) {
	int count = 0;
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			if (x + i > 8 || x + i < 0 || y + j > 8 || y + j < 0) {
				continue;
			}
			if (board[x + i][y + j] == 0) {
				count++;
			}
		}
	}
	return count++;
}


void playerVsPlayer(int first) {
	GameBoard* reversi = new GameBoard();
	int row, col;
	int consecutiveSkips = 0;
	while (!reversi->fullGame() && consecutiveSkips < 2) {
		if (!reversi->canMove(first)) {
			std::cout << "No moves available" << std::endl;
			consecutiveSkips++;
		} else {
			reversi->showMoves(first);
			reversi->printBoard();
			consecutiveSkips = 0;
			while (true) {
				std::cout << "Rows (0-7): ";
				std::cin >> row;
				std::cout << "Columns (0-7): ";
				std::cin >> col;
				reversi->clearBoard();
				if (!reversi->makeMove(row, col, first)) {
					std::cout << "Can't go there" << std::endl;
				}
				else {
					break;
				}
			}
		}
		if (!reversi->canMove(-1*first)) {
			std::cout << "No moves available" << std::endl;
			consecutiveSkips++;
		}
		else {
			reversi->showMoves(-1*first);
			reversi->printBoard();
			consecutiveSkips = 0;
			while (true) {
				std::cout << "Rows (0-7): ";
				std::cin >> row;
				std::cout << "Columns (0-7): ";
				std::cin >> col;
				reversi->clearBoard();
				if (!reversi->makeMove(row, col, -1*first)) {
					std::cout << "Can't go there" << std::endl;
				}
				else {
					break;
				}
			}
		}
	}

	int score1 = reversi->scoreBoard(1);
	int score2 = reversi->scoreBoard(-1);
	if (score1 - score2 == 0) {
		std::cout << "Tie game." << std::endl;
	}
	else if (score1 - score2 > 0) {
		std::cout << "Black wins with a score of " << score1 << " to " << score2 << std::endl;
	}
	else {
		std::cout << "White wins with a score of " << score2 << " to " << score1 << std::endl;
	}
}

void playerVsComputer(int computerNum) {
	GameBoard* reversi = new GameBoard();

	int consecutiveSkips = 0;
	int row, col;
	if (computerNum == -1) {
		while (!reversi->fullGame() && consecutiveSkips < 2) {
			if (!reversi->canMove(-computerNum)) {
				std::cout << "No moves available" << std::endl;
				consecutiveSkips++;
			}
			else {
				reversi->showMoves(-computerNum);
				reversi->printBoard();
				consecutiveSkips = 0;
				while (true) {
					std::cout << "Rows (0-7): ";
					std::cin >> row;
					std::cout << "Columns (0-7): ";
					std::cin >> col;
					reversi->clearBoard();
					if (!reversi->makeMove(row, col, -computerNum)) {
						std::cout << "Can't go there" << std::endl;
					}
					else {
						break;
					}
				}
			}
			//Computer turn
			if (reversi->fullGame()) {
				break;
			}
			else {
				reversi->printBoard();
				std::cout << "This can take up to 30 seconds, please be patient" << std::endl;

				if (reversi->makeCompyMove(reversi, computerNum)) {
					consecutiveSkips = 0;
				}
				else {
					consecutiveSkips++;
				}
				
			}
		}
	}
	else {
		while (!reversi->fullGame() && consecutiveSkips < 2) {
			//Computer turn
			if (reversi->fullGame()) {
				break;
			}
			else {
				reversi->printBoard();
				std::cout << "This can take up to 30 seconds, please be patient" << std::endl;
				
				if (reversi->makeCompyMove(reversi, computerNum)) {
					consecutiveSkips = 0;
				}
				else {
					consecutiveSkips++;
				}
				
			}
			if (!reversi->canMove(-computerNum)) {
				std::cout << "No moves available" << std::endl;
				consecutiveSkips++;
			}
			else {
				reversi->showMoves(-computerNum);
				reversi->printBoard();
				consecutiveSkips = 0;
				while (true) {
					std::cout << "Rows (0-7): ";
					std::cin >> row;
					std::cout << "Columns (0-7): ";
					std::cin >> col;
					reversi->clearBoard();
					if (!reversi->makeMove(row, col, -computerNum)) {
						std::cout << "Can't go there" << std::endl;
					}
					else {
						break;
					}
				}
			}
			
		}
	}
	int score1 = reversi->scoreBoard(1);
	int score2 = reversi->scoreBoard(-1);
	if (score1 - score2 == 0) {
		std::cout << "Tie game." << std::endl;
	}
	else if (score1 - score2 > 0) { 
		std::cout << "Black wins with a score of " << score1 << " to " << score2 << std::endl;
	}
	else {
		std::cout << "White wins with a score of " << score2 << " to " << score1 << std::endl;
	}
}

void computerVsComputer(int first) {
	GameBoard* reversi = new GameBoard();

	int consecutiveSkips = 0;

	while (!reversi->fullGame() && consecutiveSkips < 2) {
		if (reversi->fullGame()) {
			break;
		}
		else {
			reversi->printBoard();
			std::cout << "This can take up to 30 seconds, please be patient" << std::endl;
			if (reversi->makeCompyMove(reversi, first)) {
				consecutiveSkips = 0;
			}
			else {
				consecutiveSkips++;
			}
		}
		//Computer turn
		if (reversi->fullGame()) {
			break;
		}
		else {
			reversi->printBoard();
			std::cout << "This can take up to 30 seconds, please be patient" << std::endl;
				if (reversi->makeCompyMove(reversi, -first)) {
				consecutiveSkips = 0;
			}
			else {
				consecutiveSkips++;
			}
		}
	}
	int score1 = reversi->scoreBoard(1);
	int score2 = reversi->scoreBoard(-1);
	if (score1 - score2 == 0) {
		std::cout << "Tie game." << std::endl;
	}
	else if (score1 - score2 > 0) {
		std::cout << "Black wins with a score of " << score1 << " to " << score2 << std::endl;
	}
	else {
		std::cout << "White wins with a score of " << score2 << " to " << score1 << std::endl;
	}
}
int main() {
	srand((unsigned int)time(NULL));
	int first = 1 - (2 * (rand() % 2));
	std::cout << "Enter 0 to watch the computer play itself, 1 to play against the computer, or 2 to play with someone else: ";
	int answer;
	std::cin >> answer;
	if (answer == 2) {
		playerVsPlayer(first);
	}
	else if (answer == 1) {
		playerVsComputer(first);
	}
	else if (answer == 0) {
		computerVsComputer(first);
	}
}
