/* SmallSoft Decoder Pack 
 * Copyright (c) 2023,  MSDN.WhiteKnight (https://github.com/smallsoft-rus/decoder-pack) 
 * License: BSD 2.0 */
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#define STRSAFE_NO_DEPRECATE
#include <strsafe.h>
#include <commctrl.h>
#include <dshow.h>
#include "config.h"
#include "resource.h"
#define ID_CODEC_HOMEPAGE 100

HWND hMainWnd;
HWND hList;
HWND hstCodecList;HWND hstGUID;HWND hstFile;
HWND hbEnable;
HWND hbProperties;
HWND htbName;HWND htbGUID;HWND htbFile;
HWND hbCodecHomepage;

HBITMAP hbmAAC;HBITMAP hbmBass;HBITMAP hbmDIVX;
HBITMAP hbmSmallSoft;HBITMAP hbmXVID;

extern CODEC_INFO CodecTable[10];
extern UINT CountCodecs;

int Selected=0;

HBITMAP GetCodecBitmap(TCHAR* name){
if(lstrcmpi(name,L"bass")==0){	
	return hbmBass;
}
if(lstrcmpi(name,L"aac")==0){	
	return hbmAAC;
}
if(lstrcmpi(name,L"divx_h264")==0){	
	return hbmDIVX;
}
if(lstrcmpi(name,L"divx")==0){	
	return hbmDIVX;
}
if(lstrcmpi(name,L"xvid")==0){	
	return hbmXVID;
}
return hbmSmallSoft;

}



TCHAR* GetCodecHomepage(TCHAR* name){
if(lstrcmpi(name,L"bass")==0){	
	return L"http://dsp-worx.de/";
}
if(lstrcmpi(name,L"divx_h264")==0){	
	 return L"http://divx.com/";
}
if(lstrcmpi(name,L"divx")==0){	
	return L"http://divx.com/";
}
if(lstrcmpi(name,L"xvid")==0){	
	return L"http://xvid.com/";
}
if(lstrcmpi(name,L"ds_audio")==0){	
	return L"http://deinterlace.sourceforge.net/dscaler5.htm";
}
if(lstrcmpi(name,L"ds_video")==0){	
	return L"http://deinterlace.sourceforge.net/dscaler5.htm";
}
if(lstrcmpi(name,L"mkv")==0){	
	return L"http://www.sourceforge.net/projects/guliverkli2/";
}
if(lstrcmpi(name,L"mpeg")==0){	
	return L"http://www.sourceforge.net/projects/guliverkli2/";
}

return L"https://smallsoftru.wordpress.com/";

}


void InsertListElement(UINT pos){

	LVITEM item={0};	
	BOOL res=FALSE;
	WCHAR buf[300]=L"";	
	LONG lRes;
	
	if(pos>=MAX_CODECS_IN_TABLE)return;
	if(pos<0)return;	
	
item.iItem=pos;
item.mask=LVIF_TEXT;
item.pszText=CodecTable[pos].display_name;
lRes=ListView_InsertItem(hList,&item);
if(lRes==-1){return;}
	
if(CodecTable[pos].IsEnabled!=FALSE)StringCchCopy(buf,300,L"ВКЛ");
else StringCchCopy(buf,300,L"ВЫКЛ");
item.iItem=pos;
item.iSubItem=1;
item.mask=LVIF_TEXT;
item.pszText=buf;
res=SendMessage(hList,LVM_SETITEM,0,(LPARAM)&item);

item.iItem=pos;
item.iSubItem=2;
item.mask=LVIF_TEXT;
item.pszText=CodecTable[pos].formats;
res=SendMessage(hList,LVM_SETITEM,0,(LPARAM)&item);

}

void UpdateList(){
UINT i;
ListView_DeleteAllItems(hList);
for(i=0;i<CountCodecs;i++){
	InsertListElement(i);
}
}

void SetSelection(int i){
	if(i>=(int)CountCodecs||i<0)return;
int j;

ListView_SetItemState(hList,i,LVIS_SELECTED,LVIS_SELECTED);
for(j=0;j<(int)CountCodecs;j++){
	if(j==i)continue;
	ListView_SetItemState(hList,j,0,LVIS_SELECTED);
}
ListView_SetSelectionMark(hList,i);
}


void Command_FilterProperties(){
int n;
HRESULT hr;
BOOL res;
IBaseFilter* pFilter=NULL;
n=Selected;
if(n<0)return;
//hr=CreateFilter(&pFilter,CodecTable[n].guid);
pFilter=FindFilter(CodecTable[n].display_name,CLSID_LegacyAmFilterCategory);
if(pFilter==NULL){MessageBox(hMainWnd,L"Ошибка при загрузке страницы свойств",0,MB_OK|MB_ICONERROR);return;}
res=ShowFilterProperties(pFilter);
if(res==FALSE){MessageBox(hMainWnd,L"У кодека нет настраиваемых параметров",CodecTable[n].display_name,MB_OK|MB_ICONERROR);}
pFilter->Release();

}


void Command_ChangeSelection(){
int n;
n=ListView_GetSelectionMark(hList);
if(n<0)return;
if(n>=CountCodecs)return;
Selected=n;
EnableWindow(hbEnable,TRUE);

SetWindowText(htbName,CodecTable[Selected].display_name);
SendMessage(hbCodecHomepage,BM_SETIMAGE,IMAGE_BITMAP,(LONG)GetCodecBitmap(CodecTable[Selected].internal_name));
SetWindowText(htbGUID,CodecTable[Selected].guid);
SetWindowText(htbFile,CodecTable[Selected].file);
if(CodecTable[n].IsEnabled==true)SetWindowText(hbEnable,L"Выключить");
else {
	SetWindowText(hbEnable,L"Включить");
	EnableWindow(hbProperties,FALSE);
	return;
}
if(CodecTable[n].HasProperties==true)EnableWindow(hbProperties,TRUE);
else EnableWindow(hbProperties,FALSE);
}


LRESULT CALLBACK WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam){

int n;
LPNMITEMACTIVATE lpnmitem;
switch(uMsg){
	case WM_USER:
		n=ListView_GetSelectedCount(hList);
		if(n>1){SetSelection(ListView_GetSelectionMark(hList));}
		else if(n==0){SetSelection(0);}
		Command_ChangeSelection();
		break;
	case WM_NOTIFY:
	lpnmitem = (LPNMITEMACTIVATE) lParam;
	if(lpnmitem->hdr.code==NM_CLICK	){
		SendMessage(hMainWnd,WM_USER,0,0);
		break;
	}
	if(lpnmitem->hdr.code==LVN_KEYDOWN	){
		PostMessage(hMainWnd,WM_USER,0,0);
	}
	break;

	case WM_COMMAND:
		switch(LOWORD(wParam)){
	case ID_ENABLE:
		n=Selected;
		if(n<0)break;
		if(CodecTable[n].IsEnabled==true){
			Command_DisableCodec(n);
		}else{
			Command_EnableCodec(n);
		}
		UpdateList();
		SetSelection(n);
		Command_ChangeSelection();
		break;
	case ID_PROP:
		Command_FilterProperties();
		break;

	case ID_ABOUT:
		MessageBox(hMainWnd,L"SmallSoft Decoder Pack 1.1 - Оболочка настройки\n\n"
			L"Автор: SmallSoft\n"
			L"Веб-сайт: https://smallsoftru.wordpress.com/\n"
			L"Исходный код: https://github.com/smallsoft-rus/decoder-pack\n"
		,L"О программе",MB_OK);
		break;
	case ID_CODEC_HOMEPAGE:
	ShellExecute(NULL, L"open", GetCodecHomepage(CodecTable[Selected].internal_name),L"",L"",SW_SHOW);break;
	case ID_HOMEPAGE:
	ShellExecute(NULL, L"open", L"https://smallsoftru.wordpress.com/",L"",L"",SW_SHOW);break;
	case ID_SOURCEREPO:
	ShellExecute(NULL, L"open", L"https://github.com/smallsoft-rus/decoder-pack",L"",L"",SW_SHOW);break;
	break;
	case ID_HELP:
	ShellExecute(NULL, L"open", L"help\\index.html",L"",L"",SW_SHOW);
	break;
	//case ID_EXIT:SendMessage(hMainWnd,WM_CLOSE,0,0);break;
	case ID_RUN_DXDIAG:
	ShellExecute(NULL, L"open", L"dxdiag",L"",L"",SW_SHOW);
	break;
	case ID_RUN_MMCONTROL:
	ShellExecute(NULL, L"open", L"mmcontrol",L"",L"",SW_SHOW);
	break;
	case ID_RUN_SMP:
	ShellExecute(NULL, L"open", L"smplayer",L"",L"",SW_SHOW);
	break;
	case ID_RUN_REGEDIT:
	ShellExecute(NULL, L"open", L"regedit",L"",L"",SW_SHOW);
	break;
	case ID_RUN_WMP:
	ShellExecute(NULL, L"open", L"wmplayer",L"",L"",SW_SHOW);
	break;

		}break;
	case WM_CLOSE:DestroyWindow(hWnd);break;
	case WM_DESTROY:PostQuitMessage(0);break;
	default: return DefWindowProc(hWnd,uMsg,wParam,lParam);}
return 0;




}



void ProcessCommandLine(wchar_t* cmd){
   int i,j=0;
    
  

  wchar_t filename[MAX_PATH]=L"";
  LPWSTR* args=NULL;
  int NumArgs=0;
  IBaseFilter* pf=NULL;

  args=CommandLineToArgvW(cmd,&NumArgs);
if(args==NULL)return;
if(NumArgs<=2){LocalFree(args);return;}
if(lstrcmpi(args[1],L"/p")!=0){LocalFree(args);return;}
pf=FindFilter(args[2],CLSID_LegacyAmFilterCategory);
if(pf==NULL){LocalFree(args);return;}
ShowFilterProperties(pf);
LocalFree(args);
exit(0);
}







int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow){
	wchar_t wclass_name[]=L"MyClass";MSG msg;WNDCLASSEX wc;
	UINT i;
	LVCOLUMN col={0};
	INITCOMMONCONTROLSEX ic;
	ic.dwSize=sizeof(ic);
	ic.dwICC=ICC_WIN95_CLASSES;
	InitCommonControlsEx(&ic);
	CoInitialize(NULL);

	
	ProcessCommandLine(GetCommandLine());

	hbmAAC=(HBITMAP)LoadImage(hInstance,MAKEINTRESOURCE(IDB_AAC),IMAGE_BITMAP,0,0,0);
	hbmBass=(HBITMAP)LoadImage(hInstance,MAKEINTRESOURCE(IDB_BASS),IMAGE_BITMAP,0,0,0);
	hbmDIVX=(HBITMAP)LoadImage(hInstance,MAKEINTRESOURCE(IDB_DIVX),IMAGE_BITMAP,0,0,0);
	hbmXVID=(HBITMAP)LoadImage(hInstance,MAKEINTRESOURCE(IDB_XVID),IMAGE_BITMAP,0,0,0);
	hbmSmallSoft=(HBITMAP)LoadImage(hInstance,MAKEINTRESOURCE(IDB_SMALLSOFT),IMAGE_BITMAP,0,0,0);


	//create wclass structure
	wc.cbSize=sizeof(wc);
	wc.style=CS_HREDRAW|CS_VREDRAW;
	wc.lpfnWndProc=WndProc;
	wc.cbClsExtra=0;
	wc.cbWndExtra=0;
	wc.hInstance=hInstance;
	wc.hIcon=LoadIcon(hInstance,MAKEINTRESOURCE(IDI_SMALLSOFT));
	wc.hCursor=LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName=MAKEINTRESOURCE(IDR_MENU1);
	wc.lpszClassName=wclass_name;
	wc.hIconSm=NULL;
	//register class
	if(!RegisterClassEx(&wc)){
		MessageBox(NULL,L"Fail to create class",L"Error",MB_ICONEXCLAMATION|MB_OK);
		return -1;}
	//create window
	hMainWnd=CreateWindow(wclass_name,L"SmallSoft Decoder Pack - Настройка",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,0,
		660,400,NULL,NULL,hInstance,NULL);
	if(hMainWnd==NULL){MessageBox(NULL,L"Fail to create window",L"Error",MB_ICONEXCLAMATION|MB_OK);
		return -1;}
	hstCodecList=CreateWindow(L"STATIC",L"Установленные кодеки:",WS_CHILD|WS_VISIBLE,0,4,
		250,30,hMainWnd,NULL,hInstance,NULL);
	hbEnable=CreateWindow(L"BUTTON",L"Включить",WS_CHILD|WS_VISIBLE|WS_DISABLED,0,200,
		120,40,hMainWnd,(HMENU)ID_ENABLE,hInstance,NULL);
	hbProperties=CreateWindow(L"BUTTON",L"Настроить",WS_CHILD|WS_VISIBLE|WS_DISABLED,0,240,
		120,40,hMainWnd,(HMENU)ID_PROP,hInstance,NULL);
	htbName=CreateWindow(L"EDIT",L"SmallSoft Decoder Pack",WS_CHILD|WS_VISIBLE|ES_READONLY|WS_BORDER,240,200,
		250,30,hMainWnd,NULL,hInstance,NULL);

	hbCodecHomepage=CreateWindow(L"BUTTON",L"",WS_CHILD|WS_VISIBLE|BS_BITMAP,130,200,
		100,100,hMainWnd,(HMENU)ID_CODEC_HOMEPAGE,hInstance,NULL);
	SendMessage(hbCodecHomepage,BM_SETIMAGE,IMAGE_BITMAP,(LONG)hbmSmallSoft);

	hstGUID=CreateWindow(L"STATIC",L"GUID",WS_CHILD|WS_VISIBLE,240,240,
		70,25,hMainWnd,NULL,hInstance,NULL);
	hstFile=CreateWindow(L"STATIC",L"Файл",WS_CHILD|WS_VISIBLE,240,270,
		70,25,hMainWnd,NULL,hInstance,NULL);
	htbGUID=CreateWindow(L"EDIT",L"",WS_CHILD|WS_VISIBLE|ES_READONLY|WS_BORDER,310,240,
		340,25,hMainWnd,NULL,hInstance,NULL);
	htbFile=CreateWindow(L"EDIT",L"",WS_CHILD|WS_VISIBLE|ES_READONLY|WS_BORDER|ES_AUTOHSCROLL,310,270,
		340,25,hMainWnd,NULL,hInstance,NULL);

	hList=CreateWindowEx(0,WC_LISTVIEW,L"СПИСОК",WS_CHILD|WS_VISIBLE|LVS_SHOWSELALWAYS|LVS_REPORT|WS_BORDER ,0,40,
		580,160,hMainWnd,NULL,GetModuleHandle(NULL),NULL);
ListView_SetExtendedListViewStyle(hList,LVS_EX_FULLROWSELECT);

if(hList==NULL)Beep(300,100);
ShowWindow(hList,SW_SHOW);

col.iSubItem=0;
col.pszText=L"название";
col.cx=150;
col.mask=LVCF_TEXT|LVCF_SUBITEM|LVCF_WIDTH;
ListView_InsertColumn(hList,0,&col);
col.iSubItem=1;
col.pszText=L"сост.";
col.cx=60;
col.mask=LVCF_TEXT|LVCF_SUBITEM|LVCF_WIDTH;
ListView_InsertColumn(hList,1,&col);
col.iSubItem=2;
col.pszText=L"форматы";
col.cx=300;
col.mask=LVCF_TEXT|LVCF_SUBITEM|LVCF_WIDTH;
ListView_InsertColumn(hList,2,&col);






	ShowWindow(hMainWnd,nCmdShow);
	FillCodecTable();
	for(i=0;i<CountCodecs;i++){
		//MessageBox(0,CodecTable[i].file,CodecTable[i].display_name,0);
		UpdateCodecState(i);
		UpdateCodecInformation(&(CodecTable[i]));
	}
	UpdateList();
	SetSelection(0);
	Command_ChangeSelection();

	
	while(GetMessage(&msg,NULL,0,0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);}

	CoUninitialize();
	return msg.wParam;}
