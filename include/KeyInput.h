#ifndef KEYINPUT_H
#define KEYINPUT_H

#include "Component.h"


class KeyInput : public Component
{
    public:
        KeyInput();
        KeyInput(SDLKey u_k, SDLKey d_k, SDLKey l_k, SDLKey r_k, SDLKey a_k) : up_k(u_k), down_k(d_k), left_k(l_k), right_k(r_k), alt_k(a_k) { }
        virtual ~KeyInput();

        void upKey(SDLKey val) { up_k = val; }
        SDLKey upKey() { return up_k; }

        void downKey(SDLKey val) { down_k = val; }
        SDLKey downKey() { return down_k; }

        void leftKey(SDLKey val) { left_k = val; }
        SDLKey leftKey() { return left_k; }

        void rightKey(SDLKey val) { right_k = val; }
        SDLKey rightKey() { return right_k; }

        void altKey(SDLKey val) { up_k = val; }
        SDLKey altKey() { return up_k; }

        bool hasKey(SDLKey val) { return (val == up_k || val == down_k || val == left_k || val == right_k || val == alt_k); }

    protected:
    private:
        SDLKey up_k, down_k, right_k, left_k, alt_k;

};

#endif // KEYINPUT_H
