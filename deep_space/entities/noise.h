#ifndef NOISE_ENTITY
#define NOISE_ENTITY

struct SimplexNoise {
    int octaves = 10;
    float scale = 1;
    float fractal = 5;
    float base_brightness = 0.25;
    float brightness = 0.75;
};

#endif /*NOISE_ENTITY*/
