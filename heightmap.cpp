#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <vector>
#include <cmath>
#include <ctime>
#include <random>
#include <algorithm>

class Heightmap {
private:
    int size;
    std::vector<float> content;
    float current_from = 0;
    float current_to = 1;

    std::random_device rd;
    std::mt19997 engine(rd());
    std::uniform_real_distribution<float> dist(0.f, 1.f);

    void init_random(){
        for(int i=0; i<size*size; i++)
        content[i] = dist(engine);
    }

    void init_toval(float val){
        std::fill(content.begin(), content.end(), val);
    }

    void init_random_recurse(
        std::vector<float>& vector_2d,
        int x, int y, int region_size,
        int depth, float strenght
    ){
        float layer = (dist(engine)*2-1) * strenght;
        for(int i = y; i < y + region_size; i++)
        for(int j = x; j < x + region_size; j++){
            vector_2d[i*size+j] = std::clamp(vector_2d[i*size+j]+layer, 0, 1);
        }

        if(region_size <= 1 || depth<=0) return;

        int half = region_size / 2;

        init_random_recurse(vector_2d, x,      y,      half, depth-1, strenght*0.5f);
        init_random_recurse(vector_2d, x+half, y,      half, depth-1, strenght*0.5f);
        init_random_recurse(vector_2d, x,      y+half, half, depth-1, strenght*0.5f);
        init_random_recurse(vector_2d, x+half, y+half, half, depth-1, strenght*0.5f);
    }


    void smooth_interpolation(int depth){
        float influence = 1.0f/(3*(1<<depth)-2);
        std::vector<float> smoothed(size*size, 0);
        std::vector<int> weights(depth*2+1);

        for(int i=-depth; i<=depth; i++){
            weights[i+depth]= (1 << ( depth-std::abs(i) ));
        }
        
        for (int i = 0; i<size; i++)
        for (int j = 0; j<size; j++)
        for (int k=-depth; k<=depth; k++){
             int idx = std::clamp(i+k, 0, size-1);
             smoothed[i*size+j]+=weights[k+depth]*influence*content[idx*size+j];
        }

        std::swap(smoothed, content);
        std::fill(smoothed.begin(), smoothed.end(), 0.f);
        
        for (int i = 0; i<size; i++)
        for (int j = 0; j<size; j++)
        for (int k=-depth; k<=depth; k++){
             int idx = std::clamp(j+k, 0, size-1);
             smoothed[i*size+j]+= weights[k+depth]*influence*content[i*size+idx];
        }
        
        std::swap(smoothed, content);
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
    Heightmap(int size) : size(size){
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
      init_toval(0);
      init_random_recurse(content, 0, 0, size, depth, strenght);
    }
    void random_init(){
     init_random();
    }
    const std::vector<float>& get_1d_vector() const {
     return content;
    }
    void interpolate_V1(float coef, int repeats){
      //dobav sviy kod tut
      return;
    }
    void interpolate_v2( int depth, int repeats=1){
      for(int i = 0; i<repeats; i++){
        smooth_interpolation(depth);
        }
    }
};

int main(){
  Heightmap map(256);
  map.random_init();
  map.interpolate_v2(5);
  
 return 0;
}

#endif
