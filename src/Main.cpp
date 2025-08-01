#include "Game.h"

// #define TESTING

int main(void)
{
    // --- If need to test something define TESTING
#ifdef TESTING
    std::println("Hello world!");
    return 0;
#endif
    // --- testing end

    // --- Actual program start ---
    Game game("/home/nathan/dev/Njin/config.json");

    return 0;
}
