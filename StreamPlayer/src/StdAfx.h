// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__43610153_4EDE_435F_A027_E2FECB25B11E__INCLUDED_)
#define AFX_STDAFX_H__43610153_4EDE_435F_A027_E2FECB25B11E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "../lib/launet.h"
#include "../lib/megaplay.h"

#define VIDEOWND_MSG (WM_USER + 300)
#define PLAYEND_MSG  (WM_USER + 301)
#define VIEWNUM      4

typedef struct
{
	char m_sername[24];
	char m_url[40];
	char m_ch;
	char m_trantype;
	WORD m_port;
	char m_username[20];
	char m_password[20];
	char m_filename[MAX_PATH]; 
	int  m_playmode;
	HWND m_videownd;
}CAM_DECITEM;

#define ETI_FILE_MASKEND	0XABCEDF01
#define ETI_FILEMASK		"LAUNCHDIGITAL"
typedef struct
{
	char m_filemask[16];
	int  m_fileversion;
	int  videofoucc;
	int  videoid;
	short m_width;
	short m_height;
	short m_audiotag;
	short m_haveaudio;
	short m_auchs;
	short m_aurates;
	int   m_ausample;
	unsigned int m_totalframes;
	unsigned int m_firsttick;
	unsigned int m_losttick;
	int maskend;
}ETI_FILE_HEAD;

#define ETI_FRAME_KEY_MASK			0X0020		//0X0100000B

typedef struct
{
	unsigned int   nID;			 //0XB2010000
	unsigned int   nTimeTick;    //时间戳
	int			   nVideoSize;   //视频数据长度
	unsigned short nAudioSize;   //音频数据长度
	unsigned short nMuxData;	 //复合数据：视频通道、帧类型、用户数据
}ETI_FRMAME_HEAD,*PETI_FRMAME_HEAD;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__43610153_4EDE_435F_A027_E2FECB25B11E__INCLUDED_)
