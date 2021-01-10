// CRegisterDialog.cpp: 实现文件
//

#include "stdafx.h"
#include "ChatClient.h"
#include "CRegisterDialog.h"
#include "afxdialogex.h"
using namespace cwy;

// CRegisterDialog 对话框

IMPLEMENT_DYNAMIC(CRegisterDialog, CDialogEx)

CRegisterDialog::CRegisterDialog(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_REGISTER_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CRegisterDialog::~CRegisterDialog()
{
}

void CRegisterDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRegisterDialog, CDialogEx)
    ON_BN_CLICKED(IDC_REGISTER_USER, &CRegisterDialog::OnBnClickedRegisterUser)
END_MESSAGE_MAP()


// CRegisterDialog 消息处理程序


BOOL CRegisterDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  在此添加额外的初始化
    SetIcon(m_hIcon, TRUE);         // 设置大图标
    SetIcon(m_hIcon, FALSE);        // 设置小图标

    CFont font;
    font.CreatePointFont(150, _T("宋体"), NULL);

    GetDlgItem(IDC_STATIC_NICK_NAME)->SetFont(&font);
    GetDlgItem(IDC_STATIC_PASSWORD)->SetFont(&font);
    GetDlgItem(IDC_STATIC_PASSWORD_CONFIRM)->SetFont(&font);
    GetDlgItem(IDC_STATIC_VERIFY)->SetFont(&font);
    GetDlgItem(IDC_STATIC_VERIFY_CODE)->SetFont(&font);
    GetDlgItem(IDC_NICK_NAME)->SetFont(&font);
    GetDlgItem(IDC_PASSWORD)->SetFont(&font);
    GetDlgItem(IDC_PASSWORD_CONFIRM)->SetFont(&font);
    GetDlgItem(IDC_VERIFY)->SetFont(&font);
    GetDlgItem(IDC_REGISTER_USER)->SetFont(&font);

    srand((unsigned int)time(NULL));
    SetVerify();
    return TRUE;
}


void CRegisterDialog::OnBnClickedRegisterUser()
{
    // TODO: 在此添加控件通知处理程序代码
    CString str_name, str_password, str_password_confirm, str_verify, str_verify_code;
    GetDlgItemText(IDC_NICK_NAME, str_name);
    GetDlgItemText(IDC_PASSWORD, str_password);
    GetDlgItemText(IDC_PASSWORD_CONFIRM, str_password_confirm);
    GetDlgItemText(IDC_VERIFY, str_verify);
    GetDlgItemText(IDC_STATIC_VERIFY_CODE, str_verify_code);
    if (strcmp(str_password, str_password_confirm) != 0) {
	    MessageBox(_T("两次输入密码不一致"), _T("错误"), MB_ICONERROR);
    }
    else if (!cwy::VerifyCode(str_verify.GetBuffer(0), str_verify_code.GetBuffer(0))) {
	    MessageBox(_T("验证码错误"), _T("错误"), MB_ICONERROR);
	    SetVerify();
    }
    else {
	    cwy::s_HandleRecv toSend;
	    toSend.param_.register_.customer_ = const_cast<char*>(str_name.GetBuffer(0));
	    toSend.param_.register_.password_ = const_cast<char*>(str_password.GetBuffer(0));
	    SendRegisterMessage(toSend);
    }
    str_name.ReleaseBuffer();
    str_password.ReleaseBuffer();
}

void CRegisterDialog::SetVerify()
{
    int code_rand = 0;
    std::string verify_code("");
    for (int i = 0; i < 4; ++i) {
        code_rand = rand() % strlen(cwy::VERIFY_CODE);
        verify_code += cwy::VERIFY_CODE[code_rand];
    }
    SetDlgItemText(IDC_STATIC_VERIFY_CODE, verify_code.c_str());
}

void CRegisterDialog::SendRegisterMessage(const cwy::s_HandleRecv& toSend)
{
    std::string rt = cwy::EncodeJson(cwy::CommunicationType::REGISTER, toSend);
    ::send(socketClient, rt.c_str(), rt.length(), 0);
    char* buf = new char[DATA_LENGTH];
    memset(buf, 0, DATA_LENGTH);
    int nNetTimeout = 30000;
    ::setsockopt(socketClient, SOL_SOCKET, SO_RCVTIMEO, (char*)&nNetTimeout, sizeof(int));
    ::recv(socketClient, buf, DATA_LENGTH, 0);
    if (buf[0] != '\0') {
        s_HandleRecv rt;
        DecodeJson(buf, rt);
        if (strcmp(rt.param.RegisterBack.register_result, "succeed") == 0) {
            std::ostringstream tmp;
            tmp << "注册成功，账号为：" << rt.param.RegisterBack.id;
            MessageBox(tmp.str().c_str());
        }
        else if (strcmp(rt.param.RegisterBack.register_result, "failed") == 0) {
            MessageBox(_T("注册失败"), _T("错误"), MB_ICONERROR);
        }
        DeleteMemory(CommunicationType::REGISTERBACK, rt);
    }
    else {
        MessageBox(_T("网络连接错误，请稍后再试"), _T("错误"), MB_ICONERROR);
    }
    delete[]buf;
}
