
// bootHostDlg.h : 头文件
//
#include "SerialPort.h"
#pragma once

#define RCV_CMD_LEN 80
//#define UPDATE_BLOCK_SIZE 2048//0//64
#define UPDATE_BLOCK_SIZE 128//16

#define WM_DLG_MSG   WM_USER+20

typedef struct U_STCOMMANDER
{
	unsigned char head;
	unsigned char cmd;
	unsigned long addr;		
	unsigned short data_len;
	unsigned char buf[RCV_CMD_LEN];
	unsigned char sum;
}STCOMMANDER;

// CbootHostDlg 对话框
class CbootHostDlg : public CDialogEx
{
// 构造
public:
	CbootHostDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_BOOTHOST_DIALOG };

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
	afx_msg LRESULT OnUartMessage(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOpenFile();
	void EmultateCom();
	BYTE m_comArr[100];
	BYTE m_comCnt;
	CString m_strFileName;
	CSerialPort m_uartPort;
	CString m_strInfo;
	CComboBox m_ctrlUart;
	CProgressCtrl m_ctrlProg;
	CButton m_ctrlOpen;
	BOOL m_isUartOpen;
	UINT8 m_comNumber;
	afx_msg void OnBnClickedUartOpen();
	afx_msg void OnCbnSelchangeUart();
	static UINT ThreadDownload(LPVOID lp);
	UINT8 DownLoadProc();
	void ComfirmProc(UINT8* rcv);
	afx_msg LRESULT OnUpdateInfo(WPARAM wParam, LPARAM lParam);
	int m_uartSel;
	bool thread_end;
	STCOMMANDER stCommander;
	afx_msg void OnBnClickedOk();
	HANDLE m_hEventDownLoad;
	BOOL m_isDownOk;
	BOOL m_isThreadRun;
	CButton m_btnOK;
	CString m_strUarPrt;
	afx_msg void OnBnClickedClear();
	afx_msg void OnNMCustomdrawProgress1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCancel();
	void CloseAllComs();
};
