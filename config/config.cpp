/* SmallSoft Decoder Pack 
 * Copyright (c) 2023,  MSDN.WhiteKnight (https://github.com/smallsoft-rus/decoder-pack) 
 * License: BSD 2.0 */
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#define STRSAFE_NO_DEPRECATE
#include <strsafe.h>
#include <dshow.h>
#include "config.h"

//global
CODEC_INFO CodecTable[MAX_CODECS_IN_TABLE]={0};
UINT CountCodecs=0;
const TCHAR CodecsRegKey[]=L"Software\\SmallSoft\\Codecs\\";
extern HWND hMainWnd;

void GetFileDirectory(wchar_t* path,wchar_t* out){
    int i=0;
    int k=0;
    int LastIndex=0;
	TCHAR buf[MAX_PATH];
	StringCchCopy(buf,MAX_PATH,path);
    while(1){
       if(buf[i]==L'\\')LastIndex=i;
       if(buf[i]==0)break;
       i++;
       }
   lstrcpy(out,L"");
   buf[LastIndex+1]=0;
   lstrcat(out,buf);

  }

void GetCodecsDirectory(TCHAR* dir){
TCHAR buf[MAX_PATH]=L"";
int c;
GetModuleFileName(NULL,buf,MAX_PATH);
GetFileDirectory(buf,dir);
c=lstrlen(dir);
if(dir[c-1]!=L'\\'&&dir[c-1]!=L'/'){
	dir[c]=L'\\';
	dir[c+1]=0;
}
}





void GetCodecDefFile(TCHAR* name,TCHAR* out,UINT c){
if(c==0||out==NULL||name==NULL)return;
StringCchCopy(out,c,L"");
if(lstrcmpi(name,L"bass")==0){
	StringCchCopy(out,c,L"DC-Bass_Source\\DCBassSource.ax");
	return;
}
if(lstrcmpi(name,L"aac")==0){
	StringCchCopy(out,c,L"AAC\\CoreAAC.ax");
	return;
}
if(lstrcmpi(name,L"ds_audio")==0){
	StringCchCopy(out,c,L"dscaler\\MpegAudio.dll");
	return;
}
if(lstrcmpi(name,L"ds_video")==0){
	StringCchCopy(out,c,L"dscaler\\MpegVideo.dll");
	return;
}
if(lstrcmpi(name,L"divx_h264")==0){
	StringCchCopy(out,c,L"divx\\DivXDecH264.ax");
	return;
}
if(lstrcmpi(name,L"divx")==0){
	StringCchCopy(out,c,L"divx\\DivXDec.ax");
	return;
}
if(lstrcmpi(name,L"xvid")==0){
	StringCchCopy(out,c,L"xvid.ax");
	return;
}
if(lstrcmpi(name,L"mkv")==0){
	StringCchCopy(out,c,L"splitter\\MatroskaSplitter.ax");
	return;
}
if(lstrcmpi(name,L"mpeg")==0){
	StringCchCopy(out,c,L"splitter\\MpegSplitter.ax");
	return;
}

}


//CLSID_LegacyAmFilterCategory
IBaseFilter* FindFilter(TCHAR* FilterName,const CLSID cat){
	HRESULT hr; 
// Create the System Device Enumerator.
ICreateDevEnum *pSysDevEnum = NULL;
hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
    IID_ICreateDevEnum, (void **)&pSysDevEnum);
if (FAILED(hr))
{    return NULL;}

// Obtain a class enumerator for the dshow filters category.
IEnumMoniker *pEnumCat = NULL;
hr = pSysDevEnum->CreateClassEnumerator(cat, &pEnumCat, 0);

if (hr == S_OK) 
{	
    // Enumerate the monikers.
    IMoniker *pMoniker = NULL;
    ULONG cFetched;
    while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
    {
        IPropertyBag *pPropBag;
        hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, 
            (void **)&pPropBag);
        if (SUCCEEDED(hr))
        {
            // To retrieve the filter's friendly name, do the following:
            VARIANT varName;
            VariantInit(&varName);
            hr = pPropBag->Read(L"FriendlyName", &varName, 0);
            if (SUCCEEDED(hr)){
				if(lstrcmpi(FilterName,varName.bstrVal)==0){
					
            // To create an instance of the filter, do the following:
            IBaseFilter *pFilter;
            hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter,
                (void**)&pFilter);
            // Now add the filter to the graph. 
            //Remember to release pFilter later.
			VariantClear(&varName);
			pPropBag->Release();
			pMoniker->Release();
			pEnumCat->Release();
			pSysDevEnum->Release();
			return pFilter;

				}
			}
			
            pPropBag->Release();
        }
        pMoniker->Release();
    }
    pEnumCat->Release();
}
pSysDevEnum->Release();
return NULL; 

}




HRESULT CreateFilter(IBaseFilter **ppF,TCHAR* strGUID)      // Receives a pointer to the filter.
{
	GUID clsid=GUID_NULL;      // CLSID of the filter to create.
	TCHAR str[60]=L"";
    if ( ! ppF) return E_POINTER;
    *ppF = 0;
    IBaseFilter *pF = 0;
	

	StringCchCopy(str,60,strGUID);
	_wcslwr(str);
	
	CLSIDFromString(str,&clsid);
	
    HRESULT hr = CoCreateInstance(clsid, 0, CLSCTX_INPROC_SERVER,
        IID_IBaseFilter, reinterpret_cast<void**>(&pF));
    if (SUCCEEDED(hr)) {   *ppF = pF;}
	else {*ppF=NULL;}
    return hr;
}

BOOL ShowFilterProperties(IBaseFilter* pFilter){
ISpecifyPropertyPages *pProp=NULL;
if(pFilter==NULL)return FALSE;
HRESULT hr2=E_FAIL;
FILTER_INFO FilterInfo={0};
CAUUID caGUID={0};
caGUID.cElems=0;caGUID.pElems=NULL;
FilterInfo.pGraph=NULL;
IUnknown *pFilterUnk=NULL;
HRESULT hr = pFilter->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pProp);
if (SUCCEEDED(hr)&&pProp!=NULL) 
{
    // Get the filter's name and IUnknown pointer.
	
	FilterInfo.pGraph=NULL;
	StringCchCopy(FilterInfo.achName,5,L".");
    hr = pFilter->QueryFilterInfo(&FilterInfo);    
    hr2=pFilter->QueryInterface(IID_IUnknown, (void **)&pFilterUnk);
	if(FAILED(hr2)||pFilterUnk==NULL){goto exit;}

    // Show the page. 
	
	hr=pProp->GetPages(&caGUID);
	if(FAILED(hr)||caGUID.cElems==0||caGUID.pElems==NULL){goto exit;}
    if(pProp!=NULL){pProp->Release();pProp=NULL;}
    OleCreatePropertyFrame(
        hMainWnd,                   // Parent window
        0, 0,                   // Reserved
        FilterInfo.achName,     // Caption for the dialog box
        1,                      // Number of objects (just the filter)
        &pFilterUnk,            // Array of object pointers. 
        caGUID.cElems,          // Number of property pages
        caGUID.pElems,          // Array of property page CLSIDs
        0,                      // Locale identifier
        0, NULL                 // Reserved
    );
    // Clean up.
    if(pFilterUnk!=NULL){pFilterUnk->Release();pFilterUnk=NULL;}
	if(FilterInfo.pGraph!=NULL){FilterInfo.pGraph->Release();} 
	if(caGUID.pElems!=NULL){CoTaskMemFree(caGUID.pElems);}
	return TRUE;
}

exit:
if(pProp!=NULL){pProp->Release();pProp=NULL;}
if(FilterInfo.pGraph!=NULL){FilterInfo.pGraph->Release();} 
if(caGUID.pElems!=NULL){CoTaskMemFree(caGUID.pElems);}
if(pFilterUnk!=NULL){pFilterUnk->Release();pFilterUnk=NULL;}
return FALSE;
}





BOOL GetCodecInfo(WCHAR* name,CODEC_INFO* pinfo){
 LONG lRes;
 HKEY hKey;
 WCHAR buf[400];
 WCHAR key_name[256];
 DWORD size=sizeof(buf);
 BYTE b=0;

 size=sizeof(buf);
  StringCchCopy(key_name,256,CodecsRegKey);
StringCchCat(key_name,256,name);
StringCchCat(key_name,256,L"\\");
//MessageBox(0,key_name,0,0);
 lRes=RegOpenKeyW(HKEY_LOCAL_MACHINE,key_name,&hKey);
 if(lRes==ERROR_SUCCESS){
   lRes=RegQueryValueEx(hKey,NULL,NULL,NULL,(LPBYTE)buf,&size);
		if(lRes!=ERROR_SUCCESS){
			StringCchCopy(buf,400,name);
		}
	StringCchCopy(pinfo->internal_name,10,name);
	StringCchCopy(pinfo->display_name,256,buf);
	size=1;
	lRes=RegQueryValueEx(hKey,L"IsEnabled",NULL,NULL,(LPBYTE)&b,&size);
		if(lRes!=ERROR_SUCCESS){
			b=(BYTE)FALSE;
		}
	pinfo->IsEnabled=(bool)(int)b;
	size=1;
	lRes=RegQueryValueEx(hKey,L"HasProperties",NULL,NULL,(LPBYTE)&b,&size);
		if(lRes!=ERROR_SUCCESS){
			b=(BYTE)FALSE;
		}
	pinfo->HasProperties=(bool)(int)b;
	size=sizeof(buf);
	lRes=RegQueryValueEx(hKey,L"File",NULL,NULL,(LPBYTE)buf,&size);
		if(lRes!=ERROR_SUCCESS){
			StringCchCopy(buf,400,L"");
		}
	StringCchCopy(pinfo->file,MAX_PATH,buf);
	size=sizeof(buf);
	lRes=RegQueryValueEx(hKey,L"GUID",NULL,NULL,(LPBYTE)buf,&size);
		if(lRes!=ERROR_SUCCESS){
			StringCchCopy(buf,400,L"");
		}
	StringCchCopy(pinfo->guid,45,buf);
	size=sizeof(buf);
	lRes=RegQueryValueEx(hKey,L"formats",NULL,NULL,(LPBYTE)buf,&size);
		if(lRes!=ERROR_SUCCESS){
			StringCchCopy(buf,400,L"");
		}
	StringCchCopy(pinfo->formats,50,buf);
	size=sizeof(buf);
	lRes=RegQueryValueEx(hKey,L"info",NULL,NULL,(LPBYTE)buf,&size);
		if(lRes!=ERROR_SUCCESS){
			StringCchCopy(buf,400,L"");
		}
	StringCchCopy(pinfo->info,300,buf);
	RegCloseKey(hKey);
	return TRUE;
 }
 else
 {
	 return FALSE;
 }
}
void FillCodecTable(){
BOOL res;
memset(CodecTable,0,sizeof(CodecTable));
res=GetCodecInfo(L"bass",&(CodecTable[CountCodecs]));
if(res!=FALSE){CountCodecs++;}
res=GetCodecInfo(L"aac",&CodecTable[CountCodecs]);
if(res!=FALSE){CountCodecs++;}
res=GetCodecInfo(L"ds_audio",&CodecTable[CountCodecs]);
if(res!=FALSE){CountCodecs++;}
res=GetCodecInfo(L"ds_video",&CodecTable[CountCodecs]);
if(res!=FALSE){CountCodecs++;}
res=GetCodecInfo(L"divx_h264",&CodecTable[CountCodecs]);
if(res!=FALSE){CountCodecs++;}
res=GetCodecInfo(L"divx",&CodecTable[CountCodecs]);
if(res!=FALSE){CountCodecs++;}
res=GetCodecInfo(L"xvid",&CodecTable[CountCodecs]);
if(res!=FALSE){CountCodecs++;}
res=GetCodecInfo(L"mkv",&CodecTable[CountCodecs]);
if(res!=FALSE){CountCodecs++;}
res=GetCodecInfo(L"mpeg",&CodecTable[CountCodecs]);
if(res!=FALSE){CountCodecs++;}
}

BOOL RegFilterInformation(const TCHAR* guid,FILTER_INFORMATION* lpinfo){
HKEY hKey=NULL;
LONG lRes;
TCHAR key_name[500]=L"";
TCHAR buf[500];
TCHAR data[500];
DWORD size=sizeof(data);
DWORD size2;

StringCchCopy(key_name,500,L"CLSID\\");
StringCchCat(key_name,500,guid);
lRes=RegOpenKeyW(HKEY_CLASSES_ROOT,key_name,&hKey);

if(lRes!=ERROR_SUCCESS){/*ErrorMessage(lRes);*/return FALSE;}
size=sizeof(data);
lRes=RegQueryValueEx(hKey,NULL,NULL,NULL,(LPBYTE)data,&size);
if(lRes==ERROR_SUCCESS){StringCchCopy(lpinfo->ObjectName,500,data);}	
RegCloseKey(hKey);
StringCchCat(key_name,500,L"\\InprocServer32");
lRes=RegOpenKeyW(HKEY_CLASSES_ROOT,key_name,&hKey);
if(lRes!=ERROR_SUCCESS){return TRUE;}
size=sizeof(data);
lRes=RegQueryValueEx(hKey,NULL,NULL,NULL,(LPBYTE)data,&size);
if(lRes==ERROR_SUCCESS){StringCchCopy(lpinfo->FilePath,500,data);}	
RegCloseKey(hKey);
return TRUE;
}



void Command_EnableCodec(UINT i){
STARTUPINFO si;
    PROCESS_INFORMATION pi;
TCHAR cmd[500]=L"";
StringCchCopy(cmd,500,L"regsvr32 \"");
	StringCchCat(cmd,500,CodecTable[i].file);
	StringCchCat(cmd,500,L"\"");
	//MessageBox(0,cmd,L"reg",0);
	ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );
	CreateProcess(NULL,cmd,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
	WaitForInputIdle(pi.hProcess,4000);
	CodecTable[i].IsEnabled=true;
	UpdateCodecInformation(&CodecTable[i]);
}

void Command_DisableCodec(UINT i){
STARTUPINFO si;
    PROCESS_INFORMATION pi;
TCHAR cmd[500]=L"";
StringCchCopy(cmd,500,L"regsvr32 /u \"");
	StringCchCat(cmd,500,CodecTable[i].file);
	StringCchCat(cmd,500,L"\"");
	//MessageBox(0,cmd,L"reg",0);
	ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );
	CreateProcess(NULL,cmd,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
	WaitForInputIdle(pi.hProcess,4000);
	CodecTable[i].IsEnabled=false;
	UpdateCodecInformation(&CodecTable[i]);
}

void RegisterCodec(TCHAR* file){
STARTUPINFO si;
    PROCESS_INFORMATION pi;
TCHAR cmd[500]=L"";
StringCchCopy(cmd,500,L"regsvr32 /s \"");
	StringCchCat(cmd,500,file);
	StringCchCat(cmd,500,L"\"");
	//MessageBox(0,cmd,L"reg",0);
	ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );
	CreateProcess(NULL,cmd,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
	WaitForInputIdle(pi.hProcess,4000);

}

void UnregisterCodec(TCHAR* file){
STARTUPINFO si;
    PROCESS_INFORMATION pi;
TCHAR cmd[500]=L"";
StringCchCopy(cmd,500,L"regsvr32 /s /u \"");
	StringCchCat(cmd,500,file);
	StringCchCat(cmd,500,L"\"");
	ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );
	CreateProcess(NULL,cmd,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
	WaitForInputIdle(pi.hProcess,4000);

}

BOOL IsFileExists(TCHAR* file){
HANDLE hSearch=NULL;
WIN32_FIND_DATA wd={0};

hSearch=FindFirstFile(file,&wd);
if(hSearch==INVALID_HANDLE_VALUE){
	return FALSE;
}
else{
	FindClose(hSearch);
	return TRUE;
}

}


void UpdateCodecState(UINT i){
BOOL reg=FALSE;
BOOL file_exists=FALSE;
BOOL file2_exists=FALSE;
BOOL eq=FALSE;
FILTER_INFORMATION info={0};
TCHAR buf[MAX_PATH]=L"";
TCHAR buf2[100]=L"";

reg=RegFilterInformation(CodecTable[i].guid,&info);
//MessageBox(0,info.FilePath,info.ObjectName,0);
if(CodecTable[i].IsEnabled==true){
	if(reg==FALSE){
		//reg codec
		file_exists=IsFileExists(CodecTable[i].file);
		if(file_exists==FALSE){
			////critical error!
			GetCodecsDirectory(buf);
			GetCodecDefFile(CodecTable[i].internal_name,buf2,100);
			StringCchCat(buf,MAX_PATH,buf2);
			StringCchCopy(CodecTable[i].file,MAX_PATH,buf);
		}
		RegisterCodec(CodecTable[i].file);
	}//else check file
	else{
		file_exists=IsFileExists(CodecTable[i].file);
		file2_exists=IsFileExists(info.FilePath);
		eq=(lstrcmpi(CodecTable[i].file,info.FilePath)==0);
		if(file_exists!=FALSE&&eq!=FALSE)return;//OK
		if(file_exists==FALSE&&eq!=FALSE){
			////critical error!
		GetCodecsDirectory(buf);
		GetCodecDefFile(CodecTable[i].internal_name,buf2,100);
		StringCchCat(buf,MAX_PATH,buf2);
		//MessageBox(0,buf,0,0);
		if(IsFileExists(buf)==true){
			StringCchCopy(CodecTable[i].file,MAX_PATH,buf);
			RegisterCodec(buf);
		}
		else{CodecTable[i].IsEnabled=false;}
			return;
		}
		//***if not equals***
		if(file2_exists==FALSE)//should register with new file
		{/*MessageBox(0,L"++",0,0);*/RegisterCodec(CodecTable[i].file);return;}
		if(file_exists==FALSE&&file2_exists!=FALSE)//now obtain actual file
		{StringCchCopy(CodecTable[i].file,MAX_PATH,info.FilePath);return;}
	}

}else{
	if(reg!=FALSE){
		//unreg codec
		UnregisterCodec(CodecTable[i].file);
	}//else do nothing

}

}



void UpdateCodecInformation(CODEC_INFO* pinfo){
LONG lRes;
 HKEY hKey;
 WCHAR buf[400];
 WCHAR key_name[256];
 DWORD size=sizeof(buf);
 BYTE b=0;
StringCchCopy(key_name,256,CodecsRegKey);
StringCchCat(key_name,256,pinfo->internal_name);
StringCchCat(key_name,256,L"\\");
//MessageBox(0,key_name,0,0);
 lRes=RegOpenKeyW(HKEY_LOCAL_MACHINE,key_name,&hKey);
if(lRes!=ERROR_SUCCESS){
			return;
		}
RegSetValueEx(hKey,NULL,0,REG_SZ,(LPBYTE)&(pinfo->display_name),sizeof(pinfo->display_name));
b=(BYTE)(int)pinfo->IsEnabled;
RegSetValueEx(hKey,L"IsEnabled",0,REG_BINARY,(LPBYTE)&(b),1);
b=(BYTE)(int)pinfo->HasProperties;
RegSetValueEx(hKey,L"HasProperties",0,REG_BINARY,(LPBYTE)&(b),1);		
RegSetValueEx(hKey,L"File",0,REG_SZ,(LPBYTE)&(pinfo->file),sizeof(pinfo->file));
RegSetValueEx(hKey,L"GUID",0,REG_SZ,(LPBYTE)&(pinfo->guid),sizeof(pinfo->guid));
RegSetValueEx(hKey,L"formats",0,REG_SZ,(LPBYTE)&(pinfo->formats),sizeof(pinfo->formats));
//RegSetValueEx(hKey,L"info",0,REG_SZ,(LPBYTE)&(pinfo->info),sizeof(pinfo->info));

RegCloseKey(hKey);




}




