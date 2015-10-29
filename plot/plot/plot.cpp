// plot.cpp : ����Ӧ�ó������ڵ㡣
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



#define MAX_LOADSTRING 100



// ȫ�ֱ���:
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

/*
#define POLY_X_BASE 0
#define POLY_LOG_BASE 1
#define POLY_SIN_BASE 2
#define POLY_COS_BASE 3
#define POLY_EXP_BASE 4
*/

typedef struct tagData{
	int ptBeginX,ptBeginY;
	int ptEndX,ptEndY;
	int penStyle;
}PAINTDATA;


/*
typedef struct tagNode{
	int baseType; // Base Of Polynomial
	double powNum;
	double coffNum;
}POLYNODE;
*/

/*
class SyntaxTree{
	
public:

	SyntaxTree(){;}

	SyntaxTree(std::string polynomial){
				
	}
};


class POLYNODE{

	int iLen;
	SyntaxTree trSyn;

public:
	POLYNODE(){;}
	POLYNODE(std::string polynomial){
		trSyn=SyntaxTree(polynomial); // Shallow Copy
	}
	
	double calc(double x){

		return 0; // This line may never reach
	}
};
*/



// �˴���ģ���а����ĺ�����ǰ������:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);





int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	//*************

	

	//*************
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: �ڴ˷��ô��롣
	MSG msg;
	HACCEL hAccelTable;

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PLOT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PLOT));

	// ����Ϣѭ��:
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
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
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
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��:
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 500, 500, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��: ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
//
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	static std::vector<PAINTDATA> datas;
	static std::vector<PAINTDATA> coordinates;
	static std::vector<PAINTDATA> ticks;
	static std::vector<PAINTDATA> arrows;
	
	static PAINTDATA*pCurrentData=NULL;
	static int penStyle=PS_SOLID;


	switch (message)
	{
	case WM_CREATE:
		{
			HMENU menubar=CreateMenu();
			HMENU menupop=CreatePopupMenu();
			AppendMenu(menupop,MF_STRING,(UINT_PTR)PS_SOLID,L"ʵ��");
			AppendMenu(menupop,MF_STRING,(UINT_PTR)PS_DASH,L"����");
			AppendMenu(menupop,MF_STRING,(UINT_PTR)PS_DOT,L"����");
			AppendMenu(menupop,MF_STRING,(UINT_PTR)PS_DASHDOT,L"������");
			AppendMenu(menubar,MF_STRING|MF_POPUP,(UINT_PTR)menupop,L"ѡ������");
			SetMenu(hWnd,menubar);
		}
		return 0;
	case WM_COMMAND:

		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �����˵�ѡ��:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);
			
			// TODO: �ڴ���������ͼ����...
			PAINTSTRUCT ps;
			BeginPaint(hWnd,&ps);
			std::vector<PAINTDATA>::const_iterator item;

			for(item=datas.begin();item!=datas.end();item++){
				HPEN pen=CreatePen(item->penStyle,1,RGB(0,255,0));
				SelectObject(ps.hdc,pen);
				MoveToEx(ps.hdc,item->ptBeginX,item->ptBeginY,NULL);
				LineTo(ps.hdc,item->ptEndX,item->ptEndY);
				DeleteObject(pen);
			}
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	
	case WM_LBUTTONDOWN:
		{
			pCurrentData=new PAINTDATA;
			pCurrentData->penStyle=penStyle;
			pCurrentData->ptBeginX=GET_X_LPARAM(lParam);
			pCurrentData->ptBeginY=GET_Y_LPARAM(lParam);
		}
		return 0;
	case WM_LBUTTONUP:
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
				ReleaseDC(hWnd,hdc);
				datas.push_back(*pCurrentData);
			}
		}
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


// �����ڡ������Ϣ�������
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


