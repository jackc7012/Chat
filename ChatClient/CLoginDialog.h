#pragma once

#include <thread>
#include "ChatClientDlg.h"
#include "CRegisterDialog.h"
#include "CNetWorkHandle.h"
#include "CLoginWait.h"
#include "CLog.h"


// CLoginDialog 对话框

const int GET_TOKEN_TIMEOUT = 3000;

class CLoginDialog : public CDialogEx
{
    DECLARE_DYNAMIC(CLoginDialog)

public:
    CLoginDialog(CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CLoginDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_LOGIN_DIALOG };
#endif

protected:
    HICON m_hIcon;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedLogin();
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedRegister();
    SOCKET socketClient;
    SOCKADDR_IN addr;
};
