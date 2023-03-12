/* SmallSoft Decoder Pack 
 * Copyright (c) 2023,  MSDN.WhiteKnight (https://github.com/smallsoft-rus/decoder-pack) 
 * License: BSD 2.0 */
typedef 
struct{
	TCHAR internal_name[10];
	TCHAR guid[45];
	TCHAR display_name[256];
	TCHAR file[MAX_PATH];
	bool IsEnabled;
	bool HasProperties;
	TCHAR formats[50];
	TCHAR info[300];
	}
CODEC_INFO;

typedef struct {
	TCHAR ObjectName[500];
	TCHAR FilePath[500];
}FILTER_INFORMATION;

#define MAX_CODECS_IN_TABLE 10

BOOL GetCodecInfo(WCHAR* name,CODEC_INFO* pinfo);
void FillCodecTable();
void UpdateCodecState(UINT i);
void UpdateCodecInformation(CODEC_INFO* pinfo);
void Command_DisableCodec(UINT i);
void Command_EnableCodec(UINT i);
BOOL ShowFilterProperties(IBaseFilter* pFilter);
HRESULT CreateFilter(IBaseFilter **ppF,TCHAR* strGUID);
IBaseFilter* FindFilter(TCHAR* FilterName,const CLSID cat);
