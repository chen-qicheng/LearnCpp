/*
 * =====================================================================================
 *    Filename:  log.c
 *
 *    Description:  Log Trace
 *
 *	  version   1.1
 *    Created:  2023/6/25
 *
 *    Author:  pujianlong pujl@sany.com.cn
 *    Organization:  sany
 * =====================================================================================
 */
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
//#include <sys/time.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#include "log_trace.h"

#define LOG_TIME_FORMAT_STRING
#define PRINT_LINE_MAX_LEN      (1024U) 
#define PRINT_FILE_SIZE			(5*1024*1024) //5MB = 5*1024*1024

/*------------------Static variable------------------------------*/

typedef struct
{
	LOG_LEVEL_e logLevel;
	LOG_LEVEL_e initLogLevel;
	LOG_TAG_e   logTag;
	FILE 		*logFile;
	int 		logFileSize; 
	pthread_mutex_t logMutex; 
}LOG_TRACE_CONTROL;

LOG_TRACE_CONTROL g_log_trace_ctrl = {0};

static void log_trace_sig_handler(int signo) 
{
	LOG_TRACE_CONTROL *pCtrl = &g_log_trace_ctrl;
	if (SIGUSR1 == signo)
	{
		int level = pCtrl->logLevel + 1;
		if(level > LOG_LV_MAX)
		{
			level = LOG_LV_DEFAULT;
		}
		pCtrl->logLevel = level;
		printf("change log level to :%d\n", level);
	}
	else if(SIGUSR2 == signo)
	{
		pCtrl->logLevel = pCtrl->initLogLevel;
		printf("change log level to :%d\n", pCtrl->initLogLevel);
	}
	else
	{
		printf("not support this signal\n");
	}
}

static char* log_trace_time(void)
{
    static char buf[64];
    
#ifdef LOG_TIME_FORMAT_STRING 
    time_t now;
    struct tm tmp;

    now = time(NULL);
    localtime_r(&now, &tmp);
    strftime(buf, sizeof(buf), "%H:%M:%S,%F", &tmp);
#else

    struct timeval tv;

    gettimeofday(&tv, NULL);
    sprintf(buf, "%d.%d", (int)tv.tv_sec, (int)tv.tv_usec);
#endif

    return buf; 
}

//开机备份一次
static void log_trace_file_back_up(char* old_file_name)
{
	FILE *old_file, *backup_file;
	char backup_file_name[64] = {0};
	sprintf(backup_file_name, "%s-back", old_file_name);
	old_file = fopen(old_file_name, "r");
	if(old_file == NULL) return;
	
	backup_file = fopen(backup_file_name, "w+");

	signed char c = 0;
	if(old_file && backup_file)
	{
		while((c = fgetc(old_file)) != EOF)
		{
			fputc(c, backup_file);
		}

		// 关闭两个文件
		fclose(old_file);
		fclose(backup_file);
	}
}


#if 1
/**********************externnal functions*************************/
#endif
int log_trace_init(LOG_TAG_e tag, LOG_LEVEL_e level, char* fileName, int fileSize)
{
	signal(SIGUSR1, log_trace_sig_handler); // kill -10 
	signal(SIGUSR2, log_trace_sig_handler); // kill -12

	FILE *pFileTmp = NULL;
	int fileSizeTmp = 0;
	LOG_TRACE_CONTROL *pCtrl = &g_log_trace_ctrl;
	if(fileName != NULL)
	{
		log_trace_file_back_up(fileName);
		pFileTmp = fopen(fileName, "w+");
	}
	if(NULL == pFileTmp) 
	{
		fileSizeTmp = 0;
	}
	else
	{
		fileSizeTmp = (fileSize > 0)?fileSize:PRINT_FILE_SIZE;
	}
	pCtrl->logLevel = level;
	pCtrl->initLogLevel = level;
	pCtrl->logTag = tag;
	pCtrl->logFile = pFileTmp;
	pCtrl->logFileSize = fileSizeTmp;
	pthread_mutex_init(&pCtrl->logMutex, NULL);

	return 0;
}

void log_trace_print(LOG_TAG_e tag, LOG_LEVEL_e level, char *pFmt, ...)
{
	if(NULL == pFmt) return;

	LOG_TRACE_CONTROL *pCtrl = &g_log_trace_ctrl;

    if (pCtrl->logTag & tag)
    {
        if (pCtrl->logLevel <= level)
        {
    	    char fmtBuf[PRINT_LINE_MAX_LEN] = "";
    	    va_list ap;

    	    pthread_mutex_lock(&pCtrl->logMutex);
            //sprintf(fmtBuf, "[%s]%s%s", LOG_TIME(), p_fmt, p_fmt[strlen(p_fmt)-1] == '\n' ? "" : "\n");
			if(LOG_LV_DEFAULT == level)
			{
				strcpy(fmtBuf, pFmt);
			}
			else
			{
				sprintf(fmtBuf, "[%s]%s", log_trace_time(), pFmt);
			}

    	    va_start(ap, pFmt);
			
			vprintf(fmtBuf, ap);
                
    	    if (NULL != pCtrl->logFile)
            {
            	int ret = 0;
				int curPos = ftell(pCtrl->logFile);
				if(curPos == -1) perror("ftell failed\n");
				if(curPos >= pCtrl->logFileSize - PRINT_LINE_MAX_LEN)
				{
					//printf("cur pos:%d\n", cur_pos);
					ret = fseek(pCtrl->logFile, 0, SEEK_SET);
				}
				if(ret != -1)
				{
					vfprintf(pCtrl->logFile, fmtBuf, ap);
				}
				else
				{
					perror("fseek error");
				}
				fflush(pCtrl->logFile);
    	    }
    	    va_end(ap);
    	    pthread_mutex_unlock(&pCtrl->logMutex);
        }
    }

    return;
}

#if 0
// example
int main()
{
	log_trace_init(LOG_TAG_ALL, LOG_LV_DEFAULT, "log.txt", 0);

	while(1)
	{
		LOG_TRACE(BASIC_INFO,	DEBUG, 	"DEBUG\n");
		LOG_TRACE(CMD_CTRL,		INFO, 	"INFO\n");
		LOG_TRACE(DATA_TRANS,	WARN, 	"WARN\n");
		LOG_TRACE(LOCK_CMD,		ERR, 	"ERR\n");
		LOG_TRACE(NEWC_PROC,	FATAL, 	"FATAL\n");

		sleep(1);
	}

	return 0;
}
#endif
