 
#ifndef __PORTALBE_H__
#define __PORTALBE_H__

#include "stdio.h"
#include <stdlib.h>
#include <string>
#include <assert.h>

typedef unsigned char BYTE;
typedef unsigned char* LPBYTE;
typedef char    int8;
typedef short   int16;
typedef int     int32;
#ifndef WIN32
typedef long long   int64;
#else
typedef long        int64;
#endif

typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned int    uint32;
#ifndef WIN32
typedef unsigned long long   uint64;
#else
typedef unsigned long        uint64;
#endif

typedef signed char     int8_t;
typedef signed short    int16_t;
typedef signed int      int32_t;
#ifndef WIN32
typedef long long       int64_t;
#else
typedef long            int64_t;
#endif

typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;
#ifndef WIN32
typedef unsigned long long   uint64_t;
#else
typedef unsigned long        uint64_t;
#endif

#ifdef __cplusplus

#else
typedef uint8 bool;
#endif 

#ifndef true
	#define true  1
#endif 

#ifndef false
	#define false 0
#endif 

#ifndef NULL
#ifdef  __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif 


//////////////////////////////////////////////////////////////////////////
#ifndef DWORD
#define DWORD unsigned long
#endif

#ifndef WORD
#define WORD unsigned short
#endif


//////////////////////////////////////////////////////////////////////////
template<class T> 
inline T ConvertEndian(T  t) 
{ 
	T tResult = 0; 
	for (int i = 0; i < sizeof(T); ++ i) 
	{ 
		tResult <<= 8; 
		tResult |= (t & 0xFF) ; 

		t >>= 8; 
	} 

	return  tResult; 
} 

//*********************************************
template <class T>
inline void GetBigEndian(const BYTE*  pBuf,T &tResult) 
{ 
	tResult = 0; 
	for (int  I = 0; I < sizeof(T); ++ I) 
	{ 
		tResult <<= 8; 
		tResult |= *pBuf ++; 
	} 

	return  tResult; 
} 


//*********************************************
template <class T>
inline void GetLittleEndian(const BYTE*  pBuf,T &tResult) 
{ 
	tResult = 0; 
	pBuf += sizeof(T) - 1; 
	for (int  I = 0; I < sizeof(T); ++ I) 
	{ 
		tResult <<= 8; 
		tResult |= *pBuf --; 
	} 

	return  tResult; 
} 

//****
template <class T>
inline void SetBigEndian(BYTE*  pBuf, T  t) 
{ 
	pBuf += sizeof(T) - 1; 
	for (int  I = 0; I < sizeof(T); ++ I) 
	{ 
		*pBuf -- = BYTE(t & 0xFF); 
		t >>= 8; 
	} 
}  

//*********************************************
template <class T>
inline void SetLittleEndian(BYTE*  pBuf, T  t) 
{ 
	for (int  I = 0; I < sizeof(T); ++ I) 
	{ 
		*pBuf ++ = BYTE(t & 0xFF); 
		t >>= 8; 
	} 
}

#endif //__PORTALBE_H__


/*
*	 
MP4文件接口，需要与操作系统无关，并至少提供如下接口：
1. int Open (char *pFileName, int nMode); 
打开文件，指定是读还是写文件，写文件时，不能与本对象去读。
2. int AddAudioStream (int nFormat, ...);
添加指定格式的音频流，及相关参数。
返回流对应的序号。
3. int AddVideoStream (int nFormat, ...);
添加指定格式的视频流，及相关参数。只支持.264及FMPEG4。 
返回流对应的序号。
4. int AddCustomData (int nType, char *pData, int nLen);
int GetCustomData (int nType, char *pData);
定置数据，每个nType只能存储一个pData.

5.
int Write (int nStreamId, char *pData, int nLen, unsigned long nSampleTime, unsigned long nFlag); 
// 视频流时，nFlag &0x1 为1，表示关键帧，用于创建索引
int Read (int *pStreamId, char *pData, int nLen, unsigned long *pSampleTime， unsigned long *pFlag); 
读写码流数据。
6. void Close();

 */