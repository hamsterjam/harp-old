#ifndef __HARP_GLOBALS_H
#define __HARP_GLOBALS_H


#include <cstddef>


#include <ECS.h>
#include <Vec2.h>

class ALLEGRO_DISPLAY;
class ALLEGRO_TIMER;
class ALLEGRO_EVENT_QUEUE;

typedef Vec2<int> Vec2i;
typedef Vec2<double> Vec2f;

ALLEGRO_DISPLAY* disp;
ALLEGRO_TIMER* timer;
ALLEGRO_EVENT_QUEUE* eq;

const float FRAME_TIME = 1.0/60.0;
const unsigned int SCREEN_WIDTH = 640;
const unsigned int SCREEN_HEIGHT = 480;

ECS ecs(8, 32, 128);

// Components
Component COMP_POSITION     = ecs.createComponentType(sizeof(Vec2f));
Component COMP_VELOCITY     = ecs.createComponentType(sizeof(Vec2f));
Component COMP_ACCELERATION = ecs.createComponentType(sizeof(Vec2f));

#endif
