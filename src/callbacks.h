#ifndef CALLBACKS_H
#define CALLBACKS_H

class Callbacks{
public:
    void keyboardCallback(unsigned char keyPressed, int mouseX, int mouseY);
    void keyboardUpCallback(unsigned char keyReleased, int mouseX, int mouseY);

    void onDisplay();
    void onReshape(int w, int h);

    void onMouseClick(int button, int state, int x, int y);
    void onMouseMove(int x, int y);
    void onMouseScroll(int button, int dir, int x, int y);

    void onSpecialKeyPress(int key, int x, int y);
    void onSpecialKeyRelease(int key, int x, int y);

    void updateMovement();

    void timer(int);
};

#endif