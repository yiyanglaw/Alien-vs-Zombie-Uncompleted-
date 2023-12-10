#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <algorithm>  // For std::transform

// Define constants
const int MAX_ROWS = 15; // Maximum rows in the game board
const int MAX_COLS = 15; // Maximum columns in the game board
const int MAX_ZOMBIES = 9; // Maximum number of zombies


// Add this at the beginning of your code
enum Command {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    ARROW_CMD,
    HELP,
    SAVE,
    LOAD,
    QUIT
};


// Define game objects
enum GameObject {
    EMPTY,
    ALIEN,
    ZOMBIE,
    ARROW,
    HEALTH,
    POD,
    ROCK,
    TRAIL
};


// Define arrow directions
enum ArrowDirection {
    ARROW_UP,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT
};


// Define game characters
struct Character {
    int life;
    int attack;
};

struct Zombie : Character {
    int range;
};

// Define game board
struct GameBoard {
    GameObject board[MAX_ROWS][MAX_COLS];
};


// Function prototypes
void initializeGame(GameBoard& board,  Character& alien, Zombie zombies[]) ;
void displayBoard(const GameBoard& board);
void saveGame(const GameBoard& board, const int& numZombies, const Character& alien, const Zombie zombies[]);
void loadGame(GameBoard& board, int& numZombies, Character& alien, Zombie zombies[]);
void processTurn(GameBoard& board, Character& alien, Zombie zombies[], int& numZombies);
void moveAlien(GameBoard& board, Character& alien, const std::string& direction);
void moveZombies(GameBoard& board, Character& alien, Zombie zombies[]);
void attackZombie(GameBoard& board, Character& alien, Zombie& zombie);
bool isGameOver(const Character& alien, const Zombie zombies[], const int& numZombies);
void executeCommand(GameBoard& board, Character& alien, Zombie zombies[], int& numZombies, const Command& command);
void switchArrowDirection(GameBoard& board);
void resetTrails(GameBoard& board);
void checkCollisions(GameBoard& board, Character& alien, int row, int col);
void findCharacterPosition(const GameBoard& board, GameObject character, int& row, int& col);
void attackClosestZombie(GameBoard& board, int podRow, int podCol, Zombie zombies[]);
void moveZombieRandomly(GameBoard& board, int& zombieRow, int& zombieCol);
void moveZombie(GameBoard& board, int newRow, int newCol, int& zombieRow, int& zombieCol);
bool isInRange(int targetRow, int targetCol, const Character& attacker, const Character& target);
int findZombieRow(const GameBoard& board, const Zombie* zombie);
int findZombieCol(const GameBoard& board, const Zombie* zombie);
bool isValidPosition(int row, int col);

Zombie zombies[MAX_ZOMBIES];


int main() {
    // Seed for random number generation
    std::srand(std::time(0));

    GameBoard board;
    Character alien;
    int numZombies;
    Zombie zombies[MAX_ZOMBIES];

    initializeGame(board, alien, zombies);

    // Game loop
    while (!isGameOver(alien, zombies, numZombies)) {
        displayBoard(board);
        processTurn(board, alien, zombies, numZombies);
        moveZombies(board, alien, zombies);  // Add this line to move zombies

    }

    // Print the game result after the game loop
    if (alien.life > 0) {
        std::cout << "All zombies defeated! You win!" << std::endl;
    } else {
        std::cout << "Alien defeated! Game Over." << std::endl;
    }
    return 0;
}


// Function to initialize the game board, characters, and objects
// Function to initialize the game board, characters, and objects
void initializeGame(GameBoard& board,  Character& alien, Zombie zombies[]) {
    int numZombies = 10;

    // Initialize the game board with empty spaces
    for (int i = 0; i < MAX_ROWS; ++i) {
        for (int j = 0; j < MAX_COLS; ++j) {
            board.board[i][j] = EMPTY;
        }
    }

    // Place Alien at the center of the board
    int alienRow = MAX_ROWS / 2;
    int alienCol = MAX_COLS / 2;
    board.board[alienRow][alienCol] = ALIEN;

    // Place Zombies randomly on the board
    for (int i = 0; i < numZombies; ++i) {
        int zombieRow, zombieCol;
        do {
            zombieRow = std::rand() % MAX_ROWS;
            zombieCol = std::rand() % MAX_COLS;
        } while (board.board[zombieRow][zombieCol] != EMPTY);

        // Assign random attributes to zombies
        zombies[i].life = std::rand() % 50 + 50; // Example: between 50 and 100
        zombies[i].attack = std::rand() % 20 + 10; // Example: between 10 and 30
        zombies[i].range = std::rand() % 3 + 1; // Example: between 1 and 3

        // Place zombie on the board
        board.board[zombieRow][zombieCol] = static_cast<GameObject>(ZOMBIE + i);
    }

    // Place other game objects randomly on the board (e.g., arrow, health, pod, rock)
    // You can add code here to place these objects randomly.

    // Initialize Alien's attributes
    alien.life = std::rand() % 100 + 100; // Example: between 100 and 200
    alien.attack = 0; // Alien's attack starts from zero and accumulates during the game
}

// Function to display the game board
// Function to display the game board
void displayBoard(const GameBoard& board) {
    // Display column indices
    std::cout << "  ";
    for (int j = 0; j < MAX_COLS; ++j) {
        std::cout << j % 10 << " ";
    }
    std::cout << "\n";

    // Display the top border
    std::cout << "  +";
    for (int j = 0; j < MAX_COLS; ++j) {
        std::cout << "--";
    }
    std::cout << "+\n";

    for (int i = 0; i < MAX_ROWS; ++i) {
        // Display row index
        std::cout << i % 10 << "|";

        for (int j = 0; j < MAX_COLS; ++j) {
            switch (board.board[i][j]) {
                case EMPTY:
                    std::cout << " ";
                    break;
                case ALIEN:
                    std::cout << "A";
                    break;
                case ZOMBIE:
                    std::cout << "Z";
                    break;
                case ARROW:
                    std::cout << "^";
                    break;
                case HEALTH:
                    std::cout << "H";
                    break;
                case POD:
                    std::cout << "P";
                    break;
                case ROCK:
                    std::cout << "R";
                    break;
                case TRAIL:
                    std::cout << ".";
                    break;
                default:
                    if (board.board[i][j] >= ZOMBIE && board.board[i][j] < ZOMBIE + MAX_ZOMBIES) {
                        std::cout << static_cast<char>('0' + (board.board[i][j] - ZOMBIE));
                    }
                    break;
            }
            std::cout << "|";
        }
        std::cout << "\n";

        // Display the bottom border
        std::cout << "  +";
        for (int j = 0; j < MAX_COLS; ++j) {
            std::cout << "--";
        }
        std::cout << "+\n";
    }
}

// Function to save the game state to a file
// Function to save the game state to a file
void saveGame(const GameBoard& board, const int& numZombies, const Character& alien, const Zombie zombies[]) {
    std::ofstream outFile("initial_game.txt"); // You can change the filename as needed

    // Write game board data
    for (int i = 0; i < MAX_ROWS; ++i) {
        for (int j = 0; j < MAX_COLS; ++j) {
            outFile << static_cast<int>(board.board[i][j]) << " ";
        }
        outFile << "\n";
    }

    // Write Zombie data
    outFile << numZombies << "\n";
    for (int i = 0; i < numZombies; ++i) {
        outFile << zombies[i].life << " " << zombies[i].attack << " " << zombies[i].range << "\n";
    }

    // Write Alien data
    outFile << alien.life << " " << alien.attack << "\n";

    outFile.close();
}

void loadGame(GameBoard& board, int& numZombies, Character& alien, Zombie zombies[]) {
    std::ifstream inFile("initial_game.txt"); // You can change the filename as needed

    // Read game board data
    for (int i = 0; i < MAX_ROWS; ++i) {
        for (int j = 0; j < MAX_COLS; ++j) {
            int value;
            inFile >> value;
            board.board[i][j] = static_cast<GameObject>(value);
        }
    }

    // Read Alien data
    inFile >> alien.life >> alien.attack;

    // Read Zombie data
    inFile >> numZombies;
    for (int i = 0; i < numZombies; ++i) {
        inFile >> zombies[i].life >> zombies[i].attack >> zombies[i].range;
    }

    inFile.close();
}

// Function to process a turn in the game
void processTurn(GameBoard& board, Character& alien, Zombie zombies[], int& numZombies) {
    std::string input;
    Command command;

    // Get user input for the command
    std::cout << "Enter a command (type 'help' for commands list): ";
    std::cin >> input;

    // Convert input to uppercase
    std::transform(input.begin(), input.end(), input.begin(), ::toupper);

    // Determine the command based on user input
    if (input == "UP") {
        command = UP;
    } else if (input == "DOWN") {
        command = DOWN;
    } else if (input == "LEFT") {
        command = LEFT;
    } else if (input == "RIGHT") {
        command = RIGHT;
    } else if (input == "ARROW") {
        command = ARROW_CMD;
    } else if (input == "HELP") {
        command = HELP;
    } else if (input == "SAVE") {
        command = SAVE;
    } else if (input == "LOAD") {
        command = LOAD;
    } else if (input == "QUIT") {
        command = QUIT;
    } else {
        std::cout << "Invalid command. Type 'help' for commands list." << std::endl;
        return;
    }

    // Execute the chosen command
    executeCommand(board, alien, zombies,numZombies, command);
}

// Function to execute the chosen command
void executeCommand(GameBoard& board, Character& alien, Zombie zombies[], int& numZombies, const Command& command) {
    switch (command) {
        case UP:
            moveAlien(board, alien, "UP");
            break;
        case DOWN:
            moveAlien(board, alien, "DOWN");
            break;
        case LEFT:
            moveAlien(board, alien, "LEFT");
            break;
        case RIGHT:
            moveAlien(board, alien, "RIGHT");
            break;
        case ARROW_CMD:
            switchArrowDirection(board);
            break;
        case HELP:
            // Display the list of commands and their descriptions
            std::cout << "Commands List:\n";
            std::cout << "up\n";
            std::cout << "down\n";
            std::cout << "left\n";
            std::cout << "right\n";
            std::cout << "arrow\n";
            std::cout << "help\n";
            std::cout << "save\n";
            std::cout << "load\n";
            std::cout << "quit\n";
            break;
        case SAVE:
            // Save the current game to a file
            saveGame(board, numZombies, alien, zombies);
            break;
        case LOAD:
            // Load a saved game from a file
            loadGame(board, numZombies, alien, zombies);
            break;

        case QUIT:
            // Quit the game
            std::cout << "Are you sure you want to quit? (y/n): ";
            char response;
            std::cin >> response;
            if (response == 'y' || response == 'Y') {
                std::cout << "Quitting the game.\n";
                exit(0);
            }
            break;
    }
}

// Function to switch the direction of an arrow object
void switchArrowDirection(GameBoard& board) {
    int row, col;
    std::cout << "Enter the row and column of the arrow to switch: ";
    std::cin >> row >> col;

    if (isValidPosition(row, col) && board.board[row][col] == ARROW) {
        // Switch the direction of the arrow
        std::string newDirection;
        std::cout << "Enter the new direction (UP, DOWN, LEFT, RIGHT): ";
        std::cin >> newDirection;

        // Update the arrow direction based on user input
        if (newDirection == "UP") {
            board.board[row][col] = ARROW;
        } else if (newDirection == "DOWN") {
            board.board[row][col] = ARROW;
        } else if (newDirection == "LEFT") {
            board.board[row][col] = ARROW;
        } else if (newDirection == "RIGHT") {
            board.board[row][col] = ARROW;
        } else {
            std::cout << "Invalid direction. Arrow direction not changed." << std::endl;
        }
    } else {
        std::cout << "Invalid position. Arrow direction not changed." << std::endl;
    }
}
// Function to move Alien in a specified direction
void moveAlien(GameBoard& board, Character& alien, const std::string& direction) {
    int alienRow, alienCol;
    findCharacterPosition(board, ALIEN, alienRow, alienCol);

    if (direction == "UP" && isValidPosition(alienRow - 1, alienCol)) {
        // Move up
        board.board[alienRow][alienCol] = EMPTY;
        --alienRow;
    } else if (direction == "DOWN" && isValidPosition(alienRow + 1, alienCol)) {
        // Move down
        board.board[alienRow][alienCol] = EMPTY;
        ++alienRow;
    } else if (direction == "LEFT" && isValidPosition(alienRow, alienCol - 1)) {
        // Move left
        board.board[alienRow][alienCol] = EMPTY;
        --alienCol;
    } else if (direction == "RIGHT" && isValidPosition(alienRow, alienCol + 1)) {
        // Move right
        board.board[alienRow][alienCol] = EMPTY;
        ++alienCol;
    }

    // Set the new position of the alien
    board.board[alienRow][alienCol] = ALIEN;

    // Check for collisions with game objects
    checkCollisions(board, alien, alienRow, alienCol);
}






// Function to check if a position is within the game board
bool isValidPosition(int row, int col) {
    return (row >= 0 && row < MAX_ROWS && col >= 0 && col < MAX_COLS);
}

// Function to reset trails left by Alien to random non-trail game objects
void resetTrails(GameBoard& board) {
    for (int i = 0; i < MAX_ROWS; ++i) {
        for (int j = 0; j < MAX_COLS; ++j) {
            if (board.board[i][j] == TRAIL) {
                int randomObject = std::rand() % (MAX_ZOMBIES + 3); // Random non-trail object
                board.board[i][j] = static_cast<GameObject>(randomObject);
            }
        }
    }
}

// Function to move Alien in a specified direction
// Function to move Alien in a specified direction

// Function to check for collisions with game objects after Alien moves
void checkCollisions(GameBoard& board, Character& alien, int row, int col) {
    GameObject& currentObject = board.board[row][col];

    switch (currentObject) {
        case EMPTY:
            break;
        case ARROW:
            // Switch Alien's direction
            switchArrowDirection(board);
            // Add 20 attack to Alien
            alien.attack += 20;
            break;
        case HEALTH:
            // Add 20 life to Alien
            alien.life += 20;
            break;
        case POD:
            // Inflict 10 damage to the closest Zombie
            attackClosestZombie(board, row, col, zombies);
            break;
        case ROCK:
            // Stop Alien from moving
            return;
        default:
            break;
    }

    // Reset the current position to Alien
    currentObject = ALIEN;
}

// Function to find the position of a character on the board
void findCharacterPosition(const GameBoard& board, GameObject character, int& row, int& col) {
    for (int i = 0; i < MAX_ROWS; ++i) {
        for (int j = 0; j < MAX_COLS; ++j) {
            if (board.board[i][j] == character) {
                row = i;
                col = j;
                return;
            }
        }
    }
}

// Function to attack the closest Zombie to a Pod
void attackClosestZombie(GameBoard& board, int podRow, int podCol, Zombie zombies[]) {
    int closestZombieRow = -1, closestZombieCol = -1;
    int minDistance = MAX_ROWS + MAX_COLS; // Initialize with a value greater than maximum possible distance

    for (int i = 0; i < MAX_ROWS; ++i) {
        for (int j = 0; j < MAX_COLS; ++j) {
            if (board.board[i][j] >= ZOMBIE && board.board[i][j] < ZOMBIE + MAX_ZOMBIES) {
                int distance = abs(podRow - i) + abs(podCol - j);
                if (distance < minDistance) {
                    minDistance = distance;
                    closestZombieRow = i;
                    closestZombieCol = j;
                }
            }
        }
    }

    // Inflict 10 damage to the closest Zombie
    if (isValidPosition(closestZombieRow, closestZombieCol) &&
        board.board[closestZombieRow][closestZombieCol] >= ZOMBIE &&
        board.board[closestZombieRow][closestZombieCol] < ZOMBIE + MAX_ZOMBIES) {
        int zombieIndex = board.board[closestZombieRow][closestZombieCol] - ZOMBIE;
        Zombie& zombie = zombies[zombieIndex];
        zombie.life -= 10;
        if (zombie.life <= 0) {
            // Zombie defeated, remove from the board
            board.board[closestZombieRow][closestZombieCol] = EMPTY;
        }
    }
}



// Function to move Zombies on the board
void moveZombies(GameBoard& board, Character& alien, Zombie zombies[]) {
    for (int i = 0; i < MAX_ZOMBIES; ++i) {
        if (zombies[i].life > 0) {
            int zombieRow, zombieCol;
            findCharacterPosition(board, static_cast<GameObject>(ZOMBIE + i), zombieRow, zombieCol);

            // Move zombie randomly
            moveZombieRandomly(board, zombieRow, zombieCol);

            // Check if Zombie is within Alien's attack range
            if (isInRange(zombieRow, zombieCol, alien, zombies[i])) {
                attackZombie(board, alien, zombies[i]);
            }
        }
    }
}

// Function to move a Zombie randomly
void moveZombieRandomly(GameBoard& board, int& zombieRow, int& zombieCol) {
    int randomDirection = std::rand() % 4; // 0: UP, 1: DOWN, 2: LEFT, 3: RIGHT

    switch (randomDirection) {
        case 0: // UP
            moveZombie(board, zombieRow - 1, zombieCol, zombieRow, zombieCol);
            break;
        case 1: // DOWN
            moveZombie(board, zombieRow + 1, zombieCol, zombieRow, zombieCol);
            break;
        case 2: // LEFT
            moveZombie(board, zombieRow, zombieCol - 1, zombieRow, zombieCol);
            break;
        case 3: // RIGHT
            moveZombie(board, zombieRow, zombieCol + 1, zombieRow, zombieCol);
            break;
    }
}

// Function to move a Zombie to a new position
void moveZombie(GameBoard& board, int newRow, int newCol, int& zombieRow, int& zombieCol) {
    if (isValidPosition(newRow, newCol) && board.board[newRow][newCol] == EMPTY) {
        board.board[zombieRow][zombieCol] = EMPTY;
        board.board[newRow][newCol] = static_cast<GameObject>(ZOMBIE + (board.board[zombieRow][zombieCol] - ZOMBIE));
        zombieRow = newRow;
        zombieCol = newCol;
    }
}

// Function to check if a position is within the attack range of a character
bool isInRange(int targetRow, int targetCol, const Character& attacker, const Character& target) {
    int distance = abs(targetRow - MAX_ROWS / 2) + abs(targetCol - MAX_COLS / 2);
    return distance <= attacker.attack && target.life > 0;
}

// Function to attack a Zombie
void attackZombie(GameBoard& board, Character& alien, Zombie& zombie) {
    zombie.life -= alien.attack;
    if (zombie.life <= 0) {
        // Zombie defeated, remove from the board
        board.board[findZombieRow(board, &zombie)][findZombieCol(board, &zombie)] = EMPTY;
    }
}

// Function to find the row of a specific Zombie on the board
int findZombieRow(const GameBoard& board, const Zombie* zombie) {
    for (int i = 0; i < MAX_ROWS; ++i) {
        for (int j = 0; j < MAX_COLS; ++j) {
            if (&board.board[i][j] == reinterpret_cast<const GameObject*>(zombie)) {
                return i;
            }
        }
    }
    return -1; // Zombie not found
}

// Function to find the column of a specific Zombie on the board
int findZombieCol(const GameBoard& board, const Zombie* zombie) {
    for (int i = 0; i < MAX_ROWS; ++i) {
        for (int j = 0; j < MAX_COLS; ++j) {
            if (&board.board[i][j] == reinterpret_cast<const GameObject*>(zombie)) {
                return j;
            }
        }
    }
    return -1; // Zombie not found
}
// Function to check if the game is over
bool isGameOver(const Character& alien, const Zombie zombies[], const int& numZombies) {
    if (alien.life <= 0) {
        std::cout << "Alien defeated! Game Over." << std::endl;
        return true;
    }

    int activeZombies = 0;
    for (int i = 0; i < numZombies; ++i) {
        if (zombies[i].life > 0) {
            ++activeZombies;
        }
    }

    if (activeZombies == 0) {
        std::cout << "All zombies defeated! You win!" << std::endl;
        return true;
    }

    return false;
}
