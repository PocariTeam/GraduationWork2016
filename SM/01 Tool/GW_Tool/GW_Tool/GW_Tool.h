
// GW_Tool.h : main header file for the GW_Tool application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CGW_ToolApp:
// See GW_Tool.cpp for the implementation of this class
//

class CGW_ToolApp : public CWinApp
{
public:
	CGW_ToolApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CGW_ToolApp theApp;
