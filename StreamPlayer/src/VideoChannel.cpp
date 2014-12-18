// VideoChannel.cpp: implementation of the CVideoChannel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VideoChannel.h"
#include "PlayDemo.h"
#include "PlayDemoDlg.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CPlayDemoDlg *m_maindlg;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static void WINAPI s_messagecallback(LONG hHandle,int wParam,int lParam,void *context)
{
	CVideoChannel *pchannel = (CVideoChannel*)context;
	if(wParam != LAUMSG_LINKMSG)
	{
		pchannel->AlarmProcess(wParam,lParam);
		return;    //alarm message
	}
	PostMessage(m_maindlg->m_hWnd,VIDEOWND_MSG,DWORD(context),lParam);
}

/*Note:In video and audio callback function,you must return as soon as possible.
otherwise,you will lost frames or connection broken. */
/*
pbuff:       MPEG4 video data;have vol header,just send this data to decoder or save to disk.
headsize:    MPEG4 Video frame head length;
datasize:    MPEG4 video data length;
timetick:    MPEG4 time tick(ms)
biskeyframe: TRUE: key frame(I frame), FALSE : other
*/
static void WINAPI m_nomalvideo(char *pbuff,int headsize,int datasize,int timetick,int biskeyframe,void *context)
{
	int iRet;
	CVideoChannel *pParent = (CVideoChannel*)context;
	//注意: LC_PLAYM4_InputData发送数据时需要加上我们的数据头，此数据头是回调函数的pbuff前面headsize个字节
	//请直接将pbuff - headsize,size+headsize,送到LC_PLAYM4_InputData函数中.
    iRet = LC_PLAYM4_InputData(pParent->m_playhandle,(long*)(pbuff - headsize),headsize + datasize);
	if(iRet != 0)
	{
		TRACE("send video data error\n",iRet);
	}
}
/*
pbuff:       MP3 audio data;just send this data to decoder or save to disk.
headsize:    MP3 audio frame head length;
datasize:    MP3 audio data length;
timetick:    MP3 time tick(ms)
biskeyframe: Not used.
*/
static void WINAPI m_nomalaudio(char *pbuff,int headsize,int datasize,int timetick,int biskeyframe,void *context)
{
	int iRet;
	CVideoChannel *pParent = (CVideoChannel*)context;
	//注意: LC_PLAYM4_InputData发送数据时需要加上我们的数据头，此数据头是回调函数的pbuff前面headsize个字节
	//请直接将pbuff - headsize,size+headsize,送到LC_PLAYM4_InputData函数中.
	iRet = LC_PLAYM4_InputData(pParent->m_playhandle,(long*)(pbuff - headsize),headsize + datasize);
	if(iRet != 0)
	{
		TRACE("send audio data error\n",iRet);
	}
}
/*Decoder callback function*/
static long  CALLBACK s_DecodeCallBack(long hHandle,long frametype,long *frameinfo,long lParam)
{
    CVideoChannel *pchannel = (CVideoChannel*)lParam;
    if(frametype == FRAMETYPE_VIDEO)
    {
        VIDEO_FRAMEINFO *pframeinfo = (VIDEO_FRAMEINFO*)frameinfo;
        TRACE("video pchannel->handle:%d,handle:%d,timetick:%d,width:%d,height:%d,stridey:%d,strideuv:%d\n",
            pchannel->m_playhandle,hHandle,pframeinfo->timetick,pframeinfo->width,pframeinfo->height,
            pframeinfo->stridey,pframeinfo->strideuv);
    }
    else if(frametype == FRAMETYPE_AUDIO)
    {
        AUDIO_FRAMEINFO *pframeinfo = (AUDIO_FRAMEINFO*)frameinfo;
        TRACE("audio pchannel->handle:%d,handle:%d,timetick:%d,channel:%d,samplerate:%d,bits:%d,size:%d\n",
            pchannel->m_playhandle,hHandle,pframeinfo->timetick,pframeinfo->nChannels,
            pframeinfo->nSamplesPerSec,pframeinfo->wBitsPerSample,pframeinfo->size);
    }
    return 0;
}


CVideoChannel::CVideoChannel()
{
	m_hchann = -1;
	memset(&m_camitem,0,sizeof(CAM_DECITEM));

	m_timernum = 0;           //set to relink
	m_starttimer = FALSE;
	memset(&m_filehead,0,sizeof(ETI_FILE_HEAD));
	strcpy(m_filehead.m_filemask,ETI_FILEMASK);
	m_filehead.m_fileversion = 1;
	m_filehead.videofoucc = 0X58564944; //DIVX  0X34363248; // H264
	m_filehead.videoid = 0;
	m_filehead.m_width = 352;
	m_filehead.m_height = 288;
	m_filehead.m_audiotag = 0x55; //Mp3,0xFE;// G722
	m_filehead.m_haveaudio = 1;
	m_filehead.m_auchs = 1;
	m_filehead.m_aurates = 24;
	m_filehead.m_ausample = 16000;
    //设置流播放时，视频头信息  videofoucc : 0X34363248:H264(lc5000) ; 0X58564944 : DIVX(lc8000,lc9000,lc6000)
	//                          m_audiotag:  0xFE:G722(lc5000,lc6000); 0x55: mp3(lc8000,lc9000)
	//lc5000,lc6000的采样频率都为8000，不能修改
	//在视频服务器连接成功后可通过函数VSNET_ClientGetStreamInfo,获得这些参数
	m_bplay = FALSE;
}

CVideoChannel::~CVideoChannel()
{
	m_bplay = FALSE;
	StopPlay();
	if(m_camitem.m_playmode == 0)
	{
		LC_PLAYM4_CloseStream(m_playhandle);
	}
	else
	{
		LC_PLAYM4_CloseFile(m_playhandle);
	}
}

void CVideoChannel::LoadCamera(LPCSTR url,int channel,LPCSTR sername,WORD port,LPCSTR username,LPCSTR password,
							   int transtype,HWND videownd,char *filename,int playmode)
{
	m_bplay = FALSE;
	StopPlay();
	if(m_camitem.m_playmode == 0)
	{
		LC_PLAYM4_CloseStream(m_playhandle);
	}
	else
	{
		LC_PLAYM4_CloseFile(m_playhandle);
	}
	strcpy(m_camitem.m_sername,sername);
	strcpy(m_camitem.m_url,url);
	m_camitem.m_ch = channel;
	m_camitem.m_trantype = transtype;
	m_camitem.m_port = port;
	strcpy(m_camitem.m_username,username);
	strcpy(m_camitem.m_password,password);
	strcpy(m_camitem.m_filename,filename);
	m_camitem.m_playmode = playmode;
	m_camitem.m_videownd = videownd;
	
	int iRet;
	if(m_camitem.m_playmode == 0)
	{
		iRet = LC_PLAYM4_OpenStream(m_playhandle,(long*)&m_filehead,sizeof(ETI_FILE_HEAD));
	}
	else
	{
		iRet = LC_PLAYM4_OpenFile(m_playhandle,m_camitem.m_filename);
	}
	if(iRet != 0)
	{
		TRACE("Open File or stream error:%d\n",iRet);
	}
    LC_PLAYM4_SetDecodeCallBack(m_playhandle,s_DecodeCallBack,(long)this);
	LC_PLAYM4_SetFileEndMsgWnd(m_playhandle,(long)(m_maindlg->m_hWnd),PLAYEND_MSG);
}

int CVideoChannel::Play()
{
	int iRet;
	if(m_camitem.m_playmode == 0)
	{
		StartPlay();
	}
	else
	{
		iRet = LC_PLAYM4_Play(m_playhandle,(long)m_camitem.m_videownd);
		if(iRet != 0)
		{
			TRACE("LC_PLAYM4_Play File Error:%d\n",iRet);
		}
		m_bplay = TRUE;
	}
	return iRet;
}

int CVideoChannel::Stop()
{
	int iRet;
	m_bplay = FALSE;
	if(m_camitem.m_playmode == 0)
	{
		StopPlay();
	}
	else
	{
		iRet = LC_PLAYM4_Stop(m_playhandle);
		if(iRet != 0)
			TRACE("Stop File Error:%d\n",iRet);
	}
	return iRet;
}

int CVideoChannel::Fast()
{
	int iRet;
	iRet = LC_PLAYM4_Fast(m_playhandle);
	if(iRet != 0)
		TRACE("Fast Error:%d\n",iRet);
	return iRet;
}
int CVideoChannel::Slow()
{
	int iRet;
	iRet = LC_PLAYM4_Slow(m_playhandle);
	if(iRet != 0)
		TRACE("Slow Error:%d\n",iRet);
	return iRet;
}

int CVideoChannel::Pause(BOOL bstart)
{
	int iRet;
	iRet = LC_PLAYM4_Pause(m_playhandle,bstart);
	if(iRet != 0)
		TRACE("Pause Error:%d\n",iRet);
	return iRet;
}

int CVideoChannel::Refresh()
{
	int iRet;
	iRet = LC_PLAYM4_RefreshSurface(m_playhandle);
	if(iRet != 0)
		TRACE("Refresh Error:%d\n",iRet);
	return iRet;
}

int CVideoChannel::CapturePic(char *filename)
{
	int iRet;
	iRet = LC_PLAYM4_CapPic(m_playhandle,filename);
	if(iRet != 0)
		TRACE("CapturePic Error:%d\n",iRet);
	return iRet;
}

int CVideoChannel::PlaySound(BOOL bstart)
{
	int iRet;
	if(bstart)
	{
		iRet = LC_PLAYM4_PlaySound(m_playhandle);
	}
	else
	{
		iRet = LC_PLAYM4_StopSound(m_playhandle);
	}
	if(iRet != 0)
		TRACE("PlaySound Error:%d\n",iRet);
	return iRet;
}

int CVideoChannel::StartRecord(LPCSTR filename)
{
	int iRet;
	iRet = LC_PLAYM4_StartMp4Capture(m_playhandle,filename);
	if(iRet != 0)
		TRACE("StartRecord Error:%d\n",iRet);
	return iRet;
}

int CVideoChannel::StopRecord()
{
	int iRet;
	iRet = LC_PLAYM4_StopMp4Capture(m_playhandle);
	if(iRet != 0)
		TRACE("StopRecord Error:%d\n",iRet);
	return iRet;
}

int CVideoChannel::StartAsfRecord(LPCSTR filename)
{
	int iRet;
	iRet = LC_PLAYM4_StartASFFileCap(m_playhandle,filename);
	if(iRet != 0)
		TRACE("StartAsfRecord Error:%d\n",iRet);
	return iRet;
}

int CVideoChannel::StopAsfRecord()
{
	int iRet;
	iRet = LC_PLAYM4_StopCapture(m_playhandle);
	if(iRet != 0)
		TRACE("StopAsfRecord Error:%d\n",iRet);
	return iRet;
}

void CVideoChannel::Replay()
{
	int iRet;
	iRet = LC_PLAYM4_Stop(m_playhandle);
	if(iRet != 0)
	{
		TRACE("LC_PLAYM4_Stop Replay Error:%d\n",iRet);
	}
	iRet = LC_PLAYM4_Play(m_playhandle,(long)m_camitem.m_videownd);
	if(iRet != 0)
	{
		TRACE("LC_PLAYM4_Play Replay Error:%d\n",iRet);
	}
}

int CVideoChannel::StartPlay()
{
	if(m_hchann != -1)
	{
		VSNET_ClientStop(m_hchann);
		m_hchann = -1;
	}
	m_timernum = 0;           //set to relink
	m_starttimer = FALSE;


	CHANNEL_CLIENTINFO m_chinfo;

	m_chinfo.m_buffnum    = 50;
	m_chinfo.m_ch         = m_camitem.m_ch;
	m_chinfo.m_hChMsgWnd  = NULL;
	m_chinfo.m_hVideohWnd = NULL;
	m_chinfo.m_nChmsgid   = NULL;
	m_chinfo.m_password   = m_camitem.m_password;
	m_chinfo.m_playstart  = FALSE;
	m_chinfo.m_sername    = m_camitem.m_sername;
	m_chinfo.m_tranType   = m_camitem.m_trantype;
	m_chinfo.m_useoverlay = FALSE;
	m_chinfo.m_username   = m_camitem.m_username;
	m_chinfo.nColorKey    = RGB(255,0,255);
	m_chinfo.context      = this;
	m_chinfo.m_messagecallback = s_messagecallback;
	m_hchann = VSNET_ClientStart(m_camitem.m_url,&m_chinfo,m_camitem.m_port);
	if(m_hchann == -1)
	{
		return -1;
	}
	int iRet;
	iRet = LC_PLAYM4_Play(m_playhandle,(long)m_camitem.m_videownd);
	if(iRet != 0)
	{
		TRACE("LC_PLAYM4_Play Stream Error:%d\n",iRet);
	}
	m_bplay = TRUE;
	return 0;
}

void CVideoChannel::StopPlay()
{
	int iRet;
	m_bplay = FALSE;
	if(m_hchann != -1)
	{
		VSNET_ClientStop(m_hchann);
		m_hchann = -1;
	}
	m_timernum = 0;           //set to relink
	m_starttimer = FALSE;
	iRet = LC_PLAYM4_Stop(m_playhandle);
	if(iRet != 0)
	{
		TRACE("LC_PLAYM4_Stop Stream Error:%d\n",iRet);
	}
}

void CVideoChannel::RelinkTimer()
{
	m_timernum = 0; 
	m_starttimer = FALSE;
	StartPlay();
}

void CVideoChannel::AlarmProcess(int wParam,int lParam)
{
	if(wParam == LAUMSG_VIDEOMOTION)
	{
		//video move alarm
	}
	else if(wParam == LAUMSG_ALARM)
	{
		//sensor alarm
	}
	else if(wParam == LAUMSG_VIDEOLOST)
	{
		//video lost alarm
	}
	else if(wParam == LAUMSG_OUTPUTSTATUS)
	{
		//output status changed
	}
	else if(wParam == LAUMSG_CURSWITCHCHAN)
	{
		//channel switch
	}
}
void CVideoChannel::LinkProcess(int lParam)
{
	if(lParam == 0)
	{
		TRACE("Connect successful\n");
		VSNET_ClientStartNomalCap(m_hchann,m_nomalvideo,this,m_nomalaudio,this); //get raw data
	}
	else
	{
		TRACE("Connect failed\n");
		StopPlay();

		m_timernum = 0;           //set to relink
		m_starttimer = TRUE;
	}
}
