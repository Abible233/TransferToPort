
// TransferToPortDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TransferToPort.h"
#include "TransferToPortDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CTransferToPortDlg �Ի���




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


// CTransferToPortDlg ��Ϣ�������

BOOL CTransferToPortDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	// ���ز���ʼ���׽���
	if(!AfxSocketInit()){
		AfxMessageBox(_T("�����׽���ʧ�ܣ�"));
		return FALSE;
	}
	InitSocket();

	RECVPARAM *pRecvParam = new RECVPARAM;
	pRecvParam->sock = m_socket;
	pRecvParam->hwnd = m_hWnd;
	// ���������߳�
	HANDLE hThread = CreateThread(NULL, 0, RecvProc, (LPVOID)pRecvParam, 0, NULL);
	// �رոý��ճ̾�����ͷ������ü���
	CloseHandle(hThread);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTransferToPortDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CTransferToPortDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTransferToPortDlg::OnBnClickedSendButton()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// ���ؼ��е����ݱ��浽��Ӧ�ı�����
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
	
	// �����׽���
	SOCKET socket_client = socket(AF_INET, SOCK_DGRAM, 0);

	SOCKADDR_IN address_server;
	address_server.sin_addr.S_un.S_addr = inet_addr(T2A(current_ip));
	address_server.sin_family = AF_INET;
	address_server.sin_port = htons(m_ip_port);

	char send_buffer[100];

	int len = sizeof(SOCKADDR);

	// �������ݣ���CStringת��Ϊchar[]
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

	// ��������
	sendto(socket_client, send_buffer, strlen(send_buffer)+1, 0, (SOCKADDR*)&address_server, len);
	
	closesocket(socket_client);
	WSACleanup();

}




BOOL CTransferToPortDlg::InitSocket()
{
	// �����׽���
	m_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if(INVALID_SOCKET == m_socket)
	{
		MessageBox(_T("�׽��ִ���ʧ�ܣ�"));
		return FALSE;
	}
	SOCKADDR_IN addrSock;
	addrSock.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrSock.sin_family = AF_INET;
	addrSock.sin_port = htons(IP_RECEIVE_PORT);

	// ���׽���
	int retval;
	retval = bind(m_socket, (SOCKADDR*)&addrSock, sizeof(SOCKADDR));
	if(SOCKET_ERROR == retval)
	{
		closesocket(m_socket);
		MessageBox(_T("��ʧ�ܣ�"));
		return FALSE;
	}
	return TRUE;
}



DWORD WINAPI CTransferToPortDlg::RecvProc(LPVOID lpParameter)
{
	USES_CONVERSION;
	// ������̴߳��ݵ��׽��ֺ;��
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
		// ��������
		retval = recvfrom(sock, receive_buffer, 200, 0, (SOCKADDR*)&addrFrom, &len);
		if (SOCKET_ERROR == retval)
		{
			break;
		}
		char *recvIP = inet_ntoa(addrFrom.sin_addr);
		int port = addrFrom.sin_port;

		// ����������ʽ������Ϣ��IP;PORT;TEXT��
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

	// �Էֺ�Ϊ����ֱ��ȡ��Ϣ(Ӣ�ĵķֺ�)
	server_IP = str.Left(str.Find(_T(";")));
	str.Delete(0, server_IP.GetLength() + 1);

	server_port = str.Left(str.Find(_T(";")));
	str.Delete(0, server_port.GetLength() + 1);

	// ����IP��ַ
	for (int i = 0; i < 3; i++)
	{
		ip_value[i] = atoi(T2A(server_IP.Left(server_IP.Find(_T(".")))));
		server_IP.Delete(0, server_IP.Find(_T(".")) + 1);
	}
	ip_value[3] = atoi(T2A(server_IP));

	// ��ʾIP���˿ں�
	m_server_IP.SetAddress(ip_value[0], ip_value[1], ip_value[2], ip_value[3]);
	m_server_port = atoi(T2A(server_port));
	UpdateData(FALSE);

	// �����������
	GetDlgItemText(IDC_RECEIVE_TEXT_EDIT, strTemp);
	str += "\r\n";
	str += strTemp;
	// ��ʾ���н��յ�������
	SetDlgItemText(IDC_RECEIVE_TEXT_EDIT, str);

	return 0;
}

void CTransferToPortDlg::OnBnClickedButtonSetIp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BYTE ip_value[4];
	//CString ip_string;

	UpdateData(TRUE);

	if (m_ip.IsBlank())
	{
		MessageBox(_T("������IP��ַ"),_T("��ʾ"),MB_OK);
	} 
	else
	{
		//m_ip.ClearAddress();
		m_ip.GetAddress(ip_value[0], ip_value[1], ip_value[2],ip_value[3]);
		current_ip.Format(_T("%d.%d.%d.%d"), ip_value[0], ip_value[1], ip_value[2],ip_value[3]);
		m_ip.SetFocus();
		MessageBox(_T("������ɣ����Դ�����Ϣ��"),_T("��ʾ"),MB_OK);
	}
}
