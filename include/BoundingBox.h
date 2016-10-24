#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "Component.h"


class BoundingBox : public Component
{
    public:
        BoundingBox();
        BoundingBox(int w, int h) : hw(w), hh(h) { }
        virtual ~BoundingBox();

        int halfWidth() { return hw; }
        void halfWidth(int val) { hw = val; }

        int halfHeight() { return hh; }
        void halfHeight(int val) { hh = val; }

    protected:
    private:
        int hw;
        int hh;
};

#endif // BOUNDINGBOX_H
