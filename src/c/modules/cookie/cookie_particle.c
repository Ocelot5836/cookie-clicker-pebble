#include "cookie_particle.h"
#include "cookie.h"

#if PARTICLE_ENABLE
uint8_t cookie_particle_x[MAX_COOKIE_PARTICLES];
uint16_t cookie_particle_y[MAX_COOKIE_PARTICLES];
uint8_t cookie_particle_speed[MAX_COOKIE_PARTICLES];
CookieStatus_t cookie_particle_status;

void cookie_particle_spawn()
{
    if ((cookie_particle_status + 1) == 0)
    {
        return;
    }

    uint8_t index = __builtin_ctz(~cookie_particle_status);
    cookie_particle_status |= 1 << index;

    cookie_particle_x[index] = rand() & 0xFF;
    cookie_particle_y[index] = 0;
    cookie_particle_speed[index] = rand() & 0xFF;
}

void cookie_particles_kill()
{
    cookie_particle_status = 0;
}

void cookie_particles_tick(GRect *bounds)
{
    if (cookie_particle_status != 0)
    {
        for (size_t i = __builtin_ctz(cookie_particle_status); i < MAX_COOKIE_PARTICLES; i++)
        {
            if (((cookie_particle_status >> i) & 1) == 0)
            {
                continue;
            }

            uint8_t speed = cookie_particle_speed[i];
            if (speed < 0xF0)
            {
                cookie_particle_speed[i] += rand() & 0xF;
            }
            cookie_particle_y[i] += speed / 16;

            if (cookie_particle_y[i] < COOKIE_PARTICLE_YOFF)
            {
                continue;
            }

            if (cookie_particle_y[i] - COOKIE_PARTICLE_YOFF >= bounds->size.h)
            {
                cookie_particle_status &= ~(1 << i);
            }
        }
    }
}
#endif
