#include <unistd.h> //Unix Standard 
#include <stdio.h> //Standard Input Output

#include "vsc_func.h"


int main (int argc, char **argv)
{
    // ...
    SVC_Init();

    while(true)
    {
        usleep(20000); //Sleep 20ms

        /**/
        // VSC_CheckTCPConnectStatus();

        // VSC_RecvUpLinkMsgQue();

        // VSC_ProcessTask();
    }

    return 0;
}