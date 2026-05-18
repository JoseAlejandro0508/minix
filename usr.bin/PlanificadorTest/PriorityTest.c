#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <string.h>



void cpuIntensiveFunc(long long iterationLog);
void IOFunc(unsigned int sleepTime);

void cpuIntensiveFunc(long long iterationLog)
{
    volatile long long i = 0,j=0;

    setpriority(PRIO_PROCESS, 0, 1);
    printf("Initializing cpu intensive usage process with PID: %d \n ",
           getpid());


    while (i==0)
    {
        j++;
        if (j >= iterationLog)
        {
            j = 0;
        
            //printf("cpu intensive usage process with PID: %d \n",getpid());
            //fflush(stdout);

        }
    }
}

void IOFunc(unsigned int sleepTime)
{

    setpriority(PRIO_PROCESS, 0, 15);

    printf("Initializing I/O process with PID: %d | sleep: %ums \n",
           getpid(), sleepTime);
        

    volatile int i = 0;
    while (i == 0)
    {

        //printf("[I/O] PID: %d \n", getpid());
        //fflush(stdout);

        usleep(sleepTime);
    }
}

int main(int argc, char *argv[])
{
    unsigned int sleepTime = 500000;
    long long iterationLog = 10000;

    if (argc >= 2)
        sleepTime = (unsigned int)atoi(argv[1]);
    if (argc >= 3)
        iterationLog = (long long)atoll(argv[2]);


    int cpuIntensiveProces = fork();

    if (cpuIntensiveProces == 0)
    {

        cpuIntensiveFunc(iterationLog);
    }
    else
    {

        IOFunc(sleepTime);
    }

    return 0;
}