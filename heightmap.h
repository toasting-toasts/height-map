#pragma once

#include <vector>
#include <random>

class Heightmap {
private:
    int size;
    std::vector<float> content;

    float current_from = 0;
    float current_to = 1;

    std::random_device rd;
    std::mt19937 engine;
    std::uniform_real_distribution<float> dist;

    void init_random();
    void init_toval(float val);

    void init_random_recurse(
        std::vector<float>& vector_2d,
        int x, int y,
        int region_size,
        int depth,
        float strength
    );

    void smooth_interpolation(int depth);

    void scale(float factor);
    void add_toall(float val);

public:
    Heightmap(int size);

    void rescale(float from, float to);

    float at(int x, int y);

    void strong_random_init(int depth, float strength = 0.5f);

    void random_init();

    const std::vector<float>& get_1d_vector() const;

    void interpolate_V1(float coef, int repeats);

    void interpolate_v2(int depth);
};