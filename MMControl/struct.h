/* SmallSoft Multimedia Control Panel
 * Copyright (c) 2025, SmallSoft (https://github.com/smallsoft-rus/) 
 * License: BSD 2.0 */
typedef enum{
    CODEC_LEGACY=0, CODEC_DSHOW, CODEC_DMO
}CODEC_TYPE;

typedef struct{
	const TCHAR name[256];
	const CLSID* clsid;
	const TCHAR strCLSID[50];
}CAT_TABLE_ENTRY;

typedef struct {
	TCHAR ObjectName[500];
	TCHAR FilePath[500];
}FILTER_INFORMATION;
