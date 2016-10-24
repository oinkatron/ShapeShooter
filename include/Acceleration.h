#ifndef ACCELERATION_H
#define ACCELERATION_H

#include "Component.h"

class Acceleration : public Component
{
    public:
        Acceleration();
        Acceleration(float x_a, float y_a) : xa(x_a), ya(y_a) { }
        virtual ~Acceleration();

        void xAccel(float val) { xa = val; }
        float xAccel() { return xa; }

        void yAccel(float val) { ya = val; }
        float yAccel() { return ya; }

    protected:
    private:
        float xa;
        float ya;
};

#endif // ACCELERATION_H
