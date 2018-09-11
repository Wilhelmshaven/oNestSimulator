/*************************************************************
* Signature						Date                         *
* Li Hongjie                    2018.08.15                   *
*************************************************************/

// oNestSimulator.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "oNestSimulator.h"

// ������任��ʹ��WIN8���
#pragma comment(linker, "\"/manifestdependency:type='Win32'\
 name='Microsoft.Windows.Common-Controls' version='6.0.0.0'\
 processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// ȫ�ֱ��� 
enum CustomDefine
{
	MAX_LOADSTRING = 100
};
enum BottleNeckPoint
{
	NETWORK = 1,
	LB = 2,
	RGW = 3,
	OSD = 4,
	OSD_RGW = 5,
	OSD_LB = 6,
	DISK = 7
};

HINSTANCE hInst;                                // ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];                  // �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];            // ����������

cluster_configuration cluster_conf;             // ��Ⱥ��Ʋ���
simulator_result result;                        // ģ����
HANDLE hEventEmptyInput = CreateEvent(NULL, TRUE, FALSE, NULL);  // �������ж�

// �˴���ģ���а����ĺ�����ǰ������: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);        // �Ӵ���

int getEditBoxText(HWND hdlg, int ID);
int oNestSimulator(cluster_configuration cluster);
BOOL AddListViewItems(HWND hwndListView);
int itemTextSet(HWND hwndListView, LVITEM lvi, TCHAR name[4096], double para1, double para2, double para3, double para4, double para5);
int itemTextSet(HWND hwndListView, LVITEM lvi, TCHAR name[4096], double para1, double para2, TCHAR para3[4096], TCHAR para4[4096], double para5);
int itemTextSet(HWND hwndListView, LVITEM lvi, TCHAR name[4096], double para1, double para2, TCHAR para3[4096], TCHAR para4[4096], TCHAR para5[4096]);

/*============================== WinMain ==============================*/
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					 _In_opt_ HINSTANCE hPrevInstance,
					 _In_ LPWSTR    lpCmdLine,
					 _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: �ڴ˷��ô��롣

	// ��ʼ��ȫ���ַ���
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_ONESTSIMULATOR, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ONESTSIMULATOR));

	MSG msg;

	// ����Ϣѭ��: 
	while (GetMessage(&msg, nullptr, 0, 0))
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
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ONESTSIMULATOR));
	wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_ONESTSIMULATOR);
	wcex.lpszClassName  = szWindowClass;
	wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
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

	// �����ڲ��ɱ��С��������
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, 0, 500, 600, NULL, NULL, hInstance, NULL);

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
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND  - ����Ӧ�ó���˵�
//  WM_PAINT    - ����������
//  WM_DESTROY  - �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	HWND myhdlg;

	switch (message)
	{
	case WM_CREATE:
	{
		// �����ӶԻ��򲢽�����Ϊ������
		myhdlg = CreateDialog(hInst, MAKEINTRESOURCE(IDD_FORMVIEW), hWnd, (DLGPROC)DlgProc);
		ShowWindow(myhdlg, SW_SHOW);                                  // ��ʾ�ӶԻ���

		// ��������
		// ���ñ���������ʽ���ⲿ�ֱ�����������ڴ���
		LOGFONT TitleFont;
		ZeroMemory(&TitleFont, sizeof(TitleFont));                    // �����������������߰���ĳ�ֵ
		lstrcpy(TitleFont.lfFaceName, L"Microsoft Yahei");            // ��������
		TitleFont.lfWeight = FW_BOLD;                                 // ��ϸ��BOLD=700��д��CSS��֪��
		TitleFont.lfHeight = -16;                                     // �����С��������н�������
		TitleFont.lfCharSet = DEFAULT_CHARSET;                        // Ĭ���ַ���
		TitleFont.lfOutPrecision = OUT_DEVICE_PRECIS;                 // �������

		HFONT hFont = CreateFontIndirect(&TitleFont);
		HWND hWndStatic = GetDlgItem(myhdlg, IDC_TITLE);
		SendMessage(hWndStatic, WM_SETFONT, (WPARAM)hFont, 0);

	}
	case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
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
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: �ڴ˴����ʹ�� hdc ���κλ�ͼ����...
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ����Ի�����Ϣ  
INT_PTR CALLBACK DlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	HWND hButton = GetDlgItem(hdlg, IDC_BUTTON1);  
	HWND hEditBox = NULL;                         
	HWND hWndComboBox = NULL;
	HWND hListview = GetDlgItem(hdlg, IDC_RESULT);       // ListView

	switch (msg)
	{
	case WM_INITDIALOG:
	{
		// ����ListView�ĸ�ʽ 
		LVCOLUMN lvc;
		lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
		ListView_SetTextColor(hListview, RGB(0, 0, 255));                // ����������ɫ
		ListView_SetTextBkColor(hListview, RGB(199, 237, 204));          // �������ֱ�����ɫ
		ListView_SetExtendedListViewStyle(hListview, LVS_EX_GRIDLINES);  // ��ӵ�����

		lvc.pszText = L"�ڵ��ɫ";       // �б���  
		lvc.cx = 0;                      // �п�  
		lvc.iSubItem = 0;                // ������������һ�������� (0) 
		lvc.fmt = LVCFMT_CENTER;
		ListView_InsertColumn(hListview, 0, &lvc);   // ��һ��Ҫ�ϵ�
		lvc.cx = 70;
		ListView_InsertColumn(hListview, 1, &lvc);

		lvc.pszText = L"ҵ��������";
		lvc.cx = 70;
		lvc.iSubItem = 1;               // ��������  
		lvc.fmt = LVCFMT_CENTER;
		ListView_InsertColumn(hListview, 2, &lvc);

		lvc.pszText = L"ҵ��������";
		lvc.cx = 70;
		lvc.iSubItem = 2;
		lvc.fmt = LVCFMT_CENTER;
		ListView_InsertColumn(hListview, 3, &lvc);

		lvc.pszText = L"�洢������";
		lvc.cx = 70;
		lvc.iSubItem = 3;
		lvc.fmt = LVCFMT_CENTER;
		ListView_InsertColumn(hListview, 4, &lvc);

		lvc.pszText = L"�洢������";
		lvc.cx = 70;
		lvc.iSubItem = 4;
		lvc.fmt = LVCFMT_CENTER;
		ListView_InsertColumn(hListview, 5, &lvc);

		lvc.pszText = L"lo����";
		lvc.cx = 70;
		lvc.iSubItem = 5;
		lvc.fmt = LVCFMT_CENTER;
		ListView_InsertColumn(hListview, 6, &lvc);

		//��������б�
		hWndComboBox = GetDlgItem(hdlg, IDC_COMBO_REP);
		SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)L"1 ����ģʽ");
		SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)L"3 ��ɾ��ģʽ");

		hWndComboBox = GetDlgItem(hdlg, IDC_COMBO_NET);
		SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)L"1 ��");
		SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)L"0 ��");

		hWndComboBox = GetDlgItem(hdlg, IDC_COMBO_BOND);
		SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)L"1 ����");
		SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)L"4 ��·�ۺ�");

		hWndComboBox = GetDlgItem(hdlg, IDC_COMBO_SPEED);
		SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)L"10 Gb/s");
		SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)L"25 Gb/s");
		SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)L"40 Gb/s");

		// ������ʾ�ı�
		hEditBox = GetDlgItem(hdlg, IDC_EDIT_K);
		Edit_SetCueBannerText(hEditBox, L"С�ڵ���16");
		hEditBox = GetDlgItem(hdlg, IDC_EDIT_M);
		Edit_SetCueBannerText(hEditBox, L"С�ڵ���4");
		hEditBox = GetDlgItem(hdlg, IDC_EDIT_OSD);
		Edit_SetCueBannerText(hEditBox, L"OSD�ڵ���");
		hEditBox = GetDlgItem(hdlg, IDC_EDIT_RGW);
		Edit_SetCueBannerText(hEditBox, L"RGW�ڵ���");
		hEditBox = GetDlgItem(hdlg, IDC_EDIT_LB);
		Edit_SetCueBannerText(hEditBox, L"LB�ڵ���");
		hEditBox = GetDlgItem(hdlg, IDC_EDIT_DISK);
		Edit_SetCueBannerText(hEditBox, L"OSD�ڵ������");
		hEditBox = GetDlgItem(hdlg, IDC_EDIT_OSD_RGW);
		Edit_SetCueBannerText(hEditBox, L"OSD/RGW��Ͻڵ���");
		hEditBox = GetDlgItem(hdlg, IDC_EDIT_OSD_LB);
		Edit_SetCueBannerText(hEditBox, L"OSD/LB��Ͻڵ���");

		break;
	}//WM_INITDIALOG

	case WM_CREATE:
	{
		//������ť
		hButton = CreateWindow(
			NULL,                                              // Predefined class; Unicode assumed 
			NULL,                                                  // Button text 
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
			100,                                                    // x position 
			100,                                                    // y position 
			100,                                                    // Button width
			100,                                                    // Button height
			hdlg,                                                   // Parent window
			(HMENU)IDC_BUTTON1,                                   // No menu.
			(HINSTANCE)GetWindowLong(hdlg, GWL_HINSTANCE),
			NULL);                                                  // Pointer not needed.

		break;
	}//WM_CREATE

	case WM_COMMAND:
	{
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		switch (wmId)
		{
			//��ť���ܵ�ʵ��
		case IDC_BUTTON1:
		{
			TCHAR szBuff[4096];
			//MessageBox(NULL, szBuff, L"", MB_OK);

			// ����ϴεĽ��
			ListView_DeleteAllItems(hListview);
			cluster_conf.reset();

			// ��ȡ��������
			hWndComboBox = GetDlgItem(hdlg, IDC_COMBO_REP);
			SendMessage(hWndComboBox, WM_GETTEXT, 4096, (LPARAM)szBuff);
			if (szBuff[0] != '1')
			{
				cluster_conf.replica_mode = 3;
			}

			hWndComboBox = GetDlgItem(hdlg, IDC_COMBO_NET);
			SendMessage(hWndComboBox, WM_GETTEXT, 4096, (LPARAM)szBuff);
			if (szBuff[0] == '0')
			{
				cluster_conf.divided_network = false;
			}

			hWndComboBox = GetDlgItem(hdlg, IDC_COMBO_BOND);
			SendMessage(hWndComboBox, WM_GETTEXT, 4096, (LPARAM)szBuff);
			if (szBuff[0] == '4')
			{
				cluster_conf.bonding_mode = 4;
			}

			hWndComboBox = GetDlgItem(hdlg, IDC_COMBO_SPEED);
			SendMessage(hWndComboBox, WM_GETTEXT, 4096, (LPARAM)szBuff);
			switch (szBuff[0])
			{
			case '2':
			{
				cluster_conf.network_speed = 25 * 1000 / 8;
				break;
			}
			case '4':
			{
				cluster_conf.network_speed = 40 * 1000 / 8;
				break;
			}
			default:
			{
				cluster_conf.network_speed = 10 * 1000 / 8;
				break;
			}
			}

			cluster_conf.erasure_code_k = getEditBoxText(hdlg, IDC_EDIT_K);
			cluster_conf.replica = cluster_conf.erasure_code_k;
			cluster_conf.erasure_code_m = getEditBoxText(hdlg, IDC_EDIT_M);
			cluster_conf.osd_cnt = getEditBoxText(hdlg, IDC_EDIT_OSD);
			cluster_conf.rgw_cnt = getEditBoxText(hdlg, IDC_EDIT_RGW);
			cluster_conf.lb_cnt = getEditBoxText(hdlg, IDC_EDIT_LB);
			cluster_conf.osd_rgw_mixcnt = getEditBoxText(hdlg, IDC_EDIT_OSD_RGW);
			cluster_conf.osd_lb_mixcnt = getEditBoxText(hdlg, IDC_EDIT_OSD_LB);
			cluster_conf.osd_per_srv = getEditBoxText(hdlg, IDC_EDIT_DISK);

			// �ж�����Ϸ���
			if (!cluster_conf.check_valid()|| (WaitForSingleObject(hEventEmptyInput, 0) == WAIT_OBJECT_0))
			{
				MessageBox(NULL, L"�Ƿ�����", L"", MB_OK);
				ResetEvent(hEventEmptyInput);

				break;
			}

			// ����Ϸ�����������
			int neck = oNestSimulator(cluster_conf);

			// ������
			AddListViewItems(hListview);

			hEditBox = GetDlgItem(hdlg, IDC_EDIT_MAX);
			swprintf(szBuff, L"%f", result.cluster_max_flow);
			Edit_SetText(hEditBox, szBuff);

			hEditBox = GetDlgItem(hdlg, IDC_EDIT_BOTTLENECK);
			if (neck == 1)
			{
				swprintf(szBuff, L"%s", L"����ƿ��");
			}
			else
			{
				swprintf(szBuff, L"%s", L"����ƿ��");
			}
			Edit_SetText(hEditBox, szBuff);

			break;
		}

		default:
			break;
		}

		break;
	}//WM_COMMAND

	default:
		break;
	}//SWITCH

	return (INT_PTR)FALSE;
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

// ���Ǹ���װ
int getEditBoxText(HWND hdlg, int ID)
{
	TCHAR szBuff[20];
	HWND hEditBox = NULL;

	hEditBox = GetDlgItem(hdlg, ID);
	Edit_GetText(hEditBox, szBuff, 5);

	if (szBuff[0] == NULL)SetEvent(hEventEmptyInput);

	return _wtoi(szBuff);
}

// ������ģ����������
// ��������ֵ��Ϊƿ����
int oNestSimulator(cluster_configuration cluster)
{
	double max = -1;
	int bottleNeck = NETWORK;

	// ����ԭʼ����������˵��X��ϵ��
	double factor = 1;

	// ���϶��½����ƶ�
	// ���ؾ��⣺���뼴�����ȫ��ת��
	// ��Ҫ�ж��Ƿ���LB����Ϊ���ؾ���ڵ㲻�Ǳر�
	// ��Ҫ�ϲ����Ƕ����ͻ첿��LB
	if (cluster.lb_cnt > 0 || cluster.osd_lb_mixcnt > 0)
	{

		result.lb_pub_downlink = factor;
		result.lb_pub_uplink = result.lb_pub_downlink;
		max = result.lb_pub_downlink;

		// RGW�ڵ㣬����ͳ�Ƶ����ͻ첿����������RGW
		// ͬ�����뼴���������ֵ=LB����*factor/RGW����
		// RGW�ر��������ж�
		result.rgw_pub_downlink = (cluster.lb_cnt + cluster.osd_lb_mixcnt)*factor / (cluster.rgw_cnt + cluster.osd_rgw_mixcnt);
		result.rgw_pub_uplink = result.rgw_pub_downlink;
		if (result.rgw_pub_downlink > max)max = result.rgw_pub_downlink;
	}
	else
	{
		// û�и��ؾ��⣬��ֱ�Ӹ�RGW
		result.rgw_pub_downlink = factor;
		result.rgw_pub_uplink = result.rgw_pub_downlink;
		max = result.rgw_pub_downlink;
	}

	// OSD�ڵ㣬ͬ����Ҫ���ǵ����ͻ첿
	// ҵ��������=����RGW��������*RGW����/OSD����
	// �첿��lo����»��ں��浥������
	// ҵ�������У�Ϊ0
	result.osd_pub_downlink = result.rgw_pub_uplink*(cluster.rgw_cnt + cluster.osd_rgw_mixcnt) / (cluster.osd_cnt + cluster.osd_lb_mixcnt + cluster.osd_rgw_mixcnt);
	result.osd_pub_uplink = 0;
	if (result.osd_pub_downlink > max)max = result.osd_pub_downlink;

	// OSD�洢����������������ģʽ����
	// �����Ļ����洢������Ϊ��������һ���������������
	// ��ɾ��Ļ����洢������ΪK+M-1����K���������������
	if (cluster.replica_mode == 1)
	{
		// ����
		result.osd_cluster_uplink = result.osd_pub_downlink*(cluster.replica - 1);
		result.osd_cluster_downlink = result.osd_cluster_uplink;
	}
	else
	{
		// ��ɾ��
		double ec_factor = cluster.erasure_code_k + cluster.erasure_code_m - 1;
		ec_factor /= cluster.erasure_code_k;

		result.osd_cluster_uplink = result.osd_pub_downlink*ec_factor;
		result.osd_cluster_downlink = result.osd_cluster_uplink;
	}
	if (result.osd_cluster_uplink > max)max = result.osd_cluster_uplink;

	// �첿�ڵ�ҵ�����Ĵ�������OSD+RGW��OSD+LB
	// ֱ�ӽ�ҵ������洢����Ӧ������Ӿͺ���
	// OSD+RGW����Ҫ����һ��lo�������ٿ۳�����ڲ��ܣ�
	result.osd_rgw_pub_downlink = result.osd_pub_downlink + result.rgw_pub_downlink;
	if (result.osd_rgw_pub_downlink > max)max = result.osd_rgw_pub_downlink;
	result.osd_rgw_pub_uplink = result.osd_pub_uplink + result.rgw_pub_uplink;
	if (result.osd_rgw_pub_uplink > max)max = result.osd_rgw_pub_uplink;
	result.osd_lb_pub_downlink = result.osd_pub_downlink + result.lb_pub_downlink;
	if (result.osd_lb_pub_downlink > max)max = result.osd_lb_pub_downlink;
	result.osd_lb_pub_uplink = result.osd_pub_uplink + result.lb_pub_uplink;
	if (result.osd_lb_pub_uplink > max)max = result.osd_lb_pub_uplink;
	
	// ˫����һ��У��
	// �򵥰�ҵ�����ʹ洢����Ӧ�����������ͺ�
	if (!cluster.divided_network)
	{
		if (cluster.osd_cnt > 0)
		{
			result.osd_downlink = result.osd_pub_downlink + result.osd_cluster_downlink;
			result.osd_uplink = result.osd_cluster_uplink + result.osd_pub_uplink;

			if (result.osd_downlink > max)max = result.osd_downlink;
			if (result.osd_uplink > max)max = result.osd_uplink;
		}

		if (cluster.osd_rgw_mixcnt > 0)
		{
			result.osd_rgw_downlink = result.osd_downlink + result.rgw_pub_downlink;
			result.osd_rgw_uplink = result.osd_uplink + result.rgw_pub_uplink;

			if (result.osd_rgw_downlink > max)max = result.osd_rgw_downlink;
			if (result.osd_rgw_uplink > max)max = result.osd_rgw_uplink;
		}

		if (cluster.osd_lb_mixcnt > 0)
		{
			result.osd_lb_downlink = result.osd_downlink + result.lb_pub_downlink;
			result.osd_lb_uplink = result.osd_uplink + result.lb_pub_uplink;

			if (result.osd_lb_downlink > max)max = result.osd_lb_downlink;
			if (result.osd_lb_uplink > max)max = result.osd_lb_uplink;
		}
	}


	// �����濪ʼ�����Ǵ���������ֵ��
	// ��������X�����Ƕ���

	// ����������У��
	// �������·�ۺϣ���2
	// ֮ǰ��MAX���Ǵ��ϵ�������X��ϵ������������ٳ���ϵ�����õ�һ��X
	result.ethernet_adapter_limit = cluster.network_speed;
	if (cluster.bonding_mode == 4)
	{
		result.ethernet_adapter_limit *= 2;
	}
	factor = result.ethernet_adapter_limit / max;

	// ����OSD��У��
	// �øղŵõ���X��������������Ƚϣ���������ٶȸ�����У��X
	// ע����Ҫ����˫д
	// �����Ļ�������д�����ڵ��Ǹ�����*X���ʶ�ֱ�ӳ�������
	// ��ɾ��Ļ�������д�����ڵ���K+M�����ݷ�Ƭ
	// ����������Ǵ��������أ����С�����缫�ޣ������ڴ���
	result.osd_limit = cluster.osd_per_srv * 100 / 2;
	if (cluster.replica_mode == 1)
	{
		result.osd_limit /= cluster.replica;
	}
	else
	{
		double ec_factor = cluster.erasure_code_k + cluster.erasure_code_m;
		ec_factor /= cluster.erasure_code_k;

		result.osd_limit /= ec_factor;
	}
	if (factor > result.osd_limit)
	{
		factor = result.osd_limit;
		bottleNeck = DISK;
	}

	// �����ֱ�Ӹ������ȫ������X�ͺ���
	result.lb_pub_downlink *= factor;
	result.lb_pub_uplink = result.lb_pub_downlink;
	result.rgw_pub_downlink *= factor;
	result.rgw_pub_uplink = result.rgw_pub_downlink;
	result.osd_pub_downlink *= factor;
	result.osd_cluster_uplink *= factor;
	result.osd_cluster_downlink *= factor;

	// �ٲ��ϻ첿�ڵ������
	// ����۳�OSD/RGW��lo����
	// ע������lo��˳���ܵߵ�
	result.osd_rgw_lo = result.rgw_pub_uplink / (cluster.osd_cnt + cluster.osd_lb_mixcnt + cluster.osd_rgw_mixcnt);
	result.osd_rgw_pub_downlink = result.osd_pub_downlink + result.rgw_pub_downlink;
	result.osd_rgw_pub_uplink = result.osd_pub_uplink + result.rgw_pub_uplink - result.osd_rgw_lo;
	result.osd_rgw_cluster_downlink = result.osd_cluster_downlink;
	result.osd_rgw_cluster_uplink = result.osd_cluster_uplink;

	result.osd_lb_pub_downlink = result.osd_pub_downlink + result.lb_pub_downlink;
	result.osd_lb_pub_uplink = result.osd_pub_uplink + result.lb_pub_uplink;
	result.osd_lb_cluster_downlink = result.osd_cluster_downlink;
	result.osd_lb_cluster_uplink = result.osd_cluster_uplink;

	// ˫����һ�µĽ����д
	if (!cluster.divided_network)
	{
		if (cluster.osd_cnt > 0)
		{
			result.osd_downlink = result.osd_pub_downlink + result.osd_cluster_downlink;
			result.osd_uplink = result.osd_cluster_uplink + result.osd_pub_uplink;
		}

		if (cluster.osd_rgw_mixcnt > 0)
		{
			result.osd_rgw_downlink = result.osd_rgw_pub_downlink + result.osd_rgw_cluster_downlink;
			result.osd_rgw_uplink = result.osd_rgw_pub_uplink + result.osd_rgw_cluster_uplink;
		}

		if (cluster.osd_lb_mixcnt > 0)
		{
			result.osd_lb_downlink = result.osd_lb_cluster_downlink + result.osd_lb_pub_downlink;
			result.osd_lb_uplink = result.osd_lb_cluster_uplink + result.osd_lb_pub_uplink;
		}
	}

	// ���������Ⱥ�ĳ��������������и��ؾ���ڵ����֮��
	// ������Ҫ�����Ƿ��и��ؾ�������
	if (cluster.lb_cnt + cluster.osd_lb_mixcnt > 0)
	{
		result.cluster_max_flow = result.lb_pub_downlink*(cluster.lb_cnt + cluster.osd_lb_mixcnt);
	}
	else
	{
		result.cluster_max_flow = result.rgw_pub_downlink*(cluster.rgw_cnt + cluster.osd_rgw_mixcnt);
	}

	return bottleNeck;
}

// ��ListView����������
BOOL AddListViewItems(HWND hwndListView)
{
	int ListItemCount = ListView_GetItemCount(hwndListView);

	LPTSTR szBuff = new TCHAR[4096];

	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(lvi));   // �����������������߰���ĳ�ֵ
	
	lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE;

	// ����ı��ͳ���
	lvi.pszText = L"oNest";
	lvi.cchTextMax = lstrlen(lvi.pszText) + 1;
	
	// ��������
	if (cluster_conf.lb_cnt > 0)
	{
		if (!cluster_conf.divided_network)
		{
			itemTextSet(hwndListView, lvi, L"LB", result.lb_pub_uplink, result.lb_pub_downlink, L"-", L"-", L"-");
		}
		else
		{
			itemTextSet(hwndListView, lvi, L"LB", result.lb_pub_uplink, result.lb_pub_downlink, 0.0, 0.0, 0.0);
		}
	}

	if (cluster_conf.rgw_cnt > 0)
	{

		if (!cluster_conf.divided_network)
		{
			itemTextSet(hwndListView, lvi, L"RGW", result.rgw_pub_uplink, result.rgw_pub_downlink, L"-", L"-", L"-");
		}
		else
		{
			itemTextSet(hwndListView, lvi, L"RGW", result.rgw_pub_uplink, result.rgw_pub_downlink, 0.0, 0.0, 0.0);
		}
	}

	if (cluster_conf.osd_cnt > 0)
	{
		if (!cluster_conf.divided_network)
		{
			itemTextSet(hwndListView, lvi, L"OSD", result.osd_uplink, result.osd_downlink, L"-", L"-", L"-");
		}
		else
		{
			itemTextSet(hwndListView, lvi, L"OSD", result.osd_pub_uplink, result.osd_pub_downlink, result.osd_cluster_uplink, result.osd_cluster_downlink, 0.0);
		}
	}

	if (cluster_conf.osd_lb_mixcnt > 0)
	{
		if (!cluster_conf.divided_network)
		{
			itemTextSet(hwndListView, lvi, L"OSD/LB", result.osd_lb_uplink, result.osd_lb_downlink, L"-", L"-", L"-");
		}
		else
		{
			itemTextSet(hwndListView, lvi, L"OSD/LB", result.osd_lb_pub_uplink, result.osd_lb_pub_downlink, result.osd_lb_cluster_uplink, result.osd_lb_cluster_downlink, 0.0);
		}
	}

	if (cluster_conf.osd_rgw_mixcnt > 0)
	{
		if (!cluster_conf.divided_network)
		{
			itemTextSet(hwndListView, lvi, L"OSD/RGW", result.osd_rgw_uplink, result.osd_rgw_downlink, L"-", L"-", result.osd_rgw_lo);
		}
		else
		{
			itemTextSet(hwndListView, lvi, L"OSD/RGW", result.osd_rgw_pub_uplink, result.osd_rgw_pub_downlink, result.osd_rgw_cluster_uplink, result.osd_rgw_cluster_downlink, result.osd_rgw_lo);
		}
	}

	return TRUE;
}

int itemTextSet(HWND hwndListView, LVITEM lvi, TCHAR name[4096], double para1, double para2, double para3, double para4, double para5)
{
	TCHAR szBuff[4096];

	ListView_InsertItem(hwndListView, &lvi);
	ListView_SetItemText(hwndListView, 0, 1, name);

	swprintf(szBuff, L"%.2f", para1);
	ListView_SetItemText(hwndListView, 0, 2, szBuff);
	swprintf(szBuff, L"%.2f", para2);
	ListView_SetItemText(hwndListView, 0, 3, szBuff);
	swprintf(szBuff, L"%.2f", para3);
	ListView_SetItemText(hwndListView, 0, 4, szBuff);
	swprintf(szBuff, L"%.2f", para4);
	ListView_SetItemText(hwndListView, 0, 5, szBuff);
	swprintf(szBuff, L"%.2f", para5);
	ListView_SetItemText(hwndListView, 0, 6, szBuff);

	return 0;
}

int itemTextSet(HWND hwndListView, LVITEM lvi, TCHAR name[4096], double para1, double para2, TCHAR para3[4096], TCHAR para4[4096], double para5)
{
	TCHAR szBuff[4096];

	ListView_InsertItem(hwndListView, &lvi);
	ListView_SetItemText(hwndListView, 0, 1, name);

	swprintf(szBuff, L"%.2f", para1);
	ListView_SetItemText(hwndListView, 0, 2, szBuff);
	swprintf(szBuff, L"%.2f", para2);
	ListView_SetItemText(hwndListView, 0, 3, szBuff);
	ListView_SetItemText(hwndListView, 0, 4, para3);
	ListView_SetItemText(hwndListView, 0, 5, para4);
	swprintf(szBuff, L"%.2f", para5);
	ListView_SetItemText(hwndListView, 0, 6, szBuff);

	return 0;
}

int itemTextSet(HWND hwndListView, LVITEM lvi, TCHAR name[4096], double para1, double para2, TCHAR para3[4096], TCHAR para4[4096], TCHAR para5[4096])
{
	TCHAR szBuff[4096];

	ListView_InsertItem(hwndListView, &lvi);
	ListView_SetItemText(hwndListView, 0, 1, name);

	swprintf(szBuff, L"%.2f", para1);
	ListView_SetItemText(hwndListView, 0, 2, szBuff);
	swprintf(szBuff, L"%.2f", para2);
	ListView_SetItemText(hwndListView, 0, 3, szBuff);
	ListView_SetItemText(hwndListView, 0, 4, para3);
	ListView_SetItemText(hwndListView, 0, 5, para4);
	ListView_SetItemText(hwndListView, 0, 6, para4);

	return 0;
}