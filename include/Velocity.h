#ifndef VELOCITY_H
#define VELOCITY_H

#include "Component.h"


class Velocity : public Component
{
    public:
        Velocity();
        Velocity(float xx, float yy, float mx_x, float mx_y) : xv(xx), yv(yy), max_x_v(mx_x), max_y_v(mx_y) {}
        virtual ~Velocity();

        void xV(float val) { xv = val; if (xv > max_x_v) xv = max_x_v; else if (xv < -max_x_v) xv = -max_x_v; else if (xv < 0.3 && xv > -0.3) xv = 0.f; }
        float xV() { return xv; }

        void yV(float val) { yv = val; if (yv > max_y_v) yv = max_y_v; else if (yv < -max_y_v) yv = -max_y_v; else if (yv < 0.3 && yv > -0.3) yv = 0.f; }
        float yV() { return yv;}

        void maxXVel(float val) { max_x_v = val; }
        float maxXVel() { return max_x_v; }

        void maxYVel(float val) { max_y_v = val; }
        float maxYVel() { return max_y_v; }

        float angle() { return atan2(yv, xv); }
        float magnitude() { return sqrt((xv*xv)+(yv*yv)); }
        float magSqrd() { return (xv*xv)+(yv*yv); }
    protected:
    private:
        float xv;
        float yv;

        float max_x_v;
        float max_y_v;
};

#endif // VELOCITY_H
