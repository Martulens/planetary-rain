#ifndef GAMESTATE_H
#define GAMESTATE_H

struct GameState {
    int windowWidth;
    int windowHeight;

    int lastMouseX = 0;
    int lastMouseY = 0;

    bool gameOver = false;
    float elapsedTime = 0;

    GameState(int w, int h){
        windowHeight = h;
        windowWidth = w;
    }
};

#endif //GAMESTATE_H
