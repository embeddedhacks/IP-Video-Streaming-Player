//megaplay.h
#ifndef __MEGAPLAY_H__
#define __MEGAPLAY_H__

#define ERROR_FILENOTFOUND   1L
#define ERROR_FILESIZE       2L
#define ERROR_FILEFORMAT     3L
#define ERROR_FILEVERSION    4L
#define ERROR_STARTPLAY      5L
#define ERROR_PLAYNOTINIT    6L
#define ERROR_NOTREALIZE     7L
#define ERROR_CHANNELERROR   8L
#define ERROR_OTHER          9L
#define ERROR_INVALIDPARAM   10L

#define MAX_DISPLAY_RECT_NUM 4      //max display rect num
#define MAX_DELAYFRAMENUM    500    //max delay frame num
#define MAX_BUFFFRAMENUM     500    //max buff frame num
#define MIN_BUFFFRAMENUM     10     //min buff frame num

enum
{
	PARAM_DELAYTIME     = 0,        //get/set delay frame num
	PARAM_BUFFNUM       = 1,        //get/set buff frame num
	PARAM_CUR_BUFFNUM   = 2,        //get current buff frame num
};

typedef struct
{
    unsigned int    timetick;       //timetick (ms)
    unsigned int    nChannels;      //channels
    unsigned int    nSamplesPerSec; //samples
    unsigned int    wBitsPerSample; //bitspersample
    const char      *pbuff;         //audio data buff
    int             size;           //audio data size
}AUDIO_FRAMEINFO;

typedef struct
{
    unsigned int    timetick;       //timetick (ms)
    const char      *pY;            //YV12,Y buff
    const char      *pU;            //U buff
    const char      *pV;            //V buff
    int             stridey;        //Y stride
    int             strideuv;       //UV stride
    int             width;          //image width
    int             height;         //image height
}VIDEO_FRAMEINFO;

#define FRAMETYPE_VIDEO     1
#define FRAMETYPE_AUDIO     2

#define T_AUDIO16	101
#define T_AUDIO8	100
#define T_UYVY		1
#define T_YV12		3
#define T_RGB32		7

typedef struct
{
	long lWidth; //width if video, zero if audio
	long lHeight; //height if video, zero if audio
	long lStamp; //stamp (ms)
	long lType; //datatype:T_AUDIO16,T_RGB32,T_YV12
	long lFrameRate;//framerate
}FRAME_INFO;

#define CPLAY_APIIMPORT  extern "C"__declspec(dllexport)
typedef long (__stdcall *MessageCallBack)(long lType,long lPara1,long lPara2);

CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_Initial(long hWnd);
CPLAY_APIIMPORT	HRESULT __stdcall LC_PLAYM4_Free();
CPLAY_APIIMPORT	HRESULT __stdcall LC_PLAYM4_OpenFile(long hHandle,LPCTSTR sFileName);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_CloseFile(long hHandle);
CPLAY_APIIMPORT	HRESULT __stdcall LC_PLAYM4_OpenStream(long hHandle,long *pFileHeadBuf,long lSize);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_CloseStream(long hHandle);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_InputData(long hHandle, long *pBuf, long lSize);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_PlaySound(long hHandle);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_StopSound(long hHandle);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_SetVolume(long hHandle,long lVolume);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_Play(long hHandle,long hWnd);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_Stop(long hHandle);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_Pause(long hHandle,long lPause);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_RefreshSurface(long hHandle);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_SetPlayPos(long hHandle,long lPos);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_Fast(long hHandle);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_Slow(long hHandle);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_CapPic(long hHandle,LPCTSTR sFileName);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_GetPlayPos(long hHandle,long *pPos);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_GetVersion(long *lVer);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_QueryFunction(LPCTSTR sFunctionName,long *lCanUse);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_SetParameter(long hHandle,long lType,long lPara1,long lPara2);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_GetParameter(long hHandle,long lType,long *pPara1,long *pPara2);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_SetCallBack(long hHandle,long (__stdcall *CallBack)(long lType,long lPara1,long lPara2));
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_SetFileEndMsgWnd(long hHandle,long hWnd,long lMsg);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_StartASFFileCap(long hHandle,LPCTSTR sFileName,BOOL index = FALSE);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_StopCapture(long hHandle);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_StartMp4Capture(long hHandle, LPCTSTR sFileName);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_StopMp4Capture(long hHandle);
CPLAY_APIIMPORT DWORD   __stdcall LC_PLAYM4_GetFileTime(long hHandle);
CPLAY_APIIMPORT DWORD   __stdcall LC_PLAYM4_GetPlayedTime(long hHandle);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_SetPlayedTime(long hHandle,DWORD nTime);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_SetDecodeCallBack(long hHandle,long( CALLBACK *DecodeCallBack)(long hHandle,long frametype,long *frameinfo,long lParam ),long lParam);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_SetDisplayRect(long hHandle,int num,RECT *rect,HWND hWnd,BOOL bEnable);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_GetVideoSize(long hHandle,DWORD *Width,DWORD *Height);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_OneByOne(long hHandle);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_GetFrameRate(long hHandle, long* lFrameRate);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_GetBitRate(long hHandle, long* lBitRate);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_GetPlayedFrames(long hHandle,long *lCount);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_SetDecCBStream(long hHandle,long lStream);
CPLAY_APIIMPORT HRESULT __stdcall LC_PLAYM4_SetDecCallBack(long hHandle,void(CALLBACK* DecodeCallBack)(long lHandle,char* pBuf,long lSize,FRAME_INFO * pFrameInfo, void* pUserData, long lReserved), void* pUserData);
#endif
