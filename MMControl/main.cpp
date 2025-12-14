/* SmallSoft Multimedia Control Panel
 * Copyright (c) 2025, SmallSoft (https://github.com/smallsoft-rus/) 
 * License: BSD 2.0 */
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' "\
"version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <windows.h>
#define STRSAFE_NO_DEPRECATE
#include <strsafe.h>
#include <commctrl.h>
#include "FilterTool.h"
#include "resource.h"
#include "struct.h"
#define IDC_FILTER_LIST 100
#define IDC_CATEGORY_LIST 101
#define ID_HIDESYSTEM 102
void RegDeleteFilter(const TCHAR* guid);
void RegDeleteDMO(const TCHAR* guid);
void SearchIncorrectCodecs();

void SetFilterMerit(const TCHAR* name,DWORD dwMerit);
void GetMeritString(DWORD dwMerit,TCHAR* out,UINT c);
DWORD mapper_GetFilterMerit(TCHAR* name) ;

HWND hMainWnd;
HWND hbInfo;HWND hbProperties;HWND hbRegister;
HWND hbHelp;HWND hbHomepage;HWND hbAbout;
HWND hbHideSystem;

HWND hProgressDlg=NULL;
HWND hProgressBar=NULL;
bool fStopFlag=false;

HBITMAP hbmControl;HBITMAP hbmAdd;HBITMAP hbmReport;
HBITMAP hbmInfo;HBITMAP hbmHelp;HBITMAP hbmHome;

TCHAR CurrentFilterGUID[500]=L"";
TCHAR CurrentFilterName[500]=L"";
FILTER_INFORMATION FilterInformation={0};
DWORD CurrentFilterMerit=0;
CODEC_TYPE CurrentFilterType=(CODEC_TYPE)0;
bool fHideSystemFilters=true;

TCHAR strFilter[]=L"All files\0*.*\0\
Dynamic Link Libraries (*.dll)\0*.dll\0\
DirectShow Filters (*.ax)\0*.ax\0\
All supported types (*.dll;*.ax)\0*.dll;*.ax\0\
";

void RegisterCodec(TCHAR* file){
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    TCHAR cmd[500]=L"";
    StringCchCopy(cmd,500,L"regsvr32 /s \"");
    StringCchCat(cmd,500,file);
    StringCchCat(cmd,500,L"\"");
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );
    CreateProcess(NULL,cmd,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
    WaitForInputIdle(pi.hProcess,4000);
}

BOOL CALLBACK ChangeMeritDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam){
    TCHAR buf[256];
    int c;
    switch(uMsg){
    case WM_INITDIALOG:
        StringCchPrintf(buf,256,L"0x%08x",CurrentFilterMerit);
        SetDlgItemText(hDlg,IDC_MERIT,buf);
        return TRUE;

    case WM_CLOSE:EndDialog(hDlg,0);break;
    case WM_COMMAND:
        switch(LOWORD(wParam)){
        case ID_MERIT_NORMAL:
	        StringCchPrintf(buf,256,L"0x%08x",MERIT_NORMAL);
	        SetDlgItemText(hDlg,IDC_MERIT,buf);
	        break;
        case ID_MERIT_PREFERRED:
            StringCchPrintf(buf,256,L"0x%08x",MERIT_PREFERRED);
            SetDlgItemText(hDlg,IDC_MERIT,buf);
            break;
        case ID_MERIT_UNLIKELY:
            StringCchPrintf(buf,256,L"0x%08x",MERIT_UNLIKELY);
            SetDlgItemText(hDlg,IDC_MERIT,buf);
            break;
        case ID_MERIT_DO_NOT_USE:
            StringCchPrintf(buf,256,L"0x%08x",MERIT_DO_NOT_USE);
            SetDlgItemText(hDlg,IDC_MERIT,buf);
            break;
    
        case IDOK:
            GetDlgItemText(hDlg,IDC_MERIT,buf,256);
            c=swscanf(buf,L"%i",&CurrentFilterMerit);

            if(c==0){
                MessageBox(hDlg,
                L"Нужно ввести правильное шестнадцатеричное число с префиксом 0х, или выбрать предопределенное значение",
                0, MB_OK|MB_ICONWARNING);
                break;
            }

            SetFilterMerit(CurrentFilterName,CurrentFilterMerit);

        case IDCANCEL:
            EndDialog(hDlg,0);
            break;
        }break;
    }
return FALSE;
}

BOOL CALLBACK ProgressDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam){

switch(uMsg){
    case WM_INITDIALOG:	return TRUE;
    case WM_COMMAND:
        switch(LOWORD(wParam)){
        case IDCANCEL:fStopFlag=true;
        hProgressDlg=NULL;
        EndDialog(hDlg,0);
        break;
        }
        break;                  
}
return FALSE;
}

BOOL CALLBACK AboutDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam){
   	
switch(uMsg){
    case WM_INITDIALOG:
    SetDlgItemText(hDlg,IDC_TEXT,L"Утилита для управления кодеками. Поддерживает DirectShow и DMO кодеки\r\n\
ВНИМАНИЕ! Всегда удаляйте кодеки с осторожностью! Перед удалением\r\n\
смотрите, какой файл ему соответсвует. Если это файл из папки WINDOWS, не удаляйте кодек\r\n\
");
    return TRUE;

    case WM_COMMAND:
    switch(LOWORD(wParam)){
        case ID_HOMEPAGE:
        ShellExecute(NULL, L"open", L"https://smallsoftru.wordpress.com/",L"",L"",SW_SHOW);break;
        case ID_FEEDBACK:
        ShellExecute(NULL, L"open", L"https://github.com/smallsoft-rus/decoder-pack/issues/new",L"",L"",SW_SHOW);break;

        case IDOK:EndDialog(hDlg,0);break;
    }break;

}
return FALSE;
}

BOOL CALLBACK ControlDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam){
    HANDLE hSearch;
    WIN32_FIND_DATA wd;
    TCHAR cmd[500];
    int i;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

switch(uMsg){
    case WM_INITDIALOG:
    SetDlgItemText(hDlg,IDC_FILTERNAME,CurrentFilterName);	
    SetDlgItemText(hDlg,IDC_OBJECTNAME,FilterInformation.ObjectName);
    SetDlgItemText(hDlg,IDC_FILE,FilterInformation.FilePath);
    SetDlgItemText(hDlg,IDC_GUID,CurrentFilterGUID);
    GetMeritString(CurrentFilterMerit,cmd,500);
    SetDlgItemText(hDlg,IDC_MERIT,cmd);
    return TRUE;

    case WM_CLOSE:EndDialog(hDlg,0);break;
    case WM_COMMAND:switch(LOWORD(wParam)){
        case ID_CHECK:

        hSearch=FindFirstFile(FilterInformation.FilePath,&wd);

        if(hSearch==INVALID_HANDLE_VALUE){
            MessageBox(hDlg,L"Возможно, кодек зарегистрирован некорректно",NULL,MB_OK|MB_ICONWARNING);
        }
        else{
            FindClose(hSearch);
            MessageBox(hDlg,L"Кодек зарегистрирован корректно",L"Результат проверки",MB_OK|MB_ICONINFORMATION);
        }
        break;
        case ID_DELETE:
            i=MessageBox(hDlg,
            L"Данная операция удалит записи о кодеке из реестра. Ее следует применять только если нельзя \
 удалить кодек другим способом. Вы хотите продолжить?",
            L"Подтвердите действие",MB_YESNO|MB_ICONWARNING);

            if(i==IDNO)break;

            RegDeleteFilter(CurrentFilterGUID);

            if(CurrentFilterType==CODEC_DMO){RegDeleteDMO(CurrentFilterGUID);}

            Command_GetFilters();
            EndDialog(hDlg,0);
            break;

            case ID_UNREGISTER:
            i=MessageBox(hDlg,
            L"Данная операция удалит из системы все записи о выбранном кодеке, а также\
 о всех других кодеках в одной DLL с выбранным (если они есть). Если кодек зарегистрирован\
 некорректно, удаление будет невозможно. Вы хотите продолжить?",
            L"Подтвердите действие",MB_YESNO|MB_ICONWARNING);

            if(i==IDNO)break;

            StringCchCopy(cmd,500,L"regsvr32 /u \"");
            StringCchCat(cmd,500,FilterInformation.FilePath);
            StringCchCat(cmd,500,L"\"");
            ZeroMemory( &si, sizeof(si) );
            si.cb = sizeof(si);
            ZeroMemory( &pi, sizeof(pi) );
            CreateProcess(NULL,cmd,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
            WaitForInputIdle(pi.hProcess,4000);	
            Command_GetFilters();
            EndDialog(hDlg,0);
            break;

        case ID_CHANGEMERIT:
            DialogBoxW(GetModuleHandle(NULL),MAKEINTRESOURCE(IDD_MERIT),hDlg,ChangeMeritDlgProc);
            CurrentFilterMerit=mapper_GetFilterMerit(CurrentFilterName);
            GetMeritString(CurrentFilterMerit,cmd,500);
            SetDlgItemText(hDlg,IDC_MERIT,cmd);
            break;

        case IDOK:EndDialog(hDlg,0);
            break;
    }
    break;

}
return FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam){

    TCHAR cmd[500];
    OPENFILENAMEW ofn={0};
    wchar_t FileName[MAX_PATH]=L"";
    BOOL success=FALSE;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

switch(uMsg){
    case WM_COMMAND:
        switch(LOWORD(wParam)){
        case ID_HIDESYSTEM:
            fHideSystemFilters=!fHideSystemFilters;
            Command_GetFilters();
            break;
        case ID_INFO:Command_FilterInfo();break;
        case ID_PROPERTIES:Command_FilterProperties();break;
        case ID_REFRASH:Command_GetFilters();break;
        case ID_REGISTER:
            ofn.lStructSize=sizeof(OPENFILENAME);
            ofn.lpstrFile=FileName;
            ofn.nMaxFile=sizeof(FileName);
            ofn.lpstrFilter=strFilter;
            ofn.lpstrTitle=L"Выберите кодек для регистрации";
            ofn.nFilterIndex=1;
            ofn.Flags=OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_LONGNAMES|OFN_NOCHANGEDIR;
            success=GetOpenFileNameW(&ofn);

            if(!success){break;}

            StringCchCopy(cmd,500,L"regsvr32 \"");
            StringCchCat(cmd,500,FileName);
            StringCchCat(cmd,500,L"\"");
            ZeroMemory( &si, sizeof(si) );
            si.cb = sizeof(si);
            ZeroMemory( &pi, sizeof(pi) );
            CreateProcess(NULL,cmd,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
            WaitForInputIdle(pi.hProcess,4000);
            Command_GetFilters();
            break;

        case ID_REG_SYSTEM_CORE:
            RegisterCodec(L"quartz.dll");
            RegisterCodec(L"mpg2splt.ax");
            RegisterCodec(L"l3codecx.ax");
            RegisterCodec(L"qasf.dll");
            RegisterCodec(L"qdvd.dll");
            RegisterCodec(L"wmadmoe.dll");
            RegisterCodec(L"wmvdmoe2.dll");
            Command_GetFilters();
            break;

        case ID_REG_SYSTEM_ADDITIONAL:
            RegisterCodec(L"wmpasf.dll");
            RegisterCodec(L"wmvds32.ax");
            RegisterCodec(L"qdv.dll");
            RegisterCodec(L"qcap.dll");
            Command_GetFilters();
            break;

        case ID_ABOUT: DialogBoxW(GetModuleHandle(NULL),MAKEINTRESOURCE(IDD_ABOUT),hMainWnd,AboutDlgProc);
            break;

        case ID_SEARCHINCORRECT:
            hProgressDlg=CreateDialogW(GetModuleHandle(NULL),MAKEINTRESOURCE(IDD_SEARCH_PROGRESS),hMainWnd,ProgressDlgProc);
            hProgressBar=GetDlgItem(hProgressDlg,IDC_PROGRESS);
            fStopFlag=false;
            SearchIncorrectCodecs();

            if(hProgressDlg!=NULL){DestroyWindow(hProgressDlg);hProgressDlg=NULL;}
            MessageBox(hMainWnd,L"Поиск завершен.",L"Сообщение",MB_OK|MB_ICONINFORMATION);
            break;

        case ID_HOMEPAGE:
            ShellExecute(NULL, L"open", L"https://smallsoftru.wordpress.com/",L"",L"",SW_SHOW);break;
        case ID_SOURCECODE:
            ShellExecute(NULL, L"open", L"https://github.com/smallsoft-rus/decoder-pack",L"",L"",SW_SHOW);break;

        case ID_HELP:
            ShellExecute(NULL, L"open", L"readme.txt",L"",L"",SW_SHOW);
            break;

        case ID_EXIT:SendMessage(hMainWnd,WM_CLOSE,0,0);break;

        case IDC_CATEGORY_LIST: if((int)HIWORD(wParam)!=CBN_SELCHANGE) break;
            //change current category
            Command_GetFilters();
            SendMessage(hFilterList,LB_SETCURSEL,-1,0);
            break;

        case IDC_FILTER_LIST:if((int)HIWORD(wParam)!=LBN_DBLCLK) break;
            //control filter
            Command_FilterInfo();
            break;

    }break;
    case WM_CLOSE:		
        DestroyWindow(hWnd);break;

    case WM_DESTROY:PostQuitMessage(0);break;

	default: return DefWindowProc(hWnd,uMsg,wParam,lParam);
}
return 0;

}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow){
    wchar_t wclass_name[]=L"MyClass";MSG msg;WNDCLASSEX wc;

    INITCOMMONCONTROLSEX ic;
    ic.dwSize=sizeof(ic);
    ic.dwICC=ICC_WIN95_CLASSES;
    InitCommonControlsEx(&ic);

    CoInitialize(NULL);
    hbmReport=(HBITMAP)LoadImage(hInstance,MAKEINTRESOURCE(IDB_REPORT),IMAGE_BITMAP,0,0,0);
    hbmControl=(HBITMAP)LoadImage(hInstance,MAKEINTRESOURCE(IDB_CONTROL),IMAGE_BITMAP,0,0,0);
    hbmAdd=(HBITMAP)LoadImage(hInstance,MAKEINTRESOURCE(IDB_ADD),IMAGE_BITMAP,0,0,0);
    hbmInfo=(HBITMAP)LoadImage(hInstance,MAKEINTRESOURCE(IDB_INFO),IMAGE_BITMAP,0,0,0);
    hbmHelp=(HBITMAP)LoadImage(hInstance,MAKEINTRESOURCE(IDB_HELP),IMAGE_BITMAP,0,0,0);
    hbmHome=(HBITMAP)LoadImage(hInstance,MAKEINTRESOURCE(IDB_HOME),IMAGE_BITMAP,0,0,0);
	
    //create wclass structure
    wc.cbSize=sizeof(wc);
    wc.style=CS_HREDRAW|CS_VREDRAW;
    wc.lpfnWndProc=WndProc;
    wc.cbClsExtra=0;
    wc.cbWndExtra=0;
    wc.hInstance=hInstance;
    wc.hIcon=LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wc.hCursor=LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName=MAKEINTRESOURCE(IDR_MENU1);
    wc.lpszClassName=wclass_name;
    wc.hIconSm=LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

    //register class
    if(!RegisterClassEx(&wc)){
        MessageBox(NULL,L"Fail to create class",L"Error",MB_ICONEXCLAMATION|MB_OK);
        return -1;
    }

    //create window
    hMainWnd=CreateWindow(wclass_name,L"SmallSoft Multimedia Control Panel",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,0,
        500,440,NULL,NULL,hInstance,NULL);

    if(hMainWnd==NULL){
        MessageBox(NULL,L"Fail to create window",L"Error",MB_ICONEXCLAMATION|MB_OK);
        return -1;
    }

    hbInfo=CreateWindow(L"BUTTON",L"Управление",WS_CHILD|WS_VISIBLE|BS_BITMAP,350,0,
        40,40,hMainWnd,(HMENU)ID_INFO,hInstance,NULL);
    hCategoryList=CreateWindow(L"COMBOBOX",L"",WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST,0,0,
        250,400,hMainWnd,(HMENU)IDC_CATEGORY_LIST,hInstance,NULL);
    hFilterList=CreateWindow(L"LISTBOX",L"",WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL|LBS_SORT|LBS_NOTIFY  ,0,40,
        350,300,hMainWnd,(HMENU)IDC_FILTER_LIST,hInstance,NULL);
    hbHideSystem=CreateWindow(L"BUTTON",L"показать системные фильтры",WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX,0,340,
        350,30,hMainWnd,(HMENU)ID_HIDESYSTEM,hInstance,NULL);
    hbProperties=CreateWindow(L"BUTTON",L"Свойства",WS_CHILD|WS_VISIBLE|BS_BITMAP,350,40,
        40,40,hMainWnd,(HMENU)ID_PROPERTIES,hInstance,NULL);
    hbRegister=CreateWindow(L"BUTTON",L"Добавить кодек..",WS_CHILD|WS_VISIBLE|BS_BITMAP,350,80,
        40,40,hMainWnd,(HMENU)ID_REGISTER,hInstance,NULL);
    hbHelp=CreateWindow(L"BUTTON",L"Справка",WS_CHILD|WS_VISIBLE|BS_BITMAP,350,120,
        40,40,hMainWnd,(HMENU)ID_HELP,hInstance,NULL);
    hbHomepage=CreateWindow(L"BUTTON",L"Сайт программы",WS_CHILD|WS_VISIBLE|BS_BITMAP,350,160,
        40,40,hMainWnd,(HMENU)ID_HOMEPAGE,hInstance,NULL);
    hbAbout=CreateWindow(L"BUTTON",L"Информация",WS_CHILD|WS_VISIBLE|BS_BITMAP,350,200,
        40,40,hMainWnd,(HMENU)ID_ABOUT,hInstance,NULL);

    SendMessage(hbInfo,BM_SETIMAGE,IMAGE_BITMAP,(LONG)hbmControl);
    SendMessage(hbProperties,BM_SETIMAGE,IMAGE_BITMAP,(LONG)hbmReport);
    SendMessage(hbRegister,BM_SETIMAGE,IMAGE_BITMAP,(LONG)hbmAdd);
    SendMessage(hbHelp,BM_SETIMAGE,IMAGE_BITMAP,(LONG)hbmHelp);
    SendMessage(hbAbout,BM_SETIMAGE,IMAGE_BITMAP,(LONG)hbmInfo);
    SendMessage(hbHomepage,BM_SETIMAGE,IMAGE_BITMAP,(LONG)hbmHome);

    HWND hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,CW_USEDEFAULT,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,hMainWnd,NULL, hInstance,NULL);

    SetWindowPos(hwndTip, HWND_TOPMOST,0, 0, 0, 0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    TOOLINFO tf={0};
    tf.cbSize=sizeof(TOOLINFO);tf.hwnd=hMainWnd;
    tf.hinst=hInstance;
    tf.uFlags=TTF_IDISHWND|TTF_SUBCLASS;
    tf.uId=(UINT)hbInfo;
    tf.lpszText=MAKEINTRESOURCE(IDS_CONTROL);
    SendMessage(hwndTip,TTM_ADDTOOL,0,(LONG)&tf);
    tf.uId=(UINT)hbProperties;tf.lpszText=MAKEINTRESOURCE(IDS_PROP);
    SendMessage(hwndTip,TTM_ADDTOOL,0,(LONG)&tf);
    tf.uId=(UINT)hbRegister;tf.lpszText=MAKEINTRESOURCE(IDS_ADD);
    SendMessage(hwndTip,TTM_ADDTOOL,0,(LONG)&tf);
    tf.uId=(UINT)hbHelp;tf.lpszText=MAKEINTRESOURCE(IDS_HELP);
    SendMessage(hwndTip,TTM_ADDTOOL,0,(LONG)&tf);
    tf.uId=(UINT)hbHomepage;tf.lpszText=MAKEINTRESOURCE(IDS_HOMEPAGE);
    SendMessage(hwndTip,TTM_ADDTOOL,0,(LONG)&tf);
    tf.uId=(UINT)hbAbout;tf.lpszText=MAKEINTRESOURCE(IDS_ABOUT);
    SendMessage(hwndTip,TTM_ADDTOOL,0,(LONG)&tf);

    FillCategoryList();
    SendMessage(hCategoryList,CB_SETCURSEL,2,0);
    Command_GetFilters();

    ShowWindow(hMainWnd,nCmdShow);

    while(GetMessage(&msg,NULL,0,0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CoUninitialize();
    return msg.wParam;
}
