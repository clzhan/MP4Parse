#ifndef __MP4_FILE__
#define __MP4_FILE__
#pragma warning(disable:4200) 
#include "Utls.h"
#include "stdio.h"
#include "stdlib.h"
#include <vector>


#define BOX_HEAD_SIZE  (8)

typedef struct _MP4_HEADER_
{
	unsigned int nSize;
	unsigned int nType;
	uint64_t nLargeSize;
}MP4_HEADER;


class CMp4Box
{
public:
	 CMp4Box ();
	 virtual ~CMp4Box ();
	 virtual int Read (FILE *pFile);
	 virtual int Write (FILE *pFile);
	 unsigned int GetType () { return m_Header.nType;}
	 unsigned int GetSize () { return m_Header.nSize;}
	 
	 // nSize 为box数据长度， 不包括头部
	 void SetSize (unsigned int nSize) {m_Header.nSize = nSize;}

public:
	MP4_HEADER m_Header;
};



typedef struct _MP4_MATRIX_
{
	uint32_t chValue[9];
}MP4_MATRIX;

/******************************************/
/*   mvhd  很重要
/*     movie header, 影片信息      
/******************************************/
typedef struct _MP4_MVHD_
{
	uint8_t nVersion;
	uint8_t nFlags[3];
	
	uint32_t nCreateTime;
	uint32_t nModifiedTime;
	uint32_t nTimeScale;
	uint32_t nDuration;

	uint32_t nRate;					//4
	uint16_t nVolumn;				//2

	uint8_t nReserved[10];
	MP4_MATRIX    Matrix;			//36
	uint32_t preDefined[6];			//24
	uint32_t nNextTrackId;			//4

}MP4_MVHD;

class CMp4Mvhd : public CMp4Box
{
public:
	 CMp4Mvhd ();
	 ~CMp4Mvhd ();
	 virtual int Read (FILE *pFile);
	 virtual int Write (FILE *pFile);
public:	
	MP4_MVHD mvhd;	
};

/******************************************/
/*  tkhd  
/*        track header, overall information about the track 
/******************************************/
typedef struct _MP4_TRHD_
{
	uint8_t nVersion;
	uint8_t nFlags[3];

	uint32_t nCreateTime;
	uint32_t nModifiedTime;

	uint32_t nTrackId;
	uint32_t nReserved1;
	uint32_t nDuration;

	uint32_t nReserved2[2];
	uint16_t nLayer;
	uint16_t nAlterGroup;

	uint16_t nVolumn;
	uint16_t nReserved3;
	MP4_MATRIX    Matrix;
	uint32_t nWidth;
	uint32_t nHeight;	
}MP4_TRHD;

class CMp4Tkhd : public CMp4Box
{
public:
	 CMp4Tkhd ();
	 ~CMp4Tkhd ();
	 virtual int Read (FILE *pFile);
	 virtual int Write (FILE *pFile);
protected:	
	MP4_TRHD m_TrackHeader;

};

typedef struct _MP4_EDIT_ITEM_
{
	uint32_t nSegmentDuration;
	uint32_t nMediaTime;
	uint32_t nMediaRate;

}MP4_EDIT_ITEM;

class CMp4Elst : public CMp4Box
{
public:
	CMp4Elst ();
	~CMp4Elst ();
	virtual int Read (FILE *pFile);
	virtual int Write (FILE *pFile);
protected:	
	uint8_t m_nVersion;
	uint8_t m_nFlags[3];
	uint32_t m_nEditCount;

	MP4_EDIT_ITEM * m_pEditItemList;
	std::vector<MP4_EDIT_ITEM*> m_lstEdts;
};

class CMp4Edts : public CMp4Box
{
public:
	 CMp4Edts ();
	 ~CMp4Edts ();
	 virtual int Read (FILE *pFile);
	 virtual int Write (FILE *pFile);
protected:	
	CMp4Elst m_boxElst;
};

/******************************************/
/*  tref   
/*       track reference container 
/******************************************/
typedef struct _MP4_TREF_ITEM_
{
	uint32_t nSize;
	uint32_t nType;		// tmcd, chap, sync, scpt, ssrc, hint
	uint32_t nIds[0];

}MP4_TREF_ITEM;

class CMp4Tref : public CMp4Box
{
public:
	 CMp4Tref ();
	 virtual ~CMp4Tref();
	 int Read (FILE *pFile);
	 virtual int Write (FILE *pFile);
protected:	
	MP4_TREF_ITEM *m_pTrefItemList;
};

typedef struct _MP4_MDHD_		// MEDIA HEADER
{
	uint8_t nVersion;
	uint8_t nFlags[3];
	uint32_t nCreateTime;
	uint32_t nModifiedTime;

	uint32_t nTimeScale;
	uint32_t nDuration;

	uint16_t nLanguage;
	uint16_t nQuality;
	
}MP4_MDHD;

class CMp4MdiaHeader: public CMp4Box
{
public:
	 CMp4MdiaHeader ();
	 ~CMp4MdiaHeader ();
	 virtual int Read (FILE *pFile);
	 virtual int Write (FILE *pFile);
protected:	
	MP4_MDHD m_MediaHeader;

};
typedef struct _MP4_HDLR_		// Handler Reference
{
	uint8_t nVersion;
	uint8_t nFlags[3];
	uint32_t nComponentType;	// Component type pre_define
	uint32_t nHandlerType;			// handler type


	uint32_t nManufacturer;  // Component manufacturer
	uint32_t nComponentFlags;		// Component flags
	uint32_t nComponentFlagMask;		// Component flags   mask
	uint8_t chComponentName[0];
	
}MP4_HDLR;

/******************************************/
/*  hdlr  
/*      handler, declares the media (handler) type 
/******************************************/
class CMp4HandlerRef: public CMp4Box
{
public:
	 CMp4HandlerRef ();
	 ~CMp4HandlerRef ();
	 virtual int Read (FILE *pFile);
	 virtual int Write (FILE *pFile);
protected:	
	MP4_HDLR m_hdlr;
};	 


typedef struct _MP4_DREF_		// data reference
{
	uint32_t nSize;
	uint32_t nType;
	uint8_t nVersion;
	uint8_t nFlags[3];
	uint8_t chLocation[0];
	
}MP4_DREF_ENTRY;

/******************************************/
/* dref  
/*     data reference box
/******************************************/
class CMp4Dref: public CMp4Box			// dref
{
public:
	 CMp4Dref ();
	 ~CMp4Dref ();
	 virtual int Read (FILE *pFile);
	 virtual int Write (FILE *pFile);
protected:	
   	uint8_t m_nVersion;
	uint8_t m_nFlags[3];
	uint32_t m_nRefCount;

	std::vector<MP4_DREF_ENTRY *> m_lstDrefEntry;

};
 
/******************************************/
/* dinf   
/*      data infomation box 
/******************************************/
class CMp4Dinf: public CMp4Box			// dinf
{
public:
	 CMp4Dinf ();
	 ~CMp4Dinf ();
	 virtual int Read (FILE *pFile);
	 virtual int Write (FILE *pFile);
protected:
	CMp4Dref m_DataRef;
};

typedef struct _MP4_SAMPLE_DESC_	
{
	uint32_t nSize;
	uint32_t nType;
	uint8_t nReserved[6];
	uint16_t nRefShort;

}MP4_SAMPLE_DESC;

/******************************************/
/*  解码器相关参数， 如果设置不正确，则播放不正常  
/*                                               
/******************************************/
class Esds : public CMp4Box
{
	uint8_t m_nVersion;
	uint8_t m_nFlags[3];
	int decoderConfigLen;
	uint8_t* decoderConfig;

// 		ESID = 0 (0x0000)
// 		streamDependenceFlag = 0 (0x0) <1 bits>
// 		URLFlag = 0 (0x0) <1 bits>
// 		OCRstreamFlag = 0 (0x0) <1 bits>
// 		streamPriority = 0 (0x00) <5 bits>
// 		decConfigDescr
// 		objectTypeId = 64 (0x40)
// 		streamType = 5 (0x05) <6 bits>
// 		upStream = 0 (0x0) <1 bits>
// 		reserved = 1 (0x1) <1 bits>
// 		bufferSizeDB = 6144 (0x001800) <24 bits>
// 		maxBitrate = 20000 (0x00004e20)
// 		avgBitrate = 20000 (0x00004e20)
// 		decSpecificInfo
// 		info = <2 bytes> 15 90 
// 		profileLevelIndicationIndexDescr
// 		slConfigDescr
// 		predefined = 2 (0x02)
// 		ipiPtr
// 		ipIds
// 		ipmpDescrPtr
// 		langDescr
// 		qosDescr
// 		regDescr
// 		extDescr
} ;


class SampleEntry :public  CMp4Box
{ 
public:
	SampleEntry();
	~SampleEntry();

	virtual int Read (FILE *pFile);
	virtual int Write (FILE *pFile);

protected:
	uint8_t reserved[6]; 
	uint16_t nDataReferenceIndex; 
} ;

class VisualSampleEntry : public SampleEntry
{ 
public:
	VisualSampleEntry();
	~VisualSampleEntry();

	virtual int Read (FILE *pFile);
	virtual int Write (FILE *pFile);

protected:
	uint16_t m_nPreDefined;                    
	uint16_t m_nReserved1;  
	uint32_t m_szPreDefined1[3] ; 
	uint16_t m_nWidth; 
	uint16_t m_nHeight; 
	uint32_t m_nHorizreSolution ; //= 0x00480000; // 72 dpi 
	uint32_t m_nVertreSolution  ; // = 0x00480000; // 72 dpi 
	uint32_t m_nReserved2			/*= 0*/; 
	uint16_t m_nFrameCount		/*= 1*/; 
	char	 m_szCompressorName[33]; 
	uint16_t m_nDepth				/*= 0x0018*/; 
	int16_t  m_nPreDefined2		/*= -1*/; 

	Esds	m_esds;
};

// Audio Sequences 
class AudioSampleEntry : public SampleEntry 
{ 	
public:
	AudioSampleEntry();
	~AudioSampleEntry();

	virtual int Read (FILE *pFile);
	virtual int Write (FILE *pFile);

protected:
	uint32_t m_szReserved1[2];	//  = 0; 
	uint16_t m_nChannelCount;	// = 2; 
	uint16_t m_nSampleSize;	// = 16; 
	uint16_t m_nPreDefined;	// = 0; 
	uint16_t m_nReserved2 ;	//= 0 ; 
	uint32_t m_nSampleRate ;	// = {timescale of media}<<16; 

	Esds	m_esds;
};

/******************************************/
/*  stsd  
/*      sample description
/******************************************/
class CMp4Stsd: public CMp4Box			// stsd
{
public:
	CMp4Stsd ();
	~CMp4Stsd ();
	virtual int Read (FILE *pFile);
	virtual int Write (FILE *pFile);
protected:
 	uint8_t nVersion;
	uint8_t nFlags[3];
	uint32_t nEntriesCount;
//	MP4_SAMPLE_DESC Desc[0];
	std::vector<SampleEntry*> m_lstSampleEntry;

};

/******************************************/
/* stts   
/*     time to sample                  
/******************************************/
typedef struct _MP4_TIME_2_SAMPLE_				// 
{
	uint32_t nSampleCount;// chunk count
	uint32_t nSampleDuration;//Delta ;

}MP4_TIME_2_SAMPLE;

class CMp4Stts: public CMp4Box			// stts
{
public:
	CMp4Stts ()	;
	~CMp4Stts ();
	virtual int Read (FILE *pFile);
	virtual int Write (FILE *pFile);
protected:
 	uint8_t nVersion;
	uint8_t szFlags[3];
	uint32_t nEntriesCount;
	std::vector<MP4_TIME_2_SAMPLE*> m_lstSttsTable;
	//MP4_TIME_2_SAMPLE Tts[0];
};

/******************************************/
/*   stss    
/*       Sync Sample Box                                          
/******************************************/
typedef struct _MP4_SYNC_SAMPLE_				// 
{
	uint32_t nSampleNum;

}MP4_SYNC_SAMPLE;

class CMp4Stss: public CMp4Box			// stss
{
public:
	CMp4Stss () ;
	~CMp4Stss ();
	virtual int Read (FILE *pFile);
	virtual int Write (FILE *pFile);
protected:
 	uint8_t nVersion;
	uint8_t szFlags[3];
	uint32_t nEntryCount;
	std::vector<MP4_SYNC_SAMPLE*> m_lstStss;
};


/******************************************/
/*  stsc  
/*		sample-to-chunk box
/******************************************/
typedef struct _MP4_SAMPLE_2_CHUNK_				// 
{
	uint32_t nFirstChunkNum;
	uint32_t nSamplesPerChunk;
	uint32_t nSampleDescIndex;
}MP4_SAMPLE_2_CHUNK;

class CMp4Stsc: public CMp4Box			// stsc
{
public:
	CMp4Stsc ();
	~CMp4Stsc ();
	virtual int Read (FILE *pFile);
	virtual int Write (FILE *pFile);
protected:
 	uint8_t nVersion;
	uint8_t szFlags[3];
	uint32_t nEntryCount;
	std::vector<MP4_SAMPLE_2_CHUNK*> m_lstStsc;
};
 

/******************************************/
/*	stsz
/*    sample size box                                          
/******************************************/
typedef struct _MP4_SAMPLE_SIZE_				// 
{
	uint32_t nSampleSize;
}MP4_SAMPLE_SIZE;

class CMp4Stsz: public CMp4Box			// stsz
{
public:
	CMp4Stsz ();
	~CMp4Stsz ();
	virtual int Read (FILE *pFile);
	virtual int Write (FILE *pFile);
public:
 	uint8_t nVersion;
	uint8_t szFlags[3];
	uint32_t nSampleSize;
	uint32_t nSampleCount;
	std::vector<MP4_SAMPLE_SIZE*> m_lstStsz;
};


/******************************************/
/*	stco
/*      Chunk Offset Box                                         
/******************************************/
typedef struct _MP4_CHUNK_OFFSET_				 
{
	uint32_t nChunkOffset;
}MP4_CHUNK_OFFSET;

class CMp4Stco: public CMp4Box			// stco
{
public:
	CMp4Stco ();
	~CMp4Stco ();
	virtual int Read (FILE *pFile);
	virtual int Write (FILE *pFile);
protected:
 	uint8_t nVersion;
	uint8_t szFlags[3];
	uint32_t nEntryCount;
	std::vector<MP4_CHUNK_OFFSET*> m_lstOffset;
};

/******************************************/
/* stbl  
/*     sample table                   
/******************************************/
class CMp4Stbl: public CMp4Box		
{
public:
	 CMp4Stbl ();
	 ~CMp4Stbl ();
	 virtual int Read (FILE *pFile);
	 virtual int Write (FILE *pFile);

public:
	CMp4Stsd m_SampleDescription;
	CMp4Stts m_Time2Sample;
	CMp4Stss m_SyncSample;
	CMp4Stsc m_Sample2Chunk;
	CMp4Stsz m_SampleSize;
	CMp4Stco m_ChunkOffset;

};

typedef struct _MP4_VMHD_				// vmhd
{
	uint8_t nVersion;
	uint8_t nFlags[3];
	uint16_t nGraphicsMode ;	// 
	uint16_t nOpColor[3];
}MP4_VMHD;


class CMp4Vmhd: public CMp4Box			// vmhd
{
public:
	CMp4Vmhd ();
	~CMp4Vmhd ();
	virtual int Read (FILE *pFile);
	virtual int Write (FILE *pFile);

protected:
	MP4_VMHD m_vmhd;
};

typedef struct _MP4_SMHD_				// smhd
{
	uint8_t nVersion;
	uint8_t nFlags[3];
	uint16_t nBalance;	// 
	uint16_t reserved;
}MP4_SMHD;


class CMp4Smhd: public CMp4Box			// smhd
{
public:
	CMp4Smhd ();
	~CMp4Smhd ();
	virtual int Read (FILE *pFile);
	virtual int Write (FILE *pFile);
protected:

	MP4_SMHD m_smhd;
};

/******************************************/
/* minf  
/*     Media Information Box 
/******************************************/
class CMp4Minf: public CMp4Box			
{
public:
	 CMp4Minf ();
	 ~CMp4Minf ();
	 virtual int Read (FILE *pFile);
	 virtual int Write (FILE *pFile);

public:	
 	CMp4Vmhd m_VideoMediaHeader;
 	CMp4Smhd m_AudioMediaHeader;
 	CMp4Dinf m_Dinf;
 	CMp4Stbl m_Stbl;
};	 

/******************************************/
/*  mdia  
/*      media box
/******************************************/
class CMp4Mdia: public CMp4Box				
{
public:
	 CMp4Mdia ();
	 ~CMp4Mdia ();
	 virtual int Read (FILE *pFile);
	 virtual int Write (FILE *pFile);

public:	

	CMp4MdiaHeader m_mdhd;
	CMp4Minf       m_minf;
	CMp4HandlerRef m_hdlr;
};

class CMp4UserData : public CMp4Box				//udta
{
public:
	CMp4UserData ();
	~CMp4UserData ();
	virtual int Read (FILE *pFile);
	virtual int Write (FILE *pFile);
protected:	
	char * m_pData;

};

/******************************************/
/* track  
/*                                                                
/******************************************/
class CMp4Trak : public CMp4Box			
{
public:
	 CMp4Trak ();
	 ~CMp4Trak ();
	 virtual int Read (FILE *pFile);
	 virtual int Write (FILE *pFile);

	 void ReadSample();
	 void WriteSample();

	 int GetDuration();

public:	
	CMp4UserData m_udat;
	CMp4Tkhd m_boxTrackHeader;
	CMp4Edts m_boxEdts;
	CMp4Tref m_TrackRef;
	CMp4Mdia m_Media;
};

/******************************************/
/* moov  
/*                                       
/******************************************/
class CMp4Moov : public CMp4Box			 
{
public:
	 CMp4Moov ();
	 ~CMp4Moov ();
	 virtual int Read (FILE *pFile);
	 virtual int Write (FILE *pFile);

	 CMp4Mvhd* GetMvhd(){ return &m_MovieHeader; }

public:	
	int m_nTrackCount;
	CMp4Mvhd m_MovieHeader;
	std::vector<CMp4Trak *> m_lstTrack;// one audio 3 video 1 memo
};


class CMp4Mdat : public CMp4Box			//mdat
{
public:
	 CMp4Mdat ();
	 ~CMp4Mdat ();
	 virtual int Read (FILE *pFile);
	 virtual int Write (FILE *pFile);
protected:	
	char *m_pData;

};

class CMp4Ftyp : public CMp4Box
{
public:
	CMp4Ftyp ();
	~CMp4Ftyp ();
	virtual int Read (FILE *pFile);
	virtual int Write (FILE *pFile);
protected:	
	int m_nCount;
	unsigned int m_nMajor;
	unsigned int m_nMinor;
	unsigned int m_nCompatible[10];
};

#endif
