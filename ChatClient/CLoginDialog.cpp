// CLoginDialog.cpp: 实现文件
//

#include "stdafx.h"
#include "CLoginDialog.h"
#include "afxdialogex.h"

#include "ChatClient.h"
#include "ChatClientDlg.h"
#include "CRegisterDialog.h"
#include "CLoginWait.h"
#include "CLog.h"
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
END_MESSAGE_MAP()

BOOL CLoginDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    SetIcon(m_hIcon, TRUE);         // 设置大图标
    SetIcon(m_hIcon, FALSE);        // 设置小图标

    CFont font;
    font.CreatePointFont(150, _T("宋体"), NULL);

    // 登录信息
    GetDlgItem(IDC_STATICID)->SetFont(&font);
    GetDlgItem(IDC_ID)->SetFont(&font);
    GetDlgItem(IDC_STATICPASSWORD)->SetFont(&font);
    GetDlgItem(IDC_PASSWORD)->SetFont(&font);
    GetDlgItem(IDC_LOGIN)->SetFont(&font);
    GetDlgItem(IDC_REGISTER)->SetFont(&font);

    socketClient = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socketClient == INVALID_SOCKET) {
        MessageBox(_T("创建服务失败,请重试！"), _T("错误"), MB_ICONERROR);
        return FALSE;
    }

    char ip[50] = {0};
    GetPrivateProfileString("CommonInfo", "ServerIp", "127.0.0.1", ip, 50, "./Chat.ini");
    unsigned int tcpPort = GetPrivateProfileInt("CommonInfo", "TcpPort", 6000, "./Chat.ini");
    unsigned int udpPort = GetPrivateProfileInt("CommonInfo", "UdpPort", 6002, "./Chat.ini");

    SOCKADDR_IN addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(tcpPort);
    addr.sin_addr.S_un.S_addr = inet_addr(ip);

    if (::connect(socketClient, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
        MessageBox(_T("连接失败，请退出重试！"), _T("错误"), MB_ICONERROR);
    }

    return TRUE;
}

// CLoginDialog 消息处理程序

void CLoginDialog::OnBnClickedLogin()            // 登录按钮
{
    CString strId, strPassword;
    GetDlgItemText(IDC_ID, strId);
    GetDlgItemText(IDC_PASSWORD, strPassword);

    if (strId.GetLength() != 5) {
        MessageBox(_T("账号格式不对，请重新输入！"), _T("错误"), MB_ICONERROR);
        return;
    }

    s_HandleRecv toSend;
    toSend.Param.login_.id = atoll(strId.GetBuffer(0));
    RegisterSpace(&toSend.Param.login_.password, strPassword.GetBuffer(0));
    std::string sendData = EncodeJson(CommunicationType::LOGIN, toSend);
    UnregisterSpace(CommunicationType::LOGIN, toSend);
    ::send(socketClient, sendData.c_str(), sendData.length(), 0);
    CLoginWait dlg1;
    dlg1.socketClient = socketClient;
    int rt = dlg1.DoModal();
    if (rt == 1) {
        ShowWindow(SW_HIDE);
        CChatClientDlg dlg2;
        dlg2.nickName = dlg1.nickName;
        dlg2.socketClient = socketClient;
        dlg2.DoModal();
        ::shutdown(socketClient, SD_BOTH);
        ::closesocket(socketClient);
        EndDialog(0);
    } else if (rt == 2) {
        MessageBox(_T("登录失败，请输入正确的昵称和密码"), _T("错误"), MB_ICONERROR);
    } else if (rt == -1) {
        MessageBox(_T("登录超时，请检查网络状态"), _T("错误"), MB_ICONERROR);
    }
}

void CLoginDialog::OnBnClickedRegister()        // 注册按钮
{
    ShowWindow(SW_HIDE);
    CRegisterDialog dlg;
    dlg.socketClient = socketClient;
    dlg.DoModal();
    ShowWindow(SW_SHOW);
}

void CLoginDialog::OnOK()
{
    OnBnClickedLogin();
}
