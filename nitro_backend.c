
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include "nitro_backend.h"

#define EC_PATH "/sys/kernel/debug/ec/ec0/io"
#define DEFAULT_TEMP_PATH "/sys/class/thermal/thermal_zone0/temp"


#define REG_READ_CPU 0x13
#define REG_READ_GPU 0x15
#define REG_MODE_CPU 0x22
#define REG_MODE_GPU 0x21
#define REG_WRITE_CPU 0x37
#define REG_WRITE_GPU 0x3A
#define REG_COOLBOOST 0x10 

int ec_fd = -1;
char dynamic_temp_path[256]; 
static unsigned long long prev_user = 0, prev_user_low = 0, prev_sys = 0, prev_idle = 0;


void run_cmd(const char* cmd) {
    if(system(cmd)) {} 
}

void set_power_profile(int mode) {
    if (mode == 0) run_cmd("powerprofilesctl set power-saver");
    else if (mode == 1) run_cmd("powerprofilesctl set balanced");
    else if (mode == 2) run_cmd("powerprofilesctl set performance");
}

int get_current_profile() {
    FILE *fp = popen("powerprofilesctl get", "r");
    if (fp == NULL) return 1; 
    char buffer[64];
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        pclose(fp);
        if (strstr(buffer, "power-saver")) return 0;
        if (strstr(buffer, "performance")) return 2;
        return 1; 
    }
    pclose(fp);
    return 1;
}

void find_best_cpu_sensor() {
    strcpy(dynamic_temp_path, DEFAULT_TEMP_PATH);
    char type_path[256], type_content[64];
    for (int i = 0; i < 10; i++) {
        sprintf(type_path, "/sys/class/thermal/thermal_zone%d/type", i);
        FILE *f = fopen(type_path, "r");
        if (f) {
            if (fscanf(f, "%63s", type_content) == 1) {
                if (strstr(type_content, "x86_pkg_temp") != NULL) {
                    sprintf(dynamic_temp_path, "/sys/class/thermal/thermal_zone%d/temp", i);
                    fclose(f); return; 
                }
            }
            fclose(f);
        }
    }
}

int init_backend() {
    ec_fd = open(EC_PATH, O_RDWR);
    find_best_cpu_sensor();
    if (ec_fd < 0) return 0;
    return 1;
}

double get_cpu_temp() {
    FILE *f = fopen(dynamic_temp_path, "r");
    if (!f) return 0.0;
    long raw; double res = 0.0;
    if (fscanf(f, "%ld", &raw) == 1) res = raw / 1000.0;
    fclose(f); return res;
}

double get_cpu_usage() {
    FILE* file = fopen("/proc/stat", "r");
    if (!file) return 0.0;
    unsigned long long user, user_low, sys, idle;
    if (fscanf(file, "cpu %llu %llu %llu %llu", &user, &user_low, &sys, &idle) < 4) { fclose(file); return 0.0; }
    fclose(file);
    if (prev_user == 0 && prev_idle == 0) { prev_user = user; prev_user_low = user_low; prev_sys = sys; prev_idle = idle; return 0.0; }
    unsigned long long total_diff = (user - prev_user) + (user_low - prev_user_low) + (sys - prev_sys) + (idle - prev_idle);
    unsigned long long idle_diff = idle - prev_idle;
    prev_user = user; prev_user_low = user_low; prev_sys = sys; prev_idle = idle;
    if (total_diff == 0) return 0.0;
    return (double)(total_diff - idle_diff) * 100.0 / total_diff;
}

void get_gpu_stats(double *temp, int *usage) {
    *temp = 0.0; *usage = 0;
    FILE *fp = popen("nvidia-smi --query-gpu=temperature.gpu,utilization.gpu --format=csv,noheader,nounits", "r");
    if (fp == NULL) return;
    int t = 0, u = 0;
    if (fscanf(fp, "%d, %d", &t, &u) == 2) { *temp = (double)t; *usage = u; }
    pclose(fp);
}

// Wrapper per lseek/read
void ec_read_bytes(int offset, uint8_t *val) {
    if (lseek(ec_fd, offset, SEEK_SET) >= 0) {
        if(read(ec_fd, val, 1)) {}; 
    }
}

int read_ec_rpm(int start_reg) {
    if (ec_fd < 0) return -1;
    uint8_t low = 0, high = 0;
    ec_read_bytes(start_reg, &low);
    ec_read_bytes(start_reg + 1, &high);
    return (high << 8) | low;
}
int get_fan_rpm(int fan_index) {
    if (fan_index == 0) return read_ec_rpm(REG_READ_CPU);
    else if (fan_index == 1) return read_ec_rpm(REG_READ_GPU);
    return 0;
}
void write_ec(int reg, int value) {
    if (ec_fd < 0) return;
    uint8_t val = (uint8_t)value;
    if (lseek(ec_fd, reg, SEEK_SET) >= 0) {
        if(write(ec_fd, &val, 1)) {};
    }
}
void enable_manual_mode() { write_ec(REG_MODE_CPU, 12); write_ec(REG_MODE_GPU, 48); }

void set_fan_mode(int mode) {
    if (ec_fd < 0) return;
    if (mode == 1) { enable_manual_mode(); write_ec(REG_WRITE_CPU, 100); write_ec(REG_WRITE_GPU, 100); } 
    else { write_ec(REG_COOLBOOST, 0); write_ec(REG_WRITE_CPU, 50); write_ec(REG_WRITE_GPU, 50); usleep(20000); write_ec(REG_MODE_CPU, 4); write_ec(REG_MODE_GPU, 16); }
}
void set_fixed_speed(int percent) {
    if (ec_fd < 0) return;
    if (percent < 0) percent = 0; 
    if (percent > 100) percent = 100; 
    int final_value = (percent == 0) ? 0 : 50 + (percent / 2);
    enable_manual_mode(); write_ec(REG_WRITE_CPU, final_value); write_ec(REG_WRITE_GPU, final_value);
}
void close_backend() { if (ec_fd >= 0) close(ec_fd); }