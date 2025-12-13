/* SmallSoft Multimedia Control Panel
 * Copyright (c) 2025, SmallSoft (https://github.com/smallsoft-rus/) 
 * License: BSD 2.0 */
#include <windows.h>
#define STRSAFE_NO_DEPRECATE
#include <strsafe.h>
#include <stdio.h>
#include <stdlib.h>
#include <dshow.h>
#include <string.h>
#include <commctrl.h>
#include "resource.h"
#include "fil_data.h"

#include "struct.h"

HRESULT GetFilters(CLSID clsidCat);
HRESULT DisplayFilterPropertiesText(TCHAR* FilterName);
CODEC_TYPE RegFindFilter(const TCHAR* name,TCHAR* out,int ch);
BOOL RegFilterInformation(const TCHAR* guid,FILTER_INFORMATION* lpinfo);
void RegDeleteFilter(const TCHAR* guid);
void RegDeleteDMO(const TCHAR* guid);
BOOL CALLBACK ControlDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
DWORD GetFilterMerit(const TCHAR* name);
void SetFilterMerit(const TCHAR* name,DWORD dwMerit);
BOOL ParseBinaryData(BYTE* p,UINT c,REGFILTER2* data);
HRESULT GetMerit(IPropertyBag *pPropBag, DWORD *pdwMerit);
DWORD mapper_GetFilterMerit(TCHAR* name) ;

extern TCHAR CurrentFilterGUID[500];
extern TCHAR CurrentFilterName[500];
extern FILTER_INFORMATION FilterInformation;
extern HWND hMainWnd;
extern bool fHideSystemFilters;
extern HWND hProgressDlg;
extern HWND hProgressBar;
extern bool fStopFlag;
extern DWORD CurrentFilterMerit;
extern CODEC_TYPE CurrentFilterType;

TCHAR LastFilter[200]=L"";
static const GUID IID_IAMFilterData = {
  0x97f7c4d4, 0x547b, 0x4a5f, { 0x83,0x32, 0x53,0x64,0x30,0xad,0x2e,0x4d }
 };

const CAT_TABLE_ENTRY CatTable[]={	
{L"Audio Compressors ",     &CLSID_AudioCompressorCategory, L"{33D9A761-90C8-11d0-BD43-00A0C911CE86}" },
{L"Device Control Filters ",&CLSID_DeviceControlCategory,   L"{083863F1-70DE-11d0-BD40-00A0C911CE86}" },
{L"DirectShow Filters ",    &CLSID_LegacyAmFilterCategory , L"{083863F1-70DE-11d0-BD40-00A0C911CE86}" },
{L"Video Compressors ",     &CLSID_VideoCompressorCategory ,L"{33D9A760-90C8-11d0-BD43-00A0C911CE86}" },
{L"WDM Streaming VBI Codecs" ,&AM_KSCATEGORY_VBICODEC ,     L"{083863F1-70DE-11d0-BD40-00A0C911CE86}"}
};

HWND hFilterList=NULL;
HWND hCategoryList=NULL;
CLSID CurrentCategory=GUID_NULL;

UINT nCurrCategory=0;
UINT nCurrFilter=0;
UINT nTotalFilters=0;

void CurrentCatCLSID(TCHAR* out,UINT c){
int i;
i=SendMessage(hCategoryList,CB_GETCURSEL,0,0);
if(i==-1)return;
StringCchCopy(out,c,CatTable[i].strCLSID);
}

void GetMeritString(DWORD dwMerit,TCHAR* out,UINT c){
	if(out==NULL||c==0)return;
	StringCchCopy(out,c,L"");

	switch(dwMerit){
	case MERIT_PREFERRED:StringCchCopy(out,c,L"Высокий (MERIT_PREFERRED)");return;
    case MERIT_NORMAL  :StringCchCopy(out,c,L"Средний (MERIT_NORMAL)");return;
    case MERIT_UNLIKELY:StringCchCopy(out,c,L"Низкий (MERIT_UNLIKELY)");return;    
    case MERIT_DO_NOT_USE  : StringCchCopy(out,c,L"Отключен (MERIT_DO_NOT_USE)");return;
    case MERIT_SW_COMPRESSOR: StringCchCopy(out,c,L"Отключен (MERIT_SW_COMPRESSOR)");return;
    case MERIT_HW_COMPRESSOR:StringCchCopy(out,c,L"Отключен (MERIT_HW_COMPRESSOR)");return;
	case 0:StringCchCopy(out,c,L"Не определен");return;
	}

	StringCchPrintf(out,c,L"0x%08x",dwMerit);
}

DWORD GetFilterMerit(const TCHAR* name){
		HRESULT hr; 
		DWORD dwMerit=0;
// Create the System Device Enumerator.
ICreateDevEnum *pSysDevEnum = NULL;
hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
    IID_ICreateDevEnum, (void **)&pSysDevEnum);
if (FAILED(hr))
{    return dwMerit;}

// Obtain a class enumerator for the dshow filters category.
IEnumMoniker *pEnumCat = NULL;
hr = pSysDevEnum->CreateClassEnumerator(CurrentCategory, &pEnumCat, 0);

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
				if(wcscmp(name,varName.bstrVal)==0){
					hr=GetMerit(pPropBag,&dwMerit);
								
			VariantClear(&varName);
			pPropBag->Release();
			pMoniker->Release();
			pEnumCat->Release();
			pSysDevEnum->Release();
			return dwMerit;
				}
			}
			
            pPropBag->Release();
        }
        pMoniker->Release();
    }
    pEnumCat->Release();
}
pSysDevEnum->Release();
return dwMerit; 
}

void ProcessMessages(){
	MSG msg;
	while(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

BOOL CheckFilterName(const TCHAR* name){
const TCHAR* p=NULL;
const TCHAR* q=NULL;
p=wcsstr(name,TEXT("ASF"));
q=wcsstr(name,TEXT("Handler"));
if(p!=NULL&&q!=NULL)return FALSE;
p=wcsstr(name,TEXT("ASX"));
q=wcsstr(name,TEXT("Parser"));
if(p!=NULL&&q!=NULL)return FALSE;
p=wcsstr(name,TEXT("File Source"));
q=wcsstr(name,TEXT("URL"));
if(p!=NULL&&q!=NULL)return FALSE;

if(lstrcmpi(name,L"File Source (Async.)")==0)return FALSE;
if(lstrcmpi(name,L"File Writer")==0)return FALSE;
if(lstrcmpi(name,L"Infinite Pin Tee Filter")==0)return FALSE;
if(lstrcmpi(name,L"9x8Resize")==0)return FALSE;
if(lstrcmpi(name,L"Allocator Fix")==0)return FALSE;
if(lstrcmpi(name,L"AVI Draw")==0)return FALSE;
if(lstrcmpi(name,L"Bitmap Generate")==0)return FALSE;
if(lstrcmpi(name,L"Color Space Converter")==0)return FALSE;
if(lstrcmpi(name,L"File Stream Renderer")==0)return FALSE;
if(lstrcmpi(name,L"Frame Eater")==0)return FALSE;
if(lstrcmpi(name,L"Internal Script Command Renderer")==0)return FALSE;
if(lstrcmpi(name,L"Multi-file Parser")==0)return FALSE;
if(lstrcmpi(name,L"Null Renderer")==0)return FALSE;
if(lstrcmpi(name,L"SampleGrabber")==0)return FALSE;
if(lstrcmpi(name,L"Microsoft MPEG-4 Video Decompressor")==0)return FALSE;
return TRUE;
}

void FillCategoryList(){
int i;int c;
c=sizeof(CatTable)/sizeof(CatTable[0]);
SendMessage(hCategoryList,CB_RESETCONTENT,0,0);
for(i=0;i<c;i++){
	SendMessage(hCategoryList,CB_ADDSTRING,0,(LPARAM)(CatTable[i].name));
}

}

void Command_GetFilters(){
int i,j;
i=SendMessage(hCategoryList,CB_GETCURSEL,0,0);
if(i==-1)return;
j=SendMessage(hFilterList,LB_GETCURSEL,0,0);

GetFilters(*(CatTable[i].clsid));
if(j!=-1)SendMessage(hFilterList,LB_SETCURSEL,j,0);
}

void Command_FilterInfo(){
int i;
BOOL res;

i=SendMessage(hFilterList,LB_GETCURSEL,0,0);
if(i==-1){MessageBox(hMainWnd,L"Не выбран кодек",0,0);return;}
SendMessage(hFilterList,LB_GETTEXT,i,(LPARAM)CurrentFilterName);
if(lstrcmpi(CurrentFilterName,L"")==0)return;
//FIND FILTER CLSID
CurrentFilterType=RegFindFilter(CurrentFilterName,CurrentFilterGUID,256);
if(CurrentFilterType==0){MessageBox(0,L"Невозможно активировать управление для данного кодека:\nТип кодека не поддерживается",0,MB_OK|MB_ICONERROR);return;}
CurrentFilterMerit=mapper_GetFilterMerit(CurrentFilterName);

res=RegFilterInformation(CurrentFilterGUID,&FilterInformation);
if(res==FALSE){MessageBox(0,L"Невозможно активировать управление для данного кодека:\nДанные в реестре повреждены",0,MB_OK|MB_ICONERROR);return;}

DialogBoxW(GetModuleHandle(NULL),MAKEINTRESOURCE(IDD_CONTROL),hMainWnd,ControlDlgProc);

}

void AddElement(TCHAR* str){
	SendMessage(hFilterList,LB_ADDSTRING,0,(LPARAM)str);
}

void Clear(){
	SendMessage(hFilterList,LB_RESETCONTENT,0,0);
}

HRESULT GetFilters(CLSID clsidCat){
	// Create the System Device Enumerator.
HRESULT hr;
BOOL show;
ICreateDevEnum *pSysDevEnum = NULL;

Clear(); //delete existing entries
CurrentCategory=GUID_NULL; //no category
hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
    IID_ICreateDevEnum, (void **)&pSysDevEnum);
if (FAILED(hr))
{
    return hr;
}

// Obtain a class enumerator 
IEnumMoniker *pEnumCat = NULL;
hr = pSysDevEnum->CreateClassEnumerator(clsidCat, &pEnumCat, 0);

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
			if(fHideSystemFilters==false){show=TRUE;}
			else {show=FALSE;}
            if (SUCCEEDED(hr))
			{	
				if(CheckFilterName(varName.bstrVal)!=FALSE){show=TRUE;}
				if(show!=FALSE){	AddElement(varName.bstrVal);}//Add new entry
				
            }
            VariantClear(&varName);
            pPropBag->Release();
        }
        pMoniker->Release();
    }
    pEnumCat->Release();
}
pSysDevEnum->Release();
CurrentCategory=clsidCat;
return S_OK;
}

IBaseFilter* FindFilter(TCHAR* FilterName){
	HRESULT hr; 
// Create the System Device Enumerator.
ICreateDevEnum *pSysDevEnum = NULL;
hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
    IID_ICreateDevEnum, (void **)&pSysDevEnum);
if (FAILED(hr))
{    return NULL;}

// Obtain a class enumerator for the dshow filters category.
IEnumMoniker *pEnumCat = NULL;
hr = pSysDevEnum->CreateClassEnumerator(CurrentCategory, &pEnumCat, 0);

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
				if(wcscmp(FilterName,varName.bstrVal)==0){
			
            // To create an instance of the filter, do the following:
            IBaseFilter *pFilter;
            hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter,
                (void**)&pFilter);
            
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

void Command_FilterProperties(){
int i;
BOOL res;
IBaseFilter* pFilter=NULL;

i=SendMessage(hFilterList,LB_GETCURSEL,0,0);
if(i==-1)return;
SendMessage(hFilterList,LB_GETTEXT,i,(LPARAM)CurrentFilterName);
if(lstrcmpi(CurrentFilterName,L"")==0)return;
pFilter=FindFilter(CurrentFilterName);
if(pFilter==NULL){MessageBox(hMainWnd,L"Произошла ошибка",NULL,MB_OK|MB_ICONERROR);return;}

res=ShowFilterProperties(pFilter);
if(res==FALSE){MessageBox(hMainWnd,L"Кодек не имеет настраиваемых свойств",NULL,MB_OK|MB_ICONINFORMATION);}

pFilter->Release();
}

BOOL IsCodecIncorrect(FILTER_INFORMATION& FilterInformation){
	HANDLE hSearch;
	WIN32_FIND_DATA wd={0};
	TCHAR* p=NULL;

	p=wcsstr(FilterInformation.FilePath,L"%");
	if(p!=NULL){return FALSE;}
	hSearch=FindFirstFile(FilterInformation.FilePath,&wd);
if(hSearch==INVALID_HANDLE_VALUE){
	return TRUE;}
else{
FindClose(hSearch);
return FALSE;}
}

void UpdateStatus(){
TCHAR buf[256]=L"";
UINT Percent=0;
StringCchPrintf(buf,256,L"%u / %u",nCurrCategory,sizeof(CatTable)/sizeof(CatTable[0]));
SetDlgItemText(hProgressDlg,IDC_CAT,buf);
StringCchPrintf(buf,256,L"%u / %u",nCurrFilter,nTotalFilters);
StringCchCat(buf,256,L" - ");
StringCchCat(buf,256,LastFilter);
SetDlgItemText(hProgressDlg,IDC_CODEC,buf);
Percent=(UINT)((nCurrFilter/(float)nTotalFilters)*100.0f);
SendMessage(hProgressBar,PBM_SETPOS,Percent,0);
}

void SearchIncorrectCodecs(){
FILTER_INFORMATION fi={0};
TCHAR buf[500]=L"";
TCHAR guid[256]=L"";
BOOL res;
int dr;

for(nCurrCategory=0;nCurrCategory<(sizeof(CatTable)/sizeof(CatTable[0]));nCurrCategory++){
	SendMessage(hCategoryList,CB_SETCURSEL,nCurrCategory,0);
	//CurrentCategory=*(CatTable[nCurrCategory].clsid);
	Command_GetFilters();
	nTotalFilters=SendMessage(hFilterList,LB_GETCOUNT,0,0);
	for(nCurrFilter=0;nCurrFilter<nTotalFilters;nCurrFilter++){
		
		ProcessMessages();
		if(fStopFlag==true){fStopFlag=false;goto end;}
		
		
SendMessage(hFilterList,LB_GETTEXT,nCurrFilter,(LPARAM)buf);
if(lstrcmpi(buf,L"")==0){continue;}
StringCchCopy(LastFilter,200,buf);//to disp
UpdateStatus();

//FIND FILTER CLSID
res=RegFindFilter(buf,guid,256);
if(res==FALSE){continue;}

res=RegFilterInformation(guid,&fi);
if(res==FALSE){/*ther's an error*/
	dr=MessageBox(hMainWnd,L"Найден некорректно зарегистрированный кодек. Удалить его из реестра?",buf,MB_ICONQUESTION|MB_YESNO);
	if(dr==IDNO)continue;
	RegDeleteFilter(guid);
	continue;
}

res=IsCodecIncorrect(fi);
if(res==FALSE){continue;}

	SendMessage(hFilterList,LB_SETCURSEL,nCurrFilter,0);
	dr=MessageBox(hMainWnd,L"Кодек, возможно, зарегистрирован некорректно. Активировать управление?",fi.ObjectName,MB_ICONQUESTION|MB_YESNO);
	if(dr==IDNO)continue;
	Command_FilterInfo();

	}
}
end:;
}
