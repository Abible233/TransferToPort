
// TransferToPortDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"


// CTransferToPortDlg 对话框
class CTransferToPortDlg : public CDialogEx
{
// 构造
public:
	CTransferToPortDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TRANSFERTOPORT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnRecvData(WPARAM wPara, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	CString send_text_;
	afx_msg void OnBnClickedSendButton();
	CString receive_text_;
	SOCKET m_socket;
	BOOL InitSocket();
	static DWORD WINAPI RecvProc(LPVOID lpParameter);
	CIPAddressCtrl m_ip;
	
	afx_msg void OnBnClickedButtonSetIp();
	CString current_ip;
	short m_ip_port;
	CIPAddressCtrl m_server_IP;
	int m_server_port;
};


struct RECVPARAM
{
	SOCKET sock;		// 已创建的套接字
	HWND hwnd;			// 对话框句柄
};