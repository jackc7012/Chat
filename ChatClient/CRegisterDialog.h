﻿#pragma once
#include "CLoginWait.h"

// CRegisterDialog 对话框

class CRegisterDialog : public CDialogEx {
    DECLARE_DYNAMIC(CRegisterDialog)

public:
    CRegisterDialog(CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CRegisterDialog();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_REGISTER_DIALOG };
#endif

protected:
    HICON m_hIcon;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual void OnOK();

public:
    afx_msg void OnBnClickedRegisterUser();

private:
    void SetVerify();
    void SendRegisterMessage(s_HandleRecv& toSend);

public:
    SOCKET socketClient_{ 0 };
};
