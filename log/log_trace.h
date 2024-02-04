/*
 * =====================================================================================
 *       Filename:  log.h
 *
 *    Description:  Log
 *		文件采用回滚方式，文件写满后，从第一行开始重新写入
 *		文件有默认大小5MB = 5*1024*1024
 *		单行最大输出1024个字节
 *		支持kill -10切换level等级，没法一次信号增加1
 *		支持kill -12，恢复默认等级，一步到位
 *
 *        Version:  1.0
 *        Created:  Monday, November 20, 2017 12:42:34 HKT
 *
 *         Author:  Leo Wong (), leomcwong@astri.org
 *   Organization:  ASTRI
 * =====================================================================================
 */
#ifndef _LOG_TRACE_H_
#define _LOG_TRACE_H_

#define _LOG_TRACE_ENABLE

#define VERSION_STR "1.0.1"

/*--------------------Type define--------------------*/
typedef enum
{
    LOG_LV_DEFAULT  = 0,
    DEBUG    = 1,
    INFO     = 2,
    WARN     = 3,
    ERR    	 = 4,
    FATAL    = 5,
    LOG_LV_MAX  = 6,
}LOG_LEVEL_e;

typedef enum
{
    LOG_TAG_NONE    = 0x0000,
	LOG_APP			= 0x0001,
    BASIC_INFO	    = 0x0002,
    CMD_CTRL	    = 0x0004,
    DATA_TRANS	    = 0x0008,
    LOCK_CMD	    = 0x0010,
    NEWC_PROC	   	= 0x0020,
    SDK_COMM	   	= 0x0040,
    T4HJ		    = 0x0080,
    UPD_OTA			= 0x0100,
	WEATER_RRT		= 0x0200,
	WIND_SHARE	    = 0x0400,
	JSON_PROC		= 0x0800,
	FILTER_TABLE	= 0x1000,
	NOTICE			= 0x2000,
	CONFIG			= 0x4000,
    LOG_TAG_ALL	    = 0xFFFF,
}LOG_TAG_e;


//============api====================================
/*
* function: log_trace_init
* description: 初始化log跟踪程序
*	支持不输入文件名fileName,则log不保存到文件
*	自动备份：程序重启后，自动备份文件到"fileName"-back中
*	fileSize = 0，并且fileName！=NULL时，默认大小5MB
*	文件采用回滚方式，文件写满后，从第一行开始重新写入
* input:
*	tag: module name
*	levle: log level
*	pFile: file name, if don't need save log to file, please use NULL
*	fileSize: max log file size，supports input fileSize=0, the program will use 5MB instead
* output:
* 	not useful
*/
int  log_trace_init(LOG_TAG_e tag, LOG_LEVEL_e level, char* fileName, int fileSize);
void log_trace_print(LOG_TAG_e tag, LOG_LEVEL_e level, char *pFmt, ...);


#ifdef _LOG_TRACE_ENABLE
//#define LOG_TRACE(tag, level, fmt, ...)  LOG_TRACE_PRINT(tag, level, "["#level"][%s:%s:%d] "fmt"", __FILENAME__, __func__, __LINE__, __VA_ARGS__)
#define LOG_TRACE(tag, level, fmt, args...)  log_trace_print(tag, level, "["#tag"]["#level"][%s:%d] "fmt"", __FILE__, __LINE__, ##args)
// 单纯打印，为了输出can报文等消息，不需要添加头的时候使用
#define LOG_PRINT(fmt, args...)  log_trace_print(LOG_TAG_ALL, LOG_LV_DEFAULT, fmt , ##args)
#else
#define LOG_TRACE(level, fmt, ...)  
#define LOG_PRINT(fmt, ...)
#endif

#endif


