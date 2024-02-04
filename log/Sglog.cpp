/**********************************************************************************************
*
*    FILENAME: log.cpp 
*
*    AUTHOR: tanghui
*
*    HISTORY: 
*            M001  -- 2009-7-16 13:33:50  tanghui, 1st version
*
*    DATE: 2009-7-16
*
*    DESCRIPTION:日志类
*
*    COPYRIGHT:
*               (c) Copyright |2009|, Wasion Technologies, Inc., all rights reserved.
*               No part of this source code may be duplicated or distributed in any form.
**********************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>

#include <unistd.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <syslog.h>
#include <assert.h>
#include <dirent.h>
#include "Sglog.hpp"
CSgLog g_objLog;

//默认输出对象映射
Logmap_t Logmap[] = {
    { NONE_L ,    TO_NULL},
    { DEBUG_L   , TO_FILE},
    { DEBUG_K   , TO_FILE},
    { WARN_L    , TO_FILE | TO_STDOUT},
    { ERROR_L   , TO_FILE | TO_STDOUT},
};

const int freshSize = 10;
pthread_mutex_t  ecclog_mux;

CSgLog::CSgLog():logmethod(0),
m_level(DEBUG_K),
bufcount(0)
{
	unsigned int Index = 0;
	bzero( buffer , sizeof( buffer ) );
	bzero( logfilename ,sizeof( logfilename ) );
	sprintf(logfilename,"%s%s",DEBUG_LOG_PATH,DEBUG_LOG_FILE_PROFIX);
	for( Index = 0; Index < sizeof( Logmap )/sizeof( Logmap_t ); Index++ )
	{
		if( m_level == Logmap[Index].level )
		{
			logmethod = Logmap[Index].direc;
		}
	}
	tarnum = getCurrentPosition() % MAX_TGZ_NUM;
}

CSgLog::CSgLog( LogLevel_t level , const char *filename )
{
	unsigned int Index = 0;
	bzero( buffer , sizeof( buffer ) );
	logmethod = 0;
	bufcount = 0;
	m_level = level;
	pthread_mutex_init(&ecclog_mux,NULL);
	if( NULL != filename )
	{
		bzero( logfilename ,sizeof( logfilename ) );
		memcpy( logfilename , filename , strlen( filename ) );
	}
	for( Index = 0; Index < sizeof( Logmap )/sizeof( Logmap_t ); Index++ )
	{
		if( m_level  == Logmap[Index].level )
		{
			logmethod = Logmap[Index].direc;
		}
	}
	tarnum = getCurrentPosition() % MAX_TGZ_NUM;
}

CSgLog::~CSgLog()
{
    fflush();
}

int CSgLog::Log2null(const char *fmt , va_list ap )
{
    FILE *fnull = fopen( "/dev/null" , "w" );
    if(!fnull)
        return -1;
    vfprintf( fnull, fmt , ap );
    fclose( fnull );
    return 0;
}

int CSgLog::Log2file(const char *fmt, va_list ap )
{
	//char tgzfname[200]={ 0};
	char tarrmcommand[200] = {0};
	char timebuf[200] = {0};
	if(logfilename[0] == 0 )
    {
        return 0;
    }
	pthread_mutex_lock(&ecclog_mux);
    struct stat   filestatus;
    if( stat( logfilename , &filestatus ) == 0 )
    {
        if( filestatus.st_size > MAX_LOGFILE_LEN )	        //当文件长度超过制定长度时，将文件压缩后删除
        {
            sprintf( tarrmcommand, "rm -f %s &",logfilename);
            system( tarrmcommand );
        }
    }

    if( bufcount < freshSize ) // 将freshSize改小有利于快速刷新log到文件中
    {
        int tmplen = GetFormattime( timebuf );
        memcpy( buffer + bufcount , timebuf , strlen( timebuf ) );
        bufcount += tmplen;
        int headlen = GetHeader( m_level , buffer + bufcount );
        bufcount += headlen;

        bufcount += vsprintf((char *)(buffer + bufcount), fmt, ap);
    }
    else
    {
        FILE *flog;
        if((flog = fopen(logfilename, "a+")) == NULL)
        {
            bufcount    =   0;
			pthread_mutex_unlock(&ecclog_mux);
            return -1;
        }
        fwrite( buffer, bufcount , 1, flog );
        fclose( flog );
        bufcount    =   0;
        bzero( buffer , sizeof(buffer) );

        char timebuf[200] = {0};
        bufcount += GetFormattime( timebuf );
        memcpy( buffer + bufcount , timebuf , strlen( timebuf ) );
        bufcount += strlen( timebuf );

        int headlen1 = GetHeader( m_level , buffer + bufcount );
        bufcount += headlen1;
        bufcount += vsprintf((char *)(buffer + bufcount), fmt, ap);
    }
	pthread_mutex_unlock(&ecclog_mux);
    return 0;

}

int CSgLog::fflush()
{
    struct stat filestatus;
    if( stat( logfilename , &filestatus ) == 0 )
    {
        if( filestatus.st_size > MAX_LOGFILE_LEN )
        {
            char tgzfname[200]={ 0};
            char tarrmcommand[200] = { 0};
			tarnum = getCurrentPosition() % MAX_TGZ_NUM;
            sprintf( tgzfname , "%s_%02d.tgz", logfilename , tarnum);
            sprintf( tarrmcommand, "tar czf %s %s &; rm %s &", tgzfname, logfilename, logfilename);
            system( tarrmcommand );
        }
    }


    FILE *flog;
    if((flog = fopen(logfilename, "a+")) == NULL)
    {
        bufcount    =   0;
        return -1;
    }
    fwrite( buffer, bufcount , 1, flog );
    bzero( buffer , sizeof(buffer) );
    fclose( flog );
    bufcount    =   0;
    return 0;
}

int CSgLog::Log2stdout(const char *fmt, va_list ap )
{
	char timebuf[200] = {0};
	GetFormattime( timebuf );
	char headerbuf[100]={0};
	GetHeader( m_level , headerbuf );
	printf( "%s%s" , timebuf , headerbuf );
	vprintf( fmt , ap );
	return 0;
}


void CSgLog::Log2(LogLevel_t logLevel,const char *fmt, ... )
{
    if( m_level > logLevel ) return;
    if(logLevel>10)
    {
        printf("%s,%d,logLevel = %d\n",__FILE__,__LINE__,logLevel);
        assert(0==true);
    }

    va_list ap;
    va_start( ap , fmt );

    typedef int ( CSgLog:: *LogFun_t )(const char *fmtt, va_list app);
    static LogFun_t LogFuncs[] =
    {
        &CSgLog::Log2null,
        &CSgLog::Log2file,
        &CSgLog::Log2stdout,
    };

    LogLevel_t savcurlevel = m_level;
    int savelogmethod = logmethod;
    m_level = logLevel;

    if( m_level != savcurlevel )
    {
        for( unsigned int i = 0 ; i < sizeof( Logmap )/sizeof( Logmap_t ) ; i++ )
        {
            if( m_level == Logmap[i].level )
            {
                logmethod = Logmap[i].direc;
            }
        }
    }

    for( int j = 0 ; j < MAX_LOGFUNC_TYPES ; j++ )
    {
        if( ( logmethod & ( 1 << j ) ) == 0 )
        {
            continue;
        }
        ( this->*LogFuncs[j] )( fmt , ap );
    }
    va_end( ap );
    m_level = savcurlevel;
    logmethod = savelogmethod;
}

void CSgLog::Log2Buf(LogLevel_t logLevel,char *msg, char *pinbuff,int buflen)
{
	int i = 0;
    char logbuf[2048]={0};
	char timebuf[64] = {0};
	GetFormattime( timebuf );
	if(msg != NULL)
	{
		sprintf(logbuf,"%s,%s,Logbuf length=%d: ",timebuf,msg,buflen);
	}
	else
	{
    	sprintf(logbuf, "Logbuf length=%d: ", buflen);
	}
	if (buflen > 100)
    {
        buflen = 100;
    }
    int loglen  =   strlen(logbuf);
    for ( i = 0; i < buflen; ++i)
    {
        sprintf(logbuf + loglen + 3 * i, "%02x ", pinbuff[i]);
    }
    sprintf(logbuf + loglen + 3 * buflen, "\b\n");
    Log2(logLevel,"%s",logbuf);
}


int CSgLog::GetFormattime( char * formattime )
{
	if( NULL == formattime )
	{
		return 0;
	}
	time_t timer;	
	struct tm ptime;
	memset(&ptime,0,sizeof(ptime));
	time(&timer);	
	ptime = *localtime(&timer);
	sprintf( formattime , "[%2d-%02d-%02d %02d:%02d:%02d]" , 1900 + ptime.tm_year , ptime.tm_mon + 1 ,
	ptime .tm_mday , ptime.tm_hour , ptime .tm_min , ptime .tm_sec );
	return strlen( formattime );
}


int CSgLog::GetHeader( LogLevel_t level , char *buf )
{
    switch( level )
    {
	    case DEBUG_K:
	    case DEBUG_L:
	        sprintf( buf , "%s" , DEBUG_HERDER );
	        return strlen( DEBUG_HERDER );
	        break;
	    case WARN_L:
	        sprintf( buf , "%s" , WARN_HERDER );
	        return strlen( WARN_HERDER );
	        break;
	    case ERROR_L:
	        sprintf( buf , "%s" , ERROR_HERDER );
	        return strlen( ERROR_HERDER );
	        break;
	    case NONE_L: default:
	        return 0;
	        break;
    }
}

CSgLog& CSgLog::GetInstance(LogLevel_t level , const char *filename)
{
    static CSgLog _log(level, filename);
    return _log;
}


int CSgLog::getCurrentPosition(void)
{
	char cmd_buf[128] = {0};
    DIR * dir=NULL;
    char tmpfilename[128] = {0};
	char buffer_tmp[128] = {0};
    unsigned char filenamelen = 0;
    dir =opendir(DEBUG_LOG_PATH);
    if( NULL==dir )
    {
        system("mkdir /userdata/Log");
        sprintf(cmd_buf,"mkdir %s",DEBUG_LOG_PATH);
		system(cmd_buf);
		sleep(5);
		dir =opendir(DEBUG_LOG_PATH);
    }
    struct dirent * ptr=NULL;
 
    filenamelen = strlen(logfilename) - strlen(DEBUG_LOG_PATH);
    memcpy(tmpfilename, logfilename+strlen(DEBUG_LOG_PATH), filenamelen);
    strcat(tmpfilename, "_");
 
    struct stat filestatus;
    SGLOG_TGZ_FILE_INFO zipFileInfo[MAX_TGZ_NUM];
    //printf("sizeof(zipFileInfo)=%d\n", sizeof(zipFileInfo));
    memset(zipFileInfo, 0, sizeof(zipFileInfo));
    
    while((ptr = readdir(dir))!=NULL)
    {
        if(strstr(ptr->d_name,tmpfilename)>0)
        {
            char numchar[10];/*这里假设文件的个数最大不超过1000000000个*/
            memset(numchar,0,sizeof(numchar));
            strncpy(numchar,strchr(ptr->d_name,'_')+1,strlen(ptr->d_name)-strlen(".tgz")-strlen(tmpfilename));
            int temp=atoi(numchar);
	    if(!((temp >=0) && (temp <MAX_TGZ_NUM)))
	    {
		continue;
            }
            zipFileInfo[temp].tgzNoExist = 1;
			memset(buffer_tmp,0,sizeof(buffer_tmp));
			sprintf(buffer_tmp,"%s%s",DEBUG_LOG_PATH,ptr->d_name);
            if (stat(buffer_tmp, &filestatus)==0)
            {
                zipFileInfo[temp].modTime = filestatus.st_mtime;
            }
        }
    }
    closedir(dir); 

    time_t OldestTime = 0x7fffffff;
    //printf("OldestTime=%ld\n", (long)OldestTime);
    int OldestPosition = 0;
    for (int i = 0; i < MAX_TGZ_NUM; i++)
    {
        /* 有空位优先用第一个空位 */
        if (0 == zipFileInfo[i].tgzNoExist)
        {            
            return i;
        }
        if (zipFileInfo[i].modTime < OldestTime)
        {
            OldestTime = zipFileInfo[i].modTime;
            OldestPosition = i;
        }
    }
    return OldestPosition;
}


