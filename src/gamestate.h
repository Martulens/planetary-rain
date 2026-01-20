#ifndef GAMESTATE_H
#define GAMESTATE_H

struct GameState {
    int windowWidth;
    int windowHeight;

    int lastMouseX = 0;
    int lastMouseY = 0;

    bool gameOver = false;
    float elapsedTime = 0;

    bool keyW = false;
    bool keyA = false;
    bool keyS = false;
    bool keyD = false;
    bool keySpace = false;
    bool keyShift = false;

    // Rendering options
    bool wireframeMode = false;

    GameState(int w, int h) {
        windowHeight = h;
        windowWidth = w;
    }
};

#endif // GAMESTATE_H