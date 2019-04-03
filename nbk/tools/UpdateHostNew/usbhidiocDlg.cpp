// usbhidiocDlg.cpp : implementation file
/*
Project: usbhidioc.cpp
Version: 3.0
Date: 7/18/05
by Jan Axelson (jan@Lvr.com)

Purpose: demonstrates USB communications with a HID-class device.

Description: 
	Finds an attached HID-class device that matches a specified Vendor ID and Product ID.
	Retrieves the HID's capabilities.
	Sends a report to the HID and receives a report from the HID.
	Supports Input, Output, and Feature reports. 
	A list box displays a log of activity.
	A list box displays the most recent received report bytes.
	Combo boxes enable the user to select bytes to send.
	Edit boxes enable the user to specify a Vendor ID and Product ID.
	A check box causes the application to increment the bytes sent
with each report.
	Clicking the Once button causes the application to exchange one set of reports.
	Clicking the Continuous button causes the application to exchange reports periodically
(every 5 seconds).
	A button enables setting the number of Input reports the HID buffer can store.
	F
This application was created with Visual C++ 6's AppWizard as a dialog-based application.

Companion firmware and other sample code is available from www.Lvr.com.

Send comments, questions, bug reports, etc. to jan@Lvr.com.

About overlapped I/O

The API function ReadFile retrieves Input reports from the HID driver's buffer. 
Non-overlapped ReadFile is a blocking call. 
If a report isn't available, the function waits.

With overlapped I/O, the call to ReadFile returns immediately. The application then calls 
WaitForSingleObject, which returns when either the data has arrived or the specified timeout has elapsed.

This application has been tested on Windows 98 SE, Windows 2000, and Windows XP.
*/

#include "stdafx.h"

#include "usbhidioc.h"
#include "usbhidiocDlg.h"

#include <wtypes.h>
#include <initguid.h>
#include "TriDesEnc.h"
#include "MD5.h"


#define SPEEDLIGHT_600EX   0
#define ST_EX_RT           1
#define SPEEDLIGHT_910N    2
//#define DEST_DEVICE        SPEEDLIGHT_600EX
//#define DEST_DEVICE        2
//#define DEST_DEVICE SPEEDLIGHT_910N
#define DEST_DEVICE        ST_EX_RT

#define MAX_LOADSTRING 256

#define WM_DLG_MSG   WM_USER+20

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//function prototypes
#define								WM_UPDATEDATA WM_USER + 1999
char								g_erase_flag = 0;
bool								thread_end;
char								g_downLoad_enable = 0;
char								g_downLoad_error = 0;
char								g_blockLast = 0;
UINT								g_downSelect=0;
//char								g_cc2530DownFlag=0;
//HANDLE								hEventObject;

char *g_desKey = "XiaobaoApple";
HANDLE g_hEvent;
//#define FW_FILE_NAME  _T("HxStex-1.fmw")
//#define FW_FILE_NAME  _T("SpeedLight600ex.fmw")
#if (DEST_DEVICE==ST_EX_RT)

#define FW_FILE_NAME "HxStex-1.fmw" 
#define FW_STM32_LEN (1024*100)
#define FW_CC2530_LEN (1024*120)//(104192)
#define FW_VER_LEN     (512)
#define FW_TAIL_LEN	  (1024*512)

#elif (DEST_DEVICE==SPEEDLIGHT_600EX)

#define FW_FILE_NAME  "SpeedLight600ex.fmw"
#define FW_STM32_LEN  (1024*106)
#define FW_CC2530_LEN (1024*(256-8))//(1024*120)//(104192)
#define FW_VER_LEN     (512)                      // 2017/06/30 加入版本号
#define FW_TAIL_LEN	  (1024*512)

#elif (DEST_DEVICE==SPEEDLIGHT_910N)

#define FW_FILE_NAME "SpeedLight_N.fmw"
#define FW_STM32_LEN  (1024*106)
#define FW_CC2530_LEN (1024*(256-8))//(1024*120)//(104192)
#define FW_VER_LEN     (512)                      // 2017/06/30 加入版本号
#define FW_TAIL_LEN	  (1024*512)

#else
#error "no device indicate"
#endif

CRITICAL_SECTION g_cs;

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUsbhidiocDlg dialog

CUsbhidiocDlg::CUsbhidiocDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUsbhidiocDlg::IDD, pParent)
	, m_snSet(0)
	, m_devSelect(0)
	, m_strInfo(_T(""))
{
	//{{AFX_DATA_INIT(CUsbhidiocDlg)
	//m_AutoIncrement = FALSE;
	//m_ResultsString = _T("");
	//m_strBytesReceived = _T("");
	//m_strByteToSend0 = _T("");
	//m_strByteToSend1 = _T("");
	//m_ReportType = 0;
	//m_UseControlTransfersOnly = TRUE;
	//m_UseControlTransfersOnly = FALSE;
	m_ProductIDString = _T("5750");
	m_VendorIDString =	_T("0483");
	m_InputReportBufferSize = _T("32");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	MyDeviceDetected = FALSE;
	VendorID = 0x0483;
	ProductID = 0x5750;
	m_pOwner = this;
	ReportType = 0;
	//hEventObject = 0;
	m_snSet = 1;
	m_devTotal = 0;
	m_devIndex = 0;

	m_progCnt = 0;

	m_dspStay = 0;

	memset(m_DownSelection,0x00,100);

	// 初始化临界区
	InitializeCriticalSection(&g_cs);

#if (DEST_DEVICE==ST_EX_RT)
	g_downSelect = DOWN_SELECT_STM32|DOWN_SELECT_CC2530|DOWN_SELECT_SN;
#elif (DEST_DEVICE==SPEEDLIGHT_600EX)
	g_downSelect = DOWN_SELECT_STM32|DOWN_SELECT_CC2530|DOWN_SELECT_TAIL;
	//g_downSelect = DOWN_SELECT_SN;
#elif (DEST_DEVICE==SPEEDLIGHT_910N)
	g_downSelect = DOWN_SELECT_STM32|DOWN_SELECT_TAIL;
#endif

	m_strInfo += _T("感谢您的使用\r\n");
	m_strInfo += _T("开始升级程序前，您需要自备一条 mini usb 接口线\r\n");
	m_strInfo += _T("如有疑问，请联系我们\r\n");
}


//Application global variables 
//	DWORD								ActualBytesRead;
	

	//These are the vendor and product IDs to look for.
	//Uses Lakeview Research's Vendor ID.


void CUsbhidiocDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUsbhidiocDlg)
	DDX_Control(pDX, IDC_BURNPROG, m_progress);
	//	DDX_Control(pDX, IDC_txtVendorID, m_VendorID);
	//	DDX_Control(pDX, IDC_txtProductID, m_ProductID);
	//	DDX_Control(pDX, IDC_Continuous, m_Continuous);
	//	DDX_Control(pDX, IDC_AutoIncrement, m_cbutAutoIncrement);
	//	DDX_Control(pDX, IDC_cboByteToSend1, m_cboByteToSend1);
	//	DDX_Control(pDX, IDC_cboByteToSend0, m_cboByteToSend0);
	//	DDX_Control(pDX, IDC_lstBytesReceived, m_BytesReceived);
	//  DDX_Control(pDX, IDC_LIST2, m_ResultsList);
	//	DDX_Control(pDX, IDC_Once, m_Once);
	//	DDX_Check(pDX, IDC_AutoIncrement, m_AutoIncrement);
	//DDX_LBString(pDX, IDC_LIST2, m_ResultsString);
	//	DDX_LBString(pDX, IDC_lstBytesReceived, m_strBytesReceived);
	//	DDX_CBString(pDX, IDC_cboByteToSend0, m_strByteToSend0);
	//	DDX_CBString(pDX, IDC_cboByteToSend1, m_strByteToSend1);
	//	DDX_Radio(pDX, IDC_optExchangeInputAndOutputReports, m_ReportType);
	//	DDX_Check(pDX, IDC_chkUseControlTransfersOnly, m_UseControlTransfersOnly);
	//	DDX_Text(pDX, IDC_txtProductID, m_ProductIDString);
	//	DDX_Text(pDX, IDC_txtVendorID, m_VendorIDString);
	//	DDX_Text(pDX, IDC_txtInputReportBufferSize, m_InputReportBufferSize);
	//}}AFX_DATA_MAP
	//DDX_Text(pDX, IDC_SN, m_snSet);
	DDV_MinMaxInt(pDX, m_snSet, 1, 99999);
	DDX_CBIndex(pDX, IDC_DEVLIST, m_devSelect);
	DDX_Text(pDX, IDC_INFO_WIN, m_strInfo);
}

BEGIN_MESSAGE_MAP(CUsbhidiocDlg, CDialog)
	//{{AFX_MSG_MAP(CUsbhidiocDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//ON_BN_CLICKED(IDC_Once, OnOnce)
	ON_EN_CHANGE(IDC_Results, OnChangeResults)
//	ON_BN_CLICKED(IDC_Continuous, OnContinuous)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_cmdFindMyDevice, On_cmdFindMyDevice)
	ON_MESSAGE(WM_DLG_MSG,OnUpdateInfo)
//	ON_EN_CHANGE(IDC_txtVendorID, OnChange_txtVendorID)
//	ON_EN_CHANGE(IDC_txtProductID, OnChange_txtProductID)
//	ON_BN_CLICKED(IDC_optExchangeInputAndOutputReports, On_optExchangeInputAndOutputReports)
//	ON_BN_CLICKED(IDC_optExchangeFeatureReports, On_optExchangeFeatureReports)
//	ON_BN_CLICKED(IDC_chkUseControlTransfersOnly, On_chkUseControlTransfersOnly)
//	ON_EN_CHANGE(IDC_txtInputReportBufferSize, OnChange_txtInputReportBufferSize)
//	ON_BN_CLICKED(IDC_cmdInputReportBufferSize, On_cmdInputReportBufferSize)
	ON_BN_CLICKED(IDC_USB_BURN, OnUsbBurn)
	ON_MESSAGE(WM_UPDATEDATA, OnUpdateData)
	ON_BN_CLICKED(IDC_CLEAN, OnClean)
	ON_BN_CLICKED(IDC_READ, OnRead)
	//}}AFX_MSG_MAP

	//ON_WM_DEVICECHANGE()
	ON_MESSAGE(WM_DEVICECHANGE, Main_OnDeviceChange)
//	ON_BN_CLICKED(IDC_BUTTON1, &CUsbhidiocDlg::OnBnClickedButton1)
	//ON_BN_CLICKED(IDC_CC2530TEST, &CUsbhidiocDlg::OnBnClickedCc2530test)
	//ON_BN_CLICKED(IDC_BUTTON2, &CUsbhidiocDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_CC2530, &CUsbhidiocDlg::OnBnClickedCc2530)
	//ON_BN_CLICKED(IDC_RADIO1, &CUsbhidiocDlg::OnBnClickedRadio1)
	//ON_BN_CLICKED(IDC_RADIO_CC2530, &CUsbhidiocDlg::OnBnClickedRadioCc2530)
	//ON_BN_CLICKED(IDC_CHECK2, &CUsbhidiocDlg::OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_CHECK_CC2530, &CUsbhidiocDlg::OnBnClickedCheckCc2530)
	//ON_BN_CLICKED(IDC_CHECK1, &CUsbhidiocDlg::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_DOWN_STM32, &CUsbhidiocDlg::OnBnClickedDownStm32)
	ON_BN_CLICKED(IDC_SN_MAC, &CUsbhidiocDlg::OnBnClickedSnMac)
	//ON_EN_UPDATE(IDC_SN, &CUsbhidiocDlg::OnEnUpdateSn)
	//ON_EN_CHANGE(IDC_SN, &CUsbhidiocDlg::OnEnChangeSn)
	ON_LBN_SELCHANGE(IDC_LIST2, &CUsbhidiocDlg::OnLbnSelchangeList2)
	ON_CBN_SELCHANGE(IDC_DEVLIST, &CUsbhidiocDlg::OnCbnSelchangeDevlist)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &CUsbhidiocDlg::OnBnClickedButton1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUsbhidiocDlg message handlers

BOOL CUsbhidiocDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//My declares begin here

	int		ByteToSend = 0;
	CString	strByteToSend = "";
	CString	strComboBoxText="";

	//End my declares

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	/*
	My code begins here.
	Anything that needs to happen when the application starts goes in this routine.
	*/

	//Populate the combo boxes with values from 00 to FF.

	MyDeviceDetected=FALSE;

	for (int i=0;i<100;i++)
	{
		m_isDevProg[i] = false;
	}

	g_hEvent = CreateEvent(NULL,FALSE, FALSE,NULL);

	SetEvent(g_hEvent);

	SetTimer(1,300,NULL);



#if 0
	for (ByteToSend=0; ByteToSend < 256; ByteToSend++)
	{
		//Display the value as a 2-digit Hex value.

		strByteToSend.Format("%.2X",ByteToSend);

		//Add the value to the combo boxes.

		m_cboByteToSend0.AddString(strByteToSend);
		m_cboByteToSend1.AddString(strByteToSend);	
	}


	//Select default values for the combo boxes.

	m_cboByteToSend0.SetCurSel(0);
	m_cboByteToSend1.SetCurSel(128);
#endif

	//Check the autoincrement check box.

	//m_cbutAutoIncrement.SetCheck(1);

	//Set the caption for the Continous button.

	//m_Continuous.SetWindowText("Continuous");

	//end my code

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CUsbhidiocDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CUsbhidiocDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CUsbhidiocDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/*
My routines (routines specific to this application) start here.
*/

void CUsbhidiocDlg::CloseHandles()
	{
	//Close open handles.
	
	if (DeviceHandle != INVALID_HANDLE_VALUE)
		{
		CloseHandle(DeviceHandle);
		}

	if (ReadHandle != INVALID_HANDLE_VALUE)
		{
		CloseHandle(ReadHandle);
		}

	if (WriteHandle != INVALID_HANDLE_VALUE)
		{
		CloseHandle(WriteHandle);
		}
	}


BOOL CUsbhidiocDlg::DeviceNameMatch(LPARAM lParam)
	{

	// Compare the device path name of a device recently attached or removed 
	// with the device path name of the device we want to communicate with.
	
	PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)lParam;

	//DisplayData("MyDevicePathName = " + MyDevicePathName);
	DisplayData("MyDevicePathName = " + devDescriptor[m_devIndex].devPathName);

	if (lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) 
	{
		
		PDEV_BROADCAST_DEVICEINTERFACE lpdbi = (PDEV_BROADCAST_DEVICEINTERFACE)lParam;

		
		CString DeviceNameString;

		//The dbch_devicetype parameter indicates that the event applies to a device interface.
		//So the structure in LParam is actually a DEV_BROADCAST_INTERFACE structure, 
		//which begins with a DEV_BROADCAST_HDR.

		//The dbcc_name parameter of DevBroadcastDeviceInterface contains the device name. 
 
		//Compare the name of the newly attached device with the name of the device 
		//the application is accessing (myDevicePathName).

		DeviceNameString = lpdbi->dbcc_name;

		DisplayData("DeviceNameString = " + DeviceNameString);


		if ((DeviceNameString.CompareNoCase(devDescriptor[m_devIndex].devPathName)) == 0)

		{
			//The name matches.

            return true;
		}
		else
		{
            //It's a different device.

            return false;
    	}
		
	}
		else
		{
			return false;
		}	
}


void CUsbhidiocDlg::DisplayCurrentTime()
{
	//Get the current time and date and display them in the log List Box.

	CTime curTime = CTime::GetCurrentTime();
	CString CurrentTime = curTime.Format( "%H:%M:%S, %B %d, %Y" );
	DisplayData(CurrentTime);
}


void CUsbhidiocDlg::DisplayData(CString cstrDataToDisplay)
{
	//Display data in the log List Box

//	USHORT	Index;
//	Index=m_ResultsList.InsertString(-1, (LPCTSTR)cstrDataToDisplay);
//	ScrollToBottomOfListBox(Index);
}

#if 0
void CUsbhidiocDlg::DisplayFeatureReport()
{
	USHORT	ByteNumber;
	CHAR	ReceivedByte;
	
	//Display the received data in the log and the Bytes Received List boxes.
	//Start at the top of the List Box.

	m_BytesReceived.ResetContent();
	
	//Step through the received bytes and display each.

	for (ByteNumber=0; ByteNumber < Capabilities.FeatureReportByteLength; ByteNumber++)
	{
		//Get a byte.

		ReceivedByte = FeatureReport[ByteNumber];

		//Display it.

		DisplayReceivedData(ReceivedByte);
	}
}
#endif

#if 0
void CUsbhidiocDlg::DisplayInputReport()
{
	USHORT	ByteNumber;
	CHAR	ReceivedByte;
	
	//Display the received data in the log and the Bytes Received List boxes.
	//Start at the top of the List Box.

	m_BytesReceived.ResetContent();
	
	//Step through the received bytes and display each.

	for (ByteNumber=0; ByteNumber < Capabilities.InputReportByteLength; ByteNumber++)
	{
		//Get a byte.

		ReceivedByte = InputReport[ByteNumber];

		//Display it.

		DisplayReceivedData(ReceivedByte);
	}
}
#endif


void CUsbhidiocDlg::DisplayLastError(CString Operation)
{
	//Display a message and the last error in the log List Box.
	
//	LPVOID lpMsgBuf;
	USHORT Index = 0;

#if 0
	CString	strLastError = "";
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	);
#endif

	//Display the last error.

	//strLastError = Operation + (LPCTSTR)lpMsgBuf; 

	//Trim CR/LF from the error message.

	//strLastError.TrimRight();				//show no system error

	//Index = m_ResultsList.InsertString(-1, strLastError);
//	Index = m_ResultsList.InsertString(-1, Operation);
//	ScrollToBottomOfListBox(Index);
	//LocalFree(lpMsgBuf); 
}

#if 0
void CUsbhidiocDlg::DisplayReceivedData(char ReceivedByte)
{
	//Display data received from the device.

	CString	strByteRead;

	//Convert the value to a 2-character Cstring.

	strByteRead.Format("%02X", ReceivedByte);
	strByteRead = strByteRead.Right(2); 

	//Display the value in the Bytes Received List Box.

	m_BytesReceived.InsertString(-1, strByteRead);

	//Display the value in the log List Box (optional).
	//MessageToDisplay.Format("%s%s", "Byte 0: ", strByteRead); 
	//DisplayData(MessageToDisplay);	
	//UpdateData(false);
}
#endif


bool CUsbhidiocDlg::SearchTheHID()
{
	//Use a series of API calls to find a HID with a specified Vendor IF and Product ID.

	HIDD_ATTRIBUTES						Attributes;
	//DWORD								DeviceUsage;
	SP_DEVICE_INTERFACE_DATA			devInfoData;
	bool								LastDevice = FALSE;
	int									MemberIndex = 0;
	LONG								Result;	
	CString								UsageDescription;
	HANDLE								hDevInfo;
	int									sIndex=0;
	PSP_DEVICE_INTERFACE_DETAIL_DATA	detailData;

	Length = 0;
	detailData = NULL;
	m_devTotal = 0;
	//devIndex = 0;
	//DeviceHandle=NULL;

	/*
	API function: HidD_GetHidGuid
	Get the GUID for all system HIDs.
	Returns: the GUID in HidGuid.
	*/

	HidD_GetHidGuid(&HidGuid);	
	
	/*
	API function: SetupDiGetClassDevs
	Returns: a handle to a device information set for all installed devices.
	Requires: the GUID returned by GetHidGuid.
	*/
	
	hDevInfo=SetupDiGetClassDevs 
		(&HidGuid, 
		NULL, 
		NULL, 
		DIGCF_PRESENT|DIGCF_INTERFACEDEVICE);
		
	devInfoData.cbSize = sizeof(devInfoData);

	//Step through the available devices looking for the one we want. 
	//Quit on detecting the desired device or checking all available devices without success.

	MemberIndex = 0;
	LastDevice = FALSE;

	do
	{
		/*
		API function: SetupDiEnumDeviceInterfaces
		On return, MyDeviceInterfaceData contains the handle to a
		SP_DEVICE_INTERFACE_DATA structure for a detected device.
		Requires:
		The DeviceInfoSet returned in SetupDiGetClassDevs.
		The HidGuid returned in GetHidGuid.
		An index to specify a device.
		*/

		Result=SetupDiEnumDeviceInterfaces 
			(hDevInfo, 
			0, 
			&HidGuid, 
			MemberIndex, 
			&devInfoData);

		if (Result != 0)
		{
			//A device has been detected, so get more information about it.

			/*
			API function: SetupDiGetDeviceInterfaceDetail
			Returns: an SP_DEVICE_INTERFACE_DETAIL_DATA structure
			containing information about a device.
			To retrieve the information, call this function twice.
			The first time returns the size of the structure in Length.
			The second time returns a pointer to the data in DeviceInfoSet.
			Requires:
			A DeviceInfoSet returned by SetupDiGetClassDevs
			The SP_DEVICE_INTERFACE_DATA structure returned by SetupDiEnumDeviceInterfaces.
			
			The final parameter is an optional pointer to an SP_DEV_INFO_DATA structure.
			This application doesn't retrieve or use the structure.			
			If retrieving the structure, set 
			MyDeviceInfoData.cbSize = length of MyDeviceInfoData.
			and pass the structure's address.
			*/
			
			//Get the Length value.
			//The call will return with a "buffer too small" error which can be ignored.

			Result = SetupDiGetDeviceInterfaceDetail 
				(hDevInfo, 
				&devInfoData, 
				NULL, 
				0, 
				&Length, 
				NULL);

			//Allocate memory for the hDevInfo structure, using the returned Length.

			detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(Length);
			
			//Set cbSize in the detailData structure.

			detailData -> cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

			//Call the function again, this time passing it the returned buffer size.

			Result = SetupDiGetDeviceInterfaceDetail 
				(hDevInfo, 
				&devInfoData, 
				detailData, 
				Length, 
				&Required, 
				NULL);

			// Open a handle to the device.
			// To enable retrieving information about a system mouse or keyboard,
			// don't request Read or Write access for this handle.

			/*
			API function: CreateFile
			Returns: a handle that enables reading and writing to the device.
			Requires:
			The DevicePath in the detailData structure
			returned by SetupDiGetDeviceInterfaceDetail.
			*/

			//DeviceHandle=CreateFile 
			devDescriptor[sIndex].devHandle = CreateFile
				(detailData->DevicePath, 
				0, 
				FILE_SHARE_READ|FILE_SHARE_WRITE, 
				(LPSECURITY_ATTRIBUTES)NULL,
				OPEN_EXISTING, 
				0, 
				NULL);

			DisplayLastError("CreateFile: ");

			/*
			API function: HidD_GetAttributes
			Requests information from the device.
			Requires: the handle returned by CreateFile.
			Returns: a HIDD_ATTRIBUTES structure containing
			the Vendor ID, Product ID, and Product Version Number.
			Use this information to decide if the detected device is
			the one we're looking for.
			*/

			//Set the Size to the number of bytes in the structure.

			Attributes.Size = sizeof(Attributes);

			Result = HidD_GetAttributes 
				(devDescriptor[sIndex].devHandle,//DeviceHandle, 
				&Attributes);
			
			DisplayLastError("HidD_GetAttributes: ");
			
			//Is it the desired device?

			//MyDeviceDetected = FALSE;
			

			if (Attributes.VendorID == VendorID)
			{
				if (Attributes.ProductID == ProductID)
				{
					//Both the Vendor ID and Product ID match.

					m_devTotal++;
					//devIndex++;

					//MyDeviceDetected = TRUE;
					//MyDevicePathName = detailData->DevicePath;
					devDescriptor[sIndex].devPathName = detailData->DevicePath;
					DisplayData("Device detected");

					//Register to receive device notifications.

					RegisterForDeviceNotifications();

					//Get the device's capablities.

					GetDeviceCapabilities(sIndex);

					// Find out if the device is a system mouse or keyboard.
/*					
					DeviceUsage = (Capabilities.UsagePage * 256) + Capabilities.Usage;

					if (DeviceUsage == 0x102)
						{
						UsageDescription = "mouse";
						}
				
					if (DeviceUsage == 0x106)
						{
						UsageDescription = "keyboard";
						}

					if ((DeviceUsage == 0x102) | (DeviceUsage == 0x106)) 
						{
						DisplayData("");
						DisplayData("*************************");
						DisplayData("The device is a system " + UsageDescription + ".");
						DisplayData("Windows 2000 and Windows XP don't allow applications");
						DisplayData("to directly request Input reports from or "); 
						DisplayData("write Output reports to these devices.");
						DisplayData("*************************");
						DisplayData("");
						}
*/

					// Get a handle for writing Output reports.

					//WriteHandle=CreateFile 
					devDescriptor[sIndex].wHandle = CreateFile
						(detailData->DevicePath, 
						GENERIC_WRITE, 
						FILE_SHARE_READ|FILE_SHARE_WRITE, 
						(LPSECURITY_ATTRIBUTES)NULL,
						OPEN_EXISTING, 
						0, 
						NULL);

					DisplayLastError("CreateFile: ");

					// Prepare to read reports using Overlapped I/O.

					PrepareForOverlappedTransfer();

					//ReadHandle=CreateFile 
					devDescriptor[sIndex].rHandle = CreateFile
					(detailData->DevicePath, 
					GENERIC_READ, 
					FILE_SHARE_READ|FILE_SHARE_WRITE,
					(LPSECURITY_ATTRIBUTES)NULL, 
					OPEN_EXISTING, 
					FILE_FLAG_OVERLAPPED, 
					NULL);

					DisplayLastError("CreateFile (ReadHandle): ");

					//Get an event object for the overlapped structure.

					/*API function: CreateEvent
					Requires:
					  Security attributes or Null
					  Manual reset (true). Use ResetEvent to set the event object's state to non-signaled.
					  Initial state (true = signaled) 
					  Event object name (optional)
					Returns: a handle to the event object
					*/

					//if (hEventObject == 0)
				//	if(devDescriptor[sIndex].hEventObject==0)
					{
						devDescriptor[sIndex].hEventObject = CreateEvent 
							(NULL, 
							TRUE, 
							TRUE, 
							_T(""));
						DisplayLastError("CreateEvent: ") ;

						//Set the members of the overlapped structure.

						//HIDOverlapped.hEvent = hEventObject;
						//HIDOverlapped.Offset = 0;
						//HIDOverlapped.OffsetHigh = 0;
						devDescriptor[sIndex].devOverlapped.hEvent = devDescriptor[sIndex].hEventObject;
						devDescriptor[sIndex].devOverlapped.Offset = 0;
						devDescriptor[sIndex].devOverlapped.OffsetHigh = 0;
					}


					sIndex++;

				} //if (Attributes.ProductID == ProductID)

				else
				{
					//The Product ID doesn't match.

					//CloseHandle(DeviceHandle);
					CloseHandle(devDescriptor[sIndex].devHandle);
				}

			} //if (Attributes.VendorID == VendorID)

			else
			{
				//The Vendor ID doesn't match.

				//CloseHandle(DeviceHandle);
				CloseHandle(devDescriptor[sIndex].devHandle);
			}

		//Free the memory used by the detailData structure (no longer needed).
		if (NULL!=detailData)
		{
			free(detailData);
		}

		}  //if (Result != 0)

		else
		{
			//SetupDiEnumDeviceInterfaces returned 0, so there are no more devices to check.

			LastDevice=TRUE;
		}

		//If we haven't found the device yet, and haven't tried every available device,
		//try the next one.

		MemberIndex = MemberIndex + 1;

	} //do

	//while ((LastDevice == FALSE) && (MyDeviceDetected == FALSE));
	while (LastDevice == FALSE);		//查找所有HID设备



//	if (MyDeviceDetected == FALSE)
	if (m_devTotal>0)
	{
		MyDeviceDetected = true;
		DisplayData("Device detected");
	}
	else
	{
		DisplayData("Devices No detected");
	}

	//Free the memory reserved for hDevInfo by SetupDiClassDevs.

	SetupDiDestroyDeviceInfoList(hDevInfo);
	DisplayLastError("SetupDiDestroyDeviceInfoList");

	return MyDeviceDetected;
}


void CUsbhidiocDlg::GetDeviceCapabilities(int iDev)
{
	//Get the Capabilities structure for the device.

	PHIDP_PREPARSED_DATA	PreparsedData;

	/*
	API function: HidD_GetPreparsedData
	Returns: a pointer to a buffer containing the information about the device's capabilities.
	Requires: A handle returned by CreateFile.
	There's no need to access the buffer directly,
	but HidP_GetCaps and other API functions require a pointer to the buffer.
	*/

	HidD_GetPreparsedData 
		(devDescriptor[iDev].devHandle,//DeviceHandle, 
		&PreparsedData);
	DisplayLastError("HidD_GetPreparsedData: ");

	/*
	API function: HidP_GetCaps
	Learn the device's capabilities.
	For standard devices such as joysticks, you can find out the specific
	capabilities of the device.
	For a custom device, the software will probably know what the device is capable of,
	and the call only verifies the information.
	Requires: the pointer to the buffer returned by HidD_GetPreparsedData.
	Returns: a Capabilities structure containing the information.
	*/
	
	HidP_GetCaps 
		(PreparsedData, 
		&devDescriptor[iDev].Capabilities/*&Capabilities*/);
	DisplayLastError("HidP_GetCaps: ");

	//Display the capabilities

	ValueToDisplay.Format(_T("%s%X"), "Usage Page: ", devDescriptor[iDev].Capabilities.UsagePage);
	DisplayData(ValueToDisplay);
	ValueToDisplay.Format(_T("%s%d"), "Input Report Byte Length: ", devDescriptor[iDev].Capabilities.InputReportByteLength);
	DisplayData(ValueToDisplay);
	ValueToDisplay.Format(_T("%s%d"), "Output Report Byte Length: ", devDescriptor[iDev].Capabilities.OutputReportByteLength);
	DisplayData(ValueToDisplay);
	ValueToDisplay.Format(_T("%s%d"), "Feature Report Byte Length: ", devDescriptor[iDev].Capabilities.FeatureReportByteLength);
	DisplayData(ValueToDisplay);
	ValueToDisplay.Format(_T("%s%d"), "Number of Link Collection Nodes: ", devDescriptor[iDev].Capabilities.NumberLinkCollectionNodes);
	DisplayData(ValueToDisplay);
	ValueToDisplay.Format(_T("%s%d"), "Number of Input Button Caps: ", devDescriptor[iDev].Capabilities.NumberInputButtonCaps);
	DisplayData(ValueToDisplay);
	ValueToDisplay.Format(_T("%s%d"), "Number of InputValue Caps: ", devDescriptor[iDev].Capabilities.NumberInputValueCaps);
	DisplayData(ValueToDisplay);
	ValueToDisplay.Format(_T("%s%d"), "Number of InputData Indices: ", devDescriptor[iDev].Capabilities.NumberInputDataIndices);
	DisplayData(ValueToDisplay);
	ValueToDisplay.Format(_T("%s%d"), "Number of Output Button Caps: ", devDescriptor[iDev].Capabilities.NumberOutputButtonCaps);
	DisplayData(ValueToDisplay);
	ValueToDisplay.Format(_T("%s%d"), "Number of Output Value Caps: ", devDescriptor[iDev].Capabilities.NumberOutputValueCaps);
	DisplayData(ValueToDisplay);
	ValueToDisplay.Format(_T("%s%d"), "Number of Output Data Indices: ", devDescriptor[iDev].Capabilities.NumberOutputDataIndices);
	DisplayData(ValueToDisplay);
	ValueToDisplay.Format(_T("%s%d"), "Number of Feature Button Caps: ", devDescriptor[iDev].Capabilities.NumberFeatureButtonCaps);
	DisplayData(ValueToDisplay);
	ValueToDisplay.Format(_T("%s%d"), "Number of Feature Value Caps: ", devDescriptor[iDev].Capabilities.NumberFeatureValueCaps);
	DisplayData(ValueToDisplay);
	ValueToDisplay.Format(_T("%s%d"), "Number of Feature Data Indices: ", devDescriptor[iDev].Capabilities.NumberFeatureDataIndices);
	DisplayData(ValueToDisplay);

	//No need for PreparsedData any more, so free the memory it's using.

	HidD_FreePreparsedData(PreparsedData);
	DisplayLastError("HidD_FreePreparsedData: ") ;
}


LRESULT CUsbhidiocDlg::Main_OnDeviceChange(WPARAM wParam, LPARAM lParam)  
	{
  
	//DisplayData("Device change detected.");

	PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)lParam;

	switch(wParam) 
		{
		// Find out if a device has been attached or removed.
		// If yes, see if the name matches the device path name of the device we want to access.

		case DBT_DEVICEARRIVAL:
			DisplayData("A device has been attached.");

			if (DeviceNameMatch(lParam))
			{
				DisplayData("My device has been attached.");
			}
		
			return TRUE; 
	
		case DBT_DEVICEREMOVECOMPLETE:
			DisplayData("A device has been removed.");

			if (DeviceNameMatch(lParam))
			{
				DisplayData("My device has been removed.");

				// Look for the device on the next transfer attempt.

				//MyDeviceDetected = false;
			}
			return TRUE; 
	
		default:
			return TRUE; 
		} 
  }



void CUsbhidiocDlg::OnChangeResults() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
}



void CUsbhidiocDlg::OnChange_txtInputReportBufferSize() 
{
}

#if 0
void CUsbhidiocDlg::OnChange_txtProductID() 
{
	
	CString ProductIDtext;

	// Get the text in the edit box.

	CEdit* m_ProductID = (CEdit*) GetDlgItem(IDC_txtProductID);
	m_ProductID->GetWindowText(ProductIDtext); 

	// Convert the hex string in the edit box to an integer.

	ProductID = strtoul("0x" + ProductIDtext, 0, 16); 
	
	MyDeviceDetected=false;
}
#endif

#if 0
void CUsbhidiocDlg::OnChange_txtVendorID() 
{
	
	CString VendorIDtext;

	// Get the text in the edit box.

	CEdit* m_VendorID = (CEdit*) GetDlgItem(IDC_txtVendorID);
	m_VendorID->GetWindowText(VendorIDtext); 

	// Convert the hex string in the edit box to an integer.

	VendorID = strtoul("0x" + VendorIDtext, 0, 16); 
	
	MyDeviceDetected=false;
}
#endif



void CUsbhidiocDlg::On_chkUseControlTransfersOnly() 
{
	// TODO: Add your control notification handler code here
	
}


void CUsbhidiocDlg::OnClose() 
{
	//Anything that needs to occur on closing the application goes here.
	//Free any resources used by previous API calls and still allocated.

	CDialog::OnClose();
}


void CUsbhidiocDlg::On_cmdFindMyDevice() 
{
	
	unsigned char loadBlock[80]={0};
	char verBuf[20] = {0};
	char serNum[80] = {0};
	char tmpBuf[200] = {0};
	ULONG Result;
	ULONG BytesWritten;
	unsigned int ver=0;

	//m_ResultsString = _T("");

	SearchTheHID();

	sprintf_s(tmpBuf,"共发现 %d 个设备连接\r\n",m_devTotal);
	CString dspStr;
	//dspStr = tmpBuf;
	//DisplayData(dspStr);
	//m_ResultsString += tmpBuf;
	//m_strInfo += tmpBuf;
	m_strInfo = tmpBuf;
	UpdateData(FALSE);

	((CComboBox*)GetDlgItem(IDC_DEVLIST))->ResetContent();

	for (int j=0;j<m_devTotal;j++)
	{
		memset(loadBlock,0x00,UPDATE_BLOCK_SIZE+6);

		loadBlock[0] = 0x00;
		loadBlock[1] = 0x7e;
		loadBlock[2] = 0x00;                 //start addr
		loadBlock[3] = STM32_IAP_VERIRQ;
		loadBlock[4] = 0x00;
		//loadBlock[4] = 64;
		loadBlock[5] = UPDATE_BLOCK_SIZE;
		loadBlock[6] = 0x00;
		loadBlock[7] = 3;

		loadBlock[UPDATE_BLOCK_SIZE+6] = 0x00;

		for (int i=2;i<UPDATE_BLOCK_SIZE+6;i++)
		{
			loadBlock[UPDATE_BLOCK_SIZE+6]+=loadBlock[i];
		}
		//loadBlock[22] = 0x56;

		//if (WriteHandle != INVALID_HANDLE_VALUE)
		if(devDescriptor[j].wHandle!=INVALID_HANDLE_VALUE)
		{
			Result = WriteFile 
			(devDescriptor[j].wHandle,//WriteHandle, 
			loadBlock, 
			0x41,//Capabilities.OutputReportByteLength, 
			&BytesWritten, 
			NULL);
		}

		//if (ReadHandle != INVALID_HANDLE_VALUE)
		if(devDescriptor[j].rHandle!=INVALID_HANDLE_VALUE)
		{
			Result = ReadFile 
			(devDescriptor[j].rHandle,//ReadHandle, 
			InputReport, 
			devDescriptor[j].Capabilities.InputReportByteLength,//Capabilities.InputReportByteLength, 
			&NumberOfBytesRead,
			(LPOVERLAPPED) &(devDescriptor[j].devOverlapped)); 
		}

		if (!Result)
		{
			DWORD err = GetLastError();
		}
 
		//DisplayLastError("ReadFile: ") ;

		/*API call:WaitForSingleObject
		'Used with overlapped ReadFile.
		'Returns when ReadFile has received the requested amount of data or on timeout.
		'Requires an event object created with CreateEvent
		'and a timeout value in milliseconds.
		*/

		Result = WaitForSingleObject 
			(devDescriptor[j].hEventObject,//hEventObject, 
			6000);


		//DisplayLastError("WaitForSingleObject: ") ;
		sprintf_s(tmpBuf,"设备 NO.%d ",j+1);
		CString strTmp;
		strTmp = tmpBuf;
		((CComboBox*)GetDlgItem(IDC_DEVLIST))->AddString(strTmp);
		//m_ResultsString += tmpBuf;
		//UpdateData(FALSE);

		if(WAIT_OBJECT_0!=Result)
		{
			DisplayLastError("read device error ");
		}else
		{
#if (DEST_DEVICE==ST_EX_RT)
			//ver = (InputReport[0]<<8|InputReport[1]);
			sprintf_s(verBuf,"%d.%d, %d.%d",InputReport[1]>>4,InputReport[1]&0x0f,InputReport[2]>>4,InputReport[2]&0x0f);
			CString fStr = "版本号 : ";
			fStr+=verBuf;
			m_strInfo+=fStr;
			//DisplayLastError(fStr);
			//memcpy(serNum,InputReport+2,13);
			//fStr="";
#else
			sprintf_s(tmpBuf,"设备 No.%d 的固件信息：\r\n",j+1);
			//fStr+=tmpBuf;
			//fStr+=serNum;
			//DisplayLastError(fStr);
			//m_ResultsString+=tmpBuf;
			//m_ResultsString+=serNum;
			m_strInfo+=tmpBuf;

			m_strInfo+=(InputReport+1);
#endif

			//m_strInfo+=serNum;
			m_strInfo+=_T("\r\n");
			UpdateData(FALSE);

		}

	}
	((CComboBox*)GetDlgItem(IDC_DEVLIST))->SetCurSel(0);
	
}

#if 0
void CUsbhidiocDlg::OnContinuous() 
{
	//Click the Continuous button to
	//begin or stop requesting and sending periodic reports.

	CString Caption;

	//Find out whether Continuous is currently selected 
	//and take appropriate action.

	m_Continuous.GetWindowText(Caption);

	if (Caption == "Continuous")
	{
		//Enable periodic exchanges of reports.
		//Change the button caption.

		m_Continuous.SetWindowText("Stop Continuous");

		//Start by reading and writing one pair of reports.

		ReadAndWriteToDevice();

		//Enable the timer to cause periodic exchange of reports.
		//The second parameter is the number of milliseconds between report requests.

		SetTimer(ID_CLOCK_TIMER, 500, NULL);
	}
	else
	{
		//Stop periodic exchanges of reports.
		//Change the button caption.

		m_Continuous.SetWindowText("Continuous");

		//Disable the timer.

		KillTimer(ID_CLOCK_TIMER);
	}
}
#endif


void CUsbhidiocDlg::OnOK() 
{
	CDialog::OnOK();
}

#if 0
void CUsbhidiocDlg::OnOnce() 
{
	//Click the Once button to read and write one pair of reports.

	ReadAndWriteToDevice();
}
#endif

#if 0
void CUsbhidiocDlg::On_cmdInputReportBufferSize() 
{

	// Change the number of reports the HID driver's buffer can store.
	// Read back and display the result.

	CString InputReportBufferSizeText;
	ULONG	InputReportBufferSize;
	boolean	result;

	// Look for the device if necessary.

	if (MyDeviceDetected==FALSE)
		{
		MyDeviceDetected=SearchTheHID();
		}
	
	if (MyDeviceDetected==TRUE)
		{
		// Get the text in the edit box.

		CEdit* m_InputReportBufferSize = (CEdit*) GetDlgItem(IDC_txtInputReportBufferSize);
		m_InputReportBufferSize->GetWindowText(InputReportBufferSizeText); 

		// Convert the string in the edit box to an integer.

		InputReportBufferSize = strtoul(InputReportBufferSizeText, 0, 10); 	

		// Set the number of Input-report buffers.

		result = HidD_SetNumInputBuffers 
		   (DeviceHandle, 
		   InputReportBufferSize);

		DisplayLastError("HidD_SetNumInputBuffers: ");

		/// Retrieve the number of Input-report buffers.

		result = HidD_GetNumInputBuffers 
		   (DeviceHandle, 
		   &InputReportBufferSize);

		DisplayLastError("HidD_GetNumInputBuffers: ");

		// Display the result.

		if (result) 
			{
			SetDlgItemInt (IDC_txtInputReportBufferSize, InputReportBufferSize); 
			}
		else
			{

			// Windows 98 Gold doesn't support these API calls. 
			// The buffer size is always 2.

			SetDlgItemInt (IDC_txtInputReportBufferSize, 2); 
			}
	}
}
#endif

#if 0
void CUsbhidiocDlg::On_optExchangeFeatureReports() 
{

	// Exchange Feature reports.
	ReportType = 1;
}
#endif

#if 0
void CUsbhidiocDlg::On_optExchangeInputAndOutputReports() 
{
	// Exchange Input and Output reports.
	
	ReportType = 0;	
}
#endif

#if 0
void CUsbhidiocDlg::OnTimer(UINT nIDEvent) 
{
	//The timer event.
	//Read and Write one pair of reports.

	ReadAndWriteToDevice();

	CDialog::OnTimer(nIDEvent);
}
#endif


void CUsbhidiocDlg::PrepareForOverlappedTransfer()
{
	//Get a handle to the device for the overlapped ReadFiles.
}

#if 0
void CUsbhidiocDlg::ReadAndWriteToDevice()
{
	//If necessary, find the device and learn its capabilities.
	//Then send a report and request a report.

	//Clear the List Box (optional).
	//m_ResultsList.ResetContent();

	DisplayData("***HID Test Report***");
	DisplayCurrentTime();

	//If the device hasn't been detected already, look for it.

	if (MyDeviceDetected==FALSE)
	{
		MyDeviceDetected=SearchTheHID();
	}
	
	// Do nothing if the device isn't detected.

	if (MyDeviceDetected==TRUE)
	{
		switch (ReportType)
		{
			case 0:
				{
				// Output and Input Reports

				//Write a report to the device.

				WriteOutputReport();

				//Read a report from the device.

				ReadInputReport();	
			
				break;
				}
			case 1:
				{
				// Feature reports

				//Write a report to the device.

				WriteFeatureReport();

				//Read a report from the device.

				ReadFeatureReport();

				break;
				}

			default:
				{
				break;
				}
			}
		} // else
}
#endif

#if 0
void CUsbhidiocDlg::ReadFeatureReport()
{
	
	// Read a Feature report from the device.

	CString	ByteToDisplay = "";
	BOOLEAN	Result;
	
	//The first byte is the report number.
	FeatureReport[0]=0;

	//Read a report from the device.

		/*
		HidD_GetFeature
		Returns:
		True on success
		Requires: 
		A device handle returned by CreateFile.
		A buffer to hold the report.
		The report length returned by HidP_GetCaps in Capabilities.InputReportByteLength.
		*/
		
		if (DeviceHandle != INVALID_HANDLE_VALUE)
			{
				Result = HidD_GetFeature
				(DeviceHandle,
				FeatureReport,
				Capabilities.FeatureReportByteLength);

				DisplayLastError("HidD_GetFeature: ");
			}
				
			if (!Result)
			{
				//The read attempt failed, so close the handles, display a message,
				//and set MyDeviceDetected to FALSE so the next attempt will look for the device.

				CloseHandles();
				DisplayData("Can't read from device");
				MyDeviceDetected = FALSE;
			}
			else
			{
				DisplayData("Received Feature report: ");
				
				DisplayFeatureReport();
			}
}
#endif

#if 0
void CUsbhidiocDlg::ReadInputReport()
{

	// Retrieve an Input report from the device.

	CString	ByteToDisplay = "";

	DWORD	Result;
	
	//The first byte is the report number.
	InputReport[0]=0;

	// Find out if the "Use Control Transfers Only" check box is checked.

	UpdateData(true);

	if (m_UseControlTransfersOnly) 
	{

		//Read a report from the device using a control transfer.
	
		/*
		HidD_GetInputReport
		Returns:
		True on success
		Requires: 
		A device handle returned by CreateFile.
		A buffer to hold the report.
		The report length returned by HidP_GetCaps in Capabilities.InputReportByteLength.
		*/
		
		if (ReadHandle != INVALID_HANDLE_VALUE)
			{
			Result = HidD_GetInputReport
			(ReadHandle,
			InputReport,
			Capabilities.InputReportByteLength);

			DisplayLastError("HidD_GetInputReport: ");
			}
		else
			{
			Result = FALSE;
			}

		if (!Result)
			{
			//The read attempt failed, so close the handles, display a message,
			//and set MyDeviceDetected to FALSE so the next attempt will look for the device.

			CloseHandles();
			DisplayData("Can't read from device");
			MyDeviceDetected = FALSE;
			}
		else
			{
			DisplayData("Received Input report: ");
			
			//Display the report data.

			DisplayInputReport();
		
			/*
			USHORT	ByteNumber;
			CHAR	ReceivedByte;
		
			//Display the received data in the log and the Bytes Received List boxes.
			//Start at the top of the List Box.

			m_BytesReceived.ResetContent();
	
			//Step through the received bytes and display each.

			for (ByteNumber=0; ByteNumber < Capabilities.InputReportByteLength; ByteNumber++)
			{
				//Get a byte.

				ReceivedByte = InputReport[ByteNumber];

				//Display it.

				DisplayReceivedData(ReceivedByte);
			}
			*/
		}
	} 

	else
	{
 	
	/*API call:ReadFile
	'Returns: the report in InputReport.
	'Requires: a by CreateFile device handle returned
	'(for overlapped I/O, CreateFile must be called with FILE_FLAG_OVERLAPPED),
	'the Input report length in bytes returned by HidP_GetCaps,
	'and an overlapped structure whose hEvent member is set to an event object.
	*/

	if (ReadHandle != INVALID_HANDLE_VALUE)
	{
		Result = ReadFile 
		(ReadHandle, 
		InputReport, 
		Capabilities.InputReportByteLength, 
		&NumberOfBytesRead,
		(LPOVERLAPPED) &HIDOverlapped); 
	}

	if (!Result)
	{
		DWORD err = GetLastError();
	}
 
	DisplayLastError("ReadFile: ") ;

	/*API call:WaitForSingleObject
	'Used with overlapped ReadFile.
	'Returns when ReadFile has received the requested amount of data or on timeout.
	'Requires an event object created with CreateEvent
	'and a timeout value in milliseconds.
	*/

	Result = WaitForSingleObject 
		(hEventObject, 
		6000);

	DisplayLastError("WaitForSingleObject: ") ;
 
	switch (Result)
	{
	case WAIT_OBJECT_0:
		{
		DisplayData("Received Input report,");
					
		break;
		}
	case WAIT_TIMEOUT:
		{
		ValueToDisplay.Format("%s", "ReadFile timeout");
		DisplayData(ValueToDisplay);
		//Cancel the Read operation.

		/*API call: CancelIo
		Cancels the ReadFile
        Requires the device handle.
        Returns non-zero on success.
		*/
		
		Result = CancelIo(ReadHandle);
		
		//A timeout may mean that the device has been removed. 
		//Close the device handles and set MyDeviceDetected = False 
		//so the next access attempt will search for the device.
		CloseHandles();
		DisplayData("Can't read from device");
		MyDeviceDetected = FALSE;
		break;
		}
	default:
		{
		ValueToDisplay.Format("%s", "Undefined error");

		//Close the device handles and set MyDeviceDetected = False 
		//so the next access attempt will search for the device.

		CloseHandles();
		DisplayData("Can't read from device");
		MyDeviceDetected = FALSE;
		break;
		}
	}

	/*
	API call: ResetEvent
	Sets the event object to non-signaled.
	Requires a handle to the event object.
	Returns non-zero on success.
	*/

	ResetEvent(hEventObject);

	//Display the report data.

	DisplayInputReport();

	}
}
#endif


void CUsbhidiocDlg::RegisterForDeviceNotifications()
{

	// Request to receive messages when a device is attached or removed.
	// Also see WM_DEVICECHANGE in BEGIN_MESSAGE_MAP(CUsbhidiocDlg, CDialog).

	DEV_BROADCAST_DEVICEINTERFACE DevBroadcastDeviceInterface;
	HDEVNOTIFY DeviceNotificationHandle;

	DevBroadcastDeviceInterface.dbcc_size = sizeof(DevBroadcastDeviceInterface);
	DevBroadcastDeviceInterface.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	DevBroadcastDeviceInterface.dbcc_classguid = HidGuid;

	DeviceNotificationHandle =
		RegisterDeviceNotification(m_hWnd, &DevBroadcastDeviceInterface, DEVICE_NOTIFY_WINDOW_HANDLE);

}


void CUsbhidiocDlg::ScrollToBottomOfListBox(USHORT Index)
{
	/* 
	Scroll to the bottom of the list box. 
	To do so, add a line and set it as the current selection,
	possibly scrolling the window.
	Then deselect the line, 
	leaving the list box scrolled to the bottom with nothing selected.
	*/

//	m_ResultsList.SetCurSel( Index );
//	m_ResultsList.SetCurSel( -1 );
}

#if 0
void CUsbhidiocDlg::WriteFeatureReport()
{
	//Send a report to the device.

	DWORD	BytesWritten = 0;
	INT		Index =0;
	ULONG	Result;
	CString	strBytesWritten = "";

	//The first byte is the report number.

	FeatureReport[0]=0;

	//Can set the other report values here, or get them from the combo boxes.
	//OutputReport[1]=33;
	//OutputReport[2]=6;

	//Get the bytes to send from the combo boxes.
	
	//If Autoincrement is checked, increment the selection.

	if (m_cbutAutoIncrement.GetCheck()>0)
		{
		Index=m_cboByteToSend0.GetCurSel();
		Index=Index+1;
		m_cboByteToSend0.SetCurSel(Index);
		}

	if (m_cbutAutoIncrement.GetCheck()>0)
		{
		Index=m_cboByteToSend1.GetCurSel();
		Index=Index+1;
		m_cboByteToSend1.SetCurSel(Index);
		}

	//Get the values from the combo boxes.

	FeatureReport[1]=m_cboByteToSend0.GetCurSel();
	FeatureReport[2]=m_cboByteToSend1.GetCurSel();


	//The first byte is the report number.

	FeatureReport[0]=0;

	//Send a report to the device.

	/*
	HidD_SetFeature
	Sends a report to the device.
	Returns: success or failure.
	Requires:
	A device handle returned by CreateFile.
	A buffer that holds the report.
	The Output Report length returned by HidP_GetCaps,
	*/

	if (DeviceHandle != INVALID_HANDLE_VALUE)
		{
		Result = HidD_SetFeature
		(DeviceHandle,
		FeatureReport,
		Capabilities.FeatureReportByteLength);
	}

	DisplayLastError("HidD_SetFeature: ");

	if (!Result)
		{
		//The write attempt failed, so close the handles, display a message,
		//and set MyDeviceDetected to FALSE so the next attempt will look for the device.

		CloseHandles();
		DisplayData("Can't write to device");
		MyDeviceDetected = FALSE;
		}
	else
		{
		DisplayData("A Feature report was written to the device.");
		}

}
#endif

#if 0
void CUsbhidiocDlg::WriteOutputReport()
{
	//Send a report to the device.

	DWORD	BytesWritten = 0;
	INT		Index =0;
	ULONG	Result;
	CString	strBytesWritten = "";

	UpdateData(true);

	//The first byte is the report number.

	OutputReport[0]=0;

	//Can set the other report values here, or get them from the combo boxes.
	//OutputReport[1]=33;
	//OutputReport[2]=6;

	//Get the bytes to send from the combo boxes.
	
	//If Autoincrement is checked, increment the selection.

	if (m_cbutAutoIncrement.GetCheck()>0)
		{
		Index=m_cboByteToSend0.GetCurSel();
		Index=Index+1;
		if(Index > 0xff) 
			Index = 0;
		m_cboByteToSend0.SetCurSel(Index);
		}

	if (m_cbutAutoIncrement.GetCheck()>0)
		{
		Index=m_cboByteToSend1.GetCurSel();
		Index=Index+1;
		if(Index > 255)
			Index = 0;
		m_cboByteToSend1.SetCurSel(Index);
		}	

	//Get the values from the combo boxes.

	OutputReport[1]=m_cboByteToSend0.GetCurSel();
	OutputReport[2]=m_cboByteToSend1.GetCurSel();


	//The first byte is the report number.

	OutputReport[0]=0;

	if (m_UseControlTransfersOnly)
		{

		//Send a report to the device.

		/*
		HidD_SetOutputReport
		Sends a report to the device.
		Returns: success or failure.
		Requires:
		The device handle returned by CreateFile.
		A buffer that holds the report.
		The Output Report length returned by HidP_GetCaps,
		*/

		if (WriteHandle != INVALID_HANDLE_VALUE)
			{
			Result = HidD_SetOutputReport
			(WriteHandle,
			OutputReport,
			Capabilities.OutputReportByteLength);

			DisplayLastError("HidD_SetOutputReport: ");
			}

		if (Result)
			{
			DisplayData("An Output report was written to the device.");
			}
		else
			{
			//The write attempt failed, so close the handles, display a message,
			//and set MyDeviceDetected to FALSE so the next attempt will look for the device.

			CloseHandles();
			DisplayData("Can't write to device");
			MyDeviceDetected = FALSE;
			}

		}
	else
		{

		/*
		API Function: WriteFile
		Sends a report to the device.
		Returns: success or failure.
		Requires:
		A device handle returned by CreateFile.
		A buffer that holds the report.
		The Output Report length returned by HidP_GetCaps,
		A variable to hold the number of bytes written.
		*/

		if (WriteHandle != INVALID_HANDLE_VALUE)
			{
			Result = WriteFile 
			(WriteHandle, 
			OutputReport, 
			Capabilities.OutputReportByteLength, 
			&BytesWritten, 
			NULL);
		}

		//Display the result of the API call and the report bytes.

		DisplayLastError("WriteFile: ");

		if (!Result)
			{
			//The WriteFile failed, so close the handles, display a message,
			//and set MyDeviceDetected to FALSE so the next attempt will look for the device.

			CloseHandles();
			DisplayData("Can't write to device");
			MyDeviceDetected = FALSE;
			}
		else
			{
			DisplayData("An Output report was written to the device.");
			strBytesWritten.Format("%s%d", "Bytes Written: ", BytesWritten); 
			DisplayData(strBytesWritten);
			}
		}
}
#endif

void CUsbhidiocDlg::OnButton2() 
{
	// TODO: Add your control notification handler code here
	
}

void CUsbhidiocDlg::OnUsbBurn() 
{
	// TODO: Add your control notification handler code here
	BURNPARA burnPara;
	unsigned char iDev=0;
	unsigned char buf[80]={0};
	static char serNumBuf[20] = {0};
	CStdioFile f;
	CFileException e;
	MD5_ENC md5Hx;
//	ULONG Result;
//	DWORD BytesWritten;

	//m_ResultsString = _T("");

	if (TRUE==m_isThreadRun)
	{
		m_isThreadRun = FALSE;
		GetDlgItem(IDC_USB_BURN)->SetWindowText(_T("开始"));
		return;
	}else
	{
		if (!m_devTotal)
		{
			m_strInfo = _T("没有查找到设备，请连接\r\n");
			UpdateData(FALSE);
			return;
		}
		m_isThreadRun = TRUE;
		GetDlgItem(IDC_USB_BURN)->SetWindowText(_T("结束"));
	}

	m_strInfo = _T("正在校验更新文件...\r\n");
	UpdateData(FALSE);
	if(FALSE==md5Hx.HX_FwMd5Check(FW_FILE_NAME))
	{
		m_strInfo += _T("您的更新文件检测错误，可能被破坏，请重新下载");
		UpdateData(FALSE);
		return;
	}else 
	{
		//DisplayData("更新文件检测成功");
		m_strInfo += _T("校验更新文件成功");
		UpdateData(FALSE);
	}

	m_strInfo = _T("");

	//if (!f.Open(_T("HxStex-1.fmw"),CFile::modeRead|CFile::typeBinary,&e))
	if (!f.Open(_T(FW_FILE_NAME),CFile::modeRead|CFile::typeBinary,&e))
	{
		//m_ResultsString = _T("打开下载文件错误");
		m_strInfo = _T("打开下载文件错误");
		m_isThreadRun = FALSE;
		GetDlgItem(IDC_USB_BURN)->SetWindowText(_T("开始"));
		UpdateData(FALSE);
		return;
	}

	f.Read(m_stm32Buf,FW_STM32_LEN);
	f.Read(m_cc2530Buf,FW_CC2530_LEN);
#if(DEST_DEVICE==ST_EX_RT)
#else
	f.Read(m_fwInfo,FW_VER_LEN);
#endif
	f.Read(m_tailBuf,FW_TAIL_LEN);

/*
	if (true==m_isDevProg[m_devIndex])
	{
		MessageBox("设备正在编程中，请不要操作");
		return;
	}else
	{
		m_isDevProg[m_devIndex] = true;
	}
*/
	
	if (MyDeviceDetected==FALSE)
	{
		MyDeviceDetected=SearchTheHID();
	}
	if (WriteHandle == INVALID_HANDLE_VALUE)
	{
		return;
	}

	if (g_downSelect&DOWN_SELECT_SN)
	{
		burnPara.snIndex = m_snSet;
	}
	
	for (m_devIndex=0;m_devIndex<m_devTotal;m_devIndex++)
	{

#if 1
	//thread_end = false;

	m_isThreadRun = TRUE;

	if(!(m_NowDownLoad = AfxBeginThread(ThreadDownload,this)))
	{
		MessageBox(_T("下载错误!"));
		return;
	}

	m_isDevProg[m_devIndex] = TRUE;

	m_NowDownLoad->m_bAutoDelete = false;
	Sleep(10);		//if without Sleep sometimes, it will soon clear struct burnPara. then the thread will not read the para
	
	//burnPara.encFlag = &m_encrypto;

#endif
	}
	
}

void CUsbhidiocDlg::SetProcess(CWnd *proWin,int value,LPARAM Ev)
{
	//SendMessage(WM_UPDATEDATA, value);
	//if (NULL==Ev) 
	//{
	//WaitForSingleObject(g_hEvent,INFINITE);
	::PostMessage(proWin->m_hWnd, WM_UPDATEDATA, (WPARAM) value, Ev/*(LPARAM) proWin->m_nPortNr*/);
		//::SendMessage(proWin->m_hWnd, WM_UPDATEDATA, (WPARAM) value, NULL/*(LPARAM) proWin->m_nPortNr*/);

}


LRESULT CUsbhidiocDlg::OnUpdateData(WPARAM wParam, LPARAM lParam)
{
	char evt;
	int totalByte;
	int crtDev;
	//int iTmp = (int)wParam;
	int iTmp;
	CString strDsp="";
	static int proRsv[100]={0};
	
	evt = (((INFOPARA*)lParam)->info);
	totalByte = (((INFOPARA*)lParam)->burnTotal);
	//crtDev = ((INFOPARA*)lParam)->nowOperate;
	crtDev = wParam;
	iTmp = (((INFOPARA*)lParam)->progress);

	switch(evt)
	{
		case DOWNLOAD_WARNING_BURN:
			//m_ResultsList.ResetContent();
			//m_progress.SetPos(iTmp);
#if 0
			if (proRsv[crtDev]!=iTmp)
			{
				proRsv[crtDev] = iTmp;
			}else
			{
				SetEvent(g_hEvent);
				return 0;
				//goto Eixt;
			}
#endif
			if(0==iTmp)
			{
				//CString dspStr="";
				//char tmpBuf[200];
				//sprintf(tmpBuf,"设备 NO.%d 程序开始升级",crtDev+1);
				//dspStr+=tmpBuf;
				strDsp.Format(_T("设备 NO.%d 程序开始升级"),crtDev+1);
				//m_ResultsList.DeleteString(crtDev+1);
				//m_ResultsList.InsertString(crtDev+1,(LPCTSTR)strDsp);
				DisplayData(strDsp);
			}else if(iTmp<100)
			{
				//CString dspStr="";
				//char tmpBuf[200];
				//sprintf(tmpBuf,"设备 NO.%d 程序开始升级",crtDev+1);
				//dspStr+=tmpBuf;
				strDsp.Format(_T("设备 NO.%d 升级 %d％"),crtDev+1,iTmp);
				//m_ResultsList.DeleteString(crtDev+1);
				//m_ResultsList.InsertString(crtDev+1,(LPCTSTR)strDsp);
				DisplayData(strDsp);
			}else if(100==iTmp)
			{
				//CString dspStr="";
				//char tmpBuf[200];
				strDsp.Format(_T("设备 NO.%d 升级成功"),crtDev+1);
				//dspStr+=tmpBuf;
				//isDevProg[crtDev] = false;
				//m_ResultsList.DeleteString(crtDev+1);
				//m_ResultsList.InsertString(crtDev+1,(LPCTSTR)strDsp);
				DisplayData(strDsp);
			}
			//UpdateData(false);
		break;

		case DOWNLOAD_WARNING_OPENFILE_ERROR:
			strDsp="open file error, please check";
			//m_ResultsList.DeleteString(crtDev+1);
			//m_ResultsList.InsertString(crtDev+1,(LPCTSTR)strDsp);
			DisplayData(strDsp);
			m_isDevProg[crtDev] = false;
			//DisplayData("open file error, please check");
			//m_progress.SetPos(100);
		break;

		case DOWNLOAD_WARNING_DOWNLOAD_FAILED:
			//m_ResultsString += "write device error\r\n";
			//UpdateData(false);
			//DisplayData("write device error,err %d",iTmp);
			strDsp.Format(_T("write device error,err %d, 0x%x"),iTmp,totalByte);
			//m_ResultsList.DeleteString(crtDev+1);
			//m_ResultsList.InsertString(crtDev+1,(LPCTSTR)strDsp);
			DisplayData(strDsp);
			m_isDevProg[crtDev] = false;
			//DisplayData(strDsp);
		break;

		case DOWNLOAD_WARNING_PROGRAM_CC2530:
			//if(totalByte==iTmp)
			//{
			//	DisplayData("check your hardware, please hold on");
			//}
			//m_progress.SetPos(iTmp);
			//strDsp = "check your hardware, please hold on"
#if 0
			if (proRsv[crtDev]!=iTmp)
			{
				proRsv[crtDev] = iTmp;
			}else
			{
				SetEvent(g_hEvent);
				//goto Eixt;
				return 0;
			}
#endif
			strDsp.Format(_T("设备 NO.%d CHECK %d"),crtDev+1,iTmp);
			//m_ResultsList.DeleteString(crtDev+1);
			//m_ResultsList.InsertString(crtDev+1,(LPCTSTR)strDsp);
			DisplayData(strDsp);
			if (100==iTmp)
			{
				//DisplayData("update successed, thank you!");
				strDsp.Format(_T("CHECK OK"));
				//m_ResultsList.DeleteString(crtDev+1);
				//m_ResultsList.InsertString(crtDev+1,(LPCTSTR)strDsp);
				DisplayData(strDsp);
			}
		break;

		case DOWNLOAD_WARNING_SUCCESS:
			strDsp.Format(_T("设备 NO.%d 升级成功，设备会重启"),crtDev+1);
			//m_ResultsList.DeleteString(crtDev+1);
			//m_ResultsList.InsertString(crtDev+1,(LPCTSTR)strDsp);
			DisplayData(strDsp);
			m_isDevProg[crtDev] = false;
		break;

		case DOWNLOAD_WARNING_SNOK:
			/*
			if((snInit = fopen("sn.init","wt")) == NULL)
			{
				MessageBox("open file error");
			}
			totalByte++;
			fputc(totalByte>>8,snInit);
			fputc(totalByte&0xff,snInit);
			fclose(snInit);
			*/
			m_snSet++;
			UpdateData(FALSE);
			//UpdateData();
			//Invalidate();
		break;
	}

//Eixt:
	SetEvent(g_hEvent);

	return 0;
}


int CUsbhidiocDlg::ExtractFixContent(FILE* fp, int num,char* buf)
{
	int cnt=0;
	char str[100];
	if (fp==NULL)
	{
		return 1;
	}
	while (!feof(fp))
	{
		if(cnt==num)
		{
			fgets(buf,100,fp);
			return 1;
		}
		cnt++;
		fgets(str,100,fp);
	}
	return 0;

}

UINT CUsbhidiocDlg::ThreadDownload(LPVOID lp)
{
	CUsbhidiocDlg *dlg = (CUsbhidiocDlg*)lp;
	return dlg->DownLoadProc(dlg->m_devIndex);
}

UINT8 CUsbhidiocDlg::DownLoadProc(UINT8 idev)
{
	CStdioFile fDown;
	CFileException e;
	UINT32 dnBytes = 0;;
	UINT32 totalBytes;
	UINT8 loadBlock[80];
	int i;
	BOOL Result;
	TriDesEnc triDesEnc;
	DWORD bytesWritten;
	DWORD bytesRead;
	char infoBuf[100]; 

	// 进入临界区
	EnterCriticalSection(&g_cs);
	sprintf_s(infoBuf,"NO. %d 开始下载程序\r\n",idev);
	//m_ResultsString += infoBuf;
	m_strInfo += infoBuf;
	// 离开临界区
	LeaveCriticalSection(&g_cs);
	m_isDevProg[idev] = TRUE;
	::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 3);

#if 0
/*********************************************************************
//inform the device to start
**********************************************************************/
		loadBlock[0] = 0x00;
		loadBlock[1] = 0x7e;
		loadBlock[2] = 0xff;
		loadBlock[3] = STM32_IAP_START;//0x49;//0x48;//loadBlock[3] = 0x41;
		loadBlock[4] = 0x00;
		loadBlock[5] = UPDATE_BLOCK_SIZE;	
		loadBlock[6] = 0x00;//(fileLen+UPDATE_BLOCK_SIZE)-times;
		loadBlock[7] = 0x00;
		loadBlock[UPDATE_BLOCK_SIZE+6] = 0x00;
		for (i=2;i<UPDATE_BLOCK_SIZE+6;i++) {
			loadBlock[UPDATE_BLOCK_SIZE+6] += loadBlock[i];
		}

		Result = WriteFile 
			(devDescriptor[idev].wHandle, 			
			loadBlock, 
			0x41,//64,//(fileLen+UPDATE_BLOCK_SIZE)-times+5, 
			&bytesWritten, 
			NULL);

		if(!Result)
		{
			EnterCriticalSection(&g_cs);
			sprintf_s(infoBuf,"NO.%d 下载失败\r\n",idev);					
			m_strInfo += infoBuf;
			LeaveCriticalSection(&g_cs);
			m_isDevProg[idev] = FALSE;
			//::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 3);
			::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 4);
			return FALSE;
		}

		if (devDescriptor[idev].rHandle != INVALID_HANDLE_VALUE)
		{
			Result = ReadFile 
				(
				devDescriptor[idev].rHandle,
				loadBlock, 
				0x41, 
				&bytesRead,
				&devDescriptor[idev].devOverlapped
				); 
		}else
		{
			EnterCriticalSection(&g_cs);
			sprintf_s(infoBuf,"NO.%d 下载失败\r\n",idev);					
			m_strInfo += infoBuf;
			LeaveCriticalSection(&g_cs);
			m_isDevProg[idev] = FALSE;
			//::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 3);
			::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 4);
			return FALSE;
		}

		if (!Result)
		{
			DWORD err = GetLastError();
		}

		Result = WaitForSingleObject 
			((&devDescriptor[idev].devOverlapped)->hEvent,
			6000);

		if((WAIT_OBJECT_0!=Result)||(loadBlock[1]!=0))
		{
			EnterCriticalSection(&g_cs);
			sprintf_s(infoBuf,"NO.%d 下载失败\r\n",idev);					
			m_strInfo += infoBuf;
			LeaveCriticalSection(&g_cs);
			m_isDevProg[idev] = FALSE;
			//::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 3);
			::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 4);
			return FALSE;
		}		

		Sleep(10);
/***********************************************************************
		end for imforming the device to start
************************************************************************/
#endif

	while(m_isThreadRun)
	{
		if(g_downSelect&DOWN_SELECT_STM32)
		{
			totalBytes = FW_STM32_LEN;
			m_DownSelection[idev] = DOWN_SEL_MAIN;

			while (dnBytes < totalBytes)
			{
				memset(loadBlock+6,0xFF,UPDATE_BLOCK_SIZE);
				memcpy(loadBlock+6,m_stm32Buf+dnBytes,16);
				triDesEnc.DES_DecryptEx((char*)loadBlock+6,g_desKey,(char*)loadBlock+6);
				triDesEnc.DES_DecryptEx((char*)loadBlock+6+8,g_desKey,(char*)loadBlock+6+8);

				loadBlock[0] = 0x00;
				loadBlock[1] = 0x7e;
				loadBlock[2] = 0xff;
				loadBlock[3] = STM32_IAP_WRITE_PAGE;//0x41;
				loadBlock[4] = 0x00;
				//loadBlock[4] = 64;
				loadBlock[5]=UPDATE_BLOCK_SIZE;

				loadBlock[UPDATE_BLOCK_SIZE+6] = 0;

				for (i=2;i<(UPDATE_BLOCK_SIZE+6);i++) {			//the head should not be added
					loadBlock[UPDATE_BLOCK_SIZE+6] += loadBlock[i];
				}

				Result = WriteFile 
					(devDescriptor[idev].wHandle, 			
					loadBlock, 
					0x41,//UPDATE_BLOCK_SIZE+6, 
					&bytesWritten, 
					NULL);

				if(!Result)
				{
					EnterCriticalSection(&g_cs);
					sprintf_s(infoBuf,"NO.%d 下载失败\r\n",idev);					
					m_strInfo += infoBuf;
					LeaveCriticalSection(&g_cs);
					m_isDevProg[idev] = FALSE;
					//::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 3);
					::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 4);
					return FALSE;
				}

				if (devDescriptor[idev].rHandle != INVALID_HANDLE_VALUE)
				{
					Result = ReadFile 
						(
						devDescriptor[idev].rHandle, 
						loadBlock, 
						0x41, 
						&bytesRead,
						&devDescriptor[idev].devOverlapped
						); 
				}else
				{
					EnterCriticalSection(&g_cs);
					sprintf_s(infoBuf,"NO.%d 下载失败\r\n",idev);
					m_strInfo += infoBuf;
					LeaveCriticalSection(&g_cs);
					m_isDevProg[idev] = FALSE;
					//::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 3);
					::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 4);
					return FALSE;
				}

				Result = WaitForSingleObject 
					((&devDescriptor[idev].devOverlapped)->hEvent, 
					6000);

				if((WAIT_OBJECT_0!=Result)||(loadBlock[1]!=0))
				{
					//goto down_error;
					EnterCriticalSection(&g_cs);
					sprintf_s(infoBuf,"NO.%d 下载失败\r\n",idev);
					m_strInfo += infoBuf;
					LeaveCriticalSection(&g_cs);
					m_isDevProg[idev] = FALSE;
					//::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 3);
					::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 4);
					return FALSE;
				}

				dnBytes += UPDATE_BLOCK_SIZE;

				if (FALSE == m_isThreadRun)
				{
					EnterCriticalSection(&g_cs);
					sprintf_s(infoBuf,"NO.%d 下载中断\r\n",idev);
					m_strInfo += infoBuf;
					LeaveCriticalSection(&g_cs);
					m_isDevProg[idev] = FALSE;
					//::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 3);
					::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 4);
					return FALSE;
				}

				m_DownPrecent[idev] = (dnBytes*100) / totalBytes;

				//Sleep(1);

			}
		}

		if (g_downSelect&DOWN_SELECT_CC2530)
		{
			dnBytes = 0;
			totalBytes = FW_CC2530_LEN;
			m_DownSelection[idev] = DOWN_SEL_RF;
			EnterCriticalSection(&g_cs);
			sprintf_s(infoBuf,"NO. %d 下载完毕，初始化硬件中，该过程不可跳过\r\n",idev);
			m_strInfo += infoBuf;
			LeaveCriticalSection(&g_cs);
			::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 3);

			while(dnBytes < totalBytes)
			{
				memset(loadBlock+6,0xFF,UPDATE_BLOCK_SIZE);
				//memcpy(loadBlock+6,m_cc2530Buf+dnBytes+FW_STM32_LEN,16);
				memcpy(loadBlock+6,m_cc2530Buf+dnBytes,16);
				triDesEnc.DES_DecryptEx((char*)loadBlock+6,g_desKey,(char*)loadBlock+6);
				triDesEnc.DES_DecryptEx((char*)loadBlock+6+8,g_desKey,(char*)loadBlock+6+8);

				loadBlock[0] = 0x00;
				loadBlock[1] = 0x7e;
				loadBlock[2] = 0xff;
				loadBlock[3] = STM32_IAP_CC2530;//0x41;
				loadBlock[4] = 0x00;
				//loadBlock[4] = 64;
				loadBlock[5]=UPDATE_BLOCK_SIZE;

				loadBlock[UPDATE_BLOCK_SIZE+6] = 0;

				for (i=2;i<(UPDATE_BLOCK_SIZE+6);i++) {			//the head should not be added
					loadBlock[UPDATE_BLOCK_SIZE+6] += loadBlock[i];
				}

				Result = WriteFile 
					(devDescriptor[idev].wHandle, 			
					loadBlock, 
					0x41,//UPDATE_BLOCK_SIZE+6, 
					&bytesWritten, 
					NULL);

				if (devDescriptor[idev].rHandle != INVALID_HANDLE_VALUE)
				{
					Result = ReadFile 
						(
						devDescriptor[idev].rHandle, 
						loadBlock, 
						0x41, 
						&bytesRead,
						&devDescriptor[idev].devOverlapped
						); 
				}else
				{
					EnterCriticalSection(&g_cs);
					sprintf_s(infoBuf,"NO.%d 下载失败\r\n",idev);
					m_strInfo += infoBuf;
					m_isDevProg[idev] = FALSE;
					LeaveCriticalSection(&g_cs);
					//::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 3);
					::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 4);
					return FALSE;
				}

				Result = WaitForSingleObject 
					((&devDescriptor[idev].devOverlapped)->hEvent, 
					6000);

				if(WAIT_OBJECT_0!=Result)
				{
					//m_ResultsString = _T("下载程序失败\r\n");
					EnterCriticalSection(&g_cs);
					sprintf_s(infoBuf,"NO.%d 下载失败\r\n",idev);
					m_strInfo += infoBuf;
					LeaveCriticalSection(&g_cs);
					m_isDevProg[idev] = FALSE;
					//::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 3);
					::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 4);
					return FALSE;

				}else if (loadBlock[1]!=0)
				{
					EnterCriticalSection(&g_cs);
					sprintf_s(infoBuf,"NO.%d 下载失败\r\n",idev);
					m_strInfo += infoBuf;
					LeaveCriticalSection(&g_cs);
					m_isDevProg[idev] = FALSE;
					//::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 3);
					::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 4);
					return FALSE;
				}

				dnBytes += UPDATE_BLOCK_SIZE;

				if (FALSE == m_isThreadRun)
				{
					EnterCriticalSection(&g_cs);
					sprintf_s(infoBuf,"NO.%d 下载中断\r\n",idev);
					m_strInfo += infoBuf;
					LeaveCriticalSection(&g_cs);
					m_isDevProg[idev] = FALSE;
					//::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 3);
					::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 4);
					return TRUE;
				}

				m_DownPrecent[idev] = (dnBytes*100)/totalBytes;

				Sleep(1);
				
				}

			}

			if (g_downSelect&DOWN_SELECT_SN)
			{
#if 0
				m_DownSelection[idev] = DOWN_SEL_VERI;
				dnBytes = 0;
				totalBytes = 512;

				EnterCriticalSection(&g_cs);
				sprintf_s(infoBuf,"NO. %d 写入固件信息\r\n",idev);
				m_strInfo += infoBuf;
				LeaveCriticalSection(&g_cs);
				::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 3);

				while(dnBytes < totalBytes)
				{
					memset(loadBlock+6,0xFF,UPDATE_BLOCK_SIZE);
					//memcpy(loadBlock+6,m_cc2530Buf+dnBytes+FW_STM32_LEN,16);
					memcpy(loadBlock+6,m_fwInfo+dnBytes,16);		

					loadBlock[0] = 0x00;
					loadBlock[1] = 0x7e;
					loadBlock[2] = 0xff;
					loadBlock[3] = STM32_IAP_VER;//0x51;
					loadBlock[4] = (dnBytes/16);
					//loadBlock[4] = 64;
					loadBlock[5]=UPDATE_BLOCK_SIZE;

					memcpy(loadBlock+6,m_fwInfo+dnBytes,UPDATE_BLOCK_SIZE);

					loadBlock[UPDATE_BLOCK_SIZE+6] = 0;

					//for (i=1;i<(64+5);i++) {			//the head should not be added
					//loadBlock[64+5] += loadBlock[i];
					//}

					for (i=2;i<(UPDATE_BLOCK_SIZE+6);i++) {			//the head should not be added
						loadBlock[UPDATE_BLOCK_SIZE+6] += loadBlock[i];
					}

					Result = WriteFile 
						(devDescriptor[idev].wHandle,		
						loadBlock, 
						0x41,//UPDATE_BLOCK_SIZE+6, 
						&bytesWritten, 
						NULL);

					if (devDescriptor[idev].rHandle != INVALID_HANDLE_VALUE)
					{
						Result = ReadFile 
							(
							devDescriptor[idev].rHandle, 
							loadBlock, 
							0x41, 
							&bytesRead,
							&devDescriptor[idev].devOverlapped
							); 
					}else
					{
						EnterCriticalSection(&g_cs);
						sprintf_s(infoBuf,"NO.%d 下载失败\r\n",idev);
						m_strInfo += infoBuf;
						m_isDevProg[idev] = FALSE;
						LeaveCriticalSection(&g_cs);
						//::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 3);
						::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 4);
						return FALSE;
					}

					Result = WaitForSingleObject 
						((&devDescriptor[idev].devOverlapped)->hEvent, 
						6000);

					if(WAIT_OBJECT_0!=Result)
					{
						//m_ResultsString = _T("下载程序失败\r\n");
						EnterCriticalSection(&g_cs);
						sprintf_s(infoBuf,"NO.%d 下载失败\r\n",idev);
						m_strInfo += infoBuf;
						LeaveCriticalSection(&g_cs);
						m_isDevProg[idev] = FALSE;
						//::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 3);
						::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 4);
						return FALSE;

					}else if (loadBlock[1]!=0)
					{
						EnterCriticalSection(&g_cs);
						sprintf_s(infoBuf,"NO.%d 下载失败\r\n",idev);
						m_strInfo += infoBuf;
						LeaveCriticalSection(&g_cs);
						m_isDevProg[idev] = FALSE;
						//::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 3);
						::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 4);
						return FALSE;
					}

					dnBytes += UPDATE_BLOCK_SIZE;

					if (FALSE == m_isThreadRun)
					{
						EnterCriticalSection(&g_cs);
						sprintf_s(infoBuf,"NO.%d 下载中断\r\n",idev);
						m_strInfo += infoBuf;
						LeaveCriticalSection(&g_cs);
						m_isDevProg[idev] = FALSE;
						//::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 3);
						::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 4);
						return TRUE;
					}
				}
#else
			/*********************************************************************
			//send the version information
			**********************************************************************/
			loadBlock[0] = 0x00;
			loadBlock[1] = 0x7e;
			loadBlock[2] = 0xff;
			loadBlock[3] = STM32_IAP_VER;//0x49;//0x48;//loadBlock[3] = 0x41;
			loadBlock[4] = 0x00;
			loadBlock[5] = UPDATE_BLOCK_SIZE;	
			loadBlock[6] = (FW_VERSION>>8);//(fileLen+UPDATE_BLOCK_SIZE)-times;
			loadBlock[7] = (FW_VERSION&0xff);
			loadBlock[UPDATE_BLOCK_SIZE+6] = 0x00;
			for (int i=2;i<UPDATE_BLOCK_SIZE+6;i++) {
				loadBlock[UPDATE_BLOCK_SIZE+6] += loadBlock[i];
			}

			Result = WriteFile 
				(devDescriptor[idev].wHandle,			
				loadBlock, 
				0x41,//64,//(fileLen+UPDATE_BLOCK_SIZE)-times+5, 
				&bytesWritten, 
				NULL);

			if(!Result)
			{
				sprintf_s(infoBuf,"NO.%d 写入设备信息失败\r\n",idev);
				m_strInfo += infoBuf;
				LeaveCriticalSection(&g_cs);
				m_isDevProg[idev] = FALSE;
				//::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 3);
				::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 4);
				return FALSE;
			}

			if (devDescriptor[idev].rHandle != INVALID_HANDLE_VALUE)
			{
				Result = ReadFile 
					(
					devDescriptor[idev].rHandle, 
					loadBlock, 
					0x41, 
					&bytesRead,
					&devDescriptor[idev].devOverlapped
					); 
			}else
			{
				sprintf_s(infoBuf,"NO.%d 写入设备信息失败\r\n",idev);
				m_strInfo += infoBuf;
				LeaveCriticalSection(&g_cs);
				m_isDevProg[idev] = FALSE;
				//::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 3);
				::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 4);
				return FALSE;;
			}

			if (!Result)
			{
				DWORD err = GetLastError();
			}

			Result = WaitForSingleObject 
				((&devDescriptor[idev].devOverlapped)->hEvent,
				6000);

			if((WAIT_OBJECT_0!=Result)||(loadBlock[1]!=0))
			{
				EnterCriticalSection(&g_cs);
				sprintf_s(infoBuf,"NO.%d 写入设备信息失败\r\n",idev);
				m_strInfo += infoBuf;
				LeaveCriticalSection(&g_cs);
				m_isDevProg[idev] = FALSE;
				//::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 3);
				::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 4);
				return FALSE;
			}		

#endif

			}

			if (g_downSelect&DOWN_SELECT_TAIL)
			{
				dnBytes = 0;
				totalBytes = FW_TAIL_LEN;
				m_DownSelection[idev] = DOWN_SEL_TAIL;

				EnterCriticalSection(&g_cs);
				sprintf_s(infoBuf,"NO. %d 下载完毕，正在校验下载数据，请稍后 ......\r\n",idev);
				m_strInfo += infoBuf;
				LeaveCriticalSection(&g_cs);
				::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 3);

				while(dnBytes < totalBytes)
				{
					memset(loadBlock+6,0xFF,UPDATE_BLOCK_SIZE);
					//memcpy(loadBlock+6,m_cc2530Buf+dnBytes+FW_STM32_LEN,16);
					memcpy(loadBlock+6,m_tailBuf+dnBytes,16);		

					loadBlock[0] = 0x00;
					loadBlock[1] = 0x7e;
					loadBlock[2] = 0xff;
					loadBlock[3] = STM32_IAP_TAIL;//0x51;
					loadBlock[4] = 0x00;
					//loadBlock[4] = 64;
					loadBlock[5]=UPDATE_BLOCK_SIZE;

					loadBlock[UPDATE_BLOCK_SIZE+6] = 0;

					//for (i=1;i<(64+5);i++) {			//the head should not be added
					//loadBlock[64+5] += loadBlock[i];
					//}

					for (i=2;i<(UPDATE_BLOCK_SIZE+6);i++) {			//the head should not be added
						loadBlock[UPDATE_BLOCK_SIZE+6] += loadBlock[i];
					}

					Result = WriteFile 
						(devDescriptor[idev].wHandle,		
						loadBlock, 
						0x41,//UPDATE_BLOCK_SIZE+6, 
						&bytesWritten, 
						NULL);

					dnBytes += UPDATE_BLOCK_SIZE;

					if (FALSE == m_isThreadRun)
					{
						EnterCriticalSection(&g_cs);
						sprintf_s(infoBuf,"NO.%d 下载中断\r\n",idev);
						m_strInfo += infoBuf;
						LeaveCriticalSection(&g_cs);
						m_isDevProg[idev] = FALSE;
						//::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 3);
						::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 4);
						return TRUE;
					}

					m_DownPrecent[idev] = (dnBytes*100)/totalBytes;

					Sleep(1);
				}

		}
/*
		if (g_downSelect&DOWN_SELECT_SN)
		{
			m_DownSelection[idev] = DOWN_SEL_VERI;
		}*/

/*********************************************************************
//info the stm32 jump to user's code
**********************************************************************/
		loadBlock[0] = 0x00;
		loadBlock[1] = 0x7e;
		loadBlock[2] = 0xff;
		loadBlock[3] = STM32_IAP_WRITE_OK;//0x49;//0x48;//loadBlock[3] = 0x41;
		loadBlock[4] = 0x00;
		loadBlock[5] = UPDATE_BLOCK_SIZE;	
		loadBlock[6] = 0x01;//(fileLen+UPDATE_BLOCK_SIZE)-times;
		loadBlock[7] = 0x00;
		loadBlock[UPDATE_BLOCK_SIZE+6] = 0x00;
		for (i=2;i<UPDATE_BLOCK_SIZE+6;i++) {
			loadBlock[UPDATE_BLOCK_SIZE+6] += loadBlock[i];
		}

		Result = WriteFile 
			(devDescriptor[idev].wHandle,		
			loadBlock,
			0x41,//64,//(fileLen+UPDATE_BLOCK_SIZE)-times+5, 
			&bytesWritten, 
			NULL);

		if(!Result)
		{
			EnterCriticalSection(&g_cs);
			sprintf_s(infoBuf,"NO.%d 启动错误\r\n",idev);
			m_strInfo += infoBuf;
			LeaveCriticalSection(&g_cs);
			m_isDevProg[idev] = FALSE;
			//::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 3);
			::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 4);
			return TRUE;
		}

		if (devDescriptor[idev].rHandle != INVALID_HANDLE_VALUE)
		{
			Result = ReadFile 
				(
				devDescriptor[idev].rHandle, 
				loadBlock, 
				0x41, 
				&bytesRead,
				&devDescriptor[idev].devOverlapped
				); 
		}else
		{
			EnterCriticalSection(&g_cs);
			sprintf_s(infoBuf,"NO.%d 启动错误\r\n",idev);
			m_strInfo += infoBuf;
			LeaveCriticalSection(&g_cs);
			m_isDevProg[idev] = FALSE;
			//::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 3);
			::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 4);
			return TRUE;
		}

		if (!Result)
		{
			DWORD err = GetLastError();
		}

		Result = WaitForSingleObject 
			((&devDescriptor[idev].devOverlapped)->hEvent,
			6000);

		if((WAIT_OBJECT_0!=Result)||(loadBlock[1]!=0))
		{
			EnterCriticalSection(&g_cs);
			sprintf_s(infoBuf,"NO.%d 启动错误\r\n",idev);
			m_strInfo += infoBuf;
			LeaveCriticalSection(&g_cs);
			m_isDevProg[idev] = FALSE;
			//::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 3);
			::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 4);
			return TRUE;
		}

		Sleep(10);

		//encrypto

		//times = 0;
/*********************************************************************************
//finish the cc2530 download
**********************************************************************************/

		Sleep(500);

		EnterCriticalSection(&g_cs);
		sprintf_s(infoBuf,"NO. %d 下载完毕\r\n",idev);
		m_strInfo += infoBuf;
		LeaveCriticalSection(&g_cs);
		m_isDevProg[idev] = FALSE;
		::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 4);
		return FALSE;

	}
	return TRUE;

//down_error:
	//return FALSE;
}

#if 0
UINT CUsbhidiocDlg::Stm32Program(LPVOID pParam) 
{
	FILE *cipher;
	FILE *ccfile;
	FILE *snfile;
	long fileLen;
	int times = 0;
	int i = 0;
	unsigned char loadBlock[80];
	char *resFile;
	char *cc2530File;
	char *serNumFile;
	//CSerialPort *comPort; 
	//CProgressCtrl *prog;
	CWnd *win;
	int proTmp=0;
//	char inf;
	static INFOPARA infoPara;
	HANDLE hWrite;
	HANDLE hRead;
	DWORD snIndex;
//	char *enc;
	DWORD bytesWritten;
	DWORD bytesRead;
	BOOL Result;
	LPOVERLAPPED lpOver;
	int promIndex;
	TriDesEnc triDesEnc;
//	DWORD err;

	resFile = ((BURNPARA*)pParam)->filePath;
	cc2530File = ((BURNPARA*)pParam)->cc2530Path;
	serNumFile = ((BURNPARA*)pParam)->serNumFile;
	//prog	= ((BURNPARA*)pParam)->prg;
	win     = ((BURNPARA*)pParam)->msgWin;
	//enc		= ((BURNPARA*)pParam)->encFlag;
	hWrite	= ((BURNPARA*)pParam)->hWrite;
	hRead = ((BURNPARA*)pParam)->hRead;
	lpOver = ((BURNPARA*)pParam)->lpOverlapped;
	snIndex = ((BURNPARA*)pParam)->snIndex;
	promIndex = ((BURNPARA*)pParam)->progIndex;
	//serNumFile = ((BURNPARA*)pParam)->serNumFile;


	//g_erase_flag = 0;      //clear erase flag

	while (!thread_end) 
	{
		//fread(loadBlock,sizeof(char),64,cipher);
		//times += 64;
		//times += 10;
		//times += UPDATE_BLOCK_SIZE;
		if(g_downSelect&DOWN_SELECT_STM32)
		{


		
		if(resFile==NULL)
		{
			return CIPHER_FILE_OPEN_ERROR;
		}

		//if((cipher = fopen(resFile,"rb")) == NULL)
		if (fopen_s(&cipher,resFile,"rb") != 0)
		{
			//infoPara.info = DOWNLOAD_WARNING_OPENFILE_ERROR;
			//infoPara.burnTotal = 0;
			//SetProcess(win,0,(LPARAM)&infoPara);
			//Sleep(10);
			//return CIPHER_FILE_OPEN_ERROR;
			goto openfile_error;
		}

#ifdef _HUAXIN_MD5_
		fileLen = FW_STM32_LEN;
#else
		fseek(cipher,0,SEEK_END);
		fileLen = ftell(cipher); 
		rewind(cipher);
#endif

		while (times<fileLen) 
		{
			//fread(loadBlock+5,sizeof(char),64,cipher);
			memset(loadBlock+6,0xFF,UPDATE_BLOCK_SIZE);
			if ((times+UPDATE_BLOCK_SIZE)<fileLen)
			{
				fread(loadBlock+6,sizeof(char),UPDATE_BLOCK_SIZE,cipher);	
				triDesEnc.DES_DecryptEx((char*)loadBlock+6,g_desKey,(char*)loadBlock+6);
				triDesEnc.DES_DecryptEx((char*)loadBlock+6+8,g_desKey,(char*)loadBlock+6+8);
			}else
			{
				fread(loadBlock+6,sizeof(char),fileLen-times,cipher);	
				triDesEnc.DES_DecryptEx((char*)loadBlock+6,g_desKey,(char*)loadBlock+6);
				triDesEnc.DES_DecryptEx((char*)loadBlock+6+8,g_desKey,(char*)loadBlock+6+8);
			}
			//fread(loadBlock+6,sizeof(char),UPDATE_BLOCK_SIZE,cipher);			

			loadBlock[0] = 0x00;
			loadBlock[1] = 0x7e;
			loadBlock[2] = 0xff;
			loadBlock[3] = STM32_IAP_WRITE_PAGE;//0x41;
			loadBlock[4] = 0x00;
			//loadBlock[4] = 64;
			loadBlock[5]=UPDATE_BLOCK_SIZE;

			loadBlock[UPDATE_BLOCK_SIZE+6] = 0;

			//for (i=1;i<(64+5);i++) {			//the head should not be added
				//loadBlock[64+5] += loadBlock[i];
			//}

			for (i=2;i<(UPDATE_BLOCK_SIZE+6);i++) {			//the head should not be added
				loadBlock[UPDATE_BLOCK_SIZE+6] += loadBlock[i];
			}


			//comPort->WriteToPort((char*)loadBlock,UPDATE_BLOCK_SIZE+6);
			Result = WriteFile 
			(hWrite, 			
			loadBlock, 
			0x41,//UPDATE_BLOCK_SIZE+6, 
			&bytesWritten, 
			NULL);

			if(!Result)
			{
				goto down_error;
			}

			if (hRead != INVALID_HANDLE_VALUE)
			{
				Result = ReadFile 
				(
					hRead, 
					loadBlock, 
					0x41, 
					&bytesRead,
					lpOver
				); 
			}else
			{
				goto down_error;
			}

			if (!Result)
			{
				DWORD err = GetLastError();
			}

			Result = WaitForSingleObject 
			(lpOver->hEvent, 
			6000);

			if((WAIT_OBJECT_0!=Result)||(loadBlock[1]!=0))
			{
				goto down_error;
			}

			times += UPDATE_BLOCK_SIZE;

			if (proTmp<((times*100)/fileLen))
			{			

				proTmp = (times*100)/fileLen;
			
				infoPara.info = DOWNLOAD_WARNING_BURN;
				infoPara.burnTotal = times;
				//infoPara.nowOperate = promIndex;
				//SetProcess(win,proTmp,(LPARAM)&infoPara);
				infoPara.progress = proTmp;
				WaitForSingleObject(g_hEvent,INFINITE);
				SetProcess(win,promIndex,(LPARAM)&infoPara);

				//Sleep(1);
			}
			Sleep(1);


		}
//*********************************************************************
//remember close the file after operation
//*********************************************************************
		fclose(cipher);
	}//Download the stm32

/*********************************************************************
//send the version information
**********************************************************************/
		if (g_downSelect & DOWN_SELECT_SN)
		{
			int snCnt = 0;
			while (snCnt < 4)           // 4x16
			{
				loadBlock[0] = 0x00;
				loadBlock[1] = 0x7e;
				loadBlock[2] = 0x00;
				loadBlock[3] = STM32_IAP_VER;//0x49;//0x48;//loadBlock[3] = 0x41;
				loadBlock[4] = snCnt;
				loadBlock[5] = UPDATE_BLOCK_SIZE;	
				//loadBlock[6] = (FW_VERSION>>8);//(fileLen+UPDATE_BLOCK_SIZE)-times;
				//loadBlock[7] = (FW_VERSION&0xff);
				loadBlock[UPDATE_BLOCK_SIZE+6] = 0x00;
				for (i=2;i<UPDATE_BLOCK_SIZE+6;i++) {
					loadBlock[UPDATE_BLOCK_SIZE+6] += loadBlock[i];
				}

				Result = WriteFile 
				(hWrite, 			
				loadBlock, 
				0x41,//64,//(fileLen+UPDATE_BLOCK_SIZE)-times+5, 
				&bytesWritten, 
				NULL);

				if(!Result)
				{
					goto down_error;
				}

				if (hRead != INVALID_HANDLE_VALUE)
				{
					Result = ReadFile 
					(
						hRead, 
						loadBlock, 
						0x41, 
						&bytesRead,
						lpOver
					); 
				}else
				{
					goto down_error;
				}

				if (!Result)
				{
					DWORD err = GetLastError();
				}

				Result = WaitForSingleObject 
				(lpOver->hEvent, 
				6000);

				if((WAIT_OBJECT_0!=Result)||(loadBlock[1]!=0))
				{
					goto down_error;
				}	
			}
		}

		Sleep(10);

//the following code if target at program cc2530
		if (g_downSelect&DOWN_SELECT_CC2530)
		{
#ifdef _HUAXIN_MD5_
			//if((ccfile = fopen(resFile,"rb")) == NULL)
			if (0 != fopen_s(&ccfile,resFile,"rb"))
			{
				goto openfile_error;
			}
			times=0;

			fseek(ccfile,FW_STM32_LEN,SEEK_SET);

			fileLen = FW_CC2530_LEN;
#else
			if((ccfile = fopen(cc2530File,"rb")) == NULL)
			{
				goto openfile_error;
			}
			times=0;

			fseek(ccfile,0,SEEK_END);
			fileLen = ftell(ccfile); 
			rewind(ccfile);
#endif
			proTmp = 0;

			while (times<fileLen)
			{
				fread(loadBlock+6,sizeof(char),UPDATE_BLOCK_SIZE,ccfile);	
				triDesEnc.DES_DecryptEx((char*)loadBlock+6,g_desKey,(char*)loadBlock+6);
				triDesEnc.DES_DecryptEx((char*)loadBlock+6+8,g_desKey,(char*)loadBlock+6+8);

				loadBlock[0] = 0x00;
				loadBlock[1] = 0x7e;
				loadBlock[2] = 0xff;
				loadBlock[3] = STM32_IAP_CC2530;//0x51;
				loadBlock[4] = 0x00;
				//loadBlock[4] = 64;
				loadBlock[5]=UPDATE_BLOCK_SIZE;

				loadBlock[UPDATE_BLOCK_SIZE+6] = 0;

				//for (i=1;i<(64+5);i++) {			//the head should not be added
				//loadBlock[64+5] += loadBlock[i];
				//}

				for (i=2;i<(UPDATE_BLOCK_SIZE+6);i++) {			//the head should not be added
					loadBlock[UPDATE_BLOCK_SIZE+6] += loadBlock[i];
				}

				Result = WriteFile 
					(hWrite, 			
					loadBlock, 
					0x41,//UPDATE_BLOCK_SIZE+6, 
					&bytesWritten, 
					NULL);

				if (hRead != INVALID_HANDLE_VALUE)
				{
					Result = ReadFile 
						(
						hRead, 
						loadBlock, 
						0x41, 
						&bytesRead,
						lpOver
						); 
				}else
				{
					goto down_error;
				}

				Result = WaitForSingleObject 
					(lpOver->hEvent, 
					6000);

				if(WAIT_OBJECT_0!=Result)
				{
					goto down_error;
				}else if (loadBlock[1]!=0)
				{
					proTmp = loadBlock[1];
					infoPara.burnTotal = loadBlock[2];
					goto down_error;
				}

				//times += UPDATE_BLOCK_SIZE;
				infoPara.info = DOWNLOAD_WARNING_PROGRAM_CC2530;
				infoPara.burnTotal = times;
				if (proTmp<((times*100)/fileLen))
				{
					proTmp = (times*100)/fileLen;
					//infoPara.nowOperate = promIndex;
					infoPara.progress = proTmp;
					//SetProcess(win,proTmp,(LPARAM)&infoPara);
					WaitForSingleObject(g_hEvent,INFINITE);
					SetProcess(win,promIndex,(LPARAM)&infoPara);
				}
				times += UPDATE_BLOCK_SIZE;
				Sleep(1);

			}
			fclose(ccfile);
			
		}

//write the sn number and mac addr
		if (g_downSelect&DOWN_SELECT_SN)
		{
			//if((snfile = fopen(serNumFile,"rb")) == NULL)
			if (fopen_s(&snfile,serNumFile,"rb") != 0)
			{
				goto openfile_error;
			}
			
			char snBuf[100]={0};

			if(!ExtractFixContent(snfile,snIndex,snBuf))
			{
				fclose(snfile);
				goto sn_error;
			}

			loadBlock[0] = 0x00;
			loadBlock[1] = 0x7e;
			loadBlock[2] = 0xff;
			loadBlock[3] = STM32_IAP_SERNUM;
			loadBlock[4] = 0x00;
			//loadBlock[4] = 64;
			loadBlock[5]=UPDATE_BLOCK_SIZE;

			//DWORD64 *sn = (DWORD64 *)(loadBlock+6);
			//DWORD64 *mac = (DWORD64 *)(loadBlock+6+8);

			for (i=0;i<16;i++)
			{
				loadBlock[6+i] = snBuf[i];
			}

			//*sn = atoihex(snBuf);
			//*mac = atoihex(snBuf+17);
			
			//memcpy(loadBlock+6,snBuf,8);
			
			//memcpy(loadBlock+6+8,snBuf+9,8);

			loadBlock[UPDATE_BLOCK_SIZE+6] = 0;

			for (i=2;i<(UPDATE_BLOCK_SIZE+6);i++) 
			{			//the head should not be added
				loadBlock[UPDATE_BLOCK_SIZE+6] += loadBlock[i];
			}

			Result = WriteFile 
				(hWrite, 			
				loadBlock, 
				0x41,//UPDATE_BLOCK_SIZE+6, 
				&bytesWritten, 
				NULL);

			if (hRead != INVALID_HANDLE_VALUE)
			{
				Result = ReadFile 
					(
					hRead, 
					loadBlock, 
					0x41, 
					&bytesRead,
					lpOver
					); 
			}else
			{
				goto down_error;
			}

			Result = WaitForSingleObject 
				(lpOver->hEvent, 
				6000);

			if(WAIT_OBJECT_0!=Result)
			{
				goto down_error;
			}else if (loadBlock[1]!=0)
			{
				proTmp = loadBlock[1];
				infoPara.burnTotal = loadBlock[2];
				goto down_error;
			}

			infoPara.info = DOWNLOAD_WARNING_SNOK;
			infoPara.burnTotal = snIndex;
			WaitForSingleObject(g_hEvent,INFINITE);
			//SetProcess(win,proTmp,(LPARAM)&infoPara);
			SetProcess(win,promIndex,(LPARAM)&infoPara);
			Sleep(5);
		}

		infoPara.info = DOWNLOAD_WARNING_BURN;
		infoPara.burnTotal = fileLen;

		//fclose(cipher);
		//unlink(resFile);
		//fclose(ccfile);
		WaitForSingleObject(g_hEvent,INFINITE);
		//SetProcess(win,100,(LPARAM)&infoPara.info);
		infoPara.progress = 100;
		SetProcess(win,promIndex,(LPARAM)&infoPara);
		Sleep(10);

/*********************************************************************
//info the stm32 jump to user's code
**********************************************************************/
		loadBlock[0] = 0x00;
		loadBlock[1] = 0x7e;
		loadBlock[2] = 0xff;
		loadBlock[3] = STM32_IAP_WRITE_OK;//0x49;//0x48;//loadBlock[3] = 0x41;
		loadBlock[4] = 0x00;
		loadBlock[5] = UPDATE_BLOCK_SIZE;	
		loadBlock[6] = 0x01;//(fileLen+UPDATE_BLOCK_SIZE)-times;
		loadBlock[7] = 0x00;
		loadBlock[UPDATE_BLOCK_SIZE+6] = 0x00;
		for (i=2;i<UPDATE_BLOCK_SIZE+6;i++) {
			loadBlock[UPDATE_BLOCK_SIZE+6] += loadBlock[i];
		}

		Result = WriteFile 
		(hWrite, 			
		loadBlock, 
		0x41,//64,//(fileLen+UPDATE_BLOCK_SIZE)-times+5, 
		&bytesWritten, 
		NULL);

		if(!Result)
		{
			goto down_error;
		}

		if (hRead != INVALID_HANDLE_VALUE)
		{
			Result = ReadFile 
			(
				hRead, 
				loadBlock, 
				0x41, 
				&bytesRead,
				lpOver
			); 
		}else
		{
			goto down_error;
		}

		if (!Result)
		{
			DWORD err = GetLastError();
		}

		Result = WaitForSingleObject 
		(lpOver->hEvent, 
		6000);

		if((WAIT_OBJECT_0!=Result)||(loadBlock[1]!=0))
		{
			goto down_error;
		}		

		infoPara.info = DOWNLOAD_WARNING_SUCCESS;
		infoPara.burnTotal = times;
		proTmp = 100;
		//infoPara.nowOperate = promIndex;
		infoPara.progress = proTmp;
		//SetProcess(win,proTmp,(LPARAM)&infoPara);
		WaitForSingleObject(g_hEvent,INFINITE);
		SetProcess(win,promIndex,(LPARAM)&infoPara);

		Sleep(10);
		
		//encrypto

		//times = 0;
/*********************************************************************************
//finish the cc2530 download
**********************************************************************************/
		return UPDATE_SUCCESS;

	}
down_error:
	infoPara.info = DOWNLOAD_WARNING_DOWNLOAD_FAILED;
	//infoPara.nowOperate = promIndex;
	//infoPara.burnTotal = times;
	infoPara.progress = 0;
	//SetProcess(win,proTmp,(LPARAM)&infoPara);
	WaitForSingleObject(g_hEvent,INFINITE);
	SetProcess(win,promIndex,(LPARAM)&infoPara);
	Sleep(20);
	fclose(cipher);
	return UPDATE_FAILED;
openfile_error:
	//infoPara.nowOperate = promIndex;
	infoPara.progress = 0;
	infoPara.info = DOWNLOAD_WARNING_OPENFILE_ERROR;
	infoPara.burnTotal = 0;
	//SetProcess(win,0,(LPARAM)&infoPara);
	WaitForSingleObject(g_hEvent,INFINITE);
	SetProcess(win,promIndex,(LPARAM)&infoPara);
	Sleep(10);
	return CIPHER_FILE_OPEN_ERROR;
sn_error:
	//infoPara.nowOperate = promIndex;
	infoPara.progress = 0;
	infoPara.info = DOWNLOAD_WARNING_OPENFILE_ERROR;
	//SetProcess(win,proTmp,(LPARAM)&infoPara);
	WaitForSingleObject(g_hEvent,INFINITE);
	SetProcess(win,promIndex,(LPARAM)&infoPara);
	Sleep(20);
	return DOWNLOAD_WARNING_OPENFILE_ERROR;

}
#endif


void CUsbhidiocDlg::OnClean() 
{
	// TODO: Add your control notification handler code here
	//m_ResultsString = "";
	//m_ResultsList.ResetContent();
	//UpdateData(false);	
	m_strInfo = _T("");
	UpdateData(FALSE);
}



void CUsbhidiocDlg::OnRead() 
{
	// TODO: Add your control notification handler code here
	unsigned char loadBlock[80]={0};
	ULONG Result;
	ULONG BytesWritten;

	loadBlock[0] = 0x00;
	loadBlock[1] = 0x7e;
	loadBlock[2] = 0xff;
	loadBlock[3] = 0x47;
	loadBlock[4] = 0x00;
	//loadBlock[4] = 64;
	loadBlock[5] = UPDATE_BLOCK_SIZE;
	loadBlock[UPDATE_BLOCK_SIZE+6] = 0x00;

	for (int i=2;i<UPDATE_BLOCK_SIZE+6;i++)
	{
		loadBlock[UPDATE_BLOCK_SIZE+6] += loadBlock[i];
	}
	//loadBlock[UPDATE_BLOCK_SIZE+6] = 0x56;

	if (WriteHandle != INVALID_HANDLE_VALUE)
	{
		Result = WriteFile 
		(WriteHandle, 
		loadBlock, 
		0x41,//Capabilities.OutputReportByteLength, 
		&BytesWritten, 
		NULL);
	}

	//ResetEvent(hEventObject);
	ResetEvent(devDescriptor[m_devIndex].hEventObject);

	Sleep(5);

	//ReadInputReport();
	
}


void CUsbhidiocDlg::OnBnClickedCc2530()
{
	// TODO: Add your control notification handler code here

	FILE *pFw;
	int times=0;
	int fileLen;
	unsigned char loadBlock[80];
	DWORD bytesWritten;
	BOOL Result;
	int i;
	errno_t err;

	if((err=fopen_s(&pFw,"Firmware.bin","rb"))!=0)
	{
		MessageBox(_T("open file error"));
	}

	DisplayData("openfile success");

	fseek(pFw,0,SEEK_END);
	fileLen = ftell(pFw); 
	rewind(pFw);

	DisplayData("read");

	while (times<fileLen) 
	{
		//fread(loadBlock+5,sizeof(char),64,cipher);
		fread(loadBlock+6,sizeof(char),UPDATE_BLOCK_SIZE,pFw);			

		loadBlock[0] = 0x00;
		loadBlock[1] = 0x7e;
		loadBlock[2] = 0xff;
		loadBlock[3] = 0x51;
		loadBlock[4] = 0x00;
		//loadBlock[4] = 64;
		loadBlock[5]=UPDATE_BLOCK_SIZE;

		loadBlock[UPDATE_BLOCK_SIZE+6] = 0;

		//for (i=1;i<(64+5);i++) {			//the head should not be added
		//loadBlock[64+5] += loadBlock[i];
		//}

		for (i=2;i<(UPDATE_BLOCK_SIZE+6);i++) {			//the head should not be added
			loadBlock[UPDATE_BLOCK_SIZE+6] += loadBlock[i];
		}

		DisplayData("send one block");

		Result = WriteFile 
			(WriteHandle, 			
			loadBlock, 
			0x41,//UPDATE_BLOCK_SIZE+6, 
			&bytesWritten, 
			NULL);

		Sleep(10);

		times += UPDATE_BLOCK_SIZE;

	}

}


void CUsbhidiocDlg::OnBnClickedRadio1()
{
	// TODO: Add your control notification handler code here
}

#if 0
void CUsbhidiocDlg::OnBnClickedRadioCc2530()
{
	// TODO: Add your control notification handler code here
	if (((CButton *)GetDlgItem(IDC_RADIO_CC2530))->GetCheck()==TRUE)
	{
		((CButton *)GetDlgItem(IDC_RADIO_CC2530))->SetCheck(FALSE);
		g_cc2530DownFlag = 0;
	}else
	{
		((CButton *)GetDlgItem(IDC_RADIO_CC2530))->SetCheck(TRUE);
		g_cc2530DownFlag = 1;
	}
	
}
#endif


#if 0 
void CUsbhidiocDlg::OnBnClickedCheck2()
{
	// TODO: Add your control notification handler code here
}
#endif


void CUsbhidiocDlg::OnBnClickedCheckCc2530()
{
	// TODO: Add your control notification handler code here

	if (BST_CHECKED==((CButton*)GetDlgItem(IDC_CHECK_CC2530))->GetCheck())
	{
		((CButton*)GetDlgItem(IDC_CHECK_CC2530))->SetCheck(TRUE);
		g_downSelect |= DOWN_SELECT_CC2530;
	}else
	{
		((CButton*)GetDlgItem(IDC_CHECK_CC2530))->SetCheck(FALSE);
		g_downSelect &= ~DOWN_SELECT_CC2530;
	}
}

#if 0
void CUsbhidiocDlg::OnBnClickedCheck1()
{
	// TODO: Add your control notification handler code here
}
#endif


void CUsbhidiocDlg::OnBnClickedDownStm32()
{
	// TODO: Add your control notification handler code here
	if (BST_CHECKED==((CButton*)GetDlgItem(IDC_DOWN_STM32))->GetCheck())
	{
		((CButton*)GetDlgItem(IDC_DOWN_STM32))->SetCheck(TRUE);
		g_downSelect |= DOWN_SELECT_STM32;
	}else
	{
		((CButton*)GetDlgItem(IDC_DOWN_STM32))->SetCheck(FALSE);
		g_downSelect &= ~DOWN_SELECT_STM32;
	}
}


void CUsbhidiocDlg::OnBnClickedSnMac()
{
	// TODO: Add your control notification handler code here
	if (BST_CHECKED==((CButton*)GetDlgItem(IDC_SN_MAC))->GetCheck())
	{
		((CButton*)GetDlgItem(IDC_SN_MAC))->SetCheck(TRUE);
		g_downSelect |= DOWN_SELECT_SN;
	}else
	{
		((CButton*)GetDlgItem(IDC_SN_MAC))->SetCheck(FALSE);
		g_downSelect &= ~DOWN_SELECT_SN;
	}
}

DWORD64 CUsbhidiocDlg::atoihex(char s[])  
{  
	int i = 0,sign;    //输入的数前面可能还有空格或制表符应加判断  
	DWORD64 sum=0;
	while(' '==s[i]||'\t'==s[i])  
	{  
		i++;  
	}  
	sign = ('-'==s[i])?-1:1;  
	if('-'==s[i]||'+'==s[i])  
	{  
		i++;  
	}  
	while((s[i]!='\0')&&(s[i]!=';'))
	{  
		if (('A'<=s[i])&&(s[i]<='Z'))
		{
			sum = s[i]-'A'+ 0x0a +sum*16;
		}else if (('a'<=s[i])&&(s[i]<='z'))
		{
			sum = s[i]-'a'+ 0x0a + sum*16;
		}else if (('0'<=s[i])&&(s[i]<='9'))
		{
			sum = s[i]-'0'+sum*16;
		}else
		{
			sum = 0;
		}
		i++;
	}      
	return sign*sum;  
}  



void CUsbhidiocDlg::OnEnUpdateSn()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数，以将 EM_SETEVENTMASK 消息发送到该控件，
	// 同时将 ENM_UPDATE 标志“或”运算到 lParam 掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData();
}


void CUsbhidiocDlg::OnEnChangeSn()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData();
}


void CUsbhidiocDlg::OnLbnSelchangeList2()
{
	// TODO: 在此添加控件通知处理程序代码
	//MessageBox("点我干嘛");
}


void CUsbhidiocDlg::OnCbnSelchangeDevlist()
{
	// TODO: 在此添加控件通知处理程序代码
	//UpdateData(FALSE);
	m_devIndex = ((CComboBox*)GetDlgItem(IDC_DEVLIST))->GetCurSel();
}

BOOL CUsbhidiocDlg::IsDevOnProgram(void)
{
	UINT8 i;
	for (i=0;i<m_devTotal;i++)
	{
		if (TRUE==m_isDevProg[i])
		{
			return TRUE;
		}

	}

	return FALSE;
}

void CUsbhidiocDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if ((m_devTotal>0)&&(TRUE==IsDevOnProgram()))
	{	
		if(m_dspStay ++ >4)
		{
			m_dspStay = 0;

			m_progCnt ++;
			if (m_progCnt >= m_devTotal)
			{
				m_progCnt = 0;
			}
			while(!m_isDevProg[m_progCnt])
			{
				m_progCnt ++;

				if (m_progCnt >= m_devTotal)
				{
					m_progCnt = 0;
				}
			}
		}
		
		m_progress.SetPos(m_DownPrecent[m_progCnt]);
		CString strOnBar;
		if (DOWN_SEL_MAIN == m_DownSelection[m_progCnt])
		{
			strOnBar.Format(_T("NO.%d 下载   "),m_progCnt);
		}else if (DOWN_SEL_RF == m_DownSelection[m_progCnt])
		{
			strOnBar.Format(_T("NO.%d 初始化   "),m_progCnt);
		}else if (DOWN_SEL_VERI == m_DownSelection[m_progCnt])
		{
			strOnBar.Format(_T("NO.%d 校验   "),m_progCnt);
		}
		m_progress.SetWindowText(strOnBar);
	}
	CDialog::OnTimer(nIDEvent);
}

LRESULT CUsbhidiocDlg::OnUpdateInfo(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case 0:
		UpdateData(FALSE);
		//m_btnOK.SetWindowTextW(_T("下载代码"));
		m_isThreadRun = FALSE;
		break;

	case 1:
		UpdateData(FALSE);
		break;

	case 3:
		UpdateData(FALSE);
		//Sleep(1000);
		//m_isThreadRun = TRUE;
		//m_ctrlProg.SetPos(0);
		//m_btnOK.SetWindowTextW(_T("停止下载"));
		//AfxBeginThread(ThreadDownload, this);
		break;
	
	case 4:
		UpdateData(FALSE);
		m_isThreadRun = FALSE;
		GetDlgItem(IDC_USB_BURN)->SetWindowText(_T("开始"));
		break;

	}

	return NULL;
}


void CUsbhidiocDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CStdioFile cf;
	CFileException e;
	if (!cf.Open(_T("source.bin"),CFile::modeRead,&e))
	{
		MessageBox(_T("openfile error"));
		return;
	}
	CString strHex;
	char readBuf[100*1024];
	char buf[100];

	cf.Read(readBuf,100*1024);
	
	for (int i=0;i < 100*1024;i++)
	{
		sprintf_s(buf,"0x%02X, ",(UINT8)readBuf[i]);
		strHex+=buf;
		if (!((i+1)%16))
		{
			strHex+=_T("\r\n");
		}
	}
	for (int i=0;i < 100*1024;i++)
	{
		sprintf_s(buf,"0x%02X, ",(UINT8)readBuf[i]);
		strHex+=buf;
		if (!((i+1)%16))
		{
			strHex+=_T("\r\n");
		}
	}
	for (int i=0;i < (56-8)*1024;i++)
	{
		sprintf_s(buf,"0x%02X, ",(UINT8)readBuf[i]);
		strHex+=buf;
		if (!((i+1)%16))
		{
			strHex+=_T("\r\n");
		}
	}

	cf.Close();

	if (!cf.Open(_T("source.hex"),CFile::modeCreate|CFile::modeWrite,&e))
	{
		MessageBox(_T("openfile error"));
		return;
	}

	cf.WriteString(strHex);

	MessageBox(_T("OK"));

}
