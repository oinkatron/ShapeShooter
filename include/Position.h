#ifndef POSITION_H
#define POSITION_H

#include "Component.h"

class Position : public Component
{
    public:
        Position();
        Position(float xx, float yy) : x(xx), y(yy), x_o(0), y_o(0) { }
        virtual ~Position();

        float X() { return x; }
        void X(float val) { if (val != x) { x_o = x; x = val; } }

        float Y()  { return y; }
        void Y(float val) { if (val != y) { y_o = y; y = val; } }

        float lastX() { return x_o; }
        float lastY() { return y_o; }

    protected:
    private:
        float x;
        float y;
        float x_o;
        float y_o;
};

#endif // POSITION_H
