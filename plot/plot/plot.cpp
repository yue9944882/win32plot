// plot.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "plot.h"


#include<Windows.h>
#include<windowsx.h>
#include<vector>
#include<string>
#include<math.h>
#include<iterator>
#include<stack>
#include<gdiplus.h>
#include<string.h>



using namespace Gdiplus;

#pragma comment(lib,"gdiplus.lib")

#define MAX_LOADSTRING 100


#define ID_PAINT_BUTTON 0x01
#define ID_TEXT_FIELD 0x02
#define ID_STATIC_HINT 0x03

#define ID_MENU_IMAGE_CLEAR 0x11
#define ID_MENU_IMAGE_TDIST 0x12
#define ID_MENU_IMAGE_EXPORT 0x13
#define ID_MENU_IMAGE_IMPORT 0x14

#define ID_MENU_PEN_COLOR_BLACK 0x21
#define ID_MENU_PEN_COLOR_BLUE 0x22
#define ID_MENU_PEN_COLOR_RED 0x23
#define ID_MENU_PEN_COLOR_YELLOW 0x24
#define ID_MENU_PEN_COLOR_GREEN 0x25
#define ID_MENU_PEN_COLOR_PURPLE 0x26

#define ID_MENU_IMAGE_EXPORT_BMP 0x31
#define ID_MENU_IMAGE_EXPORT_PNG 0x32



// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名
float g_fdist=5;

typedef struct tagData{
	int ptBeginX,ptBeginY;
	int ptEndX,ptEndY;
	int penStyle;
}PAINTDATA;

typedef struct tagPoint{
	float x;
	float y;
}FPOINT;

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Dist(HWND ,UINT , WPARAM , LPARAM);
void paint_points(HWND hWnd,std::vector<PAINTDATA>&pts,PAINTSTRUCT&ps,COLORREF color);
void bounder_def(std::vector<PAINTDATA>&bounder);
void coordinate_def(std::vector<PAINTDATA>&coordinate,std::vector<PAINTDATA>&arrow);
void tick_def(std::vector<PAINTDATA>&ticks);
void mark_def(std::vector<PAINTDATA>&marks);
void write_bmp(HBITMAP hBitmap,TCHAR*str1);
void write_png(HBITMAP hBitmap,TCHAR*str1);
void get_export_dir(WCHAR*);


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	//*************


	//*************
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;


	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PLOT, szWindowClass, MAX_LOADSTRING);
		
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PLOT));

	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PLOT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_PLOT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 540, 640, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	static TCHAR*szBuffer;
	static char szExpr[1024]={};
	static float tick_dist=1;

	static HWND btnPaintHwnd;
	static HWND edtPaintHwnd;
	static HWND sttPaintHwnd_input;
	static HWND sttPaintHwnd_numn;
	static HWND sttPaintHwnd_nums;
	static HWND sttPaintHwnd_nume;
	static HWND sttPaintHwnd_numw;
	static HWND sttPaintHwnd_numc;
		 
	static std::string poly;

	static std::vector<PAINTDATA> datas;
	static std::vector<PAINTDATA> coordinates;
	static std::vector<PAINTDATA> ticks;
	static std::vector<PAINTDATA> arrows;
	static std::vector<PAINTDATA> bounders;
	static std::vector<FPOINT> function;
	static std::vector<PAINTDATA> marks;
	static std::vector<std::string> img_vec;

	static PAINTDATA*pCurrentData=NULL;
	static bool bExist;
	static int penStyle=PS_SOLID;

	static COLORREF data_color=RGB(0,0,0);

	switch (message)
	{
	case WM_CREATE:
		{
			bounder_def(bounders);
			coordinate_def(coordinates,arrows);
			tick_def(ticks);
			mark_def(marks);

			// Menu Defination
			/*
			HMENU menubar=CreateMenu();
			HMENU menupop=CreatePopupMenu();
			AppendMenu(menupop,MF_STRING,ID_MENU_SOLID,L"实线");
			AppendMenu(menupop,MF_STRING,ID_MENU_DASH,L"虚线");
			AppendMenu(menupop,MF_STRING,ID_MENU_DOT,L"点线");
			AppendMenu(menupop,MF_STRING,ID_MENU_DASHDOT,L"点虚线");
			AppendMenu(menubar,MF_STRING|MF_POPUP,(UINT_PTR)menupop,L"选择线型");
			SetMenu(hWnd,menubar);
			*/
			HMENU menu=CreateMenu();
			HMENU menupop_image=CreatePopupMenu();
			HMENU menupop_pen=CreatePopupMenu();
			HMENU menupop_pen_color=CreatePopupMenu();
			HMENU menupop_image_exp=CreatePopupMenu();

			AppendMenu(menupop_image,MF_STRING,ID_MENU_IMAGE_CLEAR,L"清空");
			AppendMenu(menupop_image,MF_STRING,ID_MENU_IMAGE_TDIST,L"缩放");
			//AppendMenu(menupop_image,MF_STRING,ID_MENU_IMAGE_EXPORT,L"导出");
			AppendMenu(menupop_pen_color,MF_STRING,ID_MENU_PEN_COLOR_BLACK,L"黑色");
			AppendMenu(menupop_pen_color,MF_STRING,ID_MENU_PEN_COLOR_BLUE,L"蓝色");
			AppendMenu(menupop_pen_color,MF_STRING,ID_MENU_PEN_COLOR_RED,L"红色");
			AppendMenu(menupop_pen_color,MF_STRING,ID_MENU_PEN_COLOR_GREEN,L"绿色");
			AppendMenu(menupop_pen_color,MF_STRING,ID_MENU_PEN_COLOR_YELLOW,L"黄色");
			AppendMenu(menupop_image_exp,MF_STRING,ID_MENU_IMAGE_EXPORT_BMP,L"BMP格式");
			AppendMenu(menupop_image_exp,MF_STRING,ID_MENU_IMAGE_EXPORT_PNG,L"PNG格式");
			

			AppendMenu(menu,MF_STRING|MF_POPUP,(UINT_PTR)menupop_image,L"图像");
			AppendMenu(menu,MF_STRING|MF_POPUP,(UINT_PTR)menupop_pen,L"画笔");
			AppendMenu(menupop_pen,MF_STRING|MF_POPUP,(UINT_PTR)menupop_pen_color,L"颜色");
			AppendMenu(menupop_image,MF_STRING|MF_POPUP,(UINT_PTR)menupop_image_exp,L"导出");
			AppendMenu(menupop_image,MF_STRING,ID_MENU_IMAGE_IMPORT,L"导入");
			SetMenu(hWnd,menu);

			// Child Window Defination
			btnPaintHwnd=CreateWindow(TEXT("button"),TEXT("绘图"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
				430,530,80,25,hWnd,(HMENU)ID_PAINT_BUTTON,(HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE),NULL);

			edtPaintHwnd=CreateWindow(TEXT("edit"),TEXT("请在这里输入函数式（因变量为x）"),WS_CHILD|WS_VISIBLE|WS_BORDER,
				70,530,350,25,hWnd,(HMENU)ID_TEXT_FIELD,((LPCREATESTRUCT)lParam)->hInstance,NULL);
			
			sttPaintHwnd_input=CreateWindow(TEXT("static"),TEXT("f(x)="),WS_CHILD|WS_VISIBLE,
				40,530,30,25,hWnd,(HMENU)ID_STATIC_HINT,((LPCREATESTRUCT)lParam)->hInstance,NULL);
			
			sttPaintHwnd_numn=CreateWindow(TEXT("static"),TEXT("5"),WS_CHILD|WS_VISIBLE,
				235,125,30,25,hWnd,(HMENU)ID_STATIC_HINT,((LPCREATESTRUCT)lParam)->hInstance,NULL);

			sttPaintHwnd_nums=CreateWindow(TEXT("static"),TEXT("-5"),WS_CHILD|WS_VISIBLE,
				235,375,30,25,hWnd,(HMENU)ID_STATIC_HINT,((LPCREATESTRUCT)lParam)->hInstance,NULL);
			
			sttPaintHwnd_nume=CreateWindow(TEXT("static"),TEXT("5"),WS_CHILD|WS_VISIBLE,
				375,235,30,25,hWnd,(HMENU)ID_STATIC_HINT,((LPCREATESTRUCT)lParam)->hInstance,NULL);
			
			sttPaintHwnd_numw=CreateWindow(TEXT("static"),TEXT("-5"),WS_CHILD|WS_VISIBLE,
				125,235,30,25,hWnd,(HMENU)ID_STATIC_HINT,((LPCREATESTRUCT)lParam)->hInstance,NULL);
			
			sttPaintHwnd_numc=CreateWindow(TEXT("static"),TEXT("0"),WS_CHILD|WS_VISIBLE,
				245,240,30,25,hWnd,(HMENU)ID_STATIC_HINT,((LPCREATESTRUCT)lParam)->hInstance,NULL);

		}
		return 0;
	case WM_COMMAND:

		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case ID_PAINT_BUTTON:
			switch (wmEvent)
			{
			case BN_CLICKED:
				function.clear();
				datas.clear();

				szBuffer=(TCHAR*)malloc(1024*sizeof(TCHAR));
				GetWindowText(edtPaintHwnd,szBuffer,1024);

				//MessageBox(hWnd,szBuffer,TEXT("BUTTON MESSAGE"),MB_OK);
				SetWindowText(btnPaintHwnd,L"生成中..");
				EnableWindow(btnPaintHwnd,false);

				sprintf_s(szExpr,1024,"%S",szBuffer);

				poly=std::string(szExpr);
				
				FPOINT fp;

				for(int i=-250;i<=250;i++){
					std::string tmpstr=poly;
					float x=i*((float)tick_dist)/25;
					change_fx(tmpstr);
					change_x(tmpstr,x);
					float y=parse_poly(tmpstr);
					fp.x=x;
					fp.y=y;
					function.push_back(fp);
				}

				for(int i=0;i<501-1;i++){
					int x1_off=function[i].x/(tick_dist/25);
					int y1_off=function[i].y/(tick_dist/25);
					int x2_off=function[i+1].x/(tick_dist/25);
					int y2_off=function[i+1].y/(tick_dist/25);
					if(x1_off<-250||x1_off>250)continue;
					if(y1_off<-250||y1_off>250)continue;
					if(x2_off<-250||x2_off>250)continue;
					if(y2_off<-250||y2_off>250)continue;
					PAINTDATA tmpdata;
					tmpdata.ptBeginX=260+x2_off;
					tmpdata.ptBeginY=260-y2_off;
					tmpdata.ptEndX=260+x1_off;
					tmpdata.ptEndY=260-y1_off;
					tmpdata.penStyle=PS_SOLID;
					datas.push_back(tmpdata);
				}

				PAINTSTRUCT tmpps;
				BeginPaint(hWnd,&tmpps);
				paint_points(hWnd,datas,tmpps,data_color);
				EndPaint(hWnd, &tmpps);
				free(szBuffer);
				bExist=false;
				for(int x=0;x<img_vec.size();x++){
					if(img_vec[x]==poly){
						bExist=true;
					}
				}
				if(bExist){
					;
				}else{
					img_vec.push_back(poly);
				}
				EnableWindow(btnPaintHwnd,true);
				SetWindowText(btnPaintHwnd,L"绘图");
				break;
			default:
				break;
			}
			break;
		case ID_TEXT_FIELD:
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_MENU_IMAGE_CLEAR:
			datas.clear();
			img_vec.clear();
			InvalidateRect(hWnd,NULL,true);
			UpdateWindow(hWnd);	
			break;
		case ID_MENU_IMAGE_IMPORT:
			{
				SetWindowText(btnPaintHwnd,L"生成中..");
				EnableWindow(btnPaintHwnd,false);

				WCHAR name[MAX_PATH];
				OPENFILENAME ofn;
				memset(&ofn,0,sizeof(OPENFILENAME));
				memset(name,0,MAX_PATH);
				ofn.lStructSize=sizeof(OPENFILENAME);
				ofn.lpstrFile=name;
				ofn.nMaxFile=MAX_PATH;
				static TCHAR szFilter[] = TEXT("Csv Files(*.csv)\0*.csv\0") \
					TEXT("All Files(*.*)\0*.*\0\0");
				ofn.lpstrFilter=szFilter;
				ofn.Flags=OFN_FILEMUSTEXIST;
				if(GetOpenFileName(&ofn)){
					//MessageBox(hWnd,name,L"title",MB_OK);
					WCHAR strbuf[1024];
					FILE*file;
					WCHAR*strtoken=NULL;
					WCHAR*strnext=NULL;
					_wfopen_s(&file,name,L"r");
					
					while(fgetws(strbuf,1024,file)!=NULL){
						int len=wcslen(strbuf);
						strbuf[len-1]='\0';
						strtoken=wcstok_s(strbuf,L",",&strnext);
						while(strtoken!=NULL){
							SetWindowText(edtPaintHwnd,strtoken);
							SendMessage(hWnd,WM_COMMAND,MAKELONG(ID_PAINT_BUTTON,BN_CLICKED),(LPARAM)btnPaintHwnd);
							strtoken=wcstok_s(NULL,L",",&strnext);			
						}
											
					}

					fclose(file);

					SetWindowText(btnPaintHwnd,L"绘图");
					EnableWindow(btnPaintHwnd,true);

				}
			}
			break;
		case ID_MENU_IMAGE_TDIST:

			//
			{
				TCHAR tmpBuff[512];
				DialogBox(hInst,MAKEINTRESOURCE(IDD_DISTBOX),hWnd,Dist);
				if(g_fdist>0)tick_dist=g_fdist;
				swprintf_s(tmpBuff,512,L"%f",tick_dist*5);
				SetWindowText(sttPaintHwnd_numn,tmpBuff);
				SetWindowText(sttPaintHwnd_nums,tmpBuff);
				SetWindowText(sttPaintHwnd_numw,tmpBuff);
				SetWindowText(sttPaintHwnd_nume,tmpBuff);
				datas.clear();
				InvalidateRect(hWnd,NULL,true);
				UpdateWindow(hWnd);	
			
			}
			tick_dist=g_fdist;

			break;

		case ID_MENU_IMAGE_EXPORT_BMP:
			{
				HDC hdc=GetDC(hWnd);
				HDC memdc=CreateCompatibleDC(hdc);
				RECT rect;
				GetWindowRect(hWnd,&rect);
				SIZE bmpsize;
				bmpsize.cx=520;
				bmpsize.cy=520;

				HBITMAP hBitmap=CreateCompatibleBitmap(hdc,bmpsize.cx,bmpsize.cy);
				HGDIOBJ hOldBMP=SelectObject(memdc,hBitmap);
				BitBlt(memdc,0,0,bmpsize.cx,bmpsize.cy,hdc,0,0,SRCCOPY);
				SelectObject(memdc,hOldBMP);
				DeleteObject(memdc);
				ReleaseDC(hWnd,hdc);

				TCHAR fname[512];
				SYSTEMTIME st;
				GetLocalTime(&st);
				swprintf_s(fname,L"%2d_%2d_%d_%d_%d.bmp",st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			
				write_bmp(hBitmap,fname);
				MessageBox(hWnd,L"BMP格式图像成功生成！请见程序或者CSV文件2所在目录！",L"成功",MB_OK);
			
			}
			break;

		case ID_MENU_IMAGE_EXPORT_PNG:
						
			{
				HDC hdc=GetDC(hWnd);
				HDC memdc=CreateCompatibleDC(hdc);
				RECT rect;
				GetWindowRect(hWnd,&rect);
				SIZE bmpsize;
				bmpsize.cx=520;
				bmpsize.cy=520;

				HBITMAP hBitmap=CreateCompatibleBitmap(hdc,bmpsize.cx,bmpsize.cy);
				HGDIOBJ hOldBMP=SelectObject(memdc,hBitmap);
				BitBlt(memdc,0,0,bmpsize.cx,bmpsize.cy,hdc,0,0,SRCCOPY);
				SelectObject(memdc,hOldBMP);
				DeleteObject(memdc);
				ReleaseDC(hWnd,hdc);

				TCHAR fname[512];
				SYSTEMTIME st;
				GetLocalTime(&st);
				swprintf_s(fname,L"%2d_%2d_%d_%d_%d.png",st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			
				write_png(hBitmap,fname);
				
				MessageBox(hWnd,L"PNG格式图像成功生成！请见程序或者CSV文件所在目录！",L"成功",MB_OK);
			}
			break;

		case ID_MENU_PEN_COLOR_BLACK:
			data_color=RGB(0,0,0);
			break;
		case ID_MENU_PEN_COLOR_RED:
			data_color=RGB(220,20,60);
			break;
		case ID_MENU_PEN_COLOR_BLUE:
			data_color=RGB(0,0,255);
			break;
		case ID_MENU_PEN_COLOR_GREEN:
			data_color=RGB(50,205,50);
			break;
		case ID_MENU_PEN_COLOR_YELLOW:
			data_color=RGB(255,255,0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);
			
			// TODO: 在此添加任意绘图代码...
			PAINTSTRUCT ps;
			BeginPaint(hWnd,&ps);
			// Paint Coordinates
			paint_points(hWnd,bounders,ps,RGB(0,0,0));
			paint_points(hWnd,ticks,ps,RGB(200,200,200));
			paint_points(hWnd,coordinates,ps,RGB(0,0,0));
			paint_points(hWnd,arrows,ps,RGB(0,0,0));
			paint_points(hWnd,marks,ps,RGB(0,0,0));

			paint_points(hWnd,datas,ps,data_color);

			EndPaint(hWnd, &ps);
		}
		break;
	case WM_CTLCOLORSTATIC:
		SetBkMode((HDC)wParam,TRANSPARENT);
		return (BOOL)((HBRUSH)GetStockObject(NULL_BRUSH));
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	
	case WM_LBUTTONDOWN:

		//datas.clear();
		//InvalidateRect(hWnd,NULL,true);
		//UpdateWindow(hWnd);	
			
		//SetWindowText(sttPaintHwnd_input,TEXT("改了"));
		break;
/*	case WM_LBUTTONUP:
		{
			if(pCurrentData!=NULL){
				pCurrentData->ptEndX=GET_X_LPARAM(lParam);
				pCurrentData->ptEndY=GET_Y_LPARAM(lParam);

				HDC hdc=GetDC(hWnd);
				HPEN pen=CreatePen(pCurrentData->penStyle,1,RGB(0,255,0));
				HPEN oldpen=(HPEN)SelectObject(hdc,pen);
				MoveToEx(hdc,pCurrentData->ptBeginX,pCurrentData->ptBeginY,NULL);
				LineTo(hdc,pCurrentData->ptEndX,pCurrentData->ptEndY);
				SelectObject(hdc,oldpen);
				DeleteObject(pen);
			}
		}
		return 0;
*/
	case WM_MOUSEWHEEL:
		{
			datas.clear();
			InvalidateRect(hWnd,NULL,true);
			UpdateWindow(hWnd);	
			RECT rect;
			GetWindowRect(hWnd,&rect);
			int fwKeys= LOWORD(wParam);
			short zDelta= (SHORT)HIWORD(wParam);
			int xPos=LOWORD(lParam);
			int yPos=HIWORD(lParam);
			int winy=yPos-rect.top-60;//60 
			int winx=xPos-rect.left-20;;//20
			if(zDelta>0){
				tick_dist*=0.8;
			}else{
				tick_dist*=1.25;
			}
			TCHAR tmpBuff[512];
			swprintf_s(tmpBuff,512,L"%f",tick_dist*5);
			SetWindowText(sttPaintHwnd_numn,tmpBuff);
			SetWindowText(sttPaintHwnd_nums,tmpBuff);
			SetWindowText(sttPaintHwnd_numw,tmpBuff);
			SetWindowText(sttPaintHwnd_nume,tmpBuff);
			InvalidateRect(hWnd,NULL,true);
			UpdateWindow(hWnd);			
			for(int i=0;i<img_vec.size();i++){
				std::string tmpexp=img_vec[i];
				TCHAR wexp[1024];
				swprintf_s(wexp,L"%S",tmpexp.c_str());
				SetWindowText(edtPaintHwnd,wexp);
				SendMessage(hWnd,WM_COMMAND,MAKELONG(ID_PAINT_BUTTON,BN_CLICKED),(LPARAM)btnPaintHwnd);
			}	
			SetWindowText(edtPaintHwnd,L"");
				
				/*datas.clear();
				FPOINT fp;
				function.clear();
				for(int j=-250;j<=250;j++){
					std::string tmpstr=img_vec[i];
					float x=i*((float)tick_dist)/25;
					change_fx(tmpstr);
					change_x(tmpstr,x);
					float y=parse_poly(tmpstr);
					fp.x=x;
					fp.y=y;
					function.push_back(fp);
				}

				for(int j=0;j<501-1;j++){
					int x1_off=function[j].x/(tick_dist/25);
					int y1_off=function[j].y/(tick_dist/25);
					int x2_off=function[j+1].x/(tick_dist/25);
					int y2_off=function[j+1].y/(tick_dist/25);
					if(x1_off<-250||x1_off>250)continue;
					if(y1_off<-250||y1_off>250)continue;
					if(x2_off<-250||x2_off>250)continue;
					if(y2_off<-250||y2_off>250)continue;
					PAINTDATA tmpdata;
					tmpdata.ptBeginX=260+x2_off;
					tmpdata.ptBeginY=260-y2_off;
					tmpdata.ptEndX=260+x1_off;
					tmpdata.ptEndY=260-y1_off;
					tmpdata.penStyle=PS_SOLID;
					datas.push_back(tmpdata);
				}
				
				PAINTSTRUCT tmpps;
				BeginPaint(hWnd,&tmpps);
				paint_points(hWnd,datas,tmpps,data_color);
				EndPaint(hWnd, &tmpps);
			}*/

			//InvalidateRect(hWnd,NULL,true);
			//UpdateWindow(hWnd);
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}






// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK Dist(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	TCHAR szwBuffer[512];
	char szBuffer[512];
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:

		if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}else if(LOWORD(wParam)==IDOK){
			GetDlgItemText(hDlg,IDC_EDIT1,szwBuffer,512);
			sprintf_s(szBuffer,512,"%S",szwBuffer);
			g_fdist=atof(szBuffer);
			EndDialog(hDlg,LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void paint_points(HWND hWnd,std::vector<PAINTDATA>&pts,PAINTSTRUCT&ps,COLORREF color){



	std::vector<PAINTDATA>::const_iterator item;
	HDC hdc=GetDC(hWnd);
	for(item=pts.begin();item!=pts.end();item++){
		HPEN pen=CreatePen(item->penStyle,1,color);
		
		HPEN oldpen=(HPEN)SelectObject(hdc,pen);
		SelectObject(ps.hdc,pen);
		MoveToEx(hdc,item->ptBeginX,item->ptBeginY,NULL);
		LineTo(hdc,item->ptEndX,item->ptEndY);
		DeleteObject(pen);
		SelectObject(hdc,oldpen);
		DeleteObject(pen);
	}
	ReleaseDC(hWnd,hdc);
}

void bounder_def(std::vector<PAINTDATA>&bounder){

	
	PAINTDATA tmpdata;
	int penStyle=PS_DASH;

	// Bounder Defination
	tmpdata.ptBeginX=10;
	tmpdata.ptBeginY=10;
	tmpdata.ptEndX=10;
	tmpdata.ptEndY=510;
	tmpdata.penStyle=penStyle;
	bounder.push_back(tmpdata);
	
	tmpdata.ptBeginX=10;
	tmpdata.ptBeginY=510;
	tmpdata.ptEndX=510;
	tmpdata.ptEndY=510;
	tmpdata.penStyle=penStyle;
	bounder.push_back(tmpdata);

	tmpdata.ptBeginX=510;
	tmpdata.ptBeginY=510;
	tmpdata.ptEndX=510;
	tmpdata.ptEndY=10;
	tmpdata.penStyle=penStyle;
	bounder.push_back(tmpdata);

	tmpdata.ptBeginX=510;
	tmpdata.ptBeginY=10;
	tmpdata.ptEndX=10;
	tmpdata.ptEndY=10;
	tmpdata.penStyle=penStyle;
	bounder.push_back(tmpdata);
}

void coordinate_def(std::vector<PAINTDATA>&coordinate,std::vector<PAINTDATA>&arrow){
	PAINTDATA tmpdata;
	int penStyle=PS_SOLID;

	tmpdata.ptBeginX=10;
	tmpdata.ptBeginY=260;
	tmpdata.ptEndX=510;
	tmpdata.ptEndY=260;
	tmpdata.penStyle=penStyle;
	coordinate.push_back(tmpdata);
	
	tmpdata.ptBeginX=260;
	tmpdata.ptBeginY=510;
	tmpdata.ptEndX=260;
	tmpdata.ptEndY=10;
	tmpdata.penStyle=penStyle;
	coordinate.push_back(tmpdata);
	
	tmpdata.ptBeginX=255;
	tmpdata.ptBeginY=20;
	tmpdata.ptEndX=260;
	tmpdata.ptEndY=10;
	tmpdata.penStyle=penStyle;
	arrow.push_back(tmpdata);

	tmpdata.ptBeginX=265;
	tmpdata.ptBeginY=20;
	tmpdata.ptEndX=260;
	tmpdata.ptEndY=10;
	tmpdata.penStyle=penStyle;
	arrow.push_back(tmpdata);

	tmpdata.ptBeginX=510;
	tmpdata.ptBeginY=260;
	tmpdata.ptEndX=500;
	tmpdata.ptEndY=255;
	tmpdata.penStyle=penStyle;
	arrow.push_back(tmpdata);

	tmpdata.ptBeginX=510;
	tmpdata.ptBeginY=260;
	tmpdata.ptEndX=500;
	tmpdata.ptEndY=265;
	tmpdata.penStyle=penStyle;
	arrow.push_back(tmpdata);
}

void tick_def(std::vector<PAINTDATA>&ticks){
	PAINTDATA tmpdata;
	int penStyle=PS_DASHDOT;
	for(int i=1;i<20;i++){
		tmpdata.ptBeginX=10+25*i;
		tmpdata.ptBeginY=10;
		tmpdata.ptEndX=10+25*i;
		tmpdata.ptEndY=510;
		tmpdata.penStyle=penStyle;
		ticks.push_back(tmpdata);
		
		tmpdata.ptBeginY=10+25*i;
		tmpdata.ptBeginX=10;
		tmpdata.ptEndY=10+25*i;
		tmpdata.ptEndX=510;
		tmpdata.penStyle=penStyle;
		ticks.push_back(tmpdata);
		
	}
}

void mark_def(std::vector<PAINTDATA>&marks){
	PAINTDATA tmpdata;
	int penStyle=PS_SOLID;
	for(int i=1;i<20;i++){
		tmpdata.ptBeginX=10+25*i;
		if(i%5==0)tmpdata.ptBeginY=255;
		else tmpdata.ptBeginY=258;
		tmpdata.ptEndX=10+25*i;
		if(i%5==0)tmpdata.ptEndY=265;
		else tmpdata.ptEndY=263;
		marks.push_back(tmpdata);
	
		tmpdata.ptBeginY=10+25*i;
		if(i%5==0)tmpdata.ptBeginX=255;
		else tmpdata.ptBeginX=258;
		tmpdata.ptEndY=10+25*i;
		if(i%5==0)tmpdata.ptEndX=265;
		else tmpdata.ptEndX=262;
		marks.push_back(tmpdata);
		
	}
}


void write_bmp(HBITMAP hBitmap,TCHAR*str1){

	HDC hDC =CreateDC(L"DISPLAY",NULL,NULL,NULL);  
    int iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);//当前分辨率下每个像素所占字节数    
    DeleteDC(hDC); 

	WORD   wBitCount;   //位图中每个像素所占字节数      
    if (iBits <= 1)  
        wBitCount = 1;  
    else if (iBits <= 4)  
        wBitCount = 4;  
    else if (iBits <= 8)  
        wBitCount = 8;  
    else if (iBits <= 24)  
        wBitCount = 24;  
    else  
        wBitCount = iBits;  

	DWORD   dwPaletteSize=0;    //调色板大小， 位图中像素字节大小  
    if (wBitCount <= 8)          
        dwPaletteSize = (1 << wBitCount) *    sizeof(RGBQUAD);      
    
	BITMAP  bm;        //位图属性结构  
    ::GetObject(hBitmap, sizeof(bm), (LPSTR)&bm);    
  
      
    BITMAPINFOHEADER   bi,bi1;       //位图信息头结构       
    bi.biSize            = sizeof(BITMAPINFOHEADER);    
    bi.biWidth           = bm.bmWidth;  
    bi.biHeight          = bm.bmHeight;  
    bi.biPlanes          = 1;  
    bi.biBitCount        = wBitCount;  
    bi.biCompression     = BI_RGB; //BI_RGB表示位图没有压缩  
    bi.biSizeImage       = 0;  
    bi.biXPelsPerMeter   = 0;  
    bi.biYPelsPerMeter   = 0;  
    bi.biClrUsed         = 0;  
    bi.biClrImportant    = 0;  
    bi1=bi;  
    bi1.biBitCount=24;  

	DWORD dwBmBitsSize = ((bm.bmWidth * wBitCount+31)/32) * 4 * bm.bmHeight;     
    HANDLE hDib  = ::GlobalAlloc(GHND,dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));  //为位图内容分配内存  
    LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);  
    *lpbi = bi;  
      
    HANDLE hPal = ::GetStockObject(DEFAULT_PALETTE);  // 处理调色板  
    HANDLE  hOldPal=NULL;  
    if (hPal)  
    {  
        hDC = ::GetDC(NULL);  
        hOldPal = SelectPalette(hDC,(HPALETTE)hPal, FALSE);  
        RealizePalette(hDC);  
    }  
    int nOutputBytes = 0;  
    unsigned char *pJpp,*m_pDibBits;  
    pJpp = new unsigned char [dwBmBitsSize];  
    m_pDibBits = new unsigned char [dwBmBitsSize];  
    //::GetDIBits(hDC, hBitmap, 0, (UINT) bm.bmHeight,m_pDibBits,(BITMAPINFO*)lpbi,DIB_RGB_COLORS);  
    GetDIBits(hDC, hBitmap, 0, (UINT) bm.bmHeight,(LPSTR)lpbi + sizeof(BITMAPINFOHEADER)+dwPaletteSize,(BITMAPINFO*)lpbi,DIB_RGB_COLORS);// 获取该调色板下新的像素值  

	if (hOldPal)//恢复调色板  
    {  
        SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);  
        RealizePalette(hDC);										
        ::ReleaseDC(NULL, hDC);  
    }  
    memcpy(m_pDibBits,(LPSTR)lpbi+sizeof(BITMAPINFOHEADER)+dwPaletteSize,dwBmBitsSize); 

	BITMAPFILEHEADER   bmfHdr; //位图文件头结构       
    bmfHdr.bfType = 0x4D42;  // "BM"      // 设置位图文件头  
    DWORD dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;    
    bmfHdr.bfSize = dwDIBSize;  
    bmfHdr.bfReserved1 = 0;  
    bmfHdr.bfReserved2 = 0;  
    bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;  
      
    HANDLE hFile = CreateFile(str1, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);//创建位图文件   

	int s=GetLastError();

	DWORD dwWritten;  
    WriteFile(hFile, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);    // 写入位图文件头  
    WriteFile(hFile, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);// 写入位图文件其余内容  
       
    //保存到内存  
    //char* BmpBuffer;  
    //BmpBuffer = new char [sizeof(BITMAPFILEHEADER) + dwDIBSize];  
    //memcpy(BmpBuffer,(LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER));  
    //memcpy(BmpBuffer+sizeof(BITMAPFILEHEADER),(LPSTR)lpbi, dwDIBSize);  
  
    GlobalUnlock(hDib);   //清除     
    GlobalFree(hDib);  
    CloseHandle(hFile);  

	s=GetLastError();

	//delete BmpBuffer;

	delete []pJpp;  
    delete []m_pDibBits;  


}


int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
        UINT  num = 0;          // number of image encoders
        UINT  size = 0;         // size of the image encoder array in bytes

        ImageCodecInfo* pImageCodecInfo = NULL;

        ::GetImageEncodersSize(&num, &size);
        if(size == 0)
        return -1;  // Failure

        pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
        if(pImageCodecInfo == NULL)
        return -1;  // Failure

        ::GetImageEncoders(num, size, pImageCodecInfo);

        for(UINT j = 0; j < num; ++j)
        {
                if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
                {
                        *pClsid = pImageCodecInfo[j].Clsid;
                        free(pImageCodecInfo);
                        return j;  // Success
                }
        }

        free(pImageCodecInfo);
        return -1;  // Failure
}

void write_png(HBITMAP hBitmap,TCHAR*str1){
	GdiplusStartupInput ginput;
	ULONG_PTR gditoken;
	GdiplusStartup(&gditoken,&ginput,NULL);

	Bitmap*pb=Bitmap::FromHBITMAP(hBitmap,NULL);
	WCHAR*filename=str1;
	CLSID pngclsid;
	GetEncoderClsid(L"image/png",&pngclsid);
	pb->Save(filename,&pngclsid,NULL);

	GdiplusShutdown(gditoken);

  }

void get_export_dir(WCHAR*dirname){
	OPENFILENAME ofn;
	ofn.lStructSize=sizeof(OPENFILENAME);
	ofn.lpstrFile=dirname;
	ofn.nMaxFile=MAX_PATH;
	ofn.Flags=OFN_EXPLORER|OFN_ENABLEHOOK|OFN_HIDEREADONLY|OFN_NOCHANGEDIR|OFN_PATHMUSTEXIST;
	// ofn.lpfnHook=SaveAsHookProc;
	ofn.lpstrFilter=TEXT("All File\0*.*\0\0");
	GetSaveFileName(&ofn);

	return;
}