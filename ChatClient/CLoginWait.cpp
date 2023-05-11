// CLoginWait.cpp: 实现文件
//

#include "stdafx.h"
#include "ChatClient.h"
#include "CLoginWait.h"
#include "afxdialogex.h"

// CLoginWait 对话框

IMPLEMENT_DYNAMIC(CLoginWait, CDialogEx)

CLoginWait::CLoginWait(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_LOGIN_WAIT, pParent)
{

}

CLoginWait::~CLoginWait()
{
}

void CLoginWait::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLoginWait, CDialogEx)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CLoginWait 消息处理程序


BOOL CLoginWait::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  在此添加额外的初始化
    CFont font;
    font.CreatePointFont(150, _T("宋体"), NULL);
    GetDlgItem(IDC_LOGIN_TIME_OUT)->SetFont(&font);
    GetDlgItem(IDC_LOGIN)->SetFont(&font);

    if (mode_ == 1)
    {
        SetDlgItemText(IDC_LOGIN, _T("申请账号中..请稍后...."));
    }
    else if (mode_ == 2)
    {
        char temp[20];
        memset(temp, 0, 20);
        sprintf_s(temp, 20, "%s 账号改密中..请稍后....", customerName_.c_str());
        SetDlgItemText(IDC_LOGIN, temp);
    }
    threadWait_ = std::thread(&CLoginWait::socketRecvThread, this);
    SetDlgItemInt(IDC_LOGIN_TIME_OUT, 30);
    SetTimer(1, 1000, nullptr);
    SetTimer(2, 50, nullptr);

    return TRUE;
}

void CLoginWait::OnTimer(UINT_PTR nIDEvent)
{
    switch (nIDEvent)
    {
    case 1: {
        int time = GetDlgItemInt(IDC_LOGIN_TIME_OUT);
        if (time > 0)
        {
            --time;
            SetDlgItemInt(IDC_LOGIN_TIME_OUT, time);
        }
    }
          break;

    case 2: {
        if (flag_ == LoginResult::SUCCEED || flag_ == LoginResult::NOUSER || flag_ == LoginResult::ALREADYLOGININ
            || flag_ == LoginResult::PASSWORDERROR || flag_ == LoginResult::OTHER || flag_ == LoginResult::UNKNOWNERROR)
        {
            if (threadWait_.joinable())
            {
                threadWait_.join();
            }
            EndDialog(static_cast<int>(flag_));
        }
    }
          break;
    default:
        break;
    }

    CDialogEx::OnTimer(nIDEvent);
}

void CLoginWait::socketRecvThread()
{
    char* buf = new char[DATA_LENGTH];
    memset(buf, 0, DATA_LENGTH);
    int nNetTimeout = 30000;
    ::setsockopt(socketClient_, SOL_SOCKET, SO_RCVTIMEO, (char*)&nNetTimeout, sizeof(int));
    ::recv(socketClient_, buf, DATA_LENGTH, 0);
    if (buf[0] != '\0')
    {
        s_HandleRecv rt;
        if (!DecodeJson(buf, rt))
        {
            delete[]buf;
            buf = nullptr;
            flag_ = LoginResult::UNKNOWNERROR;
            return;
        }
        if (mode_ == 0)
        {
            if (rt.type_ == CommunicationType::LOGINBACKSUCCEED)
            {
                customerName_ = rt.Param.loginBack_.customer;
                flag_ = LoginResult::SUCCEED;
            }
            else if (strcmp(rt.Param.loginBack_.description, "there is no such user") == 0)
            {
                flag_ = LoginResult::NOUSER;
            }
            else if (strcmp(rt.Param.loginBack_.description,
                "this user has already login in, please make sure or modify your password") == 0)
            {
                customerName_ = rt.Param.loginBack_.customer;
                ip_ = rt.Param.loginBack_.login_result;
                flag_ = LoginResult::ALREADYLOGININ;
            }
            else if (strcmp(rt.Param.loginBack_.description, "password error") == 0)
            {
                flag_ = LoginResult::PASSWORDERROR;
            }
        }
        else if (mode_ == 1)
        {
            if (rt.type_ == CommunicationType::REGISTERBACKSUCCEED)
            {
                customerName_ = std::to_string(rt.Param.registerBack_.id);
                flag_ = LoginResult::SUCCEED;
            }
            else
            {
                flag_ = LoginResult::OTHER;
            }
        }
        else if (mode_ == 2)
        {
            if (rt.type_ == CommunicationType::CHANGEPASSWORDBACK)
            {
                flag_ = ((rt.Param.changePasswordBack_.update_result == "succeed") ? LoginResult::SUCCEED : LoginResult::OTHER);
            }
        }
        UnregisterSpace(rt.type_, rt);
    }
    else
    {
        flag_ = LoginResult::OUTLIMIT;
    }
    delete[]buf;
    buf = nullptr;
}
