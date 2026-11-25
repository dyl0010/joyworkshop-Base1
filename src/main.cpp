#include "Config.h"
#include "Game.h"

int main(int, char**)
{
    Game game{};
    
    if (!game.Initialize(Config::DefaultWindowWidth, Config::DefaultWindowHeight, Config::DefaultWindowTitle))
    {
        return 1;
    }

    game.RunLoop();
    return 0;
}
