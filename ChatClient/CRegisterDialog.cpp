// CRegisterDialog.cpp: 实现文件
//

#include "stdafx.h"
#include "ChatClient.h"
#include "CRegisterDialog.h"
#include "afxdialogex.h"

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

void CRegisterDialog::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    OnBnClickedRegisterUser();
}

void CRegisterDialog::OnBnClickedRegisterUser()
{
    // TODO: 在此添加控件通知处理程序代码
    CString strName, strPassword, strPasswordConfirm, strVerify, strVerifyCode;
    GetDlgItemText(IDC_NICK_NAME, strName);
    GetDlgItemText(IDC_PASSWORD, strPassword);
    GetDlgItemText(IDC_PASSWORD_CONFIRM, strPasswordConfirm);
    GetDlgItemText(IDC_VERIFY, strVerify);
    GetDlgItemText(IDC_STATIC_VERIFY_CODE, strVerifyCode);
    if (strcmp(strPassword, strPasswordConfirm) != 0)
    {
        MessageBox(_T("两次输入密码不一致"), _T("错误"), MB_ICONERROR);
        SetVerify();
    }
    else if (!VerifyCode(strVerify.GetBuffer(0), strVerifyCode.GetBuffer(0)))
    {
        MessageBox(_T("验证码错误"), _T("错误"), MB_ICONERROR);
        SetVerify();
    }
    else
    {
        s_HandleRecv toSend;
        RegisterSpace(&toSend.Param.register_.customer, strName.GetBuffer(0));
        RegisterSpace(&toSend.Param.register_.password, Encryption(strPassword.GetBuffer(0)));
        SendRegisterMessage(toSend);
    }
    strName.ReleaseBuffer();
    strPassword.ReleaseBuffer();
    strPasswordConfirm.ReleaseBuffer();
    strVerify.ReleaseBuffer();
    strVerifyCode.ReleaseBuffer();
}

void CRegisterDialog::SetVerify()
{
    SetDlgItemText(IDC_VERIFY, _T(""));
    int code_rand = 0;
    std::string verify_code("");
    for (int i = 0; i < 4; ++i)
    {
        code_rand = rand() % VERIFY_CODE.length();
        verify_code += VERIFY_CODE[code_rand];
    }
    SetDlgItemText(IDC_STATIC_VERIFY_CODE, verify_code.c_str());
}

void CRegisterDialog::SendRegisterMessage(s_HandleRecv& toSend)
{
    std::string rt = EncodeJson(CommunicationType::REGISTER, toSend);
    UnregisterSpace(CommunicationType::REGISTER, toSend);
    ::send(socketClient_, rt.c_str(), rt.length(), 0);
    CLoginWait dlg1;
    dlg1.socketClient_ = socketClient_;
    dlg1.mode_ = 1;
    int ret = dlg1.DoModal();
    if (ret == 0)
    {
        std::ostringstream tmp;
        tmp << "注册成功，账号为 ：" << dlg1.customerName_;
        MessageBox(tmp.str().c_str(), _T("成功"), MB_ICONINFORMATION);
        EndDialog(0);
    }
    else
    {
        MessageBox(_T("注册失败,请重试"), _T("错误"), MB_ICONERROR);
        SetVerify();
    }
}
