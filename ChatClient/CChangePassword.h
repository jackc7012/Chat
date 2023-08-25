#pragma once
#include "CLoginWait.h"

// CChangePassword 对话框

class CChangePassword : public CDialogEx {
    DECLARE_DYNAMIC(CChangePassword)

public:
    CChangePassword(CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CChangePassword();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_CHANGEPASSWORD };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();

public:
    afx_msg void OnBnClickedChangePassword();

public:
    UINT64 customerId_{ 0 };
    SOCKET socketClient_{ INVALID_SOCKET };

private:
    void SetVerify();
    void SendChangePasswordMessage(s_HandleRecv& toSend);
};
