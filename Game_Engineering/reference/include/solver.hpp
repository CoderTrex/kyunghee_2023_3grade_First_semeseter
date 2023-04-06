#ifndef SOLVER_H_
#define SOLVER_H_

#include <vector>
#include <iostream>
#include <algorithm>

#include "../glm/glm.hpp"
#include <omp.h>
#include <tbb/tbb.h>
#include "../thirdparty/include/cyCodeBase/cyPointCloud.h"

#include "common.hpp"
#include "particle.hpp"
#include "collision_handler.hpp"
#include "velocity_field.hpp"

class Solver
{
private:
    Particle particles;
    cy::PointCloud<glm::vec3, float, 3> kdtree;

public: 
    Solver()
    {
    };

    void compute_next_state()
    {
        compute_neighborhood();

        switch (k_integration_method)
        {
            case integrator::im_euler:
                integrated_by_im_euler();
                break;
            case integrator::ex_euler:
                integrated_by_ex_euler();
                break;
            case integrator::rk2:
                integrated_by_rk2();
                break;
            case integrator::verlet:
                integrated_by_verlet();
                break;
            default:
                break;
        }
    }

    std::vector<glm::vec3> &get_gl_particle_position() { return particles.get_gl_particle_position(); }
    std::vector<glm::vec3> &get_gl_particle_color() { return particles.get_gl_particle_color(); }

    ~Solver()
    {
    };

private:
    void integrated_by_verlet()
    {
        #pragma omp parallel for
        for (int i = 0; i < k_num_particle; i++)
        {
            particles.next_position.at(i) = particles.position.at(i)
                + particles.velocity.at(i) * k_time_step
                + particles.acceleration.at(i) * (float)std::pow(k_time_step, 2) / 2.0f;
        }
        
        compute_applied_forces();   // using next_position

        #pragma omp parallel for
        for (int i = 0; i < k_num_particle; i++)
        {
            particles.next_acceleration.at(i) = particles.force.at(i) / particles.density.at(i);
        }
        
        
        #pragma omp parallel for
        for (int i = 0; i < k_num_particle; i++)
        {
            particles.next_velocity.at(i) = particles.velocity.at(i) 
                + (particles.acceleration.at(i) + particles.next_acceleration.at(i)) * k_time_step / 2.0f;
        }
        
        #pragma omp parallel for
        for (int i = 0; i < k_num_particle; i++)
        {
            collision::result ret = collision::detect_collision(particles.position.at(i), particles.next_position.at(i),
                                                                particles.velocity.at(i), particles.next_velocity.at(i));
            if (ret != collision::null_result)
            {
                particles.next_position.at(i) = ret.new_pos;
                particles.next_velocity.at(i) = ret.new_vel;
            }
        }

        particles.position = particles.next_position;
        particles.velocity = particles.next_velocity;
        particles.acceleration = particles.next_acceleration;        
    }

    void integrated_by_ex_euler() {}
    void integrated_by_im_euler() {}
    void integrated_by_rk2() {}

    void compute_applied_forces()
    {
        std::fill(particles.density.begin(), particles.density.end(), 0.0f);
        compute_density();
        compute_pressure();

        std::fill(particles.force.begin(), particles.force.end(), glm::vec3({0.0f, 0.0f, 0.0f}));
        compute_force_pressure();
        compute_force_diffusion();
        compute_force_gravity();
        compute_force_surface_tension();
    }

    void compute_neighborhood()
    {  
        for(int i = 0; i < k_num_particle; i++) { neighborhood.at(i).clear(); }

        glm::vec3 *pos = &particles.next_position[0];
        kdtree.Build(k_num_particle, pos);

        #pragma omp parallel for 
        for (int i = 0; i < k_num_particle; i++)
        {
            kdtree.GetPoints(i, particles.next_position.at(i), k_sph_s, compute_neighborhood_callback);
        }
    }

    static void compute_neighborhood_callback(unsigned int target_index, unsigned int index, glm::vec3 const &p, float distanceSquared, float &radiusSquared)
    {
        neighborhood.at(target_index).push_back(index);
    }

    void compute_density()
    {
        #pragma omp parallel for collapse(1)
        for (int i = 0; i < k_num_particle; i++)
        {
            for (unsigned int j : neighborhood.at(i))
            {
                particles.density.at(i) += k_particle_mass * sph_default_kernel(particles.next_position.at(i) - particles.next_position.at(j));
            }
        }
    }

    void compute_pressure()
    {
        #pragma omp parallel for 
        for (int i = 0; i < k_num_particle; i++)
        {
            particles.pressure.at(i) = k_fluid_stiffness * (particles.density.at(i) - k_fluid_property.density);
        }
    }

    void compute_force_pressure()
    {
        #pragma omp parallel for collapse(1)
        for (int i = 0; i < k_num_particle; i++)
        {
            glm::vec3 pressure_gradient = {0.0f, 0.0f, 0.0f};
            for (auto j : neighborhood.at(i))
            {
                if (i == j) continue;
                pressure_gradient += k_particle_mass 
                    * (float)((particles.density.at(i) / std::pow(particles.density.at(j), 2)) + (particles.density.at(j) / std::pow(particles.density.at(i), 2))) 
                    * sph_pressure_kernel_gradient(particles.next_position.at(i) - particles.next_position.at(j));
            }
            particles.force.at(i) -= 0.0002f * particles.density.at(i) * pressure_gradient;
        }
    }

    void compute_force_diffusion()
    {
        #pragma omp parallel for collapse(1)
        for (int i = 0; i < k_num_particle; i++)
        {
            glm::vec3 laplacian = {0.0f, 0.0f, 0.0f};
            for (auto j : neighborhood.at(i))
            {
                if (i == j) continue;
               
                laplacian += (velocity_field::electric_field(particles.next_position.at(j)) - velocity_field::electric_field(particles.next_position.at(i)))
                    * (k_particle_mass / particles.density.at(i))
                    * sph_diffusion_kernel_laplacian(particles.next_position.at(i) - particles.next_position.at(j));
            }

            particles.force.at(i) += 0.01f * k_fluid_property.dynamic * laplacian;
        }
    }

    void compute_force_gravity()
    {
        #pragma omp parallel for 
        for (int i = 0; i < k_num_particle; i++)
        {
            particles.force.at(i) += particles.density.at(i) * k_gravity_acceleration;
        }
    }

    void compute_force_surface_tension()
    {
        #pragma omp parallel for collapse(1)
        for (int i = 0; i < k_num_particle; i++)
        {
            glm::vec3 surface_normal = {0.0f, 0.0f, 0.0f};
            float laplacian = 0.0f;
            for (auto j : neighborhood.at(i)) 
            {
                if (i == j) continue;
                surface_normal += (k_particle_mass / particles.density.at(j)) * sph_default_kernel_gradient(particles.next_position.at(i) - particles.next_position.at(j)); 
                laplacian += (k_particle_mass / particles.density.at(j)) * sph_default_kernel_laplacian(particles.next_position.at(i) - particles.next_position.at(j));
            }
            surface_normal = glm::normalize(surface_normal);
            if(glm::length(surface_normal) > k_surface_tension_level_threshold)
            {
                particles.force.at(i) -= 0.01f * k_fluid_property.surface_tension * surface_normal * laplacian ;
            }
        }

    }

};

#endif // SOLVER_H_