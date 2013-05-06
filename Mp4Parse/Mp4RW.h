#ifndef _MP4_RW_
#define _MP4_RW_

#include "Utls.h"
#include "Single.h"
#include "Mp4Box.h"
 
enum FILE_OPEN_MODEL
{
	OPEN_MODEL_WB,	
	OPEN_MODEL_RB // r+b
};

class Mp4FileRW
{
public:
	Mp4FileRW(void);
	~Mp4FileRW(void);

public:

	int GetMovieLen();
public:
	int Open(char *pFileName, int nRead);
	void Close();
	void Read();
	void Write();

	int AddAudioStream (int nFormat, ...);
	int AddVideoStream (int nFormat, ...);
	
	int AddCustomData (int nType, char *pData, int nLen);
	int GetCustomData (int nType, char *pData);

	int Write (int nStreamId, char *pData, int nLen, uint32_t nSampleTime, uint32_t nFlag); 

	// 视频流时，nFlag &0x1 为1，表示关键帧，用于创建索引
	int Read (int *pStreamId, char *pData, int nLen, uint32_t *pSampleTime, uint32_t *pFlag); 

protected:
	int ParseHeaders();
	int WriteFileHead();
	
protected:
	char m_chFileName[128];
	FILE * m_pFile;
	long m_nFilePos;


	CMp4Ftyp m_FileType;
	CMp4Moov m_Movie;
	CMp4Mdat* m_pMediaDataList[10];
	CMp4Mdat m_boxMdat;
};


#endif