
#ifndef NITRO_BACKEND_H
#define NITRO_BACKEND_H

#ifdef __cplusplus
extern "C" {
#endif

    int init_backend();
    void close_backend();

    
    double get_cpu_temp();
    double get_cpu_usage();
    void get_gpu_stats(double *temp, int *usage);
    int get_fan_rpm(int fan_index);

    /
    void set_fan_mode(int mode); 
    void set_fixed_speed(int percent); 

    
    void set_power_profile(int mode);
    int get_current_profile();

#ifdef __cplusplus
}
#endif
#endif