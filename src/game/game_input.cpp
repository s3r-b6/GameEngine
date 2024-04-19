#include "./input.h"
#include "./tile_input.h"
#include "./player_input.h"

// The idea is to just add all functions relative to input here
// Simple input checking can go here. Else, inside the function.
// Also, should implement some state-machine to handle "input modes"
void inputFunctions(u32 player_id) {
    if (g->input->keyJustPressed('1')) { toggleTilePicker(); }
    handlePlayerMovement(player_id);
    handletileActions();
}
