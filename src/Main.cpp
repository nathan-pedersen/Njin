#include "Game.h"

// #define TESTING

int main(void)
{
    // --- If need to test something define TESTING
#ifdef TESTING
    std::println("Hello world!");
    int a = 5;
    int b = 10;
    std::println("{}", a + b);
    return 0;
#endif
    // --- testing end

    // --- Actual program start ---
    Game game("../../../config.json");

    return 0;
}
