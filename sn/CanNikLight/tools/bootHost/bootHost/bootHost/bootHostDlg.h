
// bootHostDlg.h : ͷ�ļ�
//
#include "SerialPort.h"
#pragma once


// CbootHostDlg �Ի���
class CbootHostDlg : public CDialogEx
{
// ����
public:
	CbootHostDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_BOOTHOST_DIALOG };

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
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOpenFile();
	CString m_strFileName;
	CSerialPort serialPort;
};
