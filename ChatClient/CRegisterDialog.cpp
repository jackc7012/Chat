// CRegisterDialog.cpp: 实现文件
//

#include "stdafx.h"
#include "ChatClient.h"
#include "CRegisterDialog.h"
#include "afxdialogex.h"

#include "public.h"
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
    CString strName, strPassword, strPasswordConfirm, strVerify, strVerifyCode;
    GetDlgItemText(IDC_NICK_NAME, strName);
    GetDlgItemText(IDC_PASSWORD, strPassword);
    GetDlgItemText(IDC_PASSWORD_CONFIRM, strPasswordConfirm);
    GetDlgItemText(IDC_VERIFY, strVerify);
    GetDlgItemText(IDC_STATIC_VERIFY_CODE, strVerifyCode);
    if (strcmp(strPassword, strPasswordConfirm) != 0) {
        MessageBox(_T("两次输入密码不一致"), _T("错误"), MB_ICONERROR);
    } else if (strPassword.GetLength() < 6) {
        MessageBox(_T("密码必须6位及以上"), _T("错误"), MB_ICONERROR);
    } else if (!VerifyPassWord(strPassword.GetBuffer(0), strPassword.GetLength())) {
        MessageBox(_T("密码必须包含大写、小写字母和数字"), _T("错误"), MB_ICONERROR);
    } else if (!VerifyCode(strVerify.GetBuffer(0), strVerifyCode.GetBuffer(0))) {
        MessageBox(_T("验证码错误"), _T("错误"), MB_ICONERROR);
        SetVerify();
    } else {
        s_HandleRecv toSend;
        RegisterSpace(&toSend.Param.register_.customer, strName.GetBuffer(0));
        RegisterSpace(&toSend.Param.register_.password, strPassword.GetBuffer(0));
        std::string send_data = EncodeJson(CommunicationType::REGISTER, toSend);
        UnregisterSpace(CommunicationType::REGISTER, toSend);
        ::send(socketClient, send_data.c_str(), send_data.length(), 0);
        GetRecv();
    }
}

void CRegisterDialog::SetVerify()
{
    int codeRand = 0;
    std::string verifyCode("");
    for (int i = 0; i < 4; ++i) {
        codeRand = rand() % VERIFY_CODE.length();
        verifyCode += VERIFY_CODE[codeRand];
    }
    SetDlgItemText(IDC_STATIC_VERIFY_CODE, verifyCode.c_str());
}

void CRegisterDialog::GetRecv()
{
    char buf[256] = {0};
    int nNetTimeout = 30000;
    ::setsockopt(socketClient, SOL_SOCKET, SO_RCVTIMEO, (char*)&nNetTimeout, sizeof(int));
    int ret = ::recv(socketClient, buf, 256, 0);
    if (ret > 0) {
        s_HandleRecv rt;
        DecodeJson(buf, rt);
        if (strcmp(rt.Param.registerBack_.register_result, "succeed") == 0) {
            std::string msg("注册成功！  id号为\n");
            msg += std::to_string(rt.Param.registerBack_.id);
            MessageBox(msg.c_str());
        } else if (strcmp(rt.Param.registerBack_.register_result, "failed") == 0) {
            std::string msg("注册失败！  ");
            msg += rt.Param.registerBack_.description;
            MessageBox(msg.c_str(), _T("错误"), MB_ICONERROR);
        }
        UnregisterSpace(rt.type_, rt);
    } else {
        MessageBox(_T("服务器超时，请重试！"), _T("错误"), MB_ICONERROR);
    }
    EndDialog(0);
}
