
#include "Mp4RW.h"
#include "stdio.h"


using namespace std;
Mp4FileRW::Mp4FileRW(void)
{
	m_pFile = NULL;
	memset (m_chFileName, 0, sizeof (m_chFileName));
	memset (m_pMediaDataList, 0, sizeof(m_pMediaDataList));
}

Mp4FileRW::~Mp4FileRW(void)
{
	Close();
}


// 获取电影总长度
int Mp4FileRW::GetMovieLen()
{
	int nLen = 0;
	CMp4Mvhd* pMvhd = m_Movie.GetMvhd();
	if ( pMvhd )
	{
		nLen = pMvhd->mvhd.nDuration / pMvhd->mvhd.nTimeScale;
	}


	CMp4Trak *pTrack = NULL;
	for (int i=0;i<m_Movie.m_lstTrack.size(); i++ )
	{
		pTrack = m_Movie.m_lstTrack[i];
		if ( pTrack)
		{
			//MP4_SAMPLE_SIZE*
			pTrack->m_Media.m_minf.m_Stbl.m_SampleSize.m_lstStsz;
		}
	}
	
	return nLen ;
}

// 打开文件 读或写
int Mp4FileRW::Open(char *pFileName, int nRead)
{
	if(NULL == pFileName)
		return -1;
	Close();
	int nRet = 0;
	if ( nRead )
	{
		m_pFile = fopen(pFileName, "r+b");
		nRet = ParseHeaders();
	}
	else
	{
		m_pFile = fopen(pFileName, "wb");
		if (NULL == m_pFile)
			return -1;
		WriteFileHead();
	}


	strcpy (m_chFileName, pFileName);

	return 0;
}

int Mp4FileRW::WriteFileHead()
{
	int nRet = 0;
	if ( !m_pFile )
		return -1;

	GetMovieLen();
	CMp4Ftyp fileType;
	nRet = fileType.Write( m_pFile );
	return nRet;
}
int Mp4FileRW::ParseHeaders()
{
	if (NULL == m_pFile)
		return -1;
	int nRet = 0;
	int nParsedLen = 0;
	int nRead = 0;
	MP4_HEADER Header;
	int cur_pos = ftell(m_pFile ) ;

	while ( !feof( m_pFile) )
	{
		fseek(m_pFile, cur_pos, SEEK_SET);
		
		nRead = fread (&Header, 1,  8 , m_pFile); // sizeof (Header)
		if (nRead < 8 /*sizeof (Header)*/)
		{
			nRet = -1;
			break;
		}

		int nFullBoxSize = Single::Instance().ToUInt32lit( Header.nSize );
		int nBoxDataSize = nFullBoxSize - 8;

		// ftyp box
		if ( Header.nType == m_FileType.GetType() )
		{
			m_FileType.SetSize ( nFullBoxSize-8 );
			nParsedLen = m_FileType.Read (m_pFile);
		}
		// moov box 
		else if (Header.nType == m_Movie.GetType())
		{
			m_Movie.SetSize ( nBoxDataSize );
			nParsedLen = m_Movie.Read ( m_pFile);
		}
		// mdat
		else if (Header.nType == m_boxMdat.GetType())
		{
			m_boxMdat.SetSize( nBoxDataSize );
			nParsedLen = m_boxMdat.Read( m_pFile );
		}
		else 
		{
			nParsedLen = Header.nSize - nRead;
		}

		if (nParsedLen != ( nFullBoxSize - nRead))
		{
			nRet = -1;
		}

		cur_pos += nFullBoxSize;
	}

	return nRet;
}

void Mp4FileRW::Close()
{
	if (m_pFile)
		fclose (m_pFile);
	m_pFile = NULL;
}

void Mp4FileRW::Read()
{
	CMp4Box root;

	int nSize = root.Read( m_pFile);
}




void Mp4FileRW::Write()
{

}


int Mp4FileRW::AddAudioStream (int nFormat, ...)
{

	return 0;
}
int Mp4FileRW::AddVideoStream (int nFormat, ...)
{

	return 0;
}


int Mp4FileRW::AddCustomData (int nType, char *pData, int nLen)
{

	return 0;
}
int Mp4FileRW::GetCustomData (int nType, char *pData)
{

	return 0;
}

int Mp4FileRW::Write (int nStreamId, char *pData, int nLen, uint32_t nSampleTime, uint32_t nFlag)
{

	return 0;
}
// 视频流时，nFlag &0x1 为1，表示关键帧，用于创建索引
int Mp4FileRW::Read (int *pStreamId, char *pData, int nLen, uint32_t *pSampleTime, uint32_t *pFlag)
{

	return 0;
}

