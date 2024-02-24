// CLoginDialog.cpp: 实现文件
//

#include "afxdialogex.h"

#include "CLoginDialog.h"
#include "ChatClient.h"
#include "ChatClientDlg.h"
#include "CRegisterDialog.h"
#include "CLoginWait.h"

#include "protocol.h"
#include "log.h"
using namespace cwy;

// CLoginDialog 对话框

IMPLEMENT_DYNAMIC(CLoginDialog, CDialogEx)

CLoginDialog::CLoginDialog(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_LOGIN_DIALOG, pParent)
    , info_(INFO_FILE_NAME)
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

    SetIcon(m_hIcon, TRUE);         // 设置大图标
    SetIcon(m_hIcon, FALSE);        // 设置小图标
    ModifyStyleEx(0, WS_EX_APPWINDOW);

    CFont font;
    font.CreatePointFont(150, _T("宋体"), NULL);

    // 登录信息
    GetDlgItem(IDC_STATICID)->SetFont(&font);
    GetDlgItem(IDC_ID)->SetFont(&font);
    GetDlgItem(IDC_STATICPASSWORD)->SetFont(&font);
    GetDlgItem(IDC_PASSWORD)->SetFont(&font);
    GetDlgItem(IDC_LOGIN)->SetFont(&font);
    GetDlgItem(IDC_REGISTER)->SetFont(&font);


    LogInfo logInfo = info_.GetLogInfo();
    logClient_.InitLog(1, logInfo.path_, static_cast<LogLevel>(logInfo.level_), logInfo.size_, logInfo.autoFlush_);

    socketClient_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == socketClient_)
    {
        MessageBox(_T("创建服务失败,请重试！"), _T("错误"), MB_ICONERROR);
        return FALSE;
    }

    NetWorkInfo networkInfo = info_.GetNetWorkInfo();

    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(networkInfo.tcpPort_);
    addr.sin_addr.S_un.S_addr = inet_addr(networkInfo.ip_);

    if (::connect(socketClient_, (SOCKADDR*)&addr, sizeof(addr)) != 0)
    {
        MessageBox(_T("连接失败，请重试！"), _T("错误"), MB_ICONERROR);
        return FALSE;
    }
    return TRUE;
}

void CLoginDialog::OnOK()
{
    OnBnClickedLogin();
}

void CLoginDialog::OnDestroy()
{
    CDialogEx::OnDestroy();

    closesocket(socketClient_);
}

// CLoginDialog 消息处理程序

void CLoginDialog::OnBnClickedLogin()
{
    CString strId, strPassword;
    GetDlgItemText(IDC_ID, strId);
    HandleRecv toSend;
    const char* id = strId.GetBuffer(0);
    strId.ReleaseBuffer();
    if (strcmp(id, "") == 0)
    {
        MessageBox(_T("账号不能为空"), _T("错误"), MB_ICONERROR);
        return;
    }

    GetDlgItemText(IDC_PASSWORD, strPassword);
    const char* password = strPassword.GetBuffer(0);
    strPassword.ReleaseBuffer();
    toSend.SetContent("id", id);
    toSend.SetContent("password", Encryption(password));
    std::string sendData = toSend.Write(CommunicationType::LOGIN);
    ::send(socketClient_, sendData.c_str(), sendData.length(), 0);
    CLoginWait loginWaitDlg;
    loginWaitDlg.socketClient_ = socketClient_;
    loginWaitDlg.id_ = id;
    loginWaitDlg.mode_ = Mode::LOGIN;
    LoginResult rt = static_cast<LoginResult>(loginWaitDlg.DoModal());
    if ((rt == LoginResult::SUCCEED) || (rt == LoginResult::ALREADYLOGININ))
    {
        bool loginInFlag = true;
        if (rt == LoginResult::ALREADYLOGININ)
        {
            char message[70];
            memset(message, 0, 70);
            sprintf_s(message, 70, "该用户已在 %s 登录，此操作将会强制下线,是否继续", trim(loginWaitDlg.ip_).c_str());
            if (MessageBox(message, _T("提示"), MB_YESNO) == IDYES)
            {
                toSend.Clear();
                toSend.SetContent("id", id);
                sendData = toSend.Write(CommunicationType::FORCEDELETE);
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
            CChatClientDlg chatClientDlg;
            chatClientDlg.loginFlag_ = ((rt == LoginResult::SUCCEED) ? true : false);
            chatClientDlg.customerId_ = id;
            chatClientDlg.customerName_ = loginWaitDlg.customerName_;
            chatClientDlg.socketClient_ = socketClient_;
            chatClientDlg.info_ = info_;
            chatClientDlg.DoModal();
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
}

void CLoginDialog::OnBnClickedRegister()
{
    ShowWindow(SW_HIDE);
    CRegisterDialog registerDlg;
    registerDlg.socketClient_ = socketClient_;
    registerDlg.DoModal();
    ShowWindow(SW_SHOW);
}
