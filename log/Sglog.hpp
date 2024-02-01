#ifndef _SG__LOG_H
#define _SG__LOG_H
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#define MAX_FNAME_LEN 100
#define MAX_LOGFUNC_TYPES 3
#define LOG_BUF_SIZE 4096
#define MAX_LOGFILE_LEN 1024*1024*50  /* 要减小日志当前文件大小，否则会出现日志区写满的情况 */

#define DEBUG_HERDER "Info:"
#define WARN_HERDER "Waning:"
#define ERROR_HERDER "Error:"
#define FATAL_ERROR_HERDER "Fatal_error:"

#define MAX_TGZ_NUM 1
#define DEBUG_LOG_PATH "/userdata/Log/uds_srv/"
#define DEBUG_LOG_FILE_PROFIX "Udssrv.log"

//log级别
typedef enum
{
	NONE_L=0,
	DEBUG_L,
	DEBUG_K,
	WARN_L,
	ERROR_L,
}LogLevel_t;

typedef enum
{
	TO_NULL = 0x01,
	TO_FILE = 0x02,
	TO_STDOUT = 0x04,
}LogDirec_t;

typedef struct
{
	LogLevel_t level;
	int direc;
}Logmap_t;

typedef struct 
{
    int tgzNoExist;  /* 压缩文件是否存在 */
    time_t modTime; /* 压缩文件修改时间 */
}SGLOG_TGZ_FILE_INFO;

class CSgLog
{
public:
	void Log2( LogLevel_t logLevel ,const char *fmt , ... );
	void Log2Buf(LogLevel_t logLevel,char *msg, char *pinbuff,int buflen);
	int fflush(); //将缓冲区的内容立即写入到文件中
	static CSgLog& GetInstance( LogLevel_t level , const char *filename = NULL );
	CSgLog( LogLevel_t level , const char *filename = NULL );
	CSgLog();
	~CSgLog();
	void SetSyslogname(const char *filename = NULL); 
private:
	int Log2null(const char *fmt , va_list ap );
	int Log2file (const char *fmt, va_list ap );
	int Log2stdout(const char *fmt, va_list ap );
	int Log2syslog(const char *fmt, va_list ap );
	int GetHeader( LogLevel_t level , char *buf );
	int FolderClean(void);
	int getCurrentPosition(void);
protected:
	int logmethod;
	char logfilename[MAX_FNAME_LEN];
	LogLevel_t m_level;
	char buffer[LOG_BUF_SIZE];
	int tarnum;
	int bufcount;
	int GetFormattime( char *formattime ); //获得时标
};
extern CSgLog g_objLog;
#define	LOG g_objLog.Log2
#define LogBuf g_objLog.Log2Buf
#endif

s