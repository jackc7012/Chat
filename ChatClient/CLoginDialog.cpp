// CLoginDialog.cpp: 实现文件
//

#include "stdafx.h"

#include "afxdialogex.h"

#include "CLoginDialog.h"
#include "ChatClient.h"
#include "ChatClientDlg.h"
#include "CRegisterDialog.h"
#include "CLoginWait.h"
#include "CLog.h"

#include "protocol.h"
using namespace cwy;

// CLoginDialog 对话框

IMPLEMENT_DYNAMIC(CLoginDialog, CDialogEx)

CLoginDialog::CLoginDialog(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_LOGIN_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CLoginDialog::~CLoginDialog()
{
}

void CLoginDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLoginDialog, CDialogEx)
    ON_BN_CLICKED(IDC_LOGIN, &CLoginDialog::OnBnClickedLogin)
    ON_BN_CLICKED(IDC_REGISTER, &CLoginDialog::OnBnClickedRegister)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CLoginDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    SetIcon(m_hIcon, TRUE);			// 设置大图标
    SetIcon(m_hIcon, FALSE);		// 设置小图标

    CFont font;
    font.CreatePointFont(150, _T("宋体"), NULL);

    // 登录信息
    GetDlgItem(IDC_STATICID)->SetFont(&font);
    GetDlgItem(IDC_ID)->SetFont(&font);
    GetDlgItem(IDC_STATICPASSWORD)->SetFont(&font);
    GetDlgItem(IDC_PASSWORD)->SetFont(&font);
    GetDlgItem(IDC_LOGIN)->SetFont(&font);
    GetDlgItem(IDC_REGISTER)->SetFont(&font);

    socketClient_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == socketClient_)
    {
        MessageBox(_T("创建服务失败,请重试！"), _T("错误"), MB_ICONERROR);
        return FALSE;
    }

    char ip[20];
    memset(ip, 0, 20);
    GetPrivateProfileString("CommonInfo", "ServerIp", "127.0.0.1", ip, 20, "./chat.ini");
    unsigned int port = GetPrivateProfileInt("CommonInfo", "TcpPort", 6000, "./chat.ini");

    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(port);
    addr.sin_addr.S_un.S_addr = inet_addr(ip);

    if (::connect(socketClient_, (SOCKADDR*)&addr, sizeof(addr)) != 0)
    {
        MessageBox(_T("连接失败，请重试！"), _T("错误"), MB_ICONERROR);
        return FALSE;
    }
    return TRUE;
}

// CLoginDialog 消息处理程序
void CLoginDialog::OnBnClickedLogin()
{
    CString strId, strPassword;
    GetDlgItemText(IDC_ID, strId);
    GetDlgItemText(IDC_PASSWORD, strPassword);
    s_HandleRecv toSend;
    const char* id = strId.GetBuffer(0);
    const char* password = strPassword.GetBuffer(0);
    UINT64 idNum = static_cast<UINT64>(atoll(id));
    if (strcmp(id, "") == 0)
    {
        MessageBox(_T("账号不能为空"), _T("错误"), MB_ICONERROR);
        return;
    }

    /*ShowWindow(SW_HIDE);
    CChatClientDlg dlg2;
    dlg2.customerId_ = idNum;
    dlg2.socketClient_ = socketClient_;
    dlg2.DoModal();
    EndDialog(0);*/

    toSend.Param.login_.id = idNum;
    RegisterSpace(&toSend.Param.login_.password, Encryption(password));
    std::string sendData = EncodeJson(CommunicationType::LOGIN, toSend);
    ::send(socketClient_, sendData.c_str(), sendData.length(), 0);
    UnregisterSpace(CommunicationType::LOGIN, toSend);
    CLoginWait dlg1;
    dlg1.socketClient_ = socketClient_;
    dlg1.mode_ = 0;
    LoginResult rt = static_cast<LoginResult>(dlg1.DoModal());
    if ((rt == LoginResult::SUCCEED) || (rt == LoginResult::ALREADYLOGININ))
    {
        bool loginInFlag = true;
        if (rt == LoginResult::ALREADYLOGININ)
        {
            char message[70];
            memset(message, 0, 70);
            sprintf_s(message, 70, "该用户已在 %s 登录，此操作将会强制下线,是否继续", dlg1.ip_.c_str());
            if (MessageBox(message, _T("提示"), MB_YESNO) == IDYES)
            {
                toSend.Clear();
                toSend.Param.forceDelete_.id = idNum;
                sendData = EncodeJson(CommunicationType::FORCEDELETE, toSend);
                ::send(socketClient_, sendData.c_str(), sendData.length(), 0);
            }
            else
            {
                loginInFlag = false;
            }
        }
        if (loginInFlag)
        {
            ShowWindow(SW_HIDE);
            CChatClientDlg dlg2;
            dlg2.loginFlag_ = ((rt == LoginResult::SUCCEED) ? true : false);
            dlg2.customerId_ = idNum;
            dlg2.customerName_ = dlg1.customerName_;
            dlg2.socketClient_ = socketClient_;
            dlg2.DoModal();
            EndDialog(0);
        }
    }
    else if (rt == LoginResult::NOUSER)
    {
        MessageBox(_T("登录失败，请输入正确账号"), _T("错误"), MB_ICONERROR);
    }
    else if (rt == LoginResult::PASSWORDERROR)
    {
        MessageBox(_T("登录失败，请输入正确密码"), _T("错误"), MB_ICONERROR);
    }
    else if (rt == LoginResult::OUTLIMIT)
    {
        MessageBox(_T("登录超时，请检查网络状态"), _T("错误"), MB_ICONERROR);
    }
    else if (rt == LoginResult::UNKNOWNERROR)
    {
        MessageBox(_T("未知错误,请重试"), _T("错误"), MB_ICONERROR);
    }
    strId.ReleaseBuffer();
    strPassword.ReleaseBuffer();
}

void CLoginDialog::OnBnClickedRegister()
{
    ShowWindow(SW_HIDE);
    CRegisterDialog dlg;
    dlg.socketClient_ = socketClient_;
    dlg.DoModal();
    ShowWindow(SW_SHOW);
}

void CLoginDialog::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    OnBnClickedLogin();
}


void CLoginDialog::OnDestroy()
{
    CDialogEx::OnDestroy();

    // TODO: 在此处添加消息处理程序代码
    closesocket(socketClient_);
}
