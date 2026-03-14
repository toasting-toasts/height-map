#pragma once

#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
#include <stdexcept>

/*
============================================================
Heightmap
============================================================

A simple procedural heightmap generator storing height values
in a flattened 1D vector.

Features:
- Random initialization
- Recursive terrain-like noise generation
- Smoothing interpolation
- Value scaling and normalization

Storage format:
content[y * size + x]

All values are typically kept in the range [0,1] unless rescaled.

Size must be a power of two for recursive generation.
*/

class Heightmap {
private:

    /*
    Dimension of the heightmap.

    The map contains size × size values.
    */
    int size;


    /*
    Flattened 2D heightmap data.

    Access pattern:
    content[y * size + x]
    */
    std::vector<float> content;


    /*
    Current value range stored in the heightmap.
    Used by rescale() to correctly transform values.
    */
    float current_from = 0;
    float current_to = 1;


    /*
    Random number generation utilities.
    */
    std::random_device rd;
    std::mt19937 engine;
    std::uniform_real_distribution<float> dist;


    /*
    --------------------------------------------------------
    init_random
    --------------------------------------------------------

    Fills the entire heightmap with random values
    sampled from uniform distribution [0,1].
    */
    inline void init_random(){
        for(int i = 0; i < size * size; i++)
            content[i] = dist(engine);
    }


    /*
    --------------------------------------------------------
    init_toval
    --------------------------------------------------------

    Initializes the entire heightmap to a constant value.
    */
    inline void init_toval(float val){
        std::fill(content.begin(), content.end(), val);
    }


    /*
    --------------------------------------------------------
    init_random_recurse
    --------------------------------------------------------

    Recursive terrain-like noise generator.

    Algorithm:
    1. Add a random offset to the entire region.
    2. Subdivide region into 4 smaller regions.
    3. Recursively apply smaller random offsets.

    Parameters:
    vector_2d   - reference to heightmap data
    x,y         - top-left corner of region
    region_size - size of square region
    depth       - recursion depth
    strength    - magnitude of noise

    The strength decreases every recursion level,
    producing fractal-like terrain variation.
    */
    inline void init_random_recurse(
        std::vector<float>& vector_2d,
        int x, int y,
        int region_size,
        int depth,
        float strength
    ){

        float layer = (dist(engine) * 2 - 1) * strength;

        for(int i = y; i < y + region_size; i++)
        for(int j = x; j < x + region_size; j++){

            vector_2d[i * size + j] =
                std::clamp(vector_2d[i * size + j] + layer, 0.f, 1.f);
        }

        if(region_size <= 1 || depth <= 0)
            return;

        int half = region_size / 2;

        init_random_recurse(vector_2d, x,      y,      half, depth-1, strength*0.5f);
        init_random_recurse(vector_2d, x+half, y,      half, depth-1, strength*0.5f);
        init_random_recurse(vector_2d, x,      y+half, half, depth-1, strength*0.5f);
        init_random_recurse(vector_2d, x+half, y+half, half, depth-1, strength*0.5f);
    }


    /*
    --------------------------------------------------------
    smooth_interpolation
    --------------------------------------------------------

    Applies smoothing using a weighted kernel.

    Process:
    1. Vertical smoothing pass
    2. Horizontal smoothing pass

    The smoothing radius is controlled by "depth".

    Larger depth produces smoother terrain but
    increases computational cost.
    */
    inline void smooth_interpolation(int depth){

        float influence = 1.0f/(3*(1<<depth)-2);

        std::vector<float> smoothed(size * size, 0.f);
        std::vector<float> weights(depth * 2 + 1, influence);

        for(int i = -depth; i <= depth; i++){
            weights[i + depth] *= (1 << (depth - std::abs(i))) * influence;
        }

        for(int i = 0; i < size; i++)
        for(int j = 0; j < size; j++)
        for(int k = -depth; k <= depth; k++){

            int idx = std::clamp(i + k, 0, size - 1);

            smoothed[i * size + j] +=
                weights[k + depth] * content[idx * size + j];
        }

        std::swap(smoothed, content);

        std::fill(smoothed.begin(), smoothed.end(), 0.f);

        for(int i = 0; i < size; i++)
        for(int j = 0; j < size; j++)
        for(int k = -depth; k <= depth; k++){

            int idx = std::clamp(j + k, 0, size - 1);

            smoothed[i * size + j] +=
                weights[k + depth] * content[i * size + idx];
        }

        std::swap(smoothed, content);
    }


    /*
    Multiplies every height value by a constant factor.
    */
    inline void scale(float factor){
        for(int i = 0; i < size * size; i++)
            content[i] *= factor;
    }


    /*
    Adds a constant value to all heightmap elements.
    */
    inline void add_toall(float val){
        for(int i = 0; i < size * size; i++)
            content[i] += val;
    }


public:


    /*
    --------------------------------------------------------
    Constructor
    --------------------------------------------------------

    Creates a heightmap with dimension size × size.

    Requirement:
    size must be a power of two.

    This restriction is needed for recursive terrain
    generation algorithms.
    */
    inline Heightmap(int size)
        : size(size), engine(rd()), dist(0.f,1.f)
    {
        content.resize(size * size);

        if ((size & (size - 1)) != 0)
            throw std::runtime_error("size must be power of two");
    }


    /*
    --------------------------------------------------------
    rescale
    --------------------------------------------------------

    Remaps all values from current range
    [current_from, current_to]
    to a new range [from, to].
    */
    inline void rescale(float from, float to){

        add_toall(-current_from);

        float range = current_to - current_from;

        scale((to - from) / range);

        add_toall(from);

        current_from = from;
        current_to = to;
    }


    /*
    --------------------------------------------------------
    at
    --------------------------------------------------------

    Returns height value at coordinate (x,y).

    If coordinates are outside the map,
    NaN is returned.
    */
    inline float at(int x, int y){

        if(y < 0 || y >= size || x < 0 || x >= size)
            return NAN;

        return content[y * size + x];
    }


    /*
    --------------------------------------------------------
    strong_random_init
    --------------------------------------------------------

    Generates fractal-like terrain using recursive noise.

    Parameters:
    depth     - recursion depth
    strength  - maximum noise amplitude
    */
    inline void strong_random_init(int depth, float strength = 0.5f){

        init_toval(0);

        init_random_recurse(content, 0, 0, size, depth, strength);
    }


    /*
    --------------------------------------------------------
    random_init
    --------------------------------------------------------

    Simple random initialization of the heightmap.
    */
    inline void random_init(){
        init_random();
    }


    /*
    --------------------------------------------------------
    get_1d_vector
    --------------------------------------------------------

    Returns reference to internal data container.

    :Of type vector(y*size+x)
    */
    inline const std::vector<float>& get_1d_vector() const{
        return content;
    }


    /*
    Placeholder for an alternative interpolation method.
    */
    inline void interpolate_V1(float coef, int repeats){
        // TODO
    }


    /*
    --------------------------------------------------------
    interpolate_v2
    --------------------------------------------------------

    Applies smoothing interpolation to the heightmap.

    depth controls the smoothing radius.
    */
    inline void interpolate_v2(int depth){
        smooth_interpolation(depth);
    }

    /*
    Get size of your heightmap in case you forgot it.
    */
    const int get_size() const{
        return size;
    }

};