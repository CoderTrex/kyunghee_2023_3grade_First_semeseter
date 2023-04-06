#ifndef VELOCITY_FIELD_HPP_
#define VELOCITY_FIELD_HPP_

#include <cmath>

#include "../glm/glm.hpp"
#include "common.hpp"

namespace velocity_field
{

inline glm::vec3 electric_field(glm::vec3 pos)
{
    pos = transform_world2gl(pos);
    float r = std::pow(pos[0]*pos[0] + pos[1]*pos[1], 1/2);

    glm::vec3 ret;
    ret[0] = (3 * pos[0] * pos[1])  / std::pow(r, 5);
    ret[1] = (2 * pos[1] * pos[1] -  pos[0] * pos[0]) / std::pow(r, 5);
    ret[2] = 0;
    ret = transform_gl2world(ret) * 0.1f;

    return ret;
}

// inline glm::vec3 radiation(glm::vec3 pos)
// {

// }

inline glm::vec3 sink(glm::vec3 pos_j, glm::vec3 pos_i)
{
    pos_j = transform_world2gl(pos_j);
    pos_i = transform_world2gl(pos_i);

    float r_j = std::pow(pos_j[0]*pos_j[0] + pos_j[1]*pos_j[1], 1/2);
    float r_i = std::pow(pos_i[0]*pos_i[0] + pos_i[1]*pos_i[1], 1/2);

    glm::vec3 ret;
    ret[0] = -1 / (2 * M_PI * (r_j-r_i));
    ret[1] = 0;
    ret[2] = 0;
    ret = transform_gl2world(ret);
    return ret;
}

} //namespace velocity_field

#endif // VELOCITY_FIELD_HPP_
