#ifndef RAND_GENERATOR_H_
#define RAND_GENERATOR_H_

#include <vector>
#include <cstdlib>
#include <ctime>

#include "../glm/glm.hpp"

#include "common.hpp"
 
const unsigned int rand_vec_len = 9973;

class RandGenerator
{
private:
    std::vector<float> random_num_vec_;
    unsigned int offset_;

private:
    inline unsigned int get_offset() 
    {
        if(++offset_ == rand_vec_len) {
            offset_ = 0;
        }
        return offset_;
    }
    
public:
    RandGenerator() 
    {
        offset_ = 0;
        random_num_vec_.clear();
        srand(time(NULL));
        for(int i = 0; i < rand_vec_len; i++) {
            random_num_vec_.push_back(static_cast<float> (rand() / static_cast<float> (RAND_MAX)));
        }
    };

    inline float generate_uniform(float u_min, float u_max)
    {
        return u_min + ((u_max - u_min) * random_num_vec_.at(get_offset()));
    }

    inline float generate_gaussian(float std_deviation, float mean) 
    {
        return mean + std_deviation * random_num_vec_.at(get_offset());
    }

    glm::vec3 generate_random_uniform_vec3(float u_min, float u_max)
    {
        return {
            generate_uniform(u_min, u_max),
            generate_uniform(u_min, u_max),
            generate_uniform(u_min, u_max)
        };
    }

    glm::vec3 generate_random_gaussian_vec3(float std_deviation, float mean) 
    {
        return {
            generate_gaussian(std_deviation, mean),
            generate_gaussian(std_deviation, mean),
            generate_gaussian(std_deviation, mean),
        };
    }

    glm::vec3 generate_random_direction_vec(int scalar = 1) 
    {
        float theta  = generate_uniform(-M_PI, M_PI);
        float y      = generate_uniform(-1, 1);

        return {
            scalar * std::cos(theta),
            y,
            scalar * -std::sin(theta)
        };
    }

    ~RandGenerator() {};
};


#endif // RAND_GENERATOR_H_