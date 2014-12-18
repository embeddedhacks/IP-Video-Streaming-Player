// PlayDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PlayDemo.h"
#include "PlayDemoDlg.h"
#include "VideoChannel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define RELINK_TIMER 1

CPlayDemoDlg *m_maindlg = NULL;  //main wnd pinter

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
// CPlayDemoDlg dialog

CPlayDemoDlg::CPlayDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPlayDemoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPlayDemoDlg)
	m_channel = 0;
	m_transtype = 2;
	m_ipaddr = _T("192.168.1.155");
	m_password = _T("888888");
	m_port = 3000;
	m_username = _T("888888");
	m_filename = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_curch = 0;
	int i;
	for(i=0;i<VIEWNUM;i++)
		m_video[i] = NULL;
	m_relinktimer = 0;
	m_maindlg = this;
	m_bsliderpos = FALSE;
}

void CPlayDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlayDemoDlg)
	DDX_Control(pDX, IDC_STATIC_VIDEO1, m_videownd1);
	DDX_Control(pDX, IDC_STATIC_VIDEO2, m_videownd2);
	DDX_Control(pDX, IDC_STATIC_VIDEO3, m_videownd3);
	DDX_Control(pDX, IDC_STATIC_VIDEO4, m_videownd4);
	DDX_CBIndex(pDX, IDC_COMBO_CHANNEL, m_channel);
	DDX_CBIndex(pDX, IDC_COMBO_TRANSTYPE, m_transtype);
	DDX_Text(pDX, IDC_EDIT_IP, m_ipaddr);
	DDV_MaxChars(pDX, m_ipaddr, 24);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_password);
	DDV_MaxChars(pDX, m_password, 20);
	DDX_Text(pDX, IDC_EDIT_PORT, m_port);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_username);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_filename);
	DDV_MaxChars(pDX, m_filename, 255);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPlayDemoDlg, CDialog)
	//{{AFX_MSG_MAP(CPlayDemoDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_MESSAGE(VIDEOWND_MSG,OnVideoWndMsg)
	ON_MESSAGE(PLAYEND_MSG,OnPlayEndMsg)
	ON_BN_CLICKED(IDC_BTN_CAPTURE, OnBtnCapture)
	ON_BN_CLICKED(IDC_BTN_FAST, OnBtnFast)
	ON_BN_CLICKED(IDC_BTN_FILE, OnBtnFile)
	ON_BN_CLICKED(IDC_BTN_LOAD, OnBtnLoad)
	ON_BN_CLICKED(IDC_BTN_PLAY, OnBtnPlay)
	ON_BN_CLICKED(IDC_BTN_REFRESH, OnBtnRefresh)
	ON_BN_CLICKED(IDC_BTN_SLOW, OnBtnSlow)
	ON_BN_CLICKED(IDC_BTN_STOP, OnBtnStop)
	ON_BN_CLICKED(IDC_CHECK_PLAYMODE, OnCheckPlaymode)
	ON_BN_CLICKED(IDC_CHECK_AUTO, OnCheckAuto)
	ON_BN_CLICKED(IDC_CHECK_PAUSE, OnCheckPause)
	ON_WM_HSCROLL()
	ON_WM_TIMER()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_BTN_STARTRECORD, OnBtnStartrecord)
	ON_BN_CLICKED(IDC_BTN_STOPRECORD, OnBtnStoprecord)
	ON_BN_CLICKED(IDC_BTN_STARTASFRECORD, OnBtnStartasfrecord)
	ON_BN_CLICKED(IDC_BTN_STOPASFRECORD, OnBtnStopasfrecord)
	//}}AFX_MSG_MAP
	ON_STN_CLICKED(IDC_STATIC_VIDEO1, OnStnClickedStaticVideo1)
	ON_STN_CLICKED(IDC_STATIC_VIDEO2, OnStnClickedStaticVideo2)
	ON_STN_CLICKED(IDC_STATIC_VIDEO3, OnStnClickedStaticVideo3)
	ON_STN_CLICKED(IDC_STATIC_VIDEO4, OnStnClickedStaticVideo4)
	ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlayDemoDlg message handlers

BOOL CPlayDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

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
	
	LC_PLAYM4_Initial((long)m_hWnd);
	VSNET_ClientStartup(NULL,NULL); //call this function to initialize SDK;

	int i;
	for(i=0;i<VIEWNUM;i++)
	{
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER_TIMER1 + i))->SetLineSize(1);
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER_TIMER1 + i))->SetPageSize(1);
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER_TIMER1 + i))->SetRange(0,100);
	}
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_AUDIO))->SetLineSize(100);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_AUDIO))->SetPageSize(100);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_AUDIO))->SetRange(0,65535);

	for(i=0;i<VIEWNUM;i++)
	{
		m_video[i] = new CVideoChannel();
		m_video[i]->m_playhandle = i;
	}
	m_relinktimer = SetTimer(RELINK_TIMER,1000,NULL);
	OnCheckPlaymode();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPlayDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CPlayDemoDlg::OnPaint() 
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
HCURSOR CPlayDemoDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

LRESULT CPlayDemoDlg::OnVideoWndMsg(WPARAM wParam, LPARAM lParam)
{
	int i;
	for(i=0;i<VIEWNUM;i++)
	{
		if(m_video[i] == (CVideoChannel*)wParam)
		{
			m_video[i]->LinkProcess(lParam);
		}
	}	
	return 0;
}

LRESULT CPlayDemoDlg::OnPlayEndMsg(WPARAM wParam, LPARAM lParam)
{
	if(wParam >=0 && wParam < VIEWNUM)
	{
		m_video[wParam]->Replay();
	}
	return 0;
}

void CPlayDemoDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	int i;
	for(i=0;i<VIEWNUM;i++)
	{
		if(m_video[i]) delete m_video[i];
	}

	LC_PLAYM4_Free();
	VSNET_ClientCleanup(); //call this function to destroy SDK
	if(m_relinktimer) KillTimer(m_relinktimer); m_relinktimer = 0;
}

void CPlayDemoDlg::OnStnClickedStaticVideo1()
{
	m_curch = 0;
}

void CPlayDemoDlg::OnStnClickedStaticVideo2()
{
	m_curch = 1;
}

void CPlayDemoDlg::OnStnClickedStaticVideo3()
{
	m_curch = 2;
}

void CPlayDemoDlg::OnStnClickedStaticVideo4()
{
	m_curch = 3;
}

void CPlayDemoDlg::OnTimer(UINT nIDEvent)
{
	int i;
	if(nIDEvent == RELINK_TIMER)
	{
		for(i=0;i<VIEWNUM;i++)
		{
			if(m_video[i]) 
			{
				if(m_video[i]->m_starttimer)
				{
					if(++m_video[i]->m_timernum > 10)
					{
						m_video[i]->RelinkTimer();
					}
				}
			}
		}
	}
	if(!m_bsliderpos)
	{
		int iRet;
		long pos;
		for(i=0;i<VIEWNUM;i++)
		{
			if(m_video[i]->m_bplay && m_video[i]->m_camitem.m_playmode == 1)
			{
				iRet = LC_PLAYM4_GetPlayPos(m_video[i]->m_playhandle,&pos);
				if(iRet != 0)
				{
					TRACE("LC_PLAYM4_GetPlayPos %d error:%d\n",i,iRet);
					((CSliderCtrl*)GetDlgItem(IDC_SLIDER_TIMER1 + i))->SetPos(0);
				}
				else
					((CSliderCtrl*)GetDlgItem(IDC_SLIDER_TIMER1 + i))->SetPos(pos);
			}
			else
			{
				((CSliderCtrl*)GetDlgItem(IDC_SLIDER_TIMER1 + i))->SetPos(0);
			}
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void CPlayDemoDlg::OnBtnCapture() 
{
	m_video[m_curch]->CapturePic("C:\\test.bmp");
}

void CPlayDemoDlg::OnBtnFast() 
{
	m_video[m_curch]->Fast();	
}

void CPlayDemoDlg::OnBtnFile() 
{
	CFileDialog dlgFile(
		TRUE, 
		_T("open Mp4 File"),
		"",
		OFN_HIDEREADONLY,
		"mp4 Files(*.mp4)|*.mp4||",
		NULL);
	char lpszTitle[]={""};
	dlgFile.m_ofn.lpstrTitle = lpszTitle;
	
	if (dlgFile.DoModal() != IDOK)
	{
		return;
	}
	CString lpfile = dlgFile.GetPathName();
	GetDlgItem(IDC_EDIT_FILENAME)->SetWindowText(lpfile);	
}

void CPlayDemoDlg::OnBtnLoad() 
{
	char url[30],password[30],username[30],sername[30];
	HWND videownd; 
	int  mode = ((CButton*)GetDlgItem(IDC_CHECK_PLAYMODE))->GetCheck();
	if(!UpdateData(TRUE)) return;
	sprintf(password,"%s",m_password);
	sprintf(username,"%s",m_username);
	sprintf(sername,"%s","video server");
	sprintf(url,"%s",m_ipaddr);

	if(m_curch == 0)      videownd =  m_videownd1.m_hWnd;
	else if(m_curch == 1) videownd =  m_videownd2.m_hWnd;
	else if(m_curch == 2) videownd =  m_videownd3.m_hWnd;
	else                  videownd =  m_videownd4.m_hWnd;

	m_video[m_curch]->LoadCamera(url,m_channel,sername,m_port,username,password,
		m_transtype+1,videownd,(char*)(LPCSTR)m_filename,mode);
}

void CPlayDemoDlg::OnBtnPlay() 
{
	m_video[m_curch]->Play();	
}

void CPlayDemoDlg::OnBtnRefresh() 
{
	m_video[m_curch]->Refresh();	
}

void CPlayDemoDlg::OnBtnSlow() 
{
	m_video[m_curch]->Slow();	
}

void CPlayDemoDlg::OnBtnStop() 
{
	m_video[m_curch]->Stop();	
}

void CPlayDemoDlg::OnCheckPlaymode() 
{
	int state = ((CButton*)GetDlgItem(IDC_CHECK_PLAYMODE))->GetCheck();
	GetDlgItem(IDC_EDIT_IP)->EnableWindow(!state);
	GetDlgItem(IDC_EDIT_PORT)->EnableWindow(!state);
	GetDlgItem(IDC_EDIT_USERNAME)->EnableWindow(!state);
	GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(!state);
	GetDlgItem(IDC_COMBO_CHANNEL)->EnableWindow(!state);
	GetDlgItem(IDC_COMBO_TRANSTYPE)->EnableWindow(!state);
	GetDlgItem(IDC_EDIT_FILENAME)->EnableWindow(state);
	GetDlgItem(IDC_BTN_FILE)->EnableWindow(state);
}

void CPlayDemoDlg::OnCheckAuto() 
{
	m_video[m_curch]->PlaySound(((CButton*)GetDlgItem(IDC_CHECK_AUTO))->GetCheck());
}

void CPlayDemoDlg::OnCheckPause() 
{
	m_video[m_curch]->Pause(((CButton*)GetDlgItem(IDC_CHECK_PAUSE))->GetCheck());
}

void CPlayDemoDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int iRet;
	if(((CScrollBar*)GetDlgItem(IDC_SLIDER_AUDIO)) == pScrollBar)
	{
		if(nSBCode == SB_ENDSCROLL)
		{
			iRet = LC_PLAYM4_SetVolume(m_video[m_curch]->m_playhandle,
				((CSliderCtrl*)GetDlgItem(IDC_SLIDER_AUDIO))->GetPos());
			if(iRet != 0)
			{
				TRACE("LC_PLAYM4_SetVolume error:%d\n",iRet);
			}
		}
	}	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CPlayDemoDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int i,iRet;
	for(i=0;i<VIEWNUM;i++)
	{
		if(((CScrollBar*)GetDlgItem(IDC_SLIDER_TIMER1 + i)) == pScrollBar)
		{
			if(nSBCode == SB_THUMBTRACK || nSBCode == SB_THUMBPOSITION)
			{
				m_bsliderpos = TRUE;
			}
			else if(nSBCode == SB_ENDSCROLL)
			{
				iRet = LC_PLAYM4_SetPlayPos(m_video[i]->m_playhandle,((CSliderCtrl*)GetDlgItem(IDC_SLIDER_TIMER1 + i))->GetPos());
				m_bsliderpos = FALSE;
				if(iRet != 0)
				{
					TRACE("LC_PLAYM4_SetPlayPos Error:%d\n",iRet);
				}
			}
		}	
	}	
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CPlayDemoDlg::OnBtnStartrecord() 
{
	char lpstr[MAX_PATH];
	sprintf(lpstr,"c:\\test%d.mp4",m_curch);
	m_video[m_curch]->StartRecord(lpstr);
}

void CPlayDemoDlg::OnBtnStoprecord() 
{
	m_video[m_curch]->StopRecord();
}

void CPlayDemoDlg::OnBtnStartasfrecord() 
{
	char lpstr[MAX_PATH];
	sprintf(lpstr,"c:\\test%d.asf",m_curch);
	m_video[m_curch]->StartAsfRecord(lpstr);	
}

void CPlayDemoDlg::OnBtnStopasfrecord() 
{
	m_video[m_curch]->StopAsfRecord();	
}
