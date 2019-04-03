
// bootHostDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "bootHost.h"
#include "bootHostDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define BAUDRATE 115200
//#define BAUDRATE 9600
//#define BAUDRATE 57600

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


// CbootHostDlg 对话框




CbootHostDlg::CbootHostDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CbootHostDlg::IDD, pParent)
	, m_strFileName(_T(""))
	, m_strInfo(_T(""))
	, m_uartSel(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strUarPrt = _T("");
}

void CbootHostDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strFileName);
	DDX_Text(pDX, IDC_EDIT2, m_strInfo);
	DDX_Control(pDX, IDC_UART, m_ctrlUart);
	DDX_Control(pDX, IDC_PROGRESS1, m_ctrlProg);
	DDX_Control(pDX, IDC_UART_OPEN, m_ctrlOpen);
	DDX_CBIndex(pDX, IDC_UART, m_uartSel);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Text(pDX, IDC_UART_PRT, m_strUarPrt);
}

BEGIN_MESSAGE_MAP(CbootHostDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CbootHostDlg::OnBnClickedOpenFile)
	ON_MESSAGE(WM_COMM_RXCHAR,&CbootHostDlg::OnUartMessage)
	ON_MESSAGE(WM_DLG_MSG,&CbootHostDlg::OnUpdateInfo)
	ON_BN_CLICKED(IDC_UART_OPEN, &CbootHostDlg::OnBnClickedUartOpen)
	ON_CBN_SELCHANGE(IDC_UART, &CbootHostDlg::OnCbnSelchangeUart)
	ON_BN_CLICKED(IDOK, &CbootHostDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CLEAR, &CbootHostDlg::OnBnClickedClear)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_PROGRESS1, &CbootHostDlg::OnNMCustomdrawProgress1)
	ON_BN_CLICKED(IDCANCEL, &CbootHostDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CbootHostDlg 消息处理程序

BOOL CbootHostDlg::OnInitDialog()
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
	m_ctrlOpen.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_STOP));
	// TODO: 在此添加额外的初始化代码
	//serialPort.StartMonitoring();
	m_isUartOpen = FALSE;
	EmultateCom();
	m_ctrlUart.SetCurSel(0);
	m_comNumber = m_comArr[0];

	m_hEventDownLoad = CreateEvent(
		NULL, // pointer to security attributes
		TRUE,  // flag for manual-reset event
		FALSE, // flag for initial state
		NULL // pointer to event-object name
		);
	m_isThreadRun = FALSE;

//	m_ctrlProg.SetShowText(TRUE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CbootHostDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CbootHostDlg::OnPaint()
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
HCURSOR CbootHostDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CbootHostDlg::OnBnClickedOpenFile()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE,//TRUE是创建打开文件对话框，FALSE则创建的是保存文件对话框 
		_T(".txt"),//默认的打开文件的类型 
		NULL,//默认打开的文件名 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,//打开只读文件 
		_T("bin文件(*.bin)|*.bin|所有文件 (*.*)|*.*||"));//所有可以打开的文件类型 

	if(dlg.DoModal()==IDOK)   
	{ 
		m_strFileName = dlg.GetPathName();////////取出文件路径 
		UpdateData(FALSE); 
	} 
}


LRESULT CbootHostDlg::OnUartMessage(WPARAM wParam, LPARAM lParam)
{
	ComfirmProc((UINT8*)&wParam);
	return TRUE;
}

void CbootHostDlg::EmultateCom()
{
	CString strCom;
	int nCom = 0;
	//int count = 0;
	HANDLE  hCom;
	m_comCnt = 0;
	//m_ctrlUart.Clear();
	m_ctrlUart.ResetContent();
	do {
		nCom++;

		if(nCom<10)
		{
			strCom.Format(_T("COM%d"), nCom);
		}else
		{
			strCom.Format(_T("\\\\.\\COM%d"),nCom);
		}

		hCom = CreateFile(strCom, 0, 0, 0, 
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		if(INVALID_HANDLE_VALUE == hCom )
		{
			continue;;
		}else
		{
			m_comArr[m_comCnt++] = nCom;
		}
		//m_cbCom.AddString(strCom);
		//((CComboBox*)GetDlgItem(IDC_BURN_SELECT))->AddString();
		m_ctrlUart.AddString(strCom);

		CloseHandle(hCom);	

	} while(nCom<100);

	m_comNumber = m_comArr[0];

	if (m_comCnt > 0)
	{
		//m_ctrlUart.UpdateData();
	}

	UpdateData(FALSE);
}

void CbootHostDlg::CloseAllComs()
{
}



void CbootHostDlg::OnBnClickedUartOpen()
{
	CString strCom;
	HANDLE  hCom;
	// TODO: 在此添加控件通知处理程序代码
	if (FALSE==m_isUartOpen)
	{
		if (m_comCnt == 0)
		{
			EmultateCom();
		}

		if (m_comCnt == 0)
		{
			m_strInfo = _T("无可用的串口设备\r\n");
			UpdateData(FALSE);
			return;
		}
		
		if (m_comNumber < 10)
		{
			strCom.Format(_T("COM%d"), m_comNumber);
		}else
		{
			strCom.Format(_T("\\\\.\\COM%d"), m_comNumber);
		}
		
		hCom = CreateFile(strCom, 0, 0, 0, 
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if(INVALID_HANDLE_VALUE == hCom )
		{
			m_strInfo = _T("串口无法打开\r\n");
			UpdateData(FALSE);
		}else
		{
			CloseHandle(hCom);
			if (m_uartPort.InitPort(this,m_comNumber,BAUDRATE,'N',8,1,(EV_RXFLAG | EV_RXCHAR),512)) {
				m_uartPort.StartMonitoring();
				m_isUartOpen = TRUE;
				m_ctrlOpen.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_RUN));
			}else
			{
				m_strInfo = _T("串口无法打开\r\n");
				UpdateData(FALSE);
			}
			
		}

	}else
	{
		m_isUartOpen = FALSE;
		m_uartPort.ClosePort();
		m_ctrlOpen.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_STOP));
	}
}


void CbootHostDlg::OnCbnSelchangeUart()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_comNumber = m_comArr[m_uartSel];
	if (TRUE==m_isUartOpen)
	{
		m_uartPort.ClosePort();

		if (m_uartPort.InitPort(this,m_comNumber,115200,'N',8,1,(EV_RXFLAG | EV_RXCHAR),512)) {
			m_uartPort.StartMonitoring();
			m_isUartOpen = TRUE;
			m_ctrlOpen.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_RUN));
		}else
		{
			m_strInfo = _T("串口无法打开\r\n");
			m_ctrlOpen.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_STOP));
			UpdateData(FALSE);
		}
	}
	
}

void CbootHostDlg::ComfirmProc(UINT8* rcv)
{
	unsigned char *str;
	char buf[100];
	//	char *str1;
	//    int k, nEvent;//, i;
	static unsigned char loader=0;
	static unsigned char rcvIndex=0;
	static unsigned char bufIndex=0;
//	FILE *FRead;
	//	static unsigned char sum=0;
	//	int i;
	//	static unsigned char CfmBuf[80];

	str = rcv;
	//m_strUarPrt += *str;
	//sprintf(buf,"%02x ",*str);
	//m_strUarPrt += buf;
	//UpdateData(FALSE);

	if (!loader)
	{
		if (*str==0x7e) {
			loader = 1;
			rcvIndex = 0;
			memset(stCommander.buf,0,sizeof(stCommander.buf));
			stCommander.sum = 0;
			//memset(CfmBuf,0,sizeof(CfmBuf));
			return;
		}else if(*str==0xA8)
		{
			char ret = 0x57;
			m_uartPort.WriteToPort(&ret,1);
			//m_strInfo += _T("发现设备,下载中......\r\n");
			if (!m_isThreadRun)
			{
				m_strInfo += _T("Device linked, Downloading......\r\n");
				::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 3);
			}
		}
	}else
	{
		if(0==rcvIndex)
		{
			rcvIndex++;
			stCommander.cmd = *str; //0x41
			stCommander.sum += *str;
		}else if(1==rcvIndex)
		{
			rcvIndex++;				//0x00
			stCommander.addr = (((UINT32)*str)<<16);
			stCommander.sum += *str;
		}else if(2==rcvIndex)
		{
			stCommander.addr += (((UINT32)*str)<<8);
			stCommander.sum += *str;
			rcvIndex++;
		}else if(3==rcvIndex)
		{
			stCommander.addr += ((UINT32)*str);
			stCommander.sum += *str;
			rcvIndex++;
		}else if (4==rcvIndex)
		{
			stCommander.data_len = (((UINT16)*str)<<8);
			stCommander.sum += *str;
			rcvIndex++;
		}else if (5==rcvIndex)
		{
			stCommander.data_len += ((UINT16)*str);
			stCommander.sum += *str;
			rcvIndex++;
			bufIndex = 0;
		}
		else 
		{	
			if (bufIndex<stCommander.data_len) 
			{
				stCommander.buf[bufIndex++] = *str;
				stCommander.sum += *str;
			}else
			{
				//stCommander.cmd_buf[bufIndex++] = *str;				//sum
				loader = 0;
				if (stCommander.sum != *str)
				{
					//g_downLoad_enable = 0;
					loader = 0;
					rcvIndex = 0;
					stCommander.sum = 0;
					m_strInfo = _T("verified data error\r\n");
					::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 3);
					//					g_downLoad_enable = -1;
					//					g_erase_flag = -1;
					return;
					//AfxMessageBox("校验错误，请连续工程部");
				}

				switch(stCommander.cmd)
				{
				case GET_FIRMWARE_VERSION:
					break;

				case AM1_IAP_WRITE_PAGE:
					if (!stCommander.buf[0]) 
					{
						SetEvent(m_hEventDownLoad);
						m_isDownOk = TRUE;
						rcvIndex = 0;
						stCommander.sum = 0;
						return;
					}else
					{
						SetEvent(m_hEventDownLoad);
						m_isDownOk = FALSE;
						rcvIndex = 0;
						stCommander.sum = 0;
					}
					break;

				case AM1_IAP_REPORT_ERROR:
					if (m_isThreadRun)
					{
						SetEvent(m_hEventDownLoad);
						m_isDownOk = FALSE;
						m_strInfo.Format(_T("download error ret=%d, err=%d\r\n"),(stCommander.buf[0]<<8|stCommander.buf[1]),stCommander.buf[2]);
						UpdateData(FALSE);
					}
				break;

				case AM1_IAP_ERASE_DEVICE:
					if (stCommander.buf[1]!=0) 
					{
						if (0xee==stCommander.buf[0]) 
						{
							//							g_erase_flag = -1;							
						}

					}else
					{
						if (0x55==stCommander.buf[0]) 
						{
							//							g_erase_flag = 1;
						}else if (0xaa==stCommander.buf[0])
						{
							//							g_erase_flag = 2;
						}
					}
					break;

				case AM1_IAP_READ_64BYTE:
					//if ((FRead=fopen("Read.bin","ab+"))==NULL)
					//{
					//	AfxMessageBox(_T("open file error"));
						return;
					//}
					//fseek(FRead, 0L, SEEK_END);
					//if(fwrite(stCommander.cmd_buf,stCommander.data_len,1,FRead)!=1)
						//AfxMessageBox(_T("writer error"));		
					//fclose(FRead);
					break;

				case AM1_IAP_SECKEY_PROG:
					if (stCommander.buf[0]!=0)
					{
						//						g_secProtectErr = stCommander.cmd_buf[0];
						//						g_SecProtectRet = -1;
					}else
					{
						//						g_SecProtectRet = 1;
					}
					break;



				case AM1_IAP_SECTOR_PROTECT:						
					break;

				case AM1_IAP_JUMP_UPDATE:
					//						g_JumpRet = 1;
					break;

				case AM1_IAP_UARTSHUT:
					//						g_UartShutRet = 1;
					break;

				case AM1_IAP_READ_STATUS:
					break;

				case AM1_IAP_SECURITYCHECK:
					if (stCommander.buf[0]!=0)
					{
						//						g_VerifyRet = -1;
					}else
					{
						//						g_VerifyRet = 1;
					}
					break;

				}

			}
		}
	}
	return;
}

UINT8 CbootHostDlg::DownLoadProc()
{
	CStdioFile df;
	CFileException e;
	int times = 0;
	UINT16 dlCnt = 0;
	UINT32 fLen;
	UINT32 destIcAddr = 0;
	UINT8 loadBlock[2100];
	UINT8 sdCnt = 0;
	UINT32 i;
	if (!df.Open(m_strFileName,CFile::modeRead|CFile::typeBinary,&e))
	{
		m_strInfo = _T("Open file error\r\n");
//		UpdateData(FALSE);
		::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 0);
		return FALSE;
	}

	fLen = df.GetLength();

	Sleep(500);

	while (m_isThreadRun)
	{
		//times += UPDATE_BLOCK_SIZE;
		if (fLen-times>UPDATE_BLOCK_SIZE)
		{
			dlCnt = UPDATE_BLOCK_SIZE;
		}else
		{
			dlCnt = fLen - times;
		}

		//fread(loadBlock+5,sizeof(char),UPDATE_BLOCK_SIZE,cipher);		
		memset(loadBlock,0,UPDATE_BLOCK_SIZE+8);

		df.Read(loadBlock+7,dlCnt);

		loadBlock[0] = 0x7e;
		loadBlock[1] = 0x41;
		loadBlock[2] = ((destIcAddr>>16)&0xff);
		loadBlock[3] = ((destIcAddr>>8)&0xff);
		loadBlock[4] = (destIcAddr&0xff);
		loadBlock[5] = ((dlCnt>>8)&0xff);
		loadBlock[6] = (dlCnt&0xff);


		loadBlock[dlCnt+7] = 0;

		for (i=1;i<(dlCnt+7);i++) {			//the head should not be added
			loadBlock[dlCnt+7] += loadBlock[i];
		}

		sdCnt = 0;

		while ( TRUE )
		{
			m_uartPort.WriteToPort((char*)loadBlock,dlCnt+8);

			// 用于STM8下载，一个包128字节，等待1秒钟，等不到解密完成并写入flash
			if(WaitForSingleObject(m_hEventDownLoad,10000)!=WAIT_OBJECT_0)
			{
				if (FALSE == m_isThreadRun)
				{
					break;
				}
				sdCnt++;
				if( sdCnt > 20 )
				{
					ResetEvent(m_hEventDownLoad);
					//m_strInfo += _T("串口没有反应，请检查连接是否正常\r\n");
					m_strInfo += _T("The Uart has no reply, please check the link\r\n");
					::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 0);
					//UpdateData(FALSE);
					return FALSE;
				}else
				{
					continue;
				}
			}

			if (FALSE==m_isDownOk)
			{
				//m_strInfo += _T("下载错误，请重试\r\n");
				m_strInfo += _T("Update error, please try again\r\n");
				::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 0);
				ResetEvent(m_hEventDownLoad);
				return FALSE;
			}else
			{
				ResetEvent(m_hEventDownLoad);
				break;
			}

			//ResetEvent(m_hEventDownLoad);

		}

		
		destIcAddr += dlCnt;
		times += dlCnt;


		m_ctrlProg.SetPos(times*100/fLen);

		if (times>=fLen)
		{
			m_strInfo.Format(_T("%d / %d\r\n"),times,fLen);
			//m_strInfo+=_T("程序烧录完毕\r\n");
			m_strInfo+=_T("Firmware update successed\r\n");
			::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 0);
			return TRUE;
		}else
		{
			//CString strProg;
			//strProg.Format(_T("%d / %d"),times,fLen);
			//m_ctrlProg.SetWindowText(strProg);
			m_strInfo.Format(_T("%d / %d\r\n"),times,fLen);
			::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 1);
		}
		
	}

	if (FALSE == m_isThreadRun)
	{
		m_strInfo = _T("Download interruped\r\n");
		::SendMessage(this->m_hWnd, WM_DLG_MSG, (WPARAM) 0, (LPARAM) 1);
	}

	df.Close();
	

	return TRUE;
	
}

UINT CbootHostDlg::ThreadDownload(LPVOID lp)
{
	CbootHostDlg *dlg = (CbootHostDlg*)lp;
	return dlg->DownLoadProc();
}


void CbootHostDlg::OnBnClickedOk()
{
	
	// TODO: 在此添加控件通知处理程序代码;
	if (TRUE==m_isThreadRun)
	{
		m_isThreadRun = FALSE;
		//m_btnOK.SetWindowTextW(_T("下载代码"));
		m_btnOK.SetWindowTextW(_T("Download"));
	}else
	{
		if (m_isUartOpen==TRUE)
		{/*
			m_uartPort.ClosePort();
			if (m_uartPort.InitPort(this,m_comNumber,115200,'N',8,1,(EV_RXFLAG | EV_RXCHAR),512)) {
				m_uartPort.StartMonitoring();
				m_isUartOpen = TRUE;
				m_ctrlOpen.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_RUN));
			}else
			{
				m_strInfo = _T("串口无法打开\r\n");
				UpdateData(FALSE);
				return;
			}
			*/
			m_isThreadRun = TRUE;
			m_ctrlProg.SetPos(0);
			//m_btnOK.SetWindowTextW(_T("停止下载"));
			m_btnOK.SetWindowTextW(_T("Stop"));
			AfxBeginThread(ThreadDownload, this);
			m_strInfo = _T("Begin download\r\n");
			UpdateData(FALSE);
		}else
		{
			//m_strInfo = _T("串口没有打开\r\n");
			m_strInfo = _T("Uart is not open\r\n");
			UpdateData(FALSE);
		}

	}
	
}


LRESULT CbootHostDlg::OnUpdateInfo(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case 0:
		UpdateData(FALSE);
		//m_btnOK.SetWindowTextW(_T("下载代码"));
		m_btnOK.SetWindowTextW(_T("Download"));
		m_isThreadRun = FALSE;
		return NULL;
	break;

	case 1:
		UpdateData(FALSE);
	break;

	case 3:
		UpdateData(FALSE);
		//Sleep(1000);
		m_isThreadRun = TRUE;
		//m_ctrlProg.SetPos(0);
		//m_btnOK.SetWindowTextW(_T("停止下载"));
		AfxBeginThread(ThreadDownload, this);
	break;

	}
	
}


void CbootHostDlg::OnBnClickedClear()
{
	// TODO: 在此添加控件通知处理程序代码
	m_strInfo = _T("");
	m_strUarPrt = _T("");
	UpdateData(FALSE);
}


void CbootHostDlg::OnNMCustomdrawProgress1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void CbootHostDlg::OnBnClickedCancel()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	CDialogEx::OnCancel();
}
