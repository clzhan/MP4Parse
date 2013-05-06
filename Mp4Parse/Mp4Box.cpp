#include "StdAfx.h"
#include "Mp4Box.h"
#include "Single.h"

#ifndef FREE
#define FREE(p) if (p) free(p); p = NULL;
#endif
#ifndef ZERO
#define ZERO(p) memset(&p,0,sizeof (p));
#endif

CMp4Box::CMp4Box(void)
{
	memset (&m_Header, 0, sizeof (m_Header));
	m_Header.nSize = sizeof (m_Header);
}

CMp4Box::~CMp4Box(void)
{
}
int CMp4Box::Read (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	fread (&m_Header, 1, sizeof (m_Header), pFile);
	return sizeof(m_Header);
}
int CMp4Box::Write (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	fwrite (&m_Header, 1, sizeof (m_Header), pFile);
	return sizeof(m_Header);
}
CMp4Ftyp::CMp4Ftyp()
{
	m_nMajor = 1;
	m_nMinor = 1;
	m_nCount = 0;
	memset (m_nCompatible, 0, sizeof (m_nCompatible));
	memcpy (&m_Header.nType, "ftyp", sizeof(m_Header.nType));

}
CMp4Ftyp::~CMp4Ftyp()
{

}
int CMp4Ftyp::Read (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;
	int nRead = 0;
	
	nRead = fread (&m_nMajor, 1, sizeof (m_nMajor), pFile);
	nRet += nRead;

	nRead = fread (&m_nMinor, 1, sizeof (m_nMinor), pFile);
	nRet += nRead;

	m_nCount = (m_Header.nSize - sizeof (m_nMajor) - sizeof (m_nMinor)) / sizeof(uint32_t);
	if (m_nCount > 0)
	{
		int  nLastCount = 0;
		if ( m_nCount >= 10)
		{
			nLastCount = m_nCount - 10;
			m_nCount = 10;
		}

		nRead = fread (m_nCompatible, 1, sizeof (m_nCompatible[0]) * m_nCount, pFile);
		nRet += nRead;
		if (nLastCount > 0)
		{
			nRead = sizeof (m_nCompatible[0]) * nLastCount;
			fseek (pFile, nRead, SEEK_CUR);
			nRet += nRead;
		}
	}
	
	return nRet;
}
int CMp4Ftyp::Write (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	m_Header.nSize = sizeof (m_Header) + sizeof (m_nMajor) + sizeof (m_nMinor) + sizeof (m_nCompatible[0]) * m_nCount;
	
//	m_Header.nSize = ConvertEndian();
	nRet = fwrite(&m_Header, sizeof(m_Header), 1, pFile );
	return nRet;
}


CMp4UserData::CMp4UserData()
{
	memcpy (&m_Header.nType, "udta", sizeof(m_Header.nType));
}
CMp4UserData::~CMp4UserData()
{

}

int CMp4UserData::Read (FILE *pFile)
{
	return 0;
}
int CMp4UserData::Write (FILE *pFile)
{
	return 0;
}
/****************************************************/
/* Moov box
/*
/****************************************************/
// moov
CMp4Moov::CMp4Moov()
{
	m_nTrackCount = 0;
	memcpy (&m_Header.nType, "moov", sizeof(m_Header.nType));
}
CMp4Moov::~CMp4Moov()
{

}
int CMp4Moov::Read (FILE *f )
{
	if (NULL == f )
		return -1;
	int nRet = 0;

	CMp4Trak track;
	CMp4Trak *pTrack = NULL;
	CMp4UserData udta;

	unsigned char p[5];                         
	int nInnerSize = 0;
	int nSubBoxSize = 0;
	int nCusPos = ftell( f);
	int nMoovDataSize = m_Header.nSize;
	do
	{
		fseek( f, nCusPos, SEEK_SET);

		MP4_HEADER Header;
		int nRead = fread (&Header, 1,  BOX_HEAD_SIZE , f ); // sizeof (Header)
		nRet += nRead;
		nSubBoxSize = Single::Instance().PrintBoxHeadInfo(Header, "moov");
		int nBoxDataSize = nSubBoxSize - 8;	
		// mvhd - moov head
		if( Header.nType == m_MovieHeader.GetType()  )
		{
			m_MovieHeader.SetSize( nBoxDataSize );
			nRead = m_MovieHeader.Read( f );
		} 	
		// track
		else if ( Header.nType == track.GetType() )
		{
			pTrack = new CMp4Trak();
			if ( !pTrack )
				continue;
			
			pTrack->SetSize( nBoxDataSize );
			nRead = pTrack->Read( f );
			
			m_lstTrack.push_back( pTrack );
		}
		// udta - userData
		else if(Header.nType == udta.GetType())
		{
			udta;
		}
		nCusPos += nSubBoxSize;
		nInnerSize += nSubBoxSize;

	}while( nInnerSize != nMoovDataSize);

	return nRet;
}
int CMp4Moov::Write (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	return nRet;
}


CMp4Trak::CMp4Trak()
{
	memcpy (&m_Header.nType, "trak", sizeof(m_Header.nType));
}
CMp4Trak::~CMp4Trak()
{

}


/*************************************************/
/*  Track  box 
/* 
/*************************************************/
int CMp4Trak::Read (FILE *pFile)
{
	if (NULL == pFile)
		return -1;

	int nRet = 0;
	
	// track box自身大小（所有子box之和）， 不包括头部
	int nBoxSize = m_Header.nSize;  
	int nInnerSize = 0;
	int cur_pos = ftell( pFile ) ;

	do{
		fseek( pFile , cur_pos, SEEK_SET);

		MP4_HEADER Header;
		int nRead = fread (&Header, 1,  BOX_HEAD_SIZE , pFile ); // sizeof (Header)
		nRet += nRead;

		// sub box size no head 
		int nSubBoxSize = Single::Instance().PrintBoxHeadInfo(Header, "	trak");
		int nSubBoxDataLen = nSubBoxSize - 8;

		//  tkhd
		if( m_boxTrackHeader.GetType() == Header.nType )
		{
			 //mp4_read_tkhd_box(f, box_size);
			m_boxTrackHeader.SetSize( nSubBoxDataLen );
			m_boxTrackHeader.Read( pFile );
		} 
		// edts
		else if( m_boxEdts.GetType() == Header.nType )
		{
			m_boxEdts.SetSize( nSubBoxDataLen );
			m_boxEdts.Read( pFile );
		}
		// tref
		else if( m_TrackRef.GetType() == Header.nType  )
		{
			m_TrackRef.SetSize( nSubBoxDataLen );
			m_TrackRef.Read( pFile );
		}
		// media
		else if( m_Media.GetType() == Header.nType )
		{
			m_Media.SetSize( nSubBoxDataLen );
			m_Media.Read( pFile );
		} 
		// udat
		else if ( m_udat.GetType() == Header.nType )
		{
			m_udat.SetSize( nSubBoxDataLen );
			m_udat.Read( pFile );
		}

		cur_pos += nSubBoxSize;
		nInnerSize += nSubBoxSize;

	} while(nInnerSize != nBoxSize );

	return nRet;
}

int CMp4Trak::Write (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	return nRet;
}


CMp4Mvhd::CMp4Mvhd()
{
	ZERO (mvhd);
	memcpy (&m_Header.nType, "mvhd", sizeof(m_Header.nType));
}

CMp4Mvhd::~CMp4Mvhd()
{

}

// mvhd
int CMp4Mvhd::Read (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

#if 0
	int nRead = fread( &mvhd, sizeof( mvhd ), 1,  pFile );	
	nRet += nRead;
	int nTemp = Single::Instance().ToUInt32lit( mvhd.nNextTrackId );
	nTemp = Single::Instance().ToUInt32lit( mvhd.nTimeScale );
#else

	mvhd.nVersion = Single::Instance().read_uint8( pFile ); 
	fread(mvhd.nFlags, sizeof( mvhd.nFlags), 1,  pFile );						//3
	mvhd.nCreateTime = Single::Instance().read_uint32_lit( pFile );			    //4
	mvhd.nModifiedTime = Single::Instance().read_uint32_lit( pFile );			//4
	mvhd.nTimeScale = Single::Instance().read_uint32_lit( pFile );              //4
	mvhd.nDuration = Single::Instance().read_uint32_lit( pFile );               //4

	mvhd.nRate = Single::Instance().read_uint32_lit( pFile );                   //4
	mvhd.nVolumn = Single::Instance().read_uint16_big( pFile );                 //2
	fread(&mvhd.nReserved, sizeof(mvhd.nReserved), 1,  pFile );					//2
	//fread(&mvhd.reserved2, sizeof(mvhd.reserved2), 1,  pFile );				//8
	fread(&mvhd.Matrix, sizeof(mvhd.Matrix), 1,  pFile );						//36
	fread(mvhd.preDefined, sizeof(mvhd.preDefined), 1,  pFile );				//24
	mvhd.nNextTrackId = Single::Instance().read_uint32_lit( pFile );            //4

#endif
	
	return nRet;
}
int CMp4Mvhd::Write (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	return nRet;
}

// track

CMp4Tkhd::CMp4Tkhd()
{
	ZERO (m_TrackHeader);
	memcpy (&m_Header.nType, "tkhd", sizeof(m_Header.nType));
}
CMp4Tkhd::~CMp4Tkhd()
{

}

// tkhd track header
int CMp4Tkhd::Read (FILE *f)
{
	if (NULL == f)
		return -1;
	int nBoxSize = m_Header.nSize ; // without head
	
	int nRead = 0;
#if 0
	int nSize =  sizeof( m_TrackHeader );
	int nRead = fread( &m_TrackHeader, sizeof( m_TrackHeader ), 1,  pFile );	
#else

	m_TrackHeader.nVersion = Single::Instance().read_uint8(f);
	fread(&m_TrackHeader.nFlags, sizeof(m_TrackHeader.nFlags), 1, f);
	m_TrackHeader.nCreateTime =  Single::Instance().read_uint32_lit(f);
	m_TrackHeader.nModifiedTime = Single::Instance().read_uint32_lit(f);
	m_TrackHeader.nTrackId = Single::Instance().read_uint32_lit(f);
	m_TrackHeader.nReserved1 = Single::Instance().read_uint32_lit(f);
	m_TrackHeader.nDuration = Single::Instance().read_uint32_lit(f);
	fread(&m_TrackHeader.nReserved2, sizeof(m_TrackHeader.nReserved2), 1, f);
	m_TrackHeader.nLayer = Single::Instance().read_uint16_big(f); 
	m_TrackHeader.nAlterGroup =Single::Instance().read_uint16_big(f); 
	m_TrackHeader.nVolumn = Single::Instance().read_uint16_big(f); 
	m_TrackHeader.nReserved3 = Single::Instance().read_uint16_big(f); 
	fread(&m_TrackHeader.Matrix, sizeof(m_TrackHeader.Matrix), 1, f);
	m_TrackHeader.nWidth = Single::Instance().read_uint32_lit(f);
	m_TrackHeader.nHeight = Single::Instance().read_uint32_lit(f);
	
#endif	

	return nRead;
}

int CMp4Tkhd::Write (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	return nRet;
}

/***********************************************/
/*  Elst                                                                   
/*************************************************/
CMp4Elst::CMp4Elst()
{
	m_nVersion = 1;
	memset (m_nFlags, 0, sizeof (m_nFlags));
	m_nEditCount = 0;
	memcpy (&m_Header.nType, "elst", sizeof(m_Header.nType));
}
CMp4Elst::~CMp4Elst()
{
}

int CMp4Elst::Read (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;
	MP4_HEADER Header;
	int nRead = fread (&Header, 1,  BOX_HEAD_SIZE , pFile ); // sizeof (Header)
	nRet += nRead;
	int nBoxDataLen = Single::Instance().ToUInt32lit( Header.nSize );

	m_nVersion = Single::Instance().read_uint8( pFile ); 
	fread(m_nFlags, sizeof( m_nFlags ), 1,  pFile );						//3
	m_nEditCount = Single::Instance().read_uint32_lit( pFile );			    //4

	MP4_EDIT_ITEM * pEditItemList = NULL;
	for (int n=0; n< m_nEditCount; n++ )
	{
		pEditItemList = new MP4_EDIT_ITEM;
		pEditItemList->nSegmentDuration = Single::Instance().read_uint32_lit( pFile );		
		pEditItemList->nMediaTime = Single::Instance().read_uint32_lit( pFile );		
		pEditItemList->nMediaRate = Single::Instance().read_uint32_lit( pFile );		
		m_lstEdts.push_back( pEditItemList );
	}


	//m_boxElst;

	return nRet;
}
int CMp4Elst::Write (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	return nRet;
}


CMp4Edts::CMp4Edts()
{
	memcpy (&m_Header.nType, "edts", sizeof(m_Header.nType));
}
CMp4Edts::~CMp4Edts()
{
}
int CMp4Edts::Read (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	m_Header.nSize;
	int nRet = m_boxElst.Read( pFile );

	return nRet;
}
int CMp4Edts::Write (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	return nRet;
}

CMp4Tref::CMp4Tref()
{
	m_pTrefItemList = NULL;
	memcpy (&m_Header.nType, "tref", sizeof(m_Header.nType));
}
CMp4Tref::~CMp4Tref()
{
	if (m_pTrefItemList)
		free (m_pTrefItemList);
}
int CMp4Tref::Read (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	return nRet;
}
int CMp4Tref::Write (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	return nRet;
}
CMp4MdiaHeader::CMp4MdiaHeader()
{
	memset (&m_MediaHeader, 0, sizeof(m_MediaHeader));
	memcpy (&m_Header.nType, "mdhd", sizeof(m_Header.nType));
}
CMp4MdiaHeader::~CMp4MdiaHeader()
{

}
// mdhd
int CMp4MdiaHeader::Read (FILE *f)
{
	if (NULL == f)
		return -1;
	int nRet = 0;

	m_MediaHeader.nVersion = Single::Instance().read_uint8(f);
	fread(&m_MediaHeader.nFlags, sizeof(m_MediaHeader.nFlags), 1, f);
	m_MediaHeader.nCreateTime = Single::Instance().read_uint32_lit(f);
	m_MediaHeader.nModifiedTime = Single::Instance().read_uint32_lit(f);
	m_MediaHeader.nTimeScale = Single::Instance().read_uint32_lit(f);
	m_MediaHeader.nDuration = Single::Instance().read_uint32_lit(f);
	m_MediaHeader.nLanguage = Single::Instance().read_uint16_big(f);
	m_MediaHeader.nQuality = Single::Instance().read_uint16_big(f);

	return nRet;
}
int CMp4MdiaHeader::Write (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	return nRet;
}


CMp4HandlerRef::CMp4HandlerRef()
{
	memcpy (&m_Header.nType, "hdlr", sizeof(m_Header.nType));
}
CMp4HandlerRef::~CMp4HandlerRef()
{
}
// hdlr read
int CMp4HandlerRef::Read (FILE *f)
{
	if (NULL == f )
		return -1;
	int nRet = 0;

#if 0

	m_hdlr.nVersion = Single::Instance().read_uint8(f);
	fread(&m_hdlr.nFlags, sizeof(m_hdlr.nFlags), 1, f);
	m_hdlr.nComponentType = Single::Instance().read_uint32_lit(f);
	m_hdlr.nSubType = Single::Instance().read_uint32_lit(f);
	m_hdlr.nManufacturer = Single::Instance().read_uint32_lit(f);
	m_hdlr.nComponentFlags = Single::Instance().read_uint32_lit(f);
	m_hdlr.nComponentFlagMask = Single::Instance().read_uint16_big(f);

// 	int nInnerSize = 0;
// 	fread(&nInnerSize, 1, 1, f);
// 	m_hdlr.name = new char[nInnerSize + 1];
// 	for(int i = 0; i < nInnerSize; ++i)
// 		fread(&m_hdlr.name[i], sizeof(unsigned char), 1, f);
#else

	std::string name;
	fread(&m_hdlr, m_Header.nSize, 1, f );

// 	m_hdlr.nVersion = ConvertEndian( m_hdlr.nVersion );
// 	m_hdlr.nComponentType = ConvertEndian( m_hdlr.nComponentType );
// 	m_hdlr.nHandlerType = ConvertEndian( m_hdlr.nHandlerType );
// 	m_hdlr.nManufacturer = ConvertEndian( m_hdlr.nManufacturer );
// 	m_hdlr.nComponentFlags = ConvertEndian( m_hdlr.nComponentFlags );
// 	m_hdlr.nComponentFlagMask = ConvertEndian( m_hdlr.nComponentFlagMask );

#endif

	return nRet;
}
int CMp4HandlerRef::Write (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	return nRet;
}

/*
 *	dinf
 * data reference box
 */
CMp4Dref::CMp4Dref()
{
	m_nVersion = 1;
	memset (m_nFlags, 0, sizeof (m_nFlags));
	m_nRefCount = 0;
	memcpy (&m_Header.nType, "dref", sizeof(m_Header.nType));
}
CMp4Dref::~CMp4Dref()
{
}
int CMp4Dref::Read (FILE *f )
{
	if (NULL == f )
		return -1;
	int nRet = 0;

	m_nVersion = Single::Instance().read_uint8(f);
	fread(m_nFlags, sizeof(m_nFlags), 1, f);
	m_nRefCount = Single::Instance().read_uint32_lit(f);

	return 0;
	MP4_DREF_ENTRY * pEntry = NULL;
	for(int i =0; i < m_nRefCount; ++i)
	{
		pEntry = new MP4_DREF_ENTRY ;
		pEntry->nType = Single::Instance().read_uint32_lit(f);
		pEntry->nSize = Single::Instance().read_uint32_lit(f);
		pEntry->nVersion = Single::Instance().read_uint8(f);
		fread(pEntry->nFlags, sizeof(pEntry->nFlags), 1, f);

		int nCount = (pEntry->nSize - sizeof (pEntry->nType)
			- sizeof (pEntry->nVersion) - sizeof(pEntry->nFlags));
		fread(pEntry->chLocation, sizeof (pEntry->chLocation[0])*nCount, 1, f);

		if ( pEntry )
			m_lstDrefEntry.push_back( pEntry );
	}

	return nRet;
}
int CMp4Dref::Write (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	return nRet;
}


CMp4Dinf::CMp4Dinf()
{
	memcpy (&m_Header.nType, "dinf", sizeof(m_Header.nType));
}
CMp4Dinf::~CMp4Dinf()
{
}
int CMp4Dinf::Read (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	MP4_HEADER Header;
	fread (&Header, 1, sizeof (Header), pFile);
	Header.nSize = ConvertEndian( Header.nSize );
	if ( Header.nType == m_DataRef.GetType() )
	{
		m_DataRef.SetSize( Header.nSize -8 );
		m_DataRef.Read( pFile );
	}


	return nRet;
}
int CMp4Dinf::Write (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	return nRet;
}

// smhd
CMp4Smhd::CMp4Smhd ()
{
	ZERO( m_smhd );
	memcpy (&m_Header.nType, "smhd", sizeof(m_Header.nType));
}
CMp4Smhd::~CMp4Smhd ()
{

}
int CMp4Smhd::Read (FILE *f)
{
	if (NULL == f)
		return -1;
	int nRet = 0;
	m_smhd.nVersion = Single::Instance().read_uint8(f);
	fread(&(m_smhd.nFlags), sizeof(m_smhd.nFlags), 1, f);
	m_smhd.nBalance = Single::Instance().read_uint16_lit(f);
	m_smhd.reserved = Single::Instance().read_uint16_lit(f);


	return nRet;
}
int CMp4Smhd::Write (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	return nRet;
}

// vmhd
CMp4Vmhd::CMp4Vmhd ()
{
	ZERO( m_vmhd );
	memcpy (&m_Header.nType, "vmhd", sizeof(m_Header.nType));

}
CMp4Vmhd::~CMp4Vmhd ()
{
}
int CMp4Vmhd::Read (FILE *f )
{
	if (NULL == f )
		return -1;
	int nRet = 0;

	m_vmhd.nVersion = Single::Instance().read_uint8(f);
	fread(&(m_vmhd.nFlags), sizeof(m_vmhd.nFlags), 1, f);
	m_vmhd.nGraphicsMode = Single::Instance().read_uint16_lit(f);
	fread(&(m_vmhd.nOpColor), sizeof(m_vmhd.nOpColor), 1, f);


	return nRet;
}
int CMp4Vmhd::Write (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	return nRet;
}

/**************************************************************
/* stbl                                                                    
/**************************************************************/
CMp4Stbl::CMp4Stbl()
{
// 	m_pSampleDescription = NULL;
// 	m_pTime2Sample = NULL;
// 	m_pSyncSample = NULL;
// 	m_pSample2Chunk = NULL;
// 	m_pSampleSize = NULL;
// 	m_pChunkOffset = NULL;
	memcpy (&m_Header.nType, "stbl", sizeof(m_Header.nType));
}
CMp4Stbl::~CMp4Stbl()
{
// 	FREE (m_pSampleDescription);
// 	FREE (m_pTime2Sample);
// 	FREE (m_pSyncSample);
// 	FREE (m_pSample2Chunk);
// 	FREE (m_pSampleSize);
// 	FREE (m_pChunkOffset);

}

// stbl
int CMp4Stbl::Read (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;
	// box自身大小（所有子box之和）， 不包括头部
	int nBoxSize = m_Header.nSize;  
	int nInnerSize = 0;
	int cur_pos = ftell( pFile ) ;

	do{
		fseek( pFile , cur_pos, SEEK_SET);

		MP4_HEADER Header;
		int nRead = fread (&Header, 1,  BOX_HEAD_SIZE , pFile ); 
		nRet += nRead;

		// sub box size no head 
		int nSubBoxSize = Single::Instance().PrintBoxHeadInfo(Header, "	stbl");
		Header.nSize = ConvertEndian(Header.nSize );
		int nSubBoxDataLen = Header.nSize - 8;

		char chType[5];
		memset(chType,0, 5 );
		memcpy(chType, &Header.nType, 4);

		// stsd
		if ( m_SampleDescription.GetType() == Header.nType )
		{
			m_SampleDescription.SetSize( nSubBoxDataLen );
			m_SampleDescription.Read( pFile ); 
		}
		// stts
		else if (m_Time2Sample.GetType() == Header.nType  )
		{
			m_Time2Sample.SetSize( nSubBoxDataLen );
			m_Time2Sample.Read( pFile );
		}
		// stss
		else if( m_SyncSample.GetType() == Header.nType  )
		{
			m_SyncSample.SetSize( nSubBoxDataLen );
			m_SyncSample.Read(pFile );
		} 
		// stsc
		else if(  m_Sample2Chunk.GetType() == Header.nType  )
		{
			m_Sample2Chunk.SetSize( nSubBoxDataLen );
			m_Sample2Chunk.Read(pFile );
		}
		// stsz
		else if(  m_SampleSize.GetType() == Header.nType )
		{
			m_SampleSize.SetSize( nSubBoxDataLen );
			m_SampleSize.Read( pFile );
		}
		// stco
		else if( m_ChunkOffset.GetType() == Header.nType )
		{
			m_ChunkOffset.SetSize( nSubBoxDataLen );
			m_ChunkOffset.Read( pFile );
		}
		else if(0 == strcmp(chType, "ctts") ) 
		{

		}
		cur_pos += nSubBoxSize;
		nInnerSize += nSubBoxSize;

	} while(nInnerSize != nBoxSize );


	return nRet;
}
int CMp4Stbl::Write (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	return nRet;
}

SampleEntry::SampleEntry()
{
	nDataReferenceIndex = 0;
}

SampleEntry::~SampleEntry()
{

}

int SampleEntry::Read (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	fread(reserved, sizeof(reserved), 1, pFile );  //6
	nDataReferenceIndex  = Single::Instance().read_uint16_lit( pFile );   //2

	return nRet;
}
int SampleEntry::Write (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	return nRet;
}

VisualSampleEntry::VisualSampleEntry()
{
	m_nFrameCount = 0;
	m_nHorizreSolution = 0;
	memcpy(&m_Header.nType, "mp4v", sizeof(m_Header.nType) ); 
}

VisualSampleEntry::~VisualSampleEntry()
{

}
int VisualSampleEntry::Read(FILE *f)
{
	SampleEntry::Read (f);

	m_nPreDefined		= Single::Instance().read_uint16_big(f);		//2
	m_nReserved1		= Single::Instance().read_uint16_big(f);		//2
	fread(m_szPreDefined1, sizeof(m_szPreDefined1), 1, f);				//3*4
	m_nWidth			= Single::Instance().read_uint16_lit(f);		//2
	m_nHeight			= Single::Instance().read_uint16_lit(f);		//2
	m_nHorizreSolution	= Single::Instance().read_uint32_lit(f);   //4
	m_nVertreSolution	= Single::Instance().read_uint32_lit(f);   //4
	m_nReserved2		= Single::Instance().read_uint32_lit(f);   //4
	m_nFrameCount		= Single::Instance().read_uint16_lit(f);   //2
	fread(m_szCompressorName, sizeof(m_szCompressorName), 1, f);   //33
	m_nDepth			= Single::Instance().read_uint16_lit(f);   //1
	m_nPreDefined2		= Single::Instance().read_uint16_big(f);   //2

	return 0;
}

int VisualSampleEntry::Write (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	return nRet;
}

AudioSampleEntry::AudioSampleEntry()
{
	m_nChannelCount = 0;
	m_nSampleSize = 0;
	memcpy(&m_Header.nType, "mp4a", sizeof(m_Header.nType) ); 
}
AudioSampleEntry::~AudioSampleEntry()
{

}

int AudioSampleEntry::Read(FILE *pFile)
{
	SampleEntry::Read ( pFile );

	fread(&m_szReserved1, sizeof(m_szReserved1), 1, pFile );
	m_nChannelCount = Single::Instance().read_uint16_lit(pFile);
	m_nSampleSize = Single::Instance().read_uint16_lit(pFile);
	m_nPreDefined = Single::Instance().read_uint16_lit(pFile);
	m_nReserved2 = Single::Instance().read_uint16_lit(pFile);
	m_nSampleRate = Single::Instance().read_uint32_lit(pFile);

	return 0;
}

int AudioSampleEntry::Write (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	return nRet;
}

/*
 *	stsd sample description
 */
CMp4Stsd::CMp4Stsd ()
{
	nEntriesCount = 0;
	memcpy(&m_Header.nType, "stsd", sizeof(m_Header.nType) );
}
CMp4Stsd::~CMp4Stsd ()
{
	SampleEntry * pSampleEntry = NULL;
	for ( int i=0; i< m_lstSampleEntry.size(); i++ )
	{
		pSampleEntry = m_lstSampleEntry[i];
		if ( pSampleEntry )
		{
			delete pSampleEntry;
			pSampleEntry = NULL;
		}
	}
	m_lstSampleEntry.clear();
}

int CMp4Stsd::Read(FILE *f )
{
	if ( !f )
		return -1;

	nVersion = Single::Instance().read_uint8(f);
	fread(nFlags, sizeof(nFlags), 1, f);
	nEntriesCount = Single::Instance().read_uint32_lit(f);

	SampleEntry * pSampleEntry = NULL;
	for(int i =0; i < nEntriesCount; ++i)
	{
		pSampleEntry = new SampleEntry ;
		// head
		uint32_t nSize  = Single::Instance().read_uint32_lit(f);
		uint32_t nType = Single::Instance().read_uint32_lit(f);
		if( nType == (('m'<<24)|('p'<<16)|('4'<<8)|('a')))
		{
			pSampleEntry = new AudioSampleEntry;
			pSampleEntry->SetSize( nSize-8 );

			pSampleEntry->Read( f );
			
		}
		else if( nType == (('m'<<24)|('p'<<16)|('4'<<8)|('v')))
		{
			pSampleEntry = new VisualSampleEntry;
			pSampleEntry->SetSize( nSize-8 );
			pSampleEntry->Read( f );
		} 

		if ( pSampleEntry )
			m_lstSampleEntry.push_back( pSampleEntry );
	}
	return 0;
}
int CMp4Stsd::Write (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	return nRet;
}

//  stts
CMp4Stts::CMp4Stts ()
{
	memcpy(&m_Header.nType, "stts", sizeof(m_Header.nType) );
}
CMp4Stts::~CMp4Stts ()
{
	MP4_TIME_2_SAMPLE * pEntry = NULL;
	for(int i =0; i < m_lstSttsTable.size(); ++i)
	{
		pEntry = m_lstSttsTable[i];
		if ( pEntry )
		{
			delete pEntry;
			pEntry = NULL;
		}
	}
}

//Time-to-sample atoms存储了media sample的duration 信息，
//提供了时间对具体data sample的映射方法，通过这个atom，
//你可以找到任何时间的sample，类型是'stts'。

int CMp4Stts::Read(FILE *f)
{
	if (NULL == f)
		return -1;
	int nRet = 0;

	nVersion = Single::Instance().read_uint8(f);
	fread(szFlags, sizeof(szFlags), 1, f);
	nEntriesCount = Single::Instance().read_uint32_lit(f);
	MP4_TIME_2_SAMPLE * pEntry = NULL;
	for(int i =0; i < nEntriesCount; ++i)
	{
		pEntry = new MP4_TIME_2_SAMPLE;
		pEntry->nSampleCount = Single::Instance().read_uint32_lit(f);
		pEntry->nSampleDuration = Single::Instance().read_uint32_lit(f);

		if ( pEntry )
			m_lstSttsTable.push_back( pEntry );
	}

	return nRet;
}
int CMp4Stts::Write (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	return nRet;
}

/******************************************/
/* Sync Sample Box   
/*     stss                                            
/******************************************/

CMp4Stss::CMp4Stss ()
{
	memcpy(&m_Header.nType, "stss", sizeof(m_Header.nType) );
}
CMp4Stss::~CMp4Stss ()
{
	MP4_SYNC_SAMPLE * pEntry = NULL;
	for(int i =0; i < m_lstStss.size(); ++i)
	{
		pEntry = m_lstStss[i];
		if ( pEntry )
		{
			delete pEntry;
			pEntry = NULL;
		}
	}
}

int CMp4Stss::Read(FILE *f )
{
	if (NULL == f )
		return -1;
	int nRet = 0;
	nVersion = Single::Instance().read_uint8(f);
	fread(szFlags, sizeof(szFlags), 1, f);
	nEntryCount = Single::Instance().read_uint32_lit(f);
	MP4_SYNC_SAMPLE * pEntry = NULL;
	for(int i =0; i < nEntryCount; ++i)
	{
		pEntry = new MP4_SYNC_SAMPLE;
		pEntry->nSampleNum = Single::Instance().read_uint32_lit(f);

		if ( pEntry )
			m_lstStss.push_back( pEntry );
	}
}
int CMp4Stss::Write (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	return nRet;
}

/**************************************************************/
/* stsc     Sample To Chunk Box                                                                 
/*
/**************************************************************/
CMp4Stsc::CMp4Stsc ()
{
	memcpy(&m_Header.nType, "stsc", sizeof(m_Header.nType) );
}
CMp4Stsc::~CMp4Stsc ()
{
	MP4_SAMPLE_2_CHUNK * pEntry = NULL;
	for(int i =0; i < m_lstStsc.size(); ++i)
	{
		pEntry = m_lstStsc[i];
		if ( pEntry )
		{
			delete pEntry;
			pEntry = NULL;
		}
	}
}

int CMp4Stsc::Read(FILE *f )
{
	if (NULL == f )
		return -1;
	int nRet = 0;
	nVersion = Single::Instance().read_uint8(f);
	fread(szFlags, sizeof(szFlags), 1, f);
	nEntryCount = Single::Instance().read_uint32_lit(f);
	MP4_SAMPLE_2_CHUNK * pEntry = NULL;
	for(int i =0; i < nEntryCount; ++i)
	{
		pEntry = new MP4_SAMPLE_2_CHUNK;
		pEntry->nFirstChunkNum = Single::Instance().read_uint32_lit(f);
		pEntry->nSamplesPerChunk = Single::Instance().read_uint32_lit(f);
		pEntry->nSampleDescIndex = Single::Instance().read_uint32_lit(f);

		if ( pEntry )
			m_lstStsc.push_back( pEntry );
	}


	return nRet;
}
int CMp4Stsc::Write (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	return nRet;
}


/************** ************************************************/
/*          stsz | stz2  sample size box 
/*
/************** ************************************************/
CMp4Stsz::CMp4Stsz ()
{
	memcpy(&m_Header.nType, "stsz", sizeof(m_Header.nType) );
}
CMp4Stsz::~CMp4Stsz ()
{
	MP4_SAMPLE_SIZE * pEntry = NULL;
	for(int i =0; i < m_lstStsz.size(); ++i)
	{
		pEntry = m_lstStsz[i];
		if ( pEntry )
		{
			delete pEntry;
			pEntry = NULL;
		}
	}
}

int CMp4Stsz::Read(FILE *f )
{
	if (NULL == f )
		return -1;
	int nRet = 0;
	nVersion = Single::Instance().read_uint8(f);
	fread(szFlags, sizeof(szFlags), 1, f);
	nSampleSize = Single::Instance().read_uint32_lit(f);
	nSampleCount = Single::Instance().read_uint32_lit(f);
	if ( nSampleSize == 0 )
	{
		MP4_SAMPLE_SIZE * pEntry = NULL;
		for(int i =0; i < nSampleCount; ++i)
		{
			pEntry = new MP4_SAMPLE_SIZE;
			pEntry->nSampleSize = Single::Instance().read_uint32_lit(f);
			if ( pEntry )
				m_lstStsz.push_back( pEntry );
		}
	}

	return nRet;
}
int CMp4Stsz::Write (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	return nRet;
}

// CMp4Stco
CMp4Stco::CMp4Stco ()
{
	memcpy(&m_Header.nType, "stco", sizeof(m_Header.nType) );
}
CMp4Stco::~CMp4Stco ()
{
	MP4_CHUNK_OFFSET * pEntry = NULL;
	for(int i =0; i < m_lstOffset.size(); ++i)
	{
		pEntry = m_lstOffset[i];
		if ( pEntry )
		{
			delete pEntry;
			pEntry = NULL;
		}
	}
}

/********** *************************************************/
/*         Chunk Offset Box 
/*			stco / co64
/********** *************************************************/
int CMp4Stco::Read(FILE *f )
{
	if (NULL == f )
		return -1;
	int nRet = 0;

	nVersion = Single::Instance().read_uint8(f);
	fread(szFlags, sizeof(szFlags), 1, f);
	nEntryCount = Single::Instance().read_uint32_lit(f);

	MP4_CHUNK_OFFSET * pEntry = NULL;
	for(int i =0; i < nEntryCount; ++i)
	{
		pEntry = new MP4_CHUNK_OFFSET;
		pEntry->nChunkOffset = Single::Instance().read_uint32_lit(f);
		if ( pEntry )
			m_lstOffset.push_back( pEntry );
	}

	return nRet;
}
int CMp4Stco::Write (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	return nRet;
}

/**********************************************/
/*         minf  Media Information Box                                                            
/**********************************************/
CMp4Minf::CMp4Minf()
{
	memcpy (&m_Header.nType, "minf", sizeof(m_Header.nType));
}
CMp4Minf::~CMp4Minf()
{

}
// minf 
int CMp4Minf::Read (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	// box自身大小（所有子box之和）， 不包括头部
	int nBoxSize = m_Header.nSize;  
	int nInnerSize = 0;
	int cur_pos = ftell( pFile ) ;

	do{
		// 将文件指针移到box开始处
		fseek( pFile , cur_pos, SEEK_SET);

		MP4_HEADER Header;
		int nRead = fread (&Header, 1,  BOX_HEAD_SIZE , pFile ); 
		nRet += nRead;

		// sub box size no head 
		int nSubBoxSize = Single::Instance().PrintBoxHeadInfo(Header, "	mdia");
		int nSubBoxDataLen = nSubBoxSize - 8;
		
		uint32_t type = m_AudioMediaHeader.GetType();
		// vmhd
		if ( m_VideoMediaHeader.GetType() == Header.nType )
		{
			m_VideoMediaHeader.SetSize( nSubBoxDataLen );
			m_VideoMediaHeader.Read( pFile );
		}
		// smhd
		else if (m_AudioMediaHeader.GetType() == Header.nType )
		{
			m_AudioMediaHeader.SetSize( nSubBoxDataLen );
			m_AudioMediaHeader.Read( pFile );
		}
		// dinf
		else if( m_Dinf.GetType() == Header.nType )
		{
			//mp4_read_tkhd_box(f, box_size);
			m_Dinf.SetSize( nSubBoxDataLen );
			m_Dinf.Read( pFile );
		} 
		// stbl
		else if( m_Stbl.GetType() == Header.nType )
		{
			m_Stbl.SetSize( nSubBoxDataLen );
			m_Stbl.Read( pFile );
		}
	
	
		cur_pos += nSubBoxSize;
		nInnerSize += nSubBoxSize;

	} while(nInnerSize != nBoxSize );

	return nRet;
}

int CMp4Minf::Write (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	return nRet;
}

/************************************************************************/
/*  mdia box read 
/************************************************************************/
CMp4Mdia::CMp4Mdia()
{
	memcpy (&m_Header.nType, "mdia", sizeof(m_Header.nType) );
}


CMp4Mdia::~CMp4Mdia()
{

}


int CMp4Mdia::Read (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;
	
	// box自身大小（所有子box之和）， 不包括头部
	int nBoxSize = m_Header.nSize;  
	int nInnerSize = 0;
	int cur_pos = ftell( pFile ) ;

	do{
		fseek( pFile , cur_pos, SEEK_SET);

		MP4_HEADER Header;
		int nRead = fread (&Header, 1,  BOX_HEAD_SIZE , pFile ); // sizeof (Header)
		nRet += nRead;

		// sub box size no head 
		int nSubBoxSize = Single::Instance().PrintBoxHeadInfo(Header, "	mdia");
		int nSubBoxDataLen = nSubBoxSize - 8;

		//  mdhd
		if( m_mdhd.GetType() == Header.nType )
		{
			m_mdhd.SetSize( nSubBoxDataLen );
			m_mdhd.Read( pFile );
		} 
		// hdlr
		else if( m_hdlr.GetType() == Header.nType )
		{
			m_hdlr.SetSize( nSubBoxDataLen );
			m_hdlr.Read( pFile );
		}
		// minf
 		else if( m_minf.GetType() == Header.nType  )
 		{
 			m_minf.SetSize( nSubBoxDataLen );
 			m_minf.Read( pFile );
 		}
	
		cur_pos += nSubBoxSize;
		nInnerSize += nSubBoxSize;

	} while(nInnerSize != nBoxSize );


	return nRet;
}
int CMp4Mdia::Write (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	return nRet;
}


CMp4Mdat::CMp4Mdat()
{
	m_pData = NULL;
	memcpy (&m_Header.nType, "mdat", sizeof(m_Header.nType));
}
CMp4Mdat::~CMp4Mdat()
{
	SAFE_ARRAYDELETE(m_pData);
}
int CMp4Mdat::Read (FILE *pFile)
{
	if (NULL == pFile)
		return -1;

	int nRet = 0;
	m_pData = new char[ m_Header.nSize ];
	nRet = fread (m_pData, 1, m_Header.nSize, pFile);

	return nRet;
}

int CMp4Mdat::Write (FILE *pFile)
{
	if (NULL == pFile)
		return -1;
	int nRet = 0;

	nRet = fwrite(m_pData, m_Header.nSize, 1, pFile);

	return nRet;
}

