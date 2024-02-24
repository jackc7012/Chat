#pragma once

#include <thread>
#include "protocol.h"

enum class Mode
{
    NULLMODE,
    LOGIN,
    REGISTER,
    CHANGEPWD
};

enum class LoginResult
{
    NULLLOGIN,
    SUCCEED,
    NOUSER,
    ALREADYLOGININ,
    PASSWORDERROR,
    UNKNOWNERROR,
    PASSWORD2SHORT,
    OUTLIMIT
};

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

public:
    afx_msg void OnTimer(UINT_PTR nIDEvent);

public:
    SOCKET socketClient_{ INVALID_SOCKET };
    Mode mode_{ Mode::NULLMODE };
    std::string customerName_;
    std::string id_;
    std::string ip_;

private:
    void socketRecvThread();

private:
    std::thread threadWait_;
    LoginResult flag_{ LoginResult::NULLLOGIN };
};
