#include "Single.h"


Single::Single(void)
{
}

Single::~Single(void)
{
}


int Single::PrintBoxHeadInfo(MP4_HEADER &  Header, const char *pTypeName )
{
	int nSize = Single::Instance().ToUInt32lit( Header.nSize );
	int nBoxDataSize = nSize - 8;
	char p[5];
	UInt2PrintChar( Header.nType, p );
	printf("\t\t %s subBox : %s( size = %d) \n", pTypeName, p, nSize-8);
	return nSize;
}
unsigned int Single::ToUInt32lit(unsigned int nSize)
{
	bigEnd.size = nSize;
	for(int i = 0, j = 3; i < 4; i++, j--)
		smallEnd.byte[i] = bigEnd.byte[j];

	return smallEnd.size;
}

void Single::UInt2PrintChar( unsigned int nData , char*p)
{
	memset(p, 0, 5 );
	memcpy(p, &nData, 4 );
}

uint16_t Single::read_uint16_big(FILE *f)
{
	uint16_t k = 0;
	fread(&k, sizeof(k), 1, f);
	return k;
}

uint16_t Single::read_uint16_lit(FILE *f)
{
	uint16_t k;
	fread(&k, sizeof(k), 1, f);
	return ((k&0xff00) >> 8)|((k&0xff) << 8);
}



uint32_t Single::read_uint32_lit(FILE *f)
{
	uint32_t k = 0;
	fread(&k, sizeof(k), 1, f);
	bigEnd.size = k;
	for(int i = 0, j = 3; i < 4; i++, j--)
		smallEnd.byte[i] = bigEnd.byte[j];

	return smallEnd.size;
}

uint8_t Single::read_uint8(FILE *f)
{
	uint8_t x;
	fread(&x, sizeof(x), 1, f);
	return x;
}