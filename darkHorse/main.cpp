#include <unistd.h> //Unix Standard 
#include <stdio.h> //Standard Input Output
#include <glog/logging.h>

#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"


int main (int argc, char **argv)
{
    google::InitGoogleLogging(argv[0]);//使用glog之前必须先初始化库，仅需执行一次，括号内为程序名
    FLAGS_alsologtostderr = true;     //是否将日志输出到文件和stderr
    FLAGS_colorlogtostderr = true;    //是否启用不同颜色显示
    FLAGS_log_dir = "../logs"; //
    // LOG(INFO) << "info";
    // LOG(WARNING) << "warning";
    // LOG(ERROR) << "error";
    /*LOG(FATAL) << "fatal";*/


    char buff1[1024];
    char buff2[1024];
    int ret;

    int fd1 = open("./test", O_RDWR|O_TRUNC);
    if(fd1 == -1)
    {
        printf("open ./test fail!\n");
        LOG(ERROR) << "open ./test fail!";
        return fd1;
    }
    ret = lseek(fd1, 0, SEEK_END);

    sprintf(buff1,"try to write the file");
    ret = write(fd1, buff1,sizeof(buff1));

    ret = lseek(fd1, 0, SEEK_SET);
    ret = read(fd1, buff2, sizeof(buff2));
    printf("read:%s count:%d\n", buff2, ret);
    LOG(INFO) << buff2;

    ret = close(fd1);

    google::ShutdownGoogleLogging();  //当要结束glog时必须关闭库，否则会内存溢出
    return 0;
}