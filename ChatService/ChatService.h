
// ChatService.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CChatServiceApp: 
// �йش����ʵ�֣������ ChatService.cpp
//

class CChatServiceApp : public CWinApp
{
public:
	CChatServiceApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CChatServiceApp theApp;
