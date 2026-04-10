#pragma once

#include <pebble.h>

#define PARTICLE_ENABLE !PBL_PLATFORM_APLITE

#if PARTICLE_ENABLE
#if PBL_PLATFORM_TYPE_CURRENT == PlatformTypeAplite
#define MAX_COOKIE_PARTICLES 16
typedef uint16_t CookieStatus_t;
#else
#define MAX_COOKIE_PARTICLES 64
typedef uint64_t CookieStatus_t;
#endif
#define PARTICLE_CULLING 0 && PBL_COLOR
#endif

#define SIZE_DEBUG 0
#define SCREENSHOT_MODE 0
#define DEBUG_NO_CLIP 0
