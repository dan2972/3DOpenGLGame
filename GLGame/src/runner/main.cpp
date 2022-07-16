#include <iostream>
#include "runner/game.hpp"

int main()
{
    Game game;

    game.runMainGameLoop();

    game.cleanup();

    return 0;
}
