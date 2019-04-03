// usbhidiocDlg.h : header file
//

#if !defined(AFX_USBHIDIOCDLG_H__0B2AAA84_F5A9_11D3_9F47_0050048108EA__INCLUDED_)
#define AFX_USBHIDIOCDLG_H__0B2AAA84_F5A9_11D3_9F47_0050048108EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CUsbhidiocDlg dialog

extern "C" {

// This file is in the Windows DDK available from Microsoft.
#include "hidsdi.h"

#include <setupapi.h>
#include <dbt.h>
}

#include "TextProgressCtrl.h"


#define STM32_IAP_WRITE_PAGE            0x41
#define STM32_IAP_ERASE_DEVICE	      0x42

#define STM32_IAP_CHECK_SUM		       0x44
#define STM32_IAP_READ_STATUS		       0x45
#define STM32_IAP_SECURITYCHECK	       0x46
#define STM32_IAP_READ_64BYTE            0x47 

#define STM32_IAP_WRITE_OK		0x48
#define STM32_IAP_VER			0x49
#define STM32_IAP_SERNUM		0x4A
#define STM32_IAP_USER			0x4B
//#define STM32_IAP_OK			0x4C
#define STM32_IAP_START			0x4C

#define STM32_IAP_VERIRQ			0x50
#define STM32_IAP_CC2530			0x51

#define STM32_IAP_TAIL				0x5F


#define FW_VERSION	0x2626

#define DOWNLOAD_WARNING_ERASE 0
#define DOWNLOAD_WARNING_BURN  1
#define DOWNLOAD_WARNING_ERASE_OK_NOKEY 2
#define DOWNLOAD_WARNING_ERASE_OK_KEYVERIFIED 3
#define DOWNLOAD_WARNING_VERIFY  4
#define DOWNLOAD_WARNING_PROGRAM_CC2530	5
#define DOWNLOAD_WARNING_SNOK			6
#define DOWNLOAD_WARNING_SUCCESS		8
#define DOWNLOAD_WARNING_ERASE_FAILED -1
#define DOWNLOAD_WARNING_DOWNLOAD_FAILED	-2
#define DOWNLOAD_WARNING_VERIFY_ERROR	-4
#define DOWNLOAD_WARNING_OPENFILE_ERROR	-5
#define DOWNLOAD_WARNING_SN_ERROR		-6


#define UPDATE_BLOCK_SIZE 16//64
#define SECURITY_KEY_LEN  16

#define UPDATE_SUCCESS 2
#define UPDATE_FAILED 0

#define STM32_IAP_SERNUM		0x4A

#define DOWN_SELECT_STM32		0x0001
#define DOWN_SELECT_CC2530		0x0002
#define DOWN_SELECT_SN			0x0004
#define DOWN_SELECT_TAIL		0x0008

typedef enum {
	DOWN_SEL_MAIN,
	DOWN_SEL_RF,
	DOWN_SEL_VERI,
	DOWN_SEL_TAIL,
}DOWN_SELECTION;

typedef struct DEVICE_DESCRIPTOR
{
	HANDLE wHandle;
	HANDLE rHandle;
	HANDLE devHandle;
	HANDLE hEventObject;
	HIDP_CAPS Capabilities;
	CString	devPathName;
	OVERLAPPED	devOverlapped;

}DeviceDescriptor;

typedef struct BURN_PARA
{
	char *filePath;
	char *cc2530Path;
	//char *serNum;
	char *serNumFile;
	int snIndex;
	int progIndex;
	//DWORD serFileIndex;
	CProgressCtrl *prg;
	CWnd *msgWin;
	//char *encFlag;
	HANDLE	hWrite;
	HANDLE hRead;
	LPOVERLAPPED lpOverlapped;
}BURNPARA;

typedef struct INFO_PARA
{
	char info;
	int burnTotal;
	int errCode;
	int progress;
	//int nowOperate;
}INFOPARA;

class CUsbhidiocDlg : public CDialog
{
// Construction
public:
	DWORD								ActualBytesRead;
	DWORD								BytesRead;
	//HIDP_CAPS							Capabilities;
	DWORD								cbBytesRead;
	HANDLE								DeviceHandle;
	DWORD								dwError;
	char								FeatureReport[256];
	//HANDLE								hEventObject;
	GUID								HidGuid;
	OVERLAPPED							HIDOverlapped;
	char								InputReport[256];
	ULONG								Length;
	LPOVERLAPPED						lpOverLap;
	bool								MyDeviceDetected; 
	//CString								MyDevicePathName;
	DWORD								NumberOfBytesRead;
	char								OutputReport[256];
	HANDLE								ReadHandle;
	DWORD								ReportType;
	ULONG								Required;
	CString								ValueToDisplay;
	HANDLE								WriteHandle;
	int									VendorID;
	int									ProductID;

public:
	DeviceDescriptor devDescriptor[100];
	int m_devTotal;
	int m_devIndex;
	UINT8 m_stm32Buf[128*1024];
	UINT8 m_cc2530Buf[256*1024];
	UINT8 m_tailBuf[512*1024];
	UINT8 m_isDownProcess[100];
	UINT8 m_DownPrecent[100];
	UINT8 m_DownSelection[100];
	UINT8 m_fwInfo[512];

private:
	int m_downStatue;
	//CProgressCtrl	m_progress;
	static UINT Stm32Program(LPVOID pParam);
	static void SetProcess(CWnd *proWin,int value,LPARAM Ev);
	static void VerifyWrite(FILE *fp, int num);
	static int ExtractFixContent(FILE* fp, int num,char* buf);
	static DWORD64 atoihex(char s[]);
	afx_msg LRESULT OnUpdateInfo(WPARAM wParam, LPARAM lParam);
	CWinThread* m_NowDownLoad;
	UINT8 m_dspStay;
	//HANDLE m_hEvent;


public:
	CUsbhidiocDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CUsbhidiocDlg)
	enum { IDD = IDD_USBHIDIOC_DIALOG };
	//CProgressCtrl	m_progress;
	CTextProgressCtrl m_progress;
	CEdit	m_VendorID;
	CEdit	m_ProductID;
	//CButton	m_Continuous;
	//CButton	m_cbutAutoIncrement;
	//CComboBox	m_cboByteToSend1;
	//CComboBox	m_cboByteToSend0;
	//CListBox	m_BytesReceived;
//	CListBox	m_ResultsList;
	//CButton	m_Once;
	//BOOL	m_AutoIncrement;
	//CString	m_ResultsString;
	//CString	m_strBytesReceived;
	//CString	m_strByteToSend0;
	//CString	m_strByteToSend1;
	//int		m_ReportType;
	//BOOL	m_UseControlTransfersOnly;
	CString	m_ProductIDString;
	CString	m_VendorIDString;
	CString	m_InputReportBufferSize;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUsbhidiocDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CWnd* m_pOwner;

	// Generated message map functions
	//{{AFX_MSG(CUsbhidiocDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//afx_msg void OnOnce();
	afx_msg void OnChangeResults();
	//afx_msg void OnContinuous();
	//afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void On_cmdFindMyDevice();
//	afx_msg void OnChange_txtVendorID();
//	afx_msg void OnChange_txtProductID();
//	afx_msg void On_optExchangeInputAndOutputReports();
//	afx_msg void On_optExchangeFeatureReports();
	afx_msg void On_chkUseControlTransfersOnly();
	afx_msg void OnChange_txtInputReportBufferSize();
	//afx_msg void On_cmdInputReportBufferSize();
	afx_msg void OnButton2();
	afx_msg void OnUsbBurn();
	afx_msg LRESULT OnUpdateData(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClean();
	afx_msg void OnRead();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	BOOL DeviceNameMatch(LPARAM lParam);
	bool SearchTheHID();
	LRESULT Main_OnDeviceChange(WPARAM wParam, LPARAM lParam);
	void CloseHandles();
	void DisplayCurrentTime();
	void DisplayData(CString cstrDataToDisplay);
	//void DisplayFeatureReport();
	//void DisplayInputReport();
	void DisplayLastError(CString Operation);
	void DisplayReceivedData(char ReceivedByte);
	void GetDeviceCapabilities(int iDev);
	void PrepareForOverlappedTransfer();
	void ScrollToBottomOfListBox(USHORT idx);
	//void ReadAndWriteToDevice();
	void ReadFeatureReport();
	//void ReadInputReport();
	void RegisterForDeviceNotifications();
	//void WriteFeatureReport();
	void WriteOutputReport();
	BOOL IsDevOnProgram(void);

public:
	//afx_msg void OnBnClickedButton1();
	//afx_msg void OnBnClickedCc2530test();
	//afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedCc2530();
	afx_msg void OnBnClickedRadio1();
	//afx_msg void OnBnClickedRadioCc2530();
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnBnClickedCheckCc2530();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedDownStm32();
	afx_msg void OnBnClickedSnMac();
	int m_snSet;
	afx_msg void OnEnUpdateSn();
	afx_msg void OnEnChangeSn();
	afx_msg void OnLbnSelchangeList2();
	static UINT ThreadDownload(LPVOID lp);
	UINT8 DownLoadProc(UINT8 idev);

private:
	bool m_isDevProg[100];
public:
	afx_msg void OnCbnSelchangeDevlist();
	int m_devSelect;
	BOOL m_isThreadRun;
	INFOPARA m_infoPara;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	UINT8 m_progCnt;
	CString m_strInfo;
	afx_msg void OnBnClickedButton1();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USBHIDIOCDLG_H__0B2AAA84_F5A9_11D3_9F47_0050048108EA__INCLUDED_)
