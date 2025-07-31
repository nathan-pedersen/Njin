#include "Game.h"

// #define TESTING

int main(void)
{
    // --- If need to test something define TESTING
#ifdef TESTING
#endif
    // --- testing end

    // --- Actual program start ---
    Game game("config.json");

    return 0;
}
