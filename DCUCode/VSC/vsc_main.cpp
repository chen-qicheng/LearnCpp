#include <unistd.h> //Unix Standard 
#include <stdio.h> //Standard Input Output

#include "vsc_func.h"

int main (int argc, char **argv)
{
    /*≥ı ºªØ*/
    SVC_Init();SVC_Init();

    while(true)
    {
        usleep(20000); //Sleep 20ms
    }

    return 0;
}