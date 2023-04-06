#ifndef COMMON_H_
#define COMMON_H_

#include <vector>
#include <iostream>
#include <cmath>
#include <string>
#include <map>

#include "../glm/glm.hpp"

const char k_project_name[] = "FLUID SIMULATION";

// World Params --------------------------------------------------------------------------//
const int k_world_edge_size = 32;
const glm::vec3 k_gravity_acceleration = {0.0f, 0.0f, -9.8f};
const unsigned int k_num_particle_each_side = 70;
const unsigned int k_num_particle = std::pow(k_num_particle_each_side, 2);

// Timer --------------------------------------------------------------------//
const float k_time_step = 0.01;                 // sec
const unsigned int k_max_display_time = 60;     // sec

// Material -----------------------------------------------------------------//
enum material
{
    water, mercury, air
};

struct property
{
    float density;          // kg/m^3
    float dynamic;          // kg/m-s
    float kinematic;        // m^2/s
    float surface_tension;  // N/m
};

std::map<material, property> material_property_map = 
{
    { material::water, {998, 1000, 1, 0.0728} },
    { material::mercury, {13500, 1550, 0.115, 0.485} },
    { material::air, {1.18, 18.7, 15.8, 0} }
};

material k_fluid_material = material::mercury;
property k_fluid_property = material_property_map[k_fluid_material];
const float k_fluid_stiffness = 1.0f;

const float k_surface_tension_level_threshold = 0.0f;

const float k_fluid_volume = std::pow(k_world_edge_size, 3) / 2;  // fill half of the box
const float k_particle_mass = k_fluid_property.density * k_fluid_volume / k_num_particle;
const float k_particle_radius = std::pow((((3 * k_particle_mass) / (4 * M_PI * k_fluid_property.density))), 1/3);

// SPH System ----------------------------------------------------------------------//
const unsigned int k_num_neighboring_particle = 100;
const float k_sph_s = std::pow((3 * k_fluid_volume * k_num_neighboring_particle) / (4 * M_PI * k_num_particle), 1/3);


static std::vector<std::vector<unsigned int>> neighborhood(k_num_particle, std::vector<unsigned int>(0));

inline float sph_default_kernel(glm::vec3 r)
{
    float r_len = glm::length(r);
    if (r_len <= k_sph_s)
    {
        return (315 / (64 * M_PI * std::pow(k_sph_s, 9))) * (std::pow((std::pow(k_sph_s, 2) - std::pow(r_len, 2)), 3));
    }
    else 
    {
        return 0;
    }
    
    /*
    sph_kernel_r = std::abs(sph_kernel_r);

    float rs_ratio = sph_kernel_r / k_sph_s;
    float ret = (1 / (M_PI * std::pow(k_sph_s, 3)));

    if (rs_ratio < 0 || rs_ratio > 2) 
    {
        return ret * 0.0f;
    } 
    else if (0 <= rs_ratio && rs_ratio <= 1)
    {
        return ret * (1 - (3/2) * std::pow(rs_ratio, 2) + (3/4) * std::pow(rs_ratio, 3));
    } 
    else if (1 <= rs_ratio && rs_ratio <= 2)
    {
        return ret * (1/4) * std::pow((2 - rs_ratio), 3);
    }
    
    return -1;
    */
}

inline glm::vec3 sph_default_kernel_gradient(glm::vec3 r)
{
    return r
        * (float)((-945 / (32 * M_PI * std::pow(k_sph_s, 9))) * std::pow((std::pow(k_sph_s, 2) - std::pow(glm::length(r), 2)), 2));
}

inline float sph_default_kernel_laplacian(glm::vec3 r)
{
    float r_len = glm::length(r);
    return (-945 / (32 * M_PI * std::pow(k_sph_s, 9))) * (std::pow(k_sph_s, 2) - std::pow(r_len, 2)) * (3 * std::pow(k_sph_s, 2) - 7 * std::pow(r_len, 2));
}

inline glm::vec3 sph_pressure_kernel_gradient(glm::vec3 r)
{
    float r_len = glm::length(r);
    if (0 < r_len && r_len < 1e-5) return glm::vec3(-45 / (M_PI * std::pow(k_sph_s, 6)));
    else if (-1e-5 < r_len && r_len <= 0) return glm::vec3(45 / (M_PI * std::pow(k_sph_s, 6)));
    else return r / r_len * (float)(-45 / (M_PI * std::pow(k_sph_s, 6)) * std::pow((k_sph_s - r_len), 2));  
}

inline float sph_diffusion_kernel_laplacian(glm::vec3 r)
{
    return (45 / (M_PI * std::pow(k_sph_s, 6))) * (k_sph_s - glm::length(r));
}

// Integrator --------------------------------------------------------------------//
enum integrator 
{
    ex_euler, im_euler, rk2, verlet
};
const integrator k_integration_method  = integrator::verlet;


// OpenGL -------------------------------------------------------------------//
inline glm::vec3 transform_world2gl(glm::vec3 &v) { return (v * 2.0f / (float)k_world_edge_size) - 1.0f; }
inline glm::vec3 transform_gl2world(glm::vec3 &v) { return (v + 1.0f) * (float)(k_world_edge_size / 2.0f); }

// Debug --------------------------------------------------------------------//
void print_vec(glm::vec3 vec, std::string var_name = "var_name")
{
    std::cout << var_name << ": ";
    std::cout << "[" << vec[0] << ", " 
                     << vec[1] << ", " 
                     << vec[2] << "]\n";
}

#endif // COMMON_H_