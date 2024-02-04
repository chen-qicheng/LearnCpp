#include <unistd.h> //Unix Standard 
#include <stdio.h> //Standard Input Output
#include <glog/logging.h>

#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"


int main (int argc, char **argv)
{
    google::InitGoogleLogging(argv[0]);//ʹ��glog֮ǰ�����ȳ�ʼ���⣬����ִ��һ�Σ�������Ϊ������
    FLAGS_alsologtostderr = true;     //�Ƿ���־������ļ���stderr
    FLAGS_colorlogtostderr = true;    //�Ƿ����ò�ͬ��ɫ��ʾ
    FLAGS_log_dir = "./logs"; //
    // LOG(INFO) << "info";
    // LOG(WARNING) << "warning";
    // LOG(ERROR) << "error";
    /*LOG(FATAL) << "fatal";*/

    int fd1 = open("./test", O_RDWR|O_CREAT|O_EXCL, 0666);
    if(fd1 == -1)
    {
        printf("open ./test fail!\n");
        LOG(ERROR) << "open ./test fail!";
        return fd1;
    }

    google::ShutdownGoogleLogging();  //��Ҫ����glogʱ����رտ⣬������ڴ����
    return 0;
}