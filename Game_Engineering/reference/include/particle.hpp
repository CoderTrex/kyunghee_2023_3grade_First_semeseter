#ifndef PARTICLE_HPP_
#define PARTICLE_HPP_

#include <vector>
#include <cmath>
#include "../glm/glm.hpp"
#include <omp.h>

#include "common.hpp"
#include "rand_generator.hpp"

class Particle
{
public:    
    std::vector<glm::vec3> position;
    std::vector<glm::vec3> velocity;
    std::vector<glm::vec3> acceleration;
    std::vector<glm::vec3> force;
    std::vector<float> density;
    std::vector<float> pressure;

    std::vector<glm::vec3> next_position;    
    std::vector<glm::vec3> next_velocity;
    std::vector<glm::vec3> next_acceleration;

    std::vector<glm::vec3> gl_position;
    std::vector<glm::vec3> gl_color;

private:
    RandGenerator rand_generator;

public:
    Particle()
    : position(k_num_particle)
    , velocity(k_num_particle)
    , acceleration(k_num_particle)
    , force(k_num_particle)
    , density(k_num_particle)
    , pressure(k_num_particle)
    , next_position(k_num_particle)
    , next_velocity(k_num_particle)
    , next_acceleration(k_num_particle)   
    , gl_position(k_num_particle)
    , gl_color(k_num_particle)
    {
        initialize_particle_state();
    };
    
    std::vector<glm::vec3> &get_gl_particle_position() 
    { 
        #pragma omp parallel for
        for (int i = 0; i < k_num_particle; i++)
        {
            gl_position.at(i) = transform_world2gl(position[i]);
        }
        return gl_position;
    }
    std::vector<glm::vec3> &get_gl_particle_color() { return gl_color; }

    ~Particle() {};

private:
    void initialize_particle_state()
    {
        #pragma omp parallel for
        for (int i = 0; i < k_num_particle; i++)
        {
            gl_color.at(i) = {153/255, 255/255, 255/255};
            position.at(i) = rand_generator.generate_random_uniform_vec3(0, k_world_edge_size);
            velocity.at(i) = {0.0f, 0.0f, 0.0f};
        }
    }
};

#endif // PARTICLE_HPP_