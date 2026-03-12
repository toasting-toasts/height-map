#include "heightmap.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>


Heightmap::Heightmap(int size)
    : size(size), engine(rd()), dist(0.f,1.f)
{
    content.resize(size * size);

    if ((size & (size - 1)) != 0)
        throw "size must be power of two for some functions to work";
}


void Heightmap::init_random(){
    for(int i = 0; i < size * size; i++)
        content[i] = dist(engine);
}


void Heightmap::init_toval(float val){
    std::fill(content.begin(), content.end(), val);
}


void Heightmap::init_random_recurse(
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


void Heightmap::smooth_interpolation(int depth){

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


void Heightmap::scale(float factor){

    for(int i = 0; i < size * size; i++)
        content[i] *= factor;
}


void Heightmap::add_toall(float val){

    for(int i = 0; i < size * size; i++)
        content[i] += val;
}


void Heightmap::rescale(float from, float to){

    add_toall(-current_from);

    float range = current_to - current_from;

    scale((to - from) / range);

    add_toall(from);

    current_from = from;
    current_to = to;
}


float Heightmap::at(int x, int y){

    if(y < 0 || y >= size || x < 0 || x >= size)
        return NAN;

    return content[y * size + x];
}


void Heightmap::strong_random_init(int depth, float strength){

    init_toval(0);

    init_random_recurse(content, 0, 0, size, depth, strength);
}


void Heightmap::random_init(){

    init_random();
}


const std::vector<float>& Heightmap::get_1d_vector() const{

    return content;
}


void Heightmap::interpolate_V1(float coef, int repeats){

    // TODO: implement later
}


void Heightmap::interpolate_v2(int depth){

    smooth_interpolation(depth);
}