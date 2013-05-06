
#include "stdio.h"
#include <string>
#include <stdlib.h>
#include <tchar.h>
#include "Mp4Box.h"
#include "Mp4RW.h"


#pragma warning(disable:4200) 


int _tmain(int argc, _TCHAR* argv[])
{

	char szPach[255];
	memset(szPach,0, 255 );
	Mp4FileRW * pHandle = new Mp4FileRW;
	if ( pHandle )  // sample_100kbit
	{
		pHandle->Open( "E:\\test\\sample_100kbit.mp4", OPEN_MODEL_RB);
	}

	// Ð´ÎÄ¼þ
	pHandle->Open( "E:\\test\\test_sample_100kbit.mp4", OPEN_MODEL_WB);
	pHandle->Close();

	system("pause");
	return 0;
}
