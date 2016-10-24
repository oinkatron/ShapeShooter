#ifndef LIFESPAN_H
#define LIFESPAN_H

#include "Component.h"


class LifeSpan : public Component
{
    public:
        LifeSpan();
        LifeSpan(long sp) : create_time(SDL_GetTicks()), life_span(sp) { }
        virtual ~LifeSpan();

        void createTime(long val) { create_time = val; }
        long createTime() { return create_time; }

        void lifeSpan(long val) { life_span = val; }
        long lifeSpan() { return life_span; }

        bool lifeExpired() { return (SDL_GetTicks() - create_time >= life_span); }
    protected:
    private:
        long create_time;
        long life_span;
};

#endif // LIFESPAN_H
