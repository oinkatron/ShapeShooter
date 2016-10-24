#ifndef HEALTH_H
#define HEALTH_H

#include "Component.h"


class Health : public Component
{
    public:
        Health();
        virtual ~Health();

        void maxHP(int m_hp) { max_hp = m_hp; }
        int maxHP() { return max_hp; }

        int curHP() { return cur_hp; }
        void restoreHP(int val) { cur_hp += val; if (cur_hp > max_hp) cur_hp = max_hp; }
        void decreaseHP(int val) { cur_hp -= val; if (cur_hp < 0) cur_hp = 0; }

        int percHP() { return (int)(((float)cur_hp/(float)max_hp)*100.f); }

    protected:
    private:
        int max_hp;
        int cur_hp;
};

#endif // HEALTH_H
