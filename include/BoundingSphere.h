#ifndef BOUNDINGSPHERE_H
#define BOUNDINGSPHERE_H

#include "Component.h"


class BoundingSphere : public Component
{
    public:
        BoundingSphere();
        BoundingSphere(float val) : r(val) {}
        virtual ~BoundingSphere();

        float radius() { return r; }
        void radius(float val) { r = val; }

    protected:
    private:
        float r;
};

#endif // BOUNDINGSPHERE_H
