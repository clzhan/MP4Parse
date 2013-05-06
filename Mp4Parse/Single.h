#ifndef _SINGLE_
#define _SINGLE_

#include "Utls.h"
#include <string>

#include "Mp4Box.h"

#ifndef SAFE_CLOSE_FILE
#define SAFE_CLOSE_FILE (p) \
{ if(p) fclose(p); p = NULL ;} 
#endif

#ifndef SAFE_ARRAYDELETE
#define SAFE_ARRAYDELETE( x )       \
	if( x )                          \
	   {                                \
	   delete [] x;                 \
	   x = NULL;                    \
	   }
#endif 

#ifndef SAFE_FREE
#define SAFE_FREE(p) {if (p) free (p); p = NULL;}
#endif

#ifndef SAFE_DEL
#define SAFE_DEL(p) {if (p) delete (p); p = NULL;}
#endif



static union __indian_swap
{
	uint8_t byte[4];
	uint32_t size;
}bigEnd, smallEnd;


class Single
{
public:
	static Single& Instance()
	{
		static Single s_instance;
		return s_instance;
	}
	
	~Single(void);

public:
	
	// 打印uint
	void UInt2PrintChar( unsigned int nData, char*p);
	int PrintBoxHeadInfo(MP4_HEADER &  Header, const char *pTypeName );

	// 大头转小头
	uint32_t ToUInt32lit(unsigned int nSize);

	uint32_t read_uint32_lit(FILE *f);
	uint16_t read_uint16_big(FILE *f);
	uint16_t read_uint16_lit(FILE *f);
	uint8 read_uint8(FILE *f);


protected:
	Single(void);

};

#endif