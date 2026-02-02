
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>


#define EC_PATH "/sys/kernel/debug/ec/ec0/io"
#define EC_SIZE 256

int main() {
    
    int fd = open(EC_PATH, O_RDONLY);
    if (fd < 0) {
        perror("ERRORE: Impossibile aprire l'EC. Assicurati di usare SUDO e che il modulo ec_sys sia caricato.");
        return 1;
    }

    uint8_t buffer[EC_SIZE];
    
    
    if (read(fd, buffer, EC_SIZE) != EC_SIZE) {
        perror("Errore durante la lettura");
        close(fd);
        return 1;
    }
    close(fd);

    
    printf("\n--- Mappa Memoria EC (Acer Nitro) ---\n");
    printf("     00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F\n");
    printf("-------------------------------------------------------------------\n");

    for (int i = 0; i < EC_SIZE; i += 16) {
        printf("%02X | ", i); 
        for (int j = 0; j < 16; j++) {
            uint8_t val = buffer[i+j];
            if (val == 0) 
                printf(" .  ");
            else 
                printf("%02X  ", val);
        }
        printf("\n");
    }
    printf("-------------------------------------------------------------------\n");

    return 0;
}