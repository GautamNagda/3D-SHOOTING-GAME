#include "Game.h"
#include <iostream>
#include <string>
#include <memory>

int main(int argc, char* argv[]) {
    std::cout << "========================================================\n";
    std::cout << "  CYBERSTRIKE 3D - Real-Time 3D Shooting Game Engine    \n";
    std::cout << "  Computer Graphics Project (OpenGL 3.3 Core Profile)   \n";
    std::cout << "  Full Game Architecture with Wave AI & Ray-Casting     \n";
    std::cout << "========================================================\n" << std::endl;

    bool isTestMode = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--test") {
            isTestMode = true;
        }
    }

    // Initialize Game Engine (1024 x 768 Resolution)
#ifdef __EMSCRIPTEN__
    static std::unique_ptr<CyberStrike::Game> s_game = std::make_unique<CyberStrike::Game>(1024, 768, "CyberStrike 3D");
    if (!s_game->init()) {
        std::cerr << "[FATAL] Failed to initialize CyberStrike 3D engine!" << std::endl;
        return -1;
    }
    s_game->run();
    return 0;
#else
    CyberStrike::Game game(1024, 768, "CyberStrike 3D - Real-Time 3D Shooting Game");

    if (!game.init()) {
        std::cerr << "[FATAL] Failed to initialize CyberStrike 3D engine!" << std::endl;
        return -1;
    }

    if (isTestMode) {
        bool testOk = game.runSelfTest();
        return testOk ? 0 : 1;
    }

    // Launch Full Interactive Game Loop
    game.run();

    std::cout << "\n[INFO] CyberStrike 3D terminated cleanly. Good job, Commander!" << std::endl;
    return 0;
#endif
}
