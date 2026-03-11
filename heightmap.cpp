#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <random>
#include <algorithm>

class Heightmap {
private:
    int size;
    std::vector<float> content;
    float current_from = 0;
    float current_to = 1;

    void bind_random(){
        static bool seeded = false;
        if(!seeded){
            srand(time(NULL));
            seeded = true;
        }
    }

    void init_random(){
        for(int i=0; i<size*size; i++)
        content[i] = rand()/(float)RAND_MAX;
    }

    void init_tozero(){
        for(int i=0; i<size*size; i++)
        content[i] = 0;
    }

    void init_random_recurse(
        std::vector<float>& vector_2d,
        int x, int y, int region_size,
        int depth, float strenght
    ){
        float layer = rand()/(float)RAND_MAX * strenght;
        for(int i = y; i < y + region_size; i++)
        for(int j = x; j < x + region_size; j++){
            vector_2d[i*size+j] += layer;
        }

        if(region_size <= 1 || depth<=0) return;

        int half = region_size / 2;

        init_random_recurse(vector_2d, x,      y,      half, depth-1, strenght*0.5f);
        init_random_recurse(vector_2d, x+half, y,      half, depth-1, strenght*0.5f);
        init_random_recurse(vector_2d, x,      y+half, half, depth-1, strenght*0.5f);
        init_random_recurse(vector_2d, x+half, y+half, half, depth-1, strenght*0.5f);
    }

    std::vector<float> smooth_interpolation(int depth){
        float influence = 1.0f/(3*(1<<depth-1)-2);
        std::vector<float> smoothed(size*size);
      
        for (int i = 0; i<size; i++)
        for (int j = 0; j<size; j++)
        for (int k=-depth; k<depth; k++){
             int idx = std::clamp(j+k, 0, size-1);
             smoothed[i*size+j]+=(1<<depth-abs(k))*influence*content[i*size+idx];
        }

      
        for (int i = 0; i<size; i++)
        for (int j = 0; j<size; j++)
        for (int k=-depth; k<depth; k++){
             int idx = std::clamp(i+k, 0, size-1);
             smoothed[i*size+j]+=(1<<depth-abs(k))*influence*content[idx*size+j];
        }
        
        return smoothed;
    }

    void scale(float factor){
        for(int i=0; i<size*size; i++)
            content[i] *= factor;
    }

    void add_toall(float val){
        for(int i=0; i<size*size; i++)
            content[i] += val;
    }
public:
    Heightmap(int size) : size(size) {
        bind_random();
        content.resize(size*size);
        if ((size&(size-1)) != 0) throw "Size must be a power of two for some functions to work!";
    };
    
    void rescale(float from, float to){
     add_toall(-current_from);
     float range = current_to-current_from;
     scale((to-from)/range);
     add_toall(from);
     
     current_from = from;
     current_to=to;
    }
    
    float at(int x, int y){
     if(y<0 || y>=size || x<0 || x>=size){
      return NAN;
     }
     return content[y*size+x];
    }
    
    void strong_random_init(int depth, float strenght=0.5){
      init_tozero();
      init_random_recurse(content, 0, 0, size, depth, strenght);
    }
    void random_init(){
     init_random();
    }
    std::vector<float> get_1d_vector(){
     return content;
    }
    void interpolate_V1(float coef, int repeats){
      //dobav sviy kod tut
      return;
    }
    void interpolate_v2( int depth, int repeats=1){
      for(int i = 0; i<repeats; i++){
        //todo for now
    }
};

int main(){
  Heightmap map(256):
  map.random_init();
  map.interpolate_v1();
  
 return 0;
}

#endif
