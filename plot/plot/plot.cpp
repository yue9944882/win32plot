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

using namespace std;

#define MAX_LOADSTRING 100


#define ID_PAINT_BUTTON 0x01
#define ID_TEXT_FIELD 0x02
#define ID_STATIC_HINT 0x03

#define ID_MENU_IMAGE_CLEAR 0x11

#define ID_MENU_PEN_COLOR_BLACK 0x21
#define ID_MENU_PEN_COLOR_BLUE 0x22
#define ID_MENU_PEN_COLOR_RED 0x23
#define ID_MENU_PEN_COLOR_YELLOW 0x24
#define ID_MENU_PEN_COLOR_GREEN 0x25
#define ID_MENU_PEN_COLOR_PURPLE 0x26



// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名


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
void paint_points(HWND hWnd,std::vector<PAINTDATA>&pts,PAINTSTRUCT&ps,COLORREF color);
void bounder_def(std::vector<PAINTDATA>&bounder);
void coordinate_def(std::vector<PAINTDATA>&coordinate,std::vector<PAINTDATA>&arrow);
void tick_def(std::vector<PAINTDATA>&ticks);
void mark_def(std::vector<PAINTDATA>&marks);


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

	static PAINTDATA*pCurrentData=NULL;

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
			HMENU menu_image=CreateMenu();
			HMENU menupop_image=CreatePopupMenu();
			HMENU menupop_pen=CreatePopupMenu();
			HMENU menupop_pen_color=CreatePopupMenu();
			AppendMenu(menupop_image,MF_STRING,ID_MENU_IMAGE_CLEAR,L"清空");
			AppendMenu(menupop_pen_color,MF_STRING,ID_MENU_PEN_COLOR_BLACK,L"黑色");
			AppendMenu(menupop_pen_color,MF_STRING,ID_MENU_PEN_COLOR_BLUE,L"蓝色");
			AppendMenu(menupop_pen_color,MF_STRING,ID_MENU_PEN_COLOR_RED,L"红色");
			AppendMenu(menupop_pen_color,MF_STRING,ID_MENU_PEN_COLOR_GREEN,L"绿色");
			AppendMenu(menupop_pen_color,MF_STRING,ID_MENU_PEN_COLOR_YELLOW,L"黄色");
			
			
			AppendMenu(menu_image,MF_STRING|MF_POPUP,(UINT_PTR)menupop_image,L"图像");
			AppendMenu(menu_image,MF_STRING|MF_POPUP,(UINT_PTR)menupop_pen,L"画笔");
			AppendMenu(menupop_pen,MF_STRING|MF_POPUP,(UINT_PTR)menupop_pen_color,L"颜色");
			SetMenu(hWnd,menu_image);

			// Child Window Defination
			btnPaintHwnd=CreateWindow(TEXT("button"),TEXT("绘图"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
				440,530,60,25,hWnd,(HMENU)ID_PAINT_BUTTON,(HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE),NULL);

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
				MessageBox(hWnd,szBuffer,TEXT("BUTTON MESSAGE"),MB_OK);
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
			InvalidateRect(hWnd,NULL,true);
			UpdateWindow(hWnd);	
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
		datas.clear();
		InvalidateRect(hWnd,NULL,true);
		UpdateWindow(hWnd);	
			
		SetWindowText(sttPaintHwnd_input,TEXT("改了"));
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
