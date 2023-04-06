#ifndef COLLISION_DETECTOR_H_
#define COLLISION_DETECTOR_H_

#include <iostream>

#include "../glm/glm.hpp"
#include "common.hpp"

namespace collision
{

struct plain
{
    glm::vec3 normal;
    glm::vec3 point;
};

struct result
{
    glm::vec3 new_pos;
    glm::vec3 new_vel;

    bool operator!= (result r)
    {
        return (new_pos != r.new_pos) || (new_vel != r.new_vel);
    }
};

const static result null_result = {{-1, -1, -1}, {-1, -1, -1}};

plain box_plain[6] = 
{
    {{1, 0, 0}, {0, 0, 0}},                                                     // BACK
    {{0, 1, 0}, {0, 0, 0}},                                                     // LEFT
    {{0, 0, 1}, {0, 0, 0}},                                                     // BOTTOM
    {{-1, 0, 0}, {k_world_edge_size, k_world_edge_size, k_world_edge_size}},    // FRONT
    {{0, -1, 0}, {k_world_edge_size, k_world_edge_size, k_world_edge_size}},    // RIGHT
    {{0, 0, -1}, {k_world_edge_size, k_world_edge_size, k_world_edge_size}},    // TOP
};

inline float calculate_signed_distance(int i, glm::vec3 &pos) 
{
    glm::vec3 v = pos - box_plain[i].point;
    return glm::dot(box_plain[i].normal, v);
}

result detect_collision(glm::vec3 &pos, glm::vec3 &next_pos, glm::vec3 &vel, glm::vec3 &next_vel) 
{
    float d, next_d;
    glm::vec3 new_next_vel, new_next_pos, v_n, v_t;
    for(int i = 0; i < 6; i++)
    {
        d = calculate_signed_distance(i, pos);
        next_d = calculate_signed_distance(i, next_pos);

        if (std::signbit(d) != std::signbit(next_d))
        {
            new_next_pos = next_pos - 1.5f * next_d * box_plain[i].normal;

            v_n = glm::dot(next_vel, box_plain[i].normal) * box_plain[i].normal;
            v_t = next_vel - v_n;
            new_next_vel = - 0.5f * v_n + v_t;

            return {new_next_pos, new_next_vel};
        }
    }
    return null_result;
}

} // Collision


#endif