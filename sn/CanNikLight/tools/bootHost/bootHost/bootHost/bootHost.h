
// bootHost.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CbootHostApp:
// �йش����ʵ�֣������ bootHost.cpp
//

class CbootHostApp : public CWinApp
{
public:
	CbootHostApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CbootHostApp theApp;