#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif

#define PRINTPOS(x, y) cout << "X: " << (x) << " | Y: " << (y) << endl
#define MIN(a, b) ((a) < (b)) ? (a) : (b)
#define MAX(a, b) ((a) > (b)) ? (a) : (b)

#include <climits>
#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <vector>

#include <cmath>
#include <ctime>
#include <typeinfo>

#include "./include/Position.h"
#include "./include/Velocity.h"
#include "./include/Acceleration.h"
#include "./include/BoundingBox.h"
#include "./include/BoundingSphere.h"
#include "./include/LifeSpan.h"
#include "./include/RenderComponent.h"
#include "./include/KeyInput.h"
#include "./include/MouseInput.h"

#define MAX_S16 32767
#define PI 3.1415926535
#define MAX_FPS 60
#define TIME_STEP 8 /* (1000/MAX_FPS) / 2 */
#define GRAV 150.f
#define DRAG 0.95f
#define MAX_VEL 500.f
#define INPUT_ACCEL 4000.f

//degrees to radians d * 180 / PI

using namespace std;

struct Object
{
    vector<Component*> comps;
    bool to_delete, active;
};

vector <Object*> objects;

int createObject()
{
    objects.push_back(new Object);
    objects[objects.size()-1]->active = true;
    objects[objects.size()-1]->to_delete = false;
    return objects.size()-1;
}

void destroyObject(int index)
{
    if (index > 0 && index < objects.size())
    {
        for (int i = 0; i < objects[index]->comps.size(); i++)
        {
            delete objects[index]->comps[i];
        }
        delete objects[index];
        objects.erase(objects.begin()+index);
    }
}

void destroyAllObjects()
{
    for (int i = 0; i < objects.size(); i++)
    {
        for (int ii = 0; ii < objects[i]->comps.size(); ii++)
        {
            delete objects[i]->comps[ii];
        }
        objects[i]->comps.clear();
        delete objects[i];
    }
    objects.clear();
}

template <class C>
C* getComponent(int ind)
{
   C *tmp = NULL;
   for (int i = 0; i < objects[ind]->comps.size(); i++)
   {
       if ((tmp = dynamic_cast<C*>(objects[ind]->comps[i])))
       {
           return tmp;
       }
   }

   return NULL;
}

template <class C>
bool hasComponent(int ind, C *com=NULL)
{
    //cout << "class name: " << typeid(com).name() << endl;
    C* tmp = NULL;
    for (int i = 0; i < objects[ind]->comps.size(); i++)
    {
        tmp = dynamic_cast<C*>(objects[ind]->comps[i]);
        if ((tmp = dynamic_cast<C*>(objects[ind]->comps[i])) != NULL)
        {
            return true;
        }
    }

    return false;
}

template <class C>
void addComponent(int ind, C* c)
{
    if (ind >= 0 && ind < objects.size() && c != NULL)
    {
        if (!hasComponent(ind, c))
        {
            objects[ind]->comps.push_back(c);
        }
    }
}

template <class C>
vector<int> getObjectsWithComponent()
{
    vector<int> ids;

    for (int i = 0; i < objects.size(); i++)
    {
        if (hasComponent<C>(i))
        {
            ids.push_back(i);
        }
    }

    return ids;
}

struct Projectile
{
    Position p;
    Velocity v;
    float r;

};

struct Terrain
{
    vector<int> x;
    vector<int> y;
};

struct Explosion
{
    Position p;
    float start_r, end_r;
    float cur_r;
    int life_span;
    int create_time;
    bool to_delete, active;
};

struct Player
{
    float x, y,
          x_o, y_o,
          x_v, y_v,
          x_a, y_a;

    float theta;

    bool on_ground;
};

struct Camera
{
    int x, y;
    int margin_w, margin_h;
};

int audio_rate = 22050;
Uint16 audio_format = AUDIO_S16SYS;
int audio_channels = 2;
int audio_buffers = 4096;

int channel = -1;

int scr_width = 800;
int scr_height = 600;
int scr_bpp = 32;

int wrld_width = 2000;
int wrld_height = 1000;

//timing variables
int sys_fps = 0;
int sys_fcount = 0;
int sys_fcount_time = 0;

int game_strt;

int time_accumulated;

int frame_strt = 0;
int frame_time = 0;
int last_strt = 0;

Mix_Chunk *song = NULL;

SDL_Surface *screen;

SDL_Event event;

SDL_Rect draw_rect;
int w_shift = 0;
int sin_phase;

vector<Explosion*> explosions;

Camera cam;
Player play;
Terrain terra;

//FUNCTIONS
bool init();

int randInt(int m, int mx);

bool mixSong(Mix_Chunk *src);
void drawWave(Mix_Chunk *src, int shft, int x, int y, int w, int h, bool left=false);

float sampleLength(Mix_Chunk *src) { return (float)src->alen / ((float)(audio_rate * audio_channels * 16.0f) / 8.0f); }
int sampleAverage(Mix_Chunk *src, int shift_millis, int dura, bool left=false);
int sampleData(Mix_Chunk *src, int d_i, bool left=false);

void generateTerrain(Terrain &t);
void drawTerrain(Terrain &t);

/*void drawSineWave(Wave &w);
float solveWave(Wave &w, float t);

bool belowWave(Wave &w, Player &p);*/

void updateCamera(Camera &cam);

void updateObjects(long dt);
void drawObject(int ind);
void drawObjects();

void handleKeyInput(int ind, SDLKey k, bool prsd);
void handleMouseInput(int ind, int mb, int mx, int my);

Object *objectFree(Object *o);
bool terrainFree(Object *o);

bool checkCollision(float ax, float ay, float bx, float by, int bw, int bh);
bool checkCollision(float ax, float ay, int aw, int ah, float bx, float by, int bw, int bh);

bool checkCollision(Position &p_a, BoundingBox &b_a, Position &p_b, BoundingBox &b_b);
bool checkCollision(Position &p_a, BoundingBox &b_a, Position &p_b, BoundingSphere &s_b);
bool checkCollision(Position &p_a, BoundingSphere &s_a, Position &p_b, BoundingSphere &s_b);
bool checkCollision(Position &p_a, Position &p_b, BoundingBox &b_b);
bool checkCollision(Position &p_a, Position &p_b, BoundingSphere &s_b);

bool pointOnLine(float p_x, float p_y, float l_x1, float l_y1, float l_x2, float l_y2);

void updatePlayer(Player &p, int dt);
void drawPlayer(Player &p);

void drawExplosion(Explosion &e);
void updateExplosion(Explosion &e);
void createExplosion(float x, float y, float s_r, float end_r, int l_p);
void deleteExplosions();
void deleteAllExplosions();

void drawProjectile(Projectile &e);
void updateProjectile(Projectile &e);
void createProjectile(float x, float y, float x_v, float y_v, float r);

void updatePosition(Position &p, Velocity &v);

float degToRad(float d) { return d / 180.f * PI; }
float radToDeg(float r) { return r / PI * 180.f; }

int main ( int argc, char** argv )
{
    if (!init())
    {
        cout << "Failed initialization!" << endl;
        return 1;
    }

    srand((unsigned)time(NULL));

    if (pointOnLine(10, 25, 10, 10, 20, 40))
        cout << "OINK IM WORKING" << endl;


    int start_time = SDL_GetTicks();

    cam.x = 50;
    cam.y = 50;
    cam.margin_w = 300;
    cam.margin_h = 200;

    play.x = 100;
    play.y = 100;
    play.on_ground = false;

    generateTerrain(terra);

    createExplosion(300, 200, 20, 100, 400);

    int new_o;

    new_o = createObject();
    addComponent(new_o, new LifeSpan(2500));
    addComponent(new_o, new Position(100, 200));
    addComponent(new_o, new Velocity(0.f, 0.f, 700.f, 700.f));
    addComponent(new_o, new BoundingBox(20, 20));
    addComponent(new_o, new BoundingSphere(28));
    addComponent(new_o, new Acceleration(0, 0));
    addComponent(new_o, new RenderComponent(true, true, true, false));
    addComponent(new_o, new KeyInput(SDLK_w, SDLK_s, SDLK_a, SDLK_d, SDLK_LSHIFT));

    new_o = createObject();
    addComponent(new_o, new RenderComponent(true, false, false, false));


    // program main loop
    bool done = false;
    while (!done)
    {
        /* TIMING */
        last_strt = frame_strt;
        frame_strt = SDL_GetTicks();
        frame_time = frame_strt - last_strt;

        sys_fcount++;
        if (SDL_GetTicks() - sys_fcount_time >= 1000)
        {
            sys_fps = sys_fcount;
            sys_fcount = 0;
            sys_fcount_time = SDL_GetTicks();
            cout << "Game Fps: " << sys_fps << endl;
        }

        vector<int> input_o = getObjectsWithComponent<KeyInput>();

        // INPUT PROCESSING
        while (SDL_PollEvent(&event))
        {
            // check for messages
            switch (event.type)
            {
                // exit if the window is closed
            case SDL_QUIT:
                done = true;
                break;

                // check for keypresses
            case SDL_KEYDOWN:
                {
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                        {
                            done = true;
                            break;
                        }
                        case SDLK_LEFT:
                        {
                            play.x_a = -INPUT_ACCEL;
                            if (play.x_v > 0.f)
                            {
                                play.x_v = 0.f;
                            }
                            break;
                        }
                        case SDLK_RIGHT:
                        {
                            play.x_a = INPUT_ACCEL;
                            if (play.x_v < 0.f)
                            {
                                play.x_v = 0.f;
                            }
                            break;
                        }
                        case SDLK_UP:
                        {
                            play.y_a = -INPUT_ACCEL;
                            if (play.y_v > 0.f)
                            {
                                play.y_v = 0.f;
                            }
                            break;
                        }
                        case SDLK_DOWN:
                        {
                            play.y_a = INPUT_ACCEL;
                            if (play.y_v < 0.f)
                            {
                                play.y_v = 0.f;
                            }
                            break;
                        }

                        default:
                        {
                            KeyInput *ki = NULL;
                            for (int i = 0; i < input_o.size(); i++)
                            {
                                ki = getComponent<KeyInput>(i);
                                if (ki && ki->hasKey(event.key.keysym.sym))
                                {
                                    handleKeyInput(i, event.key.keysym.sym, true);
                                }

                            }
                            break;
                        }
                    }
                    break;
                }
            case SDL_KEYUP:
                {
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_LEFT:
                        {
                            if (play.x_a < 0.f)
                            {
                                play.x_a = 0.f;
                            }
                            break;
                        }
                        case SDLK_RIGHT:
                        {
                            if (play.x_a > 0.f)
                            {
                                play.x_a = 0.f;
                            }
                            break;
                        }
                        case SDLK_UP:
                        {
                            if (play.y_a < 0.f)
                            {
                                play.y_a = 0.f;
                            }
                            break;
                        }
                        case SDLK_DOWN:
                        {
                            if (play.y_a > 0.f)
                            {
                                play.y_a = 0.f;
                            }
                        }
                        default:
                        {
                            KeyInput *ki = NULL;
                            for (int i = 0; i < input_o.size(); i++)
                            {
                                ki = getComponent<KeyInput>(i);
                                if (ki && ki->hasKey(event.key.keysym.sym))
                                {
                                    handleKeyInput(i, event.key.keysym.sym, false);
                                }

                            }
                        }
                    }
                    break;
                }
            } // end switch
        } // end of message processing

        //PHYSICS AND OBJECT UPDATING
        time_accumulated += frame_time;
        while (time_accumulated >= TIME_STEP)
        {
            updatePlayer(play, TIME_STEP);
            updateObjects(TIME_STEP);
            time_accumulated -= TIME_STEP;
        }

        for (int i = 0; i < explosions.size(); i++)
        {
            updateExplosion(*explosions[i]);
        }

        deleteExplosions();
        updateCamera(cam);
        // DRAWING STARTS HERE

        // clear screen
        SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));

        drawPlayer(play);
        drawTerrain(terra);

        for (int i = 0; i < explosions.size(); i++)
        {
            drawExplosion(*explosions[i]);
        }

        drawObjects();
         // DRAWING ENDS HERE

        // finally, update the screen :)
        SDL_Flip(screen);


        //Limit fps
        if ((SDL_GetTicks() - frame_strt) < 1000/MAX_FPS)
        {
            SDL_Delay(1000/MAX_FPS - (SDL_GetTicks() - frame_strt));
        }
    } // end main loop


    Mix_CloseAudio();

    // all is well ;)
    printf("Exited cleanly\n");
    return 0;
}

bool init()
{
    // initialize SDL video
    if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
    {
        printf( "Unable to init SDL: %s\n", SDL_GetError() );
        return false;
    }

    // make sure SDL cleans up before exit
    atexit(SDL_Quit);

    // create a new window
    screen = SDL_SetVideoMode(scr_width, scr_height, scr_bpp,
                                           SDL_HWSURFACE|SDL_DOUBLEBUF);
    if ( !screen )
    {
        printf("Unable to set %ix%i video: %s\n", scr_width, scr_height, SDL_GetError());
        return false;
    }

    if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0)
    {
        cout << "Unable to open audio device: " << Mix_GetError() << endl;
        return false;
    }

    return true;
}

void updateObjects(long dt)
{
    Velocity *v;
    Acceleration *a;
    Position *p;
    BoundingBox *bb;
    BoundingSphere *bs;

    float time_step = (float)dt / 1000.f;

    for (int i = 0; i < objects.size(); i++)
    {
        if (objects[i]->active && !objects[i]->to_delete)
        {
            v = NULL; a = NULL; p = NULL; bb = NULL; bs = NULL;
            v = getComponent<Velocity>(i);
            a = getComponent<Acceleration>(i);
            p = getComponent<Position>(i);
            bb = getComponent<BoundingBox>(i);
            bs = getComponent<BoundingSphere>(i);

            if (v)
            {
                if (a)
                {
                    v->xV(v->xV() + (a->xAccel() * time_step));
                    v->yV(v->yV() + (a->yAccel() * time_step));

                    if (a->xAccel() == 0 && a->yAccel() == 0)
                    {
                        v->xV(v->xV()*DRAG);
                        v->yV(v->yV()*DRAG);
                    }
                }

                if (p)
                {
                    p->X(p->X() + (v->xV() * time_step));
                    p->Y(p->Y() + (v->yV() * time_step));
                }
            }
        }
    }
}

void drawObjects()
{
    Position *p = NULL;
    RenderComponent *r = NULL;
    vector<int> draw_list;

    for (int i = 0; i < objects.size(); i++)
    {
        p = NULL;
        r = NULL;

        p = getComponent<Position>(i);
        r = getComponent<RenderComponent>(i);

        if (p && r)
        {
            if (checkCollision(p->X(), p->Y(), cam.x, cam.y, scr_width, scr_height))
            {
                draw_list.push_back(i);
            }
        }
    }

    for (int i = 0; i < draw_list.size(); i++)
    {
        drawObject(draw_list[i]);
    }
}

void drawObject(int ind)
{
    if (ind >= 0 && ind < objects.size())
    {
        RenderComponent *r = getComponent<RenderComponent>(ind);
        Position *p = getComponent<Position>(ind);
        Velocity *v = getComponent<Velocity>(ind);
        BoundingBox *bb = getComponent<BoundingBox>(ind);
        BoundingSphere *bs = getComponent<BoundingSphere>(ind);

        if (r->drawPosition() && p)
        {
            draw_rect.x = p->X() - 2 - cam.x;
            draw_rect.y = p->Y() - 2 - cam.y;
            draw_rect.w = 4;
            draw_rect.h = 4;

            SDL_FillRect(screen, &draw_rect, SDL_MapRGB(screen->format, 0, 0, 255));

            if (r->drawBoundingBox() && bb)
            {
                rectangleRGBA(screen, p->X() - bb->halfWidth() - cam.x, p->Y() - bb->halfHeight() - cam.y, p->X() + bb->halfWidth() - cam.x, p->Y() + bb->halfHeight() - cam.y, 128, 0, 128, 255);
            }

            if (r->drawBoundingBox() && bs)
            {
                circleRGBA(screen, p->X()-cam.x, p->Y() - cam.y, bs->radius(), 128, 0, 128, 255);
            }

            if (r->drawVelocity() && v)
            {
                lineRGBA(screen, p->X() - cam.x, p->Y() - cam.y, p->X()+v->xV() - cam.x, p->Y() + v->yV() - cam.y, 255, 0, 0, 255);
            }
        }
    }
}

void handleKeyInput(int ind, SDLKey k, bool prsd)
{
    KeyInput *k_i = NULL;
    Acceleration *a_c = NULL;
    if (ind >= 0 && ind < objects.size())
    {
        k_i = getComponent<KeyInput>(ind);
        a_c = getComponent<Acceleration>(ind);

        if (k_i && a_c)
        {
            if (k_i->upKey() == k)
            {
                if (prsd)
                {
                    a_c->yAccel(-INPUT_ACCEL);
                }
                else
                {
                    if (a_c->yAccel() < 0)
                    {
                        a_c->yAccel(0.f);
                    }
                }
            }
            else if (k_i->downKey() == k)
            {
                if (prsd)
                {
                    a_c->yAccel(INPUT_ACCEL);
                }
                else
                {
                    if(a_c->yAccel() > 0)
                    {
                        a_c->yAccel(0.f);
                    }
                }
            }
            else if (k_i->leftKey() == k)
            {
                if (prsd)
                {
                    a_c->xAccel(-INPUT_ACCEL);
                }
                else
                {
                    if (a_c->xAccel() < 0)
                        a_c->xAccel(0.f);
                }
            }
            else if (k_i->rightKey() == k)
            {
                if (prsd)
                {
                    a_c->xAccel(INPUT_ACCEL);
                }
                else
                {
                    if (a_c->xAccel() > 0)
                    {
                        a_c->xAccel(0.f);
                    }
                }
            }
            else if (k_i->altKey() == k)
            {
                if (prsd)
                {

                }
                else
                {

                }
            }
        }
    }
}

void updatePlayer(Player &p, int dt)
{
    float t_step = dt / 1000.f;
    //THIS STUFF IS WHAT RK4 IS FOR

    //Add gravity
    //p.y_v += GRAV * t_step;

    p.x_v += p.x_a * t_step; //UPDATING DERIVITIVES
    p.y_v += p.y_a * t_step;

    //p.x_v *= 0.85;
    //p.y_v *= 0.85;

    if (p.x_v < 0.3 && p.x_v > -0.3)
    {
        p.x_v = 0;
    }

    if (p.y_v < 0.3 && p.y_v > -0.3)
    {
        p.y_v = 0;
    }

    if (p.x_v > MAX_VEL)
    {
        p.x_v = MAX_VEL;
    }
    else if (p.x_v < -MAX_VEL)
    {
        p.x_v = -MAX_VEL;
    }

    if (p.y_v > MAX_VEL)
    {
        p.y_v = MAX_VEL;
    }
    else if (p.y_v < -MAX_VEL)
    {
        p.y_v = -MAX_VEL;
    }

    p.x_o = p.x;
    p.y_o = p.y;

    p.x += p.x_v * t_step; // UPDATING STATE
    p.y += p.y_v * t_step;

    p.x_v *= 0.95;
    p.y_v *= 0.95;

    if ((p.y_v*p.y_v) + (p.x_v*p.x_v) > 1)
        p.theta = atan2(p.y_v, p.x_v);
    //^THAT IS EULER INTEGRATION

}

void drawPlayer(Player &p)
{
    float x1, y1, x2, y2, x3, y3;

    x1 = p.x + cos(p.theta+(PI/2)) * 5.f;
    x2 = p.x + cos(p.theta-(PI/2)) * 5.f;
    x3 = p.x + cos(p.theta) * 20.f;

    y1 = p.y + sin(p.theta + (PI/2)) * 5.f;
    y2 = p.y + sin(p.theta - (PI/2)) * 5.f;
    y3 = p.y + sin(p.theta) * 20.f;

    /*draw_rect.x = (int)(p.x-5.f) - cam.x;
    draw_rect.y = (int)(p.y-5.f) - cam.y;
    draw_rect.w = 10;
    draw_rect.h = 10;*/

    //SDL_FillRect(screen, &draw_rect, SDL_MapRGB(screen->format, 128, 0, 128));
    //lineRGBA(screen, p.x - cam.x, p.y - cam.y, (p.x + (p.x_v / 2)) - cam.x, (p.y + (p.y_v / 2))-cam.y, 255, 0, 0, 255);

    lineRGBA(screen, x1 - cam.x, y1 - cam.y, x2 - cam.x, y2 - cam.y, 128, 0, 128, 255);
    lineRGBA(screen, x2 - cam.x, y2 - cam.y, x3 - cam.x, y3 - cam.y, 128, 0, 128, 255);
    lineRGBA(screen, x3 - cam.x, y3 - cam.y, x1 - cam.x, y1 - cam.y, 128, 0, 128, 255);

}

/*bool belowTerrain(Wave &w, Player &p)
{
    float tmp = (w.a * sin(degToRad((w.b * p.x) + w.c)) + w.d);
    if (p.y > tmp)
    {
        return true;
    }
    else
    {
        return false;
    }
}

float solveWave(Wave &w, float t)
{
    return w.a * sin(degToRad((w.b * t) + w.c)) + w.d;
}*/

void updateCamera(Camera &c)
{
    if ((int)play.x - c.x < c.margin_w )
    {
        c.x = (int)play.x - c.margin_w;
    }
    else if ((int)play.x - c.x > scr_width - c.margin_w)
    {
        c.x = (int)play.x + c.margin_w - scr_width;
    }

    if ((int)play.y - c.y < c.margin_h)
    {
        c.y = (int) play.y - c.margin_h;
    }
    else if ((int)play.y - c.y > scr_height - c.margin_h)
    {
        c.y = (int)play.y + c.margin_h - scr_height;
    }

    if (c.x + scr_width > wrld_width)
    {
        c.x = wrld_width - scr_width;
    }
    else if (c.x < 0)
    {
        c.x = 0;
    }

    if (c.y + scr_height > wrld_height)
    {
        c.y = wrld_height - scr_height;
    }
    else if (c.y < 0)
    {
        c.y = 0;
    }
}

int randInt(int m, int mx)
{
    if (m > mx)
    {
        int tmp = m;
        m = mx;
        mx = m ;
    }

    return (rand() % (mx - m))+m;
}

void generateTerrain(Terrain &t)
{
    int d_x = 0, d_y = 0;
    int t_y;

    t.x.clear();
    t.y.clear();

    t_y = randInt(wrld_height - (wrld_height/4), wrld_height);

    for (int i = 0; i <= wrld_width; i += d_x)
    {
        t.x.push_back(i);
        t.y.push_back(t_y);

        d_x = randInt(scr_width/8, scr_width/2);
        d_y = randInt(-30, 30);

        if (i + d_x > wrld_width)
        {
            if (i != wrld_width)
                d_x = wrld_width - i;
            else
                d_x = 1;
        }

        t_y += d_y;
    }
}

void drawTerrain(Terrain &t)
{
    bool beg_set = false;
    int beg_i = 0;
    int end_i = t.x.size();

    for (int i = 1; i < t.x.size(); i++)
    {
        if (!beg_set)
        {
            if (t.x[i]-cam.x >= 0)
            {
                beg_i = i-1;
                beg_set = true;
            }
        }
        else
        {
            if (t.x[i]-cam.x >= scr_width)
            {
                end_i = i;
                break;
            }
        }
    }

    for (int i = beg_i; i < end_i; i++)
    {
        //cout << "Terrain_x: " << t.x[i]-cam.x << endl << "Terrain_x2: " << t.x[i+1]-cam.y << endl;
        //cout << "Terrain_y: " << t.y[i] << endl << "Terrain_y2: " << t.y[i+1] << endl;
        lineRGBA(screen, t.x[i]-cam.x, t.y[i]-cam.y, t.x[i+1]-cam.x, t.y[i+1]-cam.y, 255, 0, 0, 255);
    }
}

void updateExplosion(Explosion &e)
{
    if (SDL_GetTicks() - e.create_time >= e.life_span)
    {
        e.to_delete = true;
        e.active = false;
        return;
    }
    else
    {
        e.cur_r = (((SDL_GetTicks() - e.create_time) * (e.end_r - e.start_r)) / e.life_span);
        //cout<< "E cur_r: " << SDL_GetTicks() - e.create_time << endl;
    }

}

void drawExplosion(Explosion &e)
{
    filledCircleRGBA(screen, e.p.X() - cam.x, e.p.Y() - cam.y, e.cur_r, 128, 0, 0, 255);
}

void deleteExplosions()
{
    for (int i = explosions.size()-1; i >= 0; i--)
    {
        if (explosions[i]->to_delete)
        {
            delete explosions[i];
            explosions.erase(explosions.begin()+i);
        }
    }
}

void deleteAllExplosions()
{
    for (int i = explosions.size()-1; i >= 0; i--)
    {
        delete explosions[i];
    }
    explosions.clear();
}

void createExplosion(float x, float y, float s_r, float end_r, int l_p)
{
    Explosion* exp = new Explosion;
    exp->p.X(x);
    exp->p.Y(y);
    exp->active = true;
    exp->to_delete = false;
    exp->cur_r = s_r;
    exp->start_r = s_r;
    exp->end_r = end_r;
    exp->life_span = l_p;

    exp->create_time = SDL_GetTicks();

    explosions.push_back(exp);
}

bool checkCollision(float ax, float ay, float bx, float by, int bw, int bh)
{
    if (ax >= bx && ax <= bx + bw && ay >= by && ay <= by + bh)
        return true;
    else
        return false;
}

bool checkCollision(float ax, float ay, int aw, int ah, float bx, float by, int bw, int bh)
{
    if (ax + aw < bx)
        return false;
    else if (ax > bx + bw)
        return false;
    else if (ay + ah < by)
        return false;
    else if (ay > by + bh)
        return false;

    return true;
}

bool pointOnLine(float p_x, float p_y, float l_x1, float l_y1, float l_x2, float l_y2)
{
    float slope;
    float b;

    if ((int)l_x2 == (int)l_x1)
    {
        if ((int)p_x == (int)l_x2)
        {
            if (p_y >= MIN(l_x1, l_x2) && p_y <= MAX(l_x1, l_x2))
            {
                return true;
            }
        }
    }
    else
    {
        float x_pos;
        slope = (l_y2-l_y1)/(l_x2-l_x1);
        b = p_y - (slope*l_x1);
        x_pos = (p_y - b) / slope;
        cout << "xpos: " << x_pos << endl;

        if (x_pos >= MIN(l_x1, l_x2) && x_pos <= MAX(l_x1, l_x2))
        {
            cout << "moo" << endl;
            return true;
        }
    }

    return false;

}


