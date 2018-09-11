/*************************************************************
* Signature						Date                         *
* Li Hongjie                    2018.08.15                   *
*************************************************************/

// oNestSimulator.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "oNestSimulator.h"

// 界面风格变换：使用WIN8风格
#pragma comment(linker, "\"/manifestdependency:type='Win32'\
 name='Microsoft.Windows.Common-Controls' version='6.0.0.0'\
 processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// 全局变量 
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

HINSTANCE hInst;                                // 当前实例
TCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

cluster_configuration cluster_conf;             // 集群设计参数
simulator_result result;                        // 模拟结果
HANDLE hEventEmptyInput = CreateEvent(NULL, TRUE, FALSE, NULL);  // 空输入判断

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);        // 子窗口

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

	// TODO: 在此放置代码。

	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_ONESTSIMULATOR, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ONESTSIMULATOR));

	MSG msg;

	// 主消息循环: 
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
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
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

	// 主窗口不可变大小（锁定）
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
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
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
		// 创建子对话框并将其作为主窗口
		myhdlg = CreateDialog(hInst, MAKEINTRESOURCE(IDD_FORMVIEW), hWnd, (DLGPROC)DlgProc);
		ShowWindow(myhdlg, SW_SHOW);                                  // 显示子对话框

		// 美化程序
		// 设置标题字体样式：这部分必须放在主窗口处理
		LOGFONT TitleFont;
		ZeroMemory(&TitleFont, sizeof(TitleFont));                    // 这个必须做，清除乱七八糟的初值
		lstrcpy(TitleFont.lfFaceName, L"Microsoft Yahei");            // 设置字体
		TitleFont.lfWeight = FW_BOLD;                                 // 粗细，BOLD=700，写过CSS都知道
		TitleFont.lfHeight = -16;                                     // 字体大小，这个很有讲究……
		TitleFont.lfCharSet = DEFAULT_CHARSET;                        // 默认字符集
		TitleFont.lfOutPrecision = OUT_DEVICE_PRECIS;                 // 输出精度

		HFONT hFont = CreateFontIndirect(&TitleFont);
		HWND hWndStatic = GetDlgItem(myhdlg, IDC_TITLE);
		SendMessage(hWndStatic, WM_SETFONT, (WPARAM)hFont, 0);

	}
	case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// 分析菜单选择: 
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
			// TODO: 在此处添加使用 hdc 的任何绘图代码...
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

// 处理对话框消息  
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
		// 设置ListView的格式 
		LVCOLUMN lvc;
		lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
		ListView_SetTextColor(hListview, RGB(0, 0, 255));                // 设置文字颜色
		ListView_SetTextBkColor(hListview, RGB(199, 237, 204));          // 设置文字背景颜色
		ListView_SetExtendedListViewStyle(hListview, LVS_EX_GRIDLINES);  // 添加导航线

		lvc.pszText = L"节点角色";       // 列标题  
		lvc.cx = 0;                      // 列宽  
		lvc.iSubItem = 0;                // 子项索引，第一列无子项 (0) 
		lvc.fmt = LVCFMT_CENTER;
		ListView_InsertColumn(hListview, 0, &lvc);   // 第一列要废掉
		lvc.cx = 70;
		ListView_InsertColumn(hListview, 1, &lvc);

		lvc.pszText = L"业务网上行";
		lvc.cx = 70;
		lvc.iSubItem = 1;               // 子项索引  
		lvc.fmt = LVCFMT_CENTER;
		ListView_InsertColumn(hListview, 2, &lvc);

		lvc.pszText = L"业务网下行";
		lvc.cx = 70;
		lvc.iSubItem = 2;
		lvc.fmt = LVCFMT_CENTER;
		ListView_InsertColumn(hListview, 3, &lvc);

		lvc.pszText = L"存储网上行";
		lvc.cx = 70;
		lvc.iSubItem = 3;
		lvc.fmt = LVCFMT_CENTER;
		ListView_InsertColumn(hListview, 4, &lvc);

		lvc.pszText = L"存储网下行";
		lvc.cx = 70;
		lvc.iSubItem = 4;
		lvc.fmt = LVCFMT_CENTER;
		ListView_InsertColumn(hListview, 5, &lvc);

		lvc.pszText = L"lo网卡";
		lvc.cx = 70;
		lvc.iSubItem = 5;
		lvc.fmt = LVCFMT_CENTER;
		ListView_InsertColumn(hListview, 6, &lvc);

		//填充下拉列表
		hWndComboBox = GetDlgItem(hdlg, IDC_COMBO_REP);
		SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)L"1 副本模式");
		SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)L"3 纠删码模式");

		hWndComboBox = GetDlgItem(hdlg, IDC_COMBO_NET);
		SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)L"1 是");
		SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)L"0 否");

		hWndComboBox = GetDlgItem(hdlg, IDC_COMBO_BOND);
		SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)L"1 主备");
		SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)L"4 链路聚合");

		hWndComboBox = GetDlgItem(hdlg, IDC_COMBO_SPEED);
		SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)L"10 Gb/s");
		SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)L"25 Gb/s");
		SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)L"40 Gb/s");

		// 设置提示文本
		hEditBox = GetDlgItem(hdlg, IDC_EDIT_K);
		Edit_SetCueBannerText(hEditBox, L"小于等于16");
		hEditBox = GetDlgItem(hdlg, IDC_EDIT_M);
		Edit_SetCueBannerText(hEditBox, L"小于等于4");
		hEditBox = GetDlgItem(hdlg, IDC_EDIT_OSD);
		Edit_SetCueBannerText(hEditBox, L"OSD节点数");
		hEditBox = GetDlgItem(hdlg, IDC_EDIT_RGW);
		Edit_SetCueBannerText(hEditBox, L"RGW节点数");
		hEditBox = GetDlgItem(hdlg, IDC_EDIT_LB);
		Edit_SetCueBannerText(hEditBox, L"LB节点数");
		hEditBox = GetDlgItem(hdlg, IDC_EDIT_DISK);
		Edit_SetCueBannerText(hEditBox, L"OSD节点磁盘数");
		hEditBox = GetDlgItem(hdlg, IDC_EDIT_OSD_RGW);
		Edit_SetCueBannerText(hEditBox, L"OSD/RGW混合节点数");
		hEditBox = GetDlgItem(hdlg, IDC_EDIT_OSD_LB);
		Edit_SetCueBannerText(hEditBox, L"OSD/LB混合节点数");

		break;
	}//WM_INITDIALOG

	case WM_CREATE:
	{
		//创建按钮
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
			//按钮功能的实现
		case IDC_BUTTON1:
		{
			TCHAR szBuff[4096];
			//MessageBox(NULL, szBuff, L"", MB_OK);

			// 清空上次的结果
			ListView_DeleteAllItems(hListview);
			cluster_conf.reset();

			// 获取输入数据
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

			// 判断输入合法性
			if (!cluster_conf.check_valid()|| (WaitForSingleObject(hEventEmptyInput, 0) == WAIT_OBJECT_0))
			{
				MessageBox(NULL, L"非法输入", L"", MB_OK);
				ResetEvent(hEventEmptyInput);

				break;
			}

			// 输入合法，处理数据
			int neck = oNestSimulator(cluster_conf);

			// 输出结果
			AddListViewItems(hListview);

			hEditBox = GetDlgItem(hdlg, IDC_EDIT_MAX);
			swprintf(szBuff, L"%f", result.cluster_max_flow);
			Edit_SetText(hEditBox, szBuff);

			hEditBox = GetDlgItem(hdlg, IDC_EDIT_BOTTLENECK);
			if (neck == 1)
			{
				swprintf(szBuff, L"%s", L"网络瓶颈");
			}
			else
			{
				swprintf(szBuff, L"%s", L"磁盘瓶颈");
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

// 就是个封装
int getEditBoxText(HWND hdlg, int ID)
{
	TCHAR szBuff[20];
	HWND hEditBox = NULL;

	hEditBox = GetDlgItem(hdlg, ID);
	Edit_GetText(hEditBox, szBuff, 5);

	if (szBuff[0] == NULL)SetEvent(hEventEmptyInput);

	return _wtoi(szBuff);
}

// 真正的模拟器在这里
// 函数返回值设为瓶颈点
int oNestSimulator(cluster_configuration cluster)
{
	double max = -1;
	int bottleNeck = NETWORK;

	// 假设原始流量，或者说是X的系数
	double factor = 1;

	// 自上而下进行推断
	// 负载均衡：输入即输出，全量转发
	// 需要判断是否有LB，因为负载均衡节点不是必备
	// 需要合并考虑独立和混部的LB
	if (cluster.lb_cnt > 0 || cluster.osd_lb_mixcnt > 0)
	{

		result.lb_pub_downlink = factor;
		result.lb_pub_uplink = result.lb_pub_downlink;
		max = result.lb_pub_downlink;

		// RGW节点，首先统计单独和混部加起来多少RGW
		// 同样输入即输出，输入值=LB数量*factor/RGW数量
		// RGW必备，无需判断
		result.rgw_pub_downlink = (cluster.lb_cnt + cluster.osd_lb_mixcnt)*factor / (cluster.rgw_cnt + cluster.osd_rgw_mixcnt);
		result.rgw_pub_uplink = result.rgw_pub_downlink;
		if (result.rgw_pub_downlink > max)max = result.rgw_pub_downlink;
	}
	else
	{
		// 没有负载均衡，则直接给RGW
		result.rgw_pub_downlink = factor;
		result.rgw_pub_uplink = result.rgw_pub_downlink;
		max = result.rgw_pub_downlink;
	}

	// OSD节点，同样需要考虑单独和混部
	// 业务网下行=单个RGW出口流量*RGW数量/OSD数量
	// 混部走lo这个事会在后面单独处理
	// 业务网下行，为0
	result.osd_pub_downlink = result.rgw_pub_uplink*(cluster.rgw_cnt + cluster.osd_rgw_mixcnt) / (cluster.osd_cnt + cluster.osd_lb_mixcnt + cluster.osd_rgw_mixcnt);
	result.osd_pub_uplink = 0;
	if (result.osd_pub_downlink > max)max = result.osd_pub_downlink;

	// OSD存储网，依据数据冗余模式处理
	// 副本的话，存储网上行为副本数减一，下行与上行相等
	// 纠删码的话，存储网上行为K+M-1除以K，下行与上行相等
	if (cluster.replica_mode == 1)
	{
		// 副本
		result.osd_cluster_uplink = result.osd_pub_downlink*(cluster.replica - 1);
		result.osd_cluster_downlink = result.osd_cluster_uplink;
	}
	else
	{
		// 纠删码
		double ec_factor = cluster.erasure_code_k + cluster.erasure_code_m - 1;
		ec_factor /= cluster.erasure_code_k;

		result.osd_cluster_uplink = result.osd_pub_downlink*ec_factor;
		result.osd_cluster_downlink = result.osd_cluster_uplink;
	}
	if (result.osd_cluster_uplink > max)max = result.osd_cluster_uplink;

	// 混部节点业务网的处理，包括OSD+RGW，OSD+LB
	// 直接将业务网与存储网对应流量相加就好了
	// OSD+RGW，需要处理一下lo（后面再扣除，这节不管）
	result.osd_rgw_pub_downlink = result.osd_pub_downlink + result.rgw_pub_downlink;
	if (result.osd_rgw_pub_downlink > max)max = result.osd_rgw_pub_downlink;
	result.osd_rgw_pub_uplink = result.osd_pub_uplink + result.rgw_pub_uplink;
	if (result.osd_rgw_pub_uplink > max)max = result.osd_rgw_pub_uplink;
	result.osd_lb_pub_downlink = result.osd_pub_downlink + result.lb_pub_downlink;
	if (result.osd_lb_pub_downlink > max)max = result.osd_lb_pub_downlink;
	result.osd_lb_pub_uplink = result.osd_pub_uplink + result.lb_pub_uplink;
	if (result.osd_lb_pub_uplink > max)max = result.osd_lb_pub_uplink;
	
	// 双网合一的校正
	// 简单把业务网和存储网对应流量加起来就好
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


	// 从下面开始，就是从下往上套值了
	// 现在先算X到底是多少

	// 基于网卡的校正
	// 如果是链路聚合，乘2
	// 之前的MAX，是从上到下最大的X的系数，这里，用网速除以系数，得到一个X
	result.ethernet_adapter_limit = cluster.network_speed;
	if (cluster.bonding_mode == 4)
	{
		result.ethernet_adapter_limit *= 2;
	}
	factor = result.ethernet_adapter_limit / max;

	// 基于OSD的校正
	// 用刚才得到的X，与磁盘吞吐量比较，如果磁盘速度更慢，校正X
	// 注意需要考虑双写
	// 副本的话，最终写到本节点是副本数*X，故而直接除副本数
	// 纠删码的话，最终写到本节点是K+M个数据分片
	// 上面算出的是磁盘最大承载，如果小于网络极限，则极限在磁盘
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

	// 下面就直接给结果中全部乘上X就好了
	result.lb_pub_downlink *= factor;
	result.lb_pub_uplink = result.lb_pub_downlink;
	result.rgw_pub_downlink *= factor;
	result.rgw_pub_uplink = result.rgw_pub_downlink;
	result.osd_pub_downlink *= factor;
	result.osd_cluster_uplink *= factor;
	result.osd_cluster_downlink *= factor;

	// 再补上混部节点的流量
	// 这里扣除OSD/RGW的lo网卡
	// 注意先算lo，顺序不能颠倒
	result.osd_rgw_lo = result.rgw_pub_uplink / (cluster.osd_cnt + cluster.osd_lb_mixcnt + cluster.osd_rgw_mixcnt);
	result.osd_rgw_pub_downlink = result.osd_pub_downlink + result.rgw_pub_downlink;
	result.osd_rgw_pub_uplink = result.osd_pub_uplink + result.rgw_pub_uplink - result.osd_rgw_lo;
	result.osd_rgw_cluster_downlink = result.osd_cluster_downlink;
	result.osd_rgw_cluster_uplink = result.osd_cluster_uplink;

	result.osd_lb_pub_downlink = result.osd_pub_downlink + result.lb_pub_downlink;
	result.osd_lb_pub_uplink = result.osd_pub_uplink + result.lb_pub_uplink;
	result.osd_lb_cluster_downlink = result.osd_cluster_downlink;
	result.osd_lb_cluster_uplink = result.osd_cluster_uplink;

	// 双网合一下的结果填写
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

	// 最后，整个集群的出口流量等于所有负载均衡节点出口之和
	// 还是需要考虑是否有负载均衡的情况
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

// 在ListView里面增加项
BOOL AddListViewItems(HWND hwndListView)
{
	int ListItemCount = ListView_GetItemCount(hwndListView);

	LPTSTR szBuff = new TCHAR[4096];

	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(lvi));   // 这个必须做，清除乱七八糟的初值
	
	lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE;

	// 项的文本和长度
	lvi.pszText = L"oNest";
	lvi.cchTextMax = lstrlen(lvi.pszText) + 1;
	
	// 插入数据
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