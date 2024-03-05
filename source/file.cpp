#include <unistd.h> //Unix Standard 
#include <stdio.h> //Standard Input Output

#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"


int open_file(void)
{
    char buff1[1024];
    char buff2[1024];
    int ret;

    int fd1 = open("./test", O_RDWR|O_TRUNC);
    if(fd1 == -1)
    {
        printf("open ./test fail!\n");
        return fd1;
    }
    ret = lseek(fd1, 0, SEEK_END);

    sprintf(buff1,"try to write the file");
    ret = write(fd1, buff1,sizeof(buff1));

    ret = lseek(fd1, 0, SEEK_SET);
    ret = read(fd1, buff2, sizeof(buff2));
    printf("read:%s count:%d\n", buff2, ret);

    ret = close(fd1);
    return 0;
}
