// CLoginDialog.cpp: 实现文件
//

#include "stdafx.h"
#include "ChatClient.h"
#include "CLoginDialog.h"
#include "afxdialogex.h"

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

	// TODO:  在此添加额外的初始化
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	CFont font;
	font.CreatePointFont(150, _T("宋体"), NULL);

	// 登录信息
	GetDlgItem(IDC_STATICIP)->SetFont(&font);
	GetDlgItem(IDC_IP)->SetFont(&font);
	GetDlgItem(IDC_STATICNAME)->SetFont(&font);
	GetDlgItem(IDC_NAME)->SetFont(&font);
	GetDlgItem(IDC_STATICPASSWORD)->SetFont(&font);
	GetDlgItem(IDC_PASSWORD)->SetFont(&font);
	GetDlgItem(IDC_LOGIN)->SetFont(&font);
	GetDlgItem(IDC_REGISTER)->SetFont(&font);

	SetDlgItemText(IDC_IP, _T("127.0.0.1"));

	return TRUE;
}

// CLoginDialog 消息处理程序
void CLoginDialog::OnBnClickedLogin()
{
	CString str_ip, str_name, str_password, str_text;
	GetDlgItemText(IDC_IP, str_ip);
	GetDlgItemText(IDC_NAME, str_name);
	GetDlgItemText(IDC_PASSWORD, str_password);
	s_HandleRecv to_send;
	const char* ip = str_ip.GetBuffer(0);
	const char* name = str_name.GetBuffer(0);
	const char* password = str_password.GetBuffer(0);
	socketClient = ::socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == socketClient) {
		MessageBox(_T("创建服务失败,请重试！"), _T("错误"), MB_ICONERROR);
		return;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = ntohs(mychat::TCP_PORT);
	addr.sin_addr.S_un.S_addr = inet_addr(ip);

	if (connect(socketClient, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
		MessageBox(_T("连接失败，请重试！"), _T("错误"), MB_ICONERROR);
		return;
	}

	to_send.Param.Login.customer = const_cast<char*>(name);
	to_send.Param.Login.password = const_cast<char*>(password);
	std::string send_data = EncodeJson(LOGIN, to_send);
	::send(socketClient, send_data.c_str(), send_data.length(), 0);
	nick_name = str_name.GetBuffer(0);
	CLoginWait dlg1;
	dlg1.socketClient = socketClient;
	int rt = dlg1.DoModal();
	if (rt == 1) {
		logClient.InitLog(name);
		ShowWindow(SW_HIDE);
		CChatClientDlg dlg2;
		dlg2.DoModal();
		EndDialog(0);
	}
	else if (rt == 2) {
		MessageBox(_T("登录失败，请输入正确的昵称和密码"), _T("错误"), MB_ICONERROR);
	}
	else if (rt == -1) {
		MessageBox(_T("登录超时，请检查网络状态"), _T("错误"), MB_ICONERROR);
	}
}

void CLoginDialog::OnBnClickedRegister()
{
	ShowWindow(SW_HIDE);
	CRegisterDialog dlg;
	dlg.DoModal();
	ShowWindow(SW_SHOW);
}