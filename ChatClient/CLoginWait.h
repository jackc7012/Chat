#pragma once

#include <thread>
#include "public.h"
// CLoginWait 对话框

class CLoginWait : public CDialogEx
{
	DECLARE_DYNAMIC(CLoginWait)

public:
	CLoginWait(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CLoginWait();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGIN_WAIT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	SOCKET socketClient{ 0 };
	void socketRecvThread();
	std::thread threadWait;
	int flag{ 0 };
};
