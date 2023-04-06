#ifndef TIMER_H_
#define TIMER_H_

#include "common.hpp"

class Timer
{
  private:
    float simulation_time_;
    float timestep_;
    float refresh_interval_;
    float next_display_time_;

  public:
    Timer() {};
    ~Timer() {};

    void reset(float refresh_interval) {
        timestep_          = k_time_step;
        simulation_time_   = 0.0f;
        refresh_interval_ = refresh_interval;
        next_display_time_ = 0.0f + refresh_interval_;
    }

    float get_simluation_time() {
        return simulation_time_;
    }

    void update_simulation_time() {
        simulation_time_ += k_time_step;
    }

    void update_next_display_time() {
        next_display_time_ += refresh_interval_;
    }
    
    bool is_time_to_draw() {
        return (simulation_time_ >= next_display_time_);
    }

    bool is_time_to_stop() {
        return (simulation_time_ >= k_max_display_time);
    }
};


#endif // TIMER_H_
