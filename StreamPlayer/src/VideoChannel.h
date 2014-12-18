// VideoChannel.h: interface for the CVideoChannel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIDEOCHANNEL_H__D1736789_C3E0_4E20_88A2_304B89946F22__INCLUDED_)
#define AFX_VIDEOCHANNEL_H__D1736789_C3E0_4E20_88A2_304B89946F22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVideoChannel  
{
public:
	CVideoChannel();
	virtual ~CVideoChannel();
public:
	int         m_playhandle;  //播放句柄
	LONG        m_hchann;      //connect handle
	BOOL        m_starttimer;  //relink timer start
	int         m_timernum;    //relink timer count
	CAM_DECITEM m_camitem;     //link paramerters 
	ETI_FILE_HEAD m_filehead;  //流播放文件头
	FILE       *m_fpFile;      
	HANDLE      m_filemutex;
	BOOL        m_bplay;
public:
	void        LoadCamera(LPCSTR url,int channel,LPCSTR sername,WORD port,LPCSTR username,LPCSTR password,
							   int transtype,HWND videownd,char *filename,int playmode);
	int         StartPlay();
	void        StopPlay();
	void        AlarmProcess(int wParam,int lParam);
	void        RelinkTimer();
	void        LinkProcess(int lParam);

	void        Replay();
	int         PlaySound(BOOL bstart);
	int         CapturePic(char *filename);
	int         Slow();
	int         Fast();
	int         Stop();
	int         Pause(BOOL bstart);
	int         Play();
	int         Refresh();
	int         StartRecord(LPCSTR filename);
	int         StopRecord();
    int         StopAsfRecord();
    int         StartAsfRecord(LPCSTR filename);
};

#endif // !defined(AFX_VIDEOCHANNEL_H__D1736789_C3E0_4E20_88A2_304B89946F22__INCLUDED_)
