// CLoginDialog.cpp: 实现文件
//

#include "stdafx.h"
#include "ChatClient.h"
#include "CLoginDialog.h"
#include "afxdialogex.h"

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

    socketClient = ::socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == socketClient)
    {
        MessageBox(_T("创建服务失败,请重试！"), _T("错误"), MB_ICONERROR);
        return FALSE;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(cwy::TCP_PORT);
    addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

    if (::connect(socketClient, (SOCKADDR*)&addr, sizeof(addr)) != 0)
    {
        MessageBox(_T("连接失败，请重试！"), _T("错误"), MB_ICONERROR);
        return FALSE;
    }
    char* buf = new char[DATA_LENGTH];
    memset(buf, 0, DATA_LENGTH);
    int nNetTimeout = GET_TOKEN_TIMEOUT;
    ::setsockopt(socketClient, SOL_SOCKET, SO_RCVTIMEO, (char*)&nNetTimeout, sizeof(int));
    ::recv(socketClient, buf, DATA_LENGTH, 0);
    //client_token = CHandleJson::CHandleJson(buf).GetJsonValueString("content");
    delete[]buf;
    return TRUE;
}

// CLoginDialog 消息处理程序
void CLoginDialog::OnBnClickedLogin()
{
    CString str_id, str_password, str_text;
    GetDlgItemText(IDC_ID, str_id);
    GetDlgItemText(IDC_PASSWORD, str_password);
    s_HandleRecv to_send;
    const char* id = str_id.GetBuffer(0);
    const char* password = str_password.GetBuffer(0);

    //to_send.param_.login_.id_ = atoll(id);
    //to_send.param_.login_.password_ = const_cast<char*>(password);
    std::string send_data = EncodeJson(CommunicationType::LOGIN, to_send);
    ::send(socketClient, send_data.c_str(), send_data.length(), 0);
    //client_nick_name = str_name.GetBuffer(0);
    CLoginWait dlg1;
    dlg1.socketClient = socketClient;
    int rt = dlg1.DoModal();
    if (rt == 1)
    {
        //logClient.InitLog(name);
        ShowWindow(SW_HIDE);
        CChatClientDlg dlg2;
        dlg2.DoModal();
        EndDialog(0);
    }
    else if (rt == 2)
    {
        MessageBox(_T("登录失败，请输入正确的昵称和密码"), _T("错误"), MB_ICONERROR);
    }
    else if (rt == 3)
    {
        MessageBox(_T("该用户已经登录，请勿重复登录"), _T("错误"), MB_ICONERROR);
    }
    else if (rt == -1)
    {
        MessageBox(_T("登录超时，请检查网络状态"), _T("错误"), MB_ICONERROR);
    }
}

void CLoginDialog::OnBnClickedRegister()
{
    ShowWindow(SW_HIDE);
    CRegisterDialog dlg;
    dlg.socketClient = socketClient;
    dlg.DoModal();
    ShowWindow(SW_SHOW);
}