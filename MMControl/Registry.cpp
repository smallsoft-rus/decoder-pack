/* SmallSoft Multimedia Control Panel
 * Copyright (c) 2025, SmallSoft (https://github.com/smallsoft-rus/) 
 * License: BSD 2.0 */
#include <windows.h>
#define STRSAFE_NO_DEPRECATE
#include <strsafe.h>
#include <dshow.h>
#include "fil_data.h"
#include "struct.h"

const TCHAR strMapperCLSID[]=L"{083863F1-70DE-11d0-BD40-00A0C911CE86}";

const TCHAR strNameString[]=L"FriendlyName";

void ErrorMessage(LONG code) ;
BOOL ParseBinaryData(BYTE* p,UINT c,REGFILTER2* data);
ULONG PackBinaryData(BYTE* p,ULONG c,REGFILTER2* data);
UINT mapper_FilterMeritBinaryData(const TCHAR* name,DWORD merit,BYTE* pdata,UINT c);
void CurrentCatCLSID(TCHAR* out,UINT c);

void SetFilterMerit(const TCHAR* name,DWORD dwMerit){
HKEY hKeyCLSID;
HKEY hKeyFilter=NULL;
LONG lRes;
DWORD i=-1;
TCHAR key_name[256]=L"";
TCHAR buf[500];
TCHAR data[500];
TCHAR strFiltersKey[500]=L"";
DWORD size=sizeof(data);
DWORD size2;

REGFILTER2 rgf={0};
BOOL b;
BYTE* p=NULL;

BYTE* pd=NULL;
BYTE* pointer=NULL;
HRESULT hr;

StringCchCopy(strFiltersKey,500,L"CLSID\\");
CurrentCatCLSID(buf,500);
StringCchCat(strFiltersKey,500,buf);
StringCchCat(strFiltersKey,500,L"\\Instance\\");

lRes=RegOpenKeyW(HKEY_CLASSES_ROOT,strFiltersKey,&hKeyCLSID);
if(lRes!=ERROR_SUCCESS){
	MessageBox(0,L"Не удалось получить доступ к HKEY_CLASSES_ROOT:Возможно, у вас нет прав доступа к реестру\n",0,MB_OK|MB_ICONERROR);
	return;
}

while(1){
	i++;
	size2=256;
	lRes=RegEnumKeyEx(hKeyCLSID,i,key_name,&size2,NULL,NULL,NULL,NULL);
	if(lRes!=ERROR_SUCCESS){break;}
	StringCchCopy(buf,500,strFiltersKey);
	StringCchCat(buf,500,L"\\");
	StringCchCat(buf,500,key_name);
	lRes=RegOpenKeyW(HKEY_CLASSES_ROOT,buf,&hKeyFilter);
	if(lRes!=ERROR_SUCCESS)continue;
	size=sizeof(data);
	lRes=RegQueryValueEx(hKeyFilter,strNameString,NULL,NULL,(LPBYTE)data,&size);
	if(lRes!=ERROR_SUCCESS){RegCloseKey(hKeyFilter);continue;}
	if(lstrcmpi(name,data)!=0){RegCloseKey(hKeyFilter);continue;}
	//FOUND!!!
	
	size=0;
	lRes=RegQueryValueEx(hKeyFilter,L"FilterData",NULL,NULL,NULL,&size);
	if(lRes!=ERROR_SUCCESS||size==0){RegCloseKey(hKeyFilter);continue;}
	p=(BYTE*)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,size);
	if(p==NULL){RegCloseKey(hKeyFilter);continue;}

	lRes=RegQueryValueEx(hKeyFilter,L"FilterData",NULL,NULL,(LPBYTE)p,&size);
	if(lRes!=ERROR_SUCCESS){
		RegCloseKey(hKeyFilter);
		MessageBox(0,L"Произошла ошибка чтения данных кодека из реестра\nПриносим извенения",0,MB_OK|MB_ICONERROR);
		HeapFree(GetProcessHeap(),0,p);
		continue;
	}
	
	//we have binary data
	
	*((DWORD*)&(p[4]))=dwMerit;//изменение приоритета (с четвертого байта)
	//write packed data to registry
	lRes=RegSetValueEx(hKeyFilter,L"FilterData",0,REG_BINARY,p,size);
	if(lRes!=ERROR_SUCCESS){
		MessageBox(0,L"Не удалось изменить приоритет (Ошибка записи данных в реестр)\n",0,MB_OK|MB_ICONERROR);
		}
	HeapFree(GetProcessHeap(),0,p);
	RegCloseKey(hKeyFilter);	
	RegCloseKey(hKeyCLSID);
	//MessageBox(0,L"Операция выполнена успешно",0,MB_OK|MB_ICONASTERISK);
	
	return;
}
RegCloseKey(hKeyCLSID);
MessageBox(0,L"Не удалось изменить приоритет (Неопознанная ошибка)\n",0,MB_OK|MB_ICONERROR);
	
}

CODEC_TYPE RegFindFilter(const TCHAR* name,TCHAR* out,int ch){
HKEY hKeyCLSID;
HKEY hKeyFilter=NULL;
LONG lRes;
DWORD i=-1;
TCHAR key_name[256]=L"";
TCHAR buf[500]=L"{083863F1-70DE-11d0-BD40-00A0C911CE86}";
TCHAR data[500];
TCHAR strFiltersKey[500]=L"";
DWORD size=sizeof(data);
DWORD size2;
StringCchCopy(strFiltersKey,500,L"CLSID\\");
CurrentCatCLSID(buf,500);
StringCchCat(strFiltersKey,500,buf);
StringCchCat(strFiltersKey,500,L"\\Instance\\");//search in current category
lRes=RegOpenKeyW(HKEY_CLASSES_ROOT,strFiltersKey,&hKeyCLSID);
if(lRes!=ERROR_SUCCESS){goto try2;}

while(1){
	i++;
	size2=256;
	lRes=RegEnumKeyEx(hKeyCLSID,i,key_name,&size2,NULL,NULL,NULL,NULL);
	if(lRes!=ERROR_SUCCESS){break;}
	StringCchCopy(buf,500,strFiltersKey);
	StringCchCat(buf,500,L"\\");
	StringCchCat(buf,500,key_name);
	lRes=RegOpenKeyW(HKEY_CLASSES_ROOT,buf,&hKeyFilter);
	if(lRes!=ERROR_SUCCESS)continue;
	size=sizeof(data);
	StringCchCopy(data,500,L"");
	lRes=RegQueryValueEx(hKeyFilter,strNameString,NULL,NULL,(LPBYTE)data,&size);
	if(lRes!=ERROR_SUCCESS){RegCloseKey(hKeyFilter);continue;}
	if(lstrcmpi(name,data)!=0){RegCloseKey(hKeyFilter);continue;}
	size=sizeof(data);
	StringCchCopy(data,500,L"");
	//FOUND!!!
	lRes=RegQueryValueEx(hKeyFilter,L"CLSID",NULL,NULL,(LPBYTE)data,&size);
	if(lRes!=ERROR_SUCCESS){StringCchCopy(data,500,key_name);}
	RegCloseKey(hKeyFilter);
	//MessageBox(0,key_name,0,0);
	StringCchCopy(out,ch,data);
	RegCloseKey(hKeyCLSID);
	return CODEC_DSHOW;
}

RegCloseKey(hKeyCLSID);

//try 2
try2:
lRes=RegOpenKeyW(HKEY_CLASSES_ROOT,L"CLSID",&hKeyCLSID);//search in InprocServer registrations
if(lRes!=ERROR_SUCCESS){return CODEC_LEGACY;}
i=-1;

while(1){
	i++;
	size2=256;
	lRes=RegEnumKeyEx(hKeyCLSID,i,key_name,&size2,NULL,NULL,NULL,NULL);
	if(lRes!=ERROR_SUCCESS){break;}
	StringCchCopy(buf,500,L"CLSID");
	StringCchCat(buf,500,L"\\");
	StringCchCat(buf,500,key_name);
	lRes=RegOpenKeyW(HKEY_CLASSES_ROOT,buf,&hKeyFilter);
	if(lRes!=ERROR_SUCCESS)continue;
	size=sizeof(data);
	lRes=RegQueryValueEx(hKeyFilter,NULL,NULL,NULL,(LPBYTE)data,&size);
	if(lRes!=ERROR_SUCCESS){RegCloseKey(hKeyFilter);continue;}
	if(lstrcmpi(name,data)!=0){RegCloseKey(hKeyFilter);continue;}
	//FOUND!!!
	RegCloseKey(hKeyFilter);
	StringCchCopy(out,ch,key_name);
	RegCloseKey(hKeyCLSID);
	return CODEC_DMO;
}

RegCloseKey(hKeyCLSID);

return CODEC_LEGACY;
}

void ErrorMessage(LONG code) 
{ 
    // Retrieve the system error message for the last-error code
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = (DWORD)code; 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process
    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf))+120); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf),
        TEXT("Ошибка %d: %s"), 
         dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
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

void RegDeleteKeyTree(HKEY key,const TCHAR* subkey){
HKEY hKey;
LONG lRes;
DWORD i=-1;
TCHAR key_name[256]=L"";
TCHAR buf[500];
DWORD size2;
lRes=RegOpenKeyW(key,subkey,&hKey);
if(lRes!=ERROR_SUCCESS){return;}
while(1){
	i++;
	size2=256;
	lRes=RegEnumKeyEx(hKey,i,key_name,&size2,NULL,NULL,NULL,NULL);
	if(lRes!=ERROR_SUCCESS){break;}
	StringCchCopy(buf,500,subkey);StringCchCat(buf,500,L"\\");
	StringCchCat(buf,500,key_name);
	RegDeleteKeyTree(key,buf);
}
RegCloseKey(hKey);
RegDeleteKey(key,subkey);
}

BOOL FindKeyByCLSID(const TCHAR* clsid,TCHAR* key,TCHAR* out,UINT c){
HKEY hKeyCLSID;
HKEY hKeyFilter=NULL;
LONG lRes;
DWORD i=-1;
TCHAR key_name[256]=L"";
TCHAR buf[500]=L"{083863F1-70DE-11d0-BD40-00A0C911CE86}";
TCHAR data[500];
DWORD size=sizeof(data);
DWORD size2;

lRes=RegOpenKeyW(HKEY_CLASSES_ROOT,key,&hKeyCLSID);

if(lRes!=ERROR_SUCCESS){
	StringCchCopy(out,c,L"");
	return FALSE;
}

while(1){
	i++;
	size2=256;
	lRes=RegEnumKeyEx(hKeyCLSID,i,key_name,&size2,NULL,NULL,NULL,NULL);
	if(lRes!=ERROR_SUCCESS){break;}
	StringCchCopy(buf,500,key);
	StringCchCat(buf,500,L"\\");
	StringCchCat(buf,500,key_name);
	lRes=RegOpenKeyW(HKEY_CLASSES_ROOT,buf,&hKeyFilter);
	if(lRes!=ERROR_SUCCESS)continue;
	size=sizeof(data);
	StringCchCopy(data,500,L"");
	lRes=RegQueryValueEx(hKeyFilter,L"CLSID",NULL,NULL,(LPBYTE)data,&size);
	if(lRes!=ERROR_SUCCESS){RegCloseKey(hKeyFilter);continue;}
	if(lstrcmpi(clsid,data)!=0){RegCloseKey(hKeyFilter);continue;}
	//FOUND!!!	
	RegCloseKey(hKeyFilter);
	//MessageBox(0,key_name,0,0);
	StringCchCopy(out,c,key);
	StringCchCat(out,c,L"\\");
	StringCchCat(out,c,key_name);
	RegCloseKey(hKeyCLSID);
	return TRUE;
}

RegCloseKey(hKeyCLSID);
StringCchCopy(out,c,L"");
return FALSE;

}

void RegDeleteFilter(const TCHAR* guid){
TCHAR buf[500];
TCHAR strFiltersKey[500];
BOOL res;
//delete inproc server registration
StringCchCopy(buf,500,L"CLSID\\");
StringCchCat(buf,500,guid);
RegDeleteKeyTree(HKEY_CLASSES_ROOT,buf);
//delete from current cat
StringCchCopy(strFiltersKey,500,L"CLSID\\");
CurrentCatCLSID(buf,500);
StringCchCat(strFiltersKey,500,buf);
StringCchCat(strFiltersKey,500,L"\\Instance\\");
res=FindKeyByCLSID(guid,strFiltersKey,buf,500);
if(res==FALSE){
	StringCchCopy(buf,500,strFiltersKey);
	StringCchCat(buf,500,L"\\");
	StringCchCat(buf,500,guid);
}
RegDeleteKeyTree(HKEY_CLASSES_ROOT,buf);
//delete from AMLegacyFilters Cat
	StringCchCopy(buf,500,L"CLSID\\{083863F1-70DE-11d0-BD40-00A0C911CE86}\\Instance\\");	
	StringCchCat(buf,500,guid);
	RegDeleteKeyTree(HKEY_CLASSES_ROOT,buf);
}

void RegDeleteDMO(const TCHAR* guid){
TCHAR buf[60];
TCHAR key[256];
int i=0,j=0;
for(i=0;i<60;i++){
	if(guid[i]==0)break;
	if(guid[i]==L'{'||guid[i]==L'}')continue;
	buf[j]=guid[i];
	j++;
}
buf[j]=0;
StringCchCopy(key,256,L"DirectShow\\MediaObjects\\");
StringCchCat(key,256,buf);
RegDeleteKeyTree(HKEY_CLASSES_ROOT,key);
}
