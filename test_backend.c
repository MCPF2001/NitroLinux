
#include <stdio.h>
#include <unistd.h>
#include "nitro_backend.h"

int main() {
    printf("--- Test Backend Acer Nitro v2 ---\n");
    
    init_backend(); 

    for(int i = 0; i < 5; i++) {
        double temp = get_cpu_temp();
        int fan_cpu = get_fan_rpm(0);
        int fan_gpu = get_fan_rpm(1);

        printf("CPU Temp: %.1f°C | Fan CPU: %d RPM | Fan GPU: %d RPM\n", 
               temp, fan_cpu, fan_gpu);
        
        sleep(1);
    }

    close_backend();
    return 0;
}