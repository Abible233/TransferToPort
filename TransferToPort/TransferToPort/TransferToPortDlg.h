
// TransferToPortDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"


// CTransferToPortDlg �Ի���
class CTransferToPortDlg : public CDialogEx
{
// ����
public:
	CTransferToPortDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TRANSFERTOPORT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
	SOCKET sock;		// �Ѵ������׽���
	HWND hwnd;			// �Ի�����
};