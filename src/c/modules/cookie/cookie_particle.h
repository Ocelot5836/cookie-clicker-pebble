#pragma once

#include <pebble.h>

#define MAX_COOKIE_PARTICLES 64
#define COOKIE_PARTICLE_WIDTH 17
#define COOKIE_PARTICLE_HEIGHT 17
#define COOKIE_PARTICLE_YOFF COOKIE_PARTICLE_HEIGHT * 2
typedef uint64_t CookieStatus_t;

extern uint8_t cookie_particle_x[MAX_COOKIE_PARTICLES];
extern uint16_t cookie_particle_y[MAX_COOKIE_PARTICLES];
extern uint8_t cookie_particle_speed[MAX_COOKIE_PARTICLES];
extern CookieStatus_t cookie_particle_status;

void cookie_particle_spawn();

void cookie_particles_kill();

void cookie_particles_tick(GRect *Area);
