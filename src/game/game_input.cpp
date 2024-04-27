#include "./player_input.h"

// The idea is to just add all functions relative to input here
// Simple input checking can go here. Else, inside the function.
// Also, should implement some state-machine to handle "input modes"
void inputFunctions() { handlePlayerMovement(gameState->player_id); }
