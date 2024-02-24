// CLoginWait.cpp: 实现文件
//

#include "ChatClient.h"
#include "CLoginWait.h"

using namespace cwy;

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

    CFont font;
    font.CreatePointFont(150, _T("宋体"), NULL);
    GetDlgItem(IDC_LOGIN_TIME_OUT)->SetFont(&font);
    GetDlgItem(IDC_LOGIN)->SetFont(&font);

    if (mode_ == Mode::LOGIN)
    {
        SetDlgItemText(IDC_LOGIN, _T("登录中..请稍后...."));
    }
    else if (mode_ == Mode::REGISTER)
    {
        SetDlgItemText(IDC_LOGIN, _T("申请账号中..请稍后...."));
    }
    else if (mode_ == Mode::CHANGEPWD)
    {
        char temp[50];
        memset(temp, 0, 50);
        sprintf_s(temp, 50, "%s 账号改密中..请稍后....", customerName_.c_str());
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
        case 1:
        {
            int time = GetDlgItemInt(IDC_LOGIN_TIME_OUT);
            if (time > 0)
            {
                --time;
                SetDlgItemInt(IDC_LOGIN_TIME_OUT, time);
            }
            break;
        }

        case 2:
        {
            if (flag_ > LoginResult::NULLLOGIN && flag_ <= LoginResult::OUTLIMIT)
            {
                if (threadWait_.joinable())
                {
                    threadWait_.join();
                }
                EndDialog(static_cast<int>(flag_));
            }
            break;
        }

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
        HandleRecv rt(socketClient_, buf);
        CommunicationType ct = rt.GetType();
        switch (mode_)
        {
            case Mode::LOGIN: // login
            {
                if (rt.GetContent("id") != id_)
                {
                    // 不处理
                }
                else if (ct == CommunicationType::LOGINBACKSUCCEED)
                {
                    customerName_ = rt.GetContent("customer");
                    flag_ = LoginResult::SUCCEED;
                }
                else if (rt.GetContent("description") == "there is no such user")
                {
                    flag_ = LoginResult::NOUSER;
                }
                else if (rt.GetContent("description") == "this user has already login in, please make sure or modify your password")
                {
                    ip_ = rt.GetContent("ip");
                    customerName_ = rt.GetContent("customer");
                    flag_ = LoginResult::ALREADYLOGININ;
                }
                else if (rt.GetContent("description") == "password error")
                {
                    flag_ = LoginResult::PASSWORDERROR;
                }
                else if (rt.GetContent("description") == "unknown error")
                {
                    flag_ = LoginResult::UNKNOWNERROR;
                }
                break;
            }

            case Mode::REGISTER: // register
            {
                if (rt.GetContent("customer") != customerName_)
                {
                    // 不处理
                }
                else if (ct == CommunicationType::REGISTERBACKSUCCEED)
                {
                    id_ = rt.GetContent("id");
                    flag_ = LoginResult::SUCCEED;
                }
                else if (rt.GetContent("description") == "password too short")
                {
                    flag_ = LoginResult::PASSWORD2SHORT;
                }
                break;
            }

            case Mode::CHANGEPWD: // change password
            {
                if (rt.GetContent("id") != id_)
                {
                    // 不处理
                }
                else if (rt.GetContent("update_result") == "succeed")
                {
                    flag_ = LoginResult::SUCCEED;
                }
                else if (rt.GetContent("description") == "old_password error")
                {
                    flag_ = LoginResult::PASSWORDERROR;
                }
                else if (rt.GetContent("description") == "unknown error")
                {
                    flag_ = LoginResult::UNKNOWNERROR;
                }
                break;
            }

            default:
                break;
        }
    }
    else
    {
        flag_ = LoginResult::OUTLIMIT;
    }
    delete[]buf;
    buf = nullptr;
}
