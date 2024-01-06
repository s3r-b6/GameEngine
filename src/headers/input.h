#pragma once

#include "types.h"

struct Input {
    glm::ivec2 mousePos;
    bool mouseInWindow;
    bool showCursor;
};

global Input *gInput;
