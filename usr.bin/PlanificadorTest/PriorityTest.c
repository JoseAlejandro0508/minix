#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void cpuIntensiveFunc(void);
void IOFunc(void);

void cpuIntensiveFunc(void) {
    volatile long long  i = 0;
    printf("Initializing cpu intensive usage process with PID: %d \n",getpid());
    while (i < 100000000000) {
        i++;
        if (i == 100000000000) {
            i = 0;
            printf("cpu intensive usage process  PID: %d \n",getpid());
            
        }
    }
}

void IOFunc(void) {
    int i = 0;
    printf("Initializing I/O process with PID: %d \n",getpid());

   
    while (i==0) {

       
        printf("Simulating Output ...  %d  \n",getpid());

    }
}

int main(int argc, char *argv[]) {
    int cpuIntensiveProces;
    cpuIntensiveProces = fork();
    
    if (cpuIntensiveProces == 0) {
        cpuIntensiveFunc();
    } else {
        IOFunc();
    }
    
    return 0;
}