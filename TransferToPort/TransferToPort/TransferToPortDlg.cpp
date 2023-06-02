
// TransferToPortDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TransferToPort.h"
#include "TransferToPortDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTransferToPortDlg 对话框




CTransferToPortDlg::CTransferToPortDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTransferToPortDlg::IDD, pParent)
	, send_text_(_T(""))
	, receive_text_(_T(""))
	, m_ip_port(0)
	, m_server_port(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTransferToPortDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SEND_TEXT_EDIT, send_text_);
	DDX_Text(pDX, IDC_RECEIVE_TEXT_EDIT, receive_text_);
	DDX_Control(pDX, IDC_IPADDRESS1, m_ip);
	DDX_Text(pDX, IDC_EDIT1, m_ip_port);
	DDX_Control(pDX, IDC_IPADDRESS_SERVER, m_server_IP);
	DDX_Text(pDX, IDC_EDIT_SERVER_PORT, m_server_port);
}

BEGIN_MESSAGE_MAP(CTransferToPortDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_RECVDATA, OnRecvData)
	ON_BN_CLICKED(IDC_SEND_BUTTON, &CTransferToPortDlg::OnBnClickedSendButton)
	ON_BN_CLICKED(IDC_BUTTON_SET_IP, &CTransferToPortDlg::OnBnClickedButtonSetIp)
END_MESSAGE_MAP()


// CTransferToPortDlg 消息处理程序

BOOL CTransferToPortDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	// 加载并初始化套接字
	if(!AfxSocketInit()){
		AfxMessageBox(_T("加载套接字失败！"));
		return FALSE;
	}
	InitSocket();

	RECVPARAM *pRecvParam = new RECVPARAM;
	pRecvParam->sock = m_socket;
	pRecvParam->hwnd = m_hWnd;
	// 创建接收线程
	HANDLE hThread = CreateThread(NULL, 0, RecvProc, (LPVOID)pRecvParam, 0, NULL);
	// 关闭该接收程句柄，释放其引用计数
	CloseHandle(hThread);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTransferToPortDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTransferToPortDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTransferToPortDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTransferToPortDlg::OnBnClickedSendButton()
{
	// TODO: 在此添加控件通知处理程序代码
	// 将控件中的数据保存到对应的变量中
	UpdateData(TRUE);
	
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( 1, 1 );

	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {                                 
		return;
	}

	if ( LOBYTE( wsaData.wVersion ) != 1 ||
		HIBYTE( wsaData.wVersion ) != 1 ) {                                 
			WSACleanup( );
			return; 
	}
	USES_CONVERSION;
	
	// 创建套接字
	SOCKET socket_client = socket(AF_INET, SOCK_DGRAM, 0);

	SOCKADDR_IN address_server;
	address_server.sin_addr.S_un.S_addr = inet_addr(T2A(current_ip));
	address_server.sin_family = AF_INET;
	address_server.sin_port = htons(m_ip_port);

	char send_buffer[100];

	int len = sizeof(SOCKADDR);

	// 处理数据，将CString转换为char[]
	if ("" == send_text_)
	{
		send_buffer[0] = '\0';
	} 
	else
	{
		int send_text_length = send_text_.GetLength();
		for(int i = 0; i < send_text_length; i++)
		{
			send_buffer[i] = send_text_.GetAt(i);
		}
		send_buffer[send_text_length] = '\0';
	}

	// 发送数据
	sendto(socket_client, send_buffer, strlen(send_buffer)+1, 0, (SOCKADDR*)&address_server, len);
	
	closesocket(socket_client);
	WSACleanup();

}




BOOL CTransferToPortDlg::InitSocket()
{
	// 创建套接字
	m_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if(INVALID_SOCKET == m_socket)
	{
		MessageBox(_T("套接字创建失败！"));
		return FALSE;
	}
	SOCKADDR_IN addrSock;
	addrSock.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrSock.sin_family = AF_INET;
	addrSock.sin_port = htons(IP_RECEIVE_PORT);

	// 绑定套接字
	int retval;
	retval = bind(m_socket, (SOCKADDR*)&addrSock, sizeof(SOCKADDR));
	if(SOCKET_ERROR == retval)
	{
		closesocket(m_socket);
		MessageBox(_T("绑定失败！"));
		return FALSE;
	}
	return TRUE;
}



DWORD WINAPI CTransferToPortDlg::RecvProc(LPVOID lpParameter)
{
	USES_CONVERSION;
	// 获得主线程传递的套接字和句柄
	SOCKET sock = ((RECVPARAM*)lpParameter)->sock;
	HWND hwnd = ((RECVPARAM*)lpParameter)->hwnd;
	delete lpParameter;

	SOCKADDR_IN addrFrom;
	int len = sizeof(SOCKADDR);

	char receive_buffer[200];
	char recv_temp[2000];
	int retval;
	while (TRUE)
	{
		// 接收数据
		retval = recvfrom(sock, receive_buffer, 200, 0, (SOCKADDR*)&addrFrom, &len);
		if (SOCKET_ERROR == retval)
		{
			break;
		}
		char *recvIP = inet_ntoa(addrFrom.sin_addr);
		int port = addrFrom.sin_port;

		// 以这样的形式传递信息“IP;PORT;TEXT”
		sprintf(recv_temp, "%s;%d;%s", recvIP, port, receive_buffer);
		::PostMessage(hwnd, WM_RECVDATA, 0, (LPARAM)recv_temp);
	}
	return 0;
}




LRESULT CTransferToPortDlg::OnRecvData(WPARAM wPara, LPARAM lParam)
{
	CString str;
	USES_CONVERSION;
	//m_server_IP.SetAddress('1','1','1','1');
	str = A2T((char*)lParam);

	CString server_IP;
	CString server_port;
	CString strTemp;
	BYTE ip_value[4];

	// 以分号为间隔分别读取信息(英文的分号)
	server_IP = str.Left(str.Find(_T(";")));
	str.Delete(0, server_IP.GetLength() + 1);

	server_port = str.Left(str.Find(_T(";")));
	str.Delete(0, server_port.GetLength() + 1);

	// 处理IP地址
	for (int i = 0; i < 3; i++)
	{
		ip_value[i] = atoi(T2A(server_IP.Left(server_IP.Find(_T(".")))));
		server_IP.Delete(0, server_IP.Find(_T(".")) + 1);
	}
	ip_value[3] = atoi(T2A(server_IP));

	// 显示IP及端口号
	m_server_IP.SetAddress(ip_value[0], ip_value[1], ip_value[2], ip_value[3]);
	m_server_port = atoi(T2A(server_port));
	UpdateData(FALSE);

	// 获得已有数据
	GetDlgItemText(IDC_RECEIVE_TEXT_EDIT, strTemp);
	str += "\r\n";
	str += strTemp;
	// 显示所有接收到的数据
	SetDlgItemText(IDC_RECEIVE_TEXT_EDIT, str);

	return 0;
}

void CTransferToPortDlg::OnBnClickedButtonSetIp()
{
	// TODO: 在此添加控件通知处理程序代码
	BYTE ip_value[4];
	//CString ip_string;

	UpdateData(TRUE);

	if (m_ip.IsBlank())
	{
		MessageBox(_T("请输入IP地址"),_T("提示"),MB_OK);
	} 
	else
	{
		//m_ip.ClearAddress();
		m_ip.GetAddress(ip_value[0], ip_value[1], ip_value[2],ip_value[3]);
		current_ip.Format(_T("%d.%d.%d.%d"), ip_value[0], ip_value[1], ip_value[2],ip_value[3]);
		m_ip.SetFocus();
		MessageBox(_T("设置完成，可以传送消息！"),_T("提示"),MB_OK);
	}
}
