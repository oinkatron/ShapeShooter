#ifndef RENDERCOMPONENT_H
#define RENDERCOMPONENT_H

#include "Component.h"


class RenderComponent : public Component
{
    public:
        RenderComponent();
        RenderComponent(bool d_p, bool d_v, bool d_bb, bool d_s) : draw_pos(d_p), draw_vel(d_v), draw_bounds(d_bb), draw_ship(d_s) { }
        virtual ~RenderComponent();

        void toggleDrawBoundingBox() { if (draw_bounds) { draw_bounds = false; } else draw_bounds = true; }
        void toggleDrawVelocity() { if (draw_vel) draw_vel = false; else draw_vel = true; }
        void toggleDrawShip() { if (draw_ship) draw_ship = false; else draw_ship = true; }
        void toggleDrawPosition() { if (draw_pos) draw_pos = false; else draw_pos = true; }

        bool drawBoundingBox() { return draw_bounds; }
        void drawBoundingBox(bool val) { draw_bounds = val; }

        bool drawVelocity() { return draw_vel; }
        void drawVelocity(bool val) { draw_vel = val; }

        bool drawShip() { return draw_ship; }
        void drawShip(bool val) { draw_ship = val; }

        bool drawPosition() { return draw_pos; }
        void drawPosition(bool val) { draw_pos = val; }

        void Depth(int d) { depth = d; }
        int Depth() { return depth; }

    private:
        bool draw_bounds;
        bool draw_vel;
        bool draw_ship;
        bool draw_pos;

        int depth;
};

#endif // RENDERCOMPONENT_H
