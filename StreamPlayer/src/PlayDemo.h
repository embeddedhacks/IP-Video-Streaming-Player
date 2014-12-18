// PlayDemo.h : main header file for the PLAYDEMO application
//

#if !defined(AFX_PLAYDEMO_H__EE4CC9F0_314A_45CC_9809_852E94762E34__INCLUDED_)
#define AFX_PLAYDEMO_H__EE4CC9F0_314A_45CC_9809_852E94762E34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPlayDemoApp:
// See PlayDemo.cpp for the implementation of this class
//

class CPlayDemoApp : public CWinApp
{
public:
	CPlayDemoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlayDemoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPlayDemoApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLAYDEMO_H__EE4CC9F0_314A_45CC_9809_852E94762E34__INCLUDED_)
