// CRegisterDialog.cpp: 实现文件
//

#include "afxdialogex.h"

#include "ChatClient.h"
#include "CRegisterDialog.h"
#include "CLoginWait.h"

#include "protocol.h"

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

    SetIcon(m_hIcon, TRUE);         // 设置大图标
    SetIcon(m_hIcon, FALSE);        // 设置小图标
    ModifyStyleEx(0, WS_EX_APPWINDOW);

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
    OnBnClickedRegisterUser();
}

void CRegisterDialog::OnBnClickedRegisterUser()
{
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
    else if (strlen(strPasswordConfirm) < 6)
    {
        MessageBox(_T("密码少于6位"), _T("错误"), MB_ICONERROR);
        SetVerify();
    }
    else if (!VerifyCode(strVerify.GetBuffer(0), strVerifyCode.GetBuffer(0)))
    {
        MessageBox(_T("验证码错误"), _T("错误"), MB_ICONERROR);
    }
    else
    {
        SendRegisterMessage(strName.GetBuffer(0), strPassword.GetBuffer(0));
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
    int codeRand = 0;
    std::string verifyCode("");
    for (int i = 0; i < 4; ++i)
    {
        codeRand = rand() % VERIFY_CODE.length();
        verifyCode += VERIFY_CODE[codeRand];
    }
    SetDlgItemText(IDC_STATIC_VERIFY_CODE, verifyCode.c_str());
}

void CRegisterDialog::SendRegisterMessage(const std::string& customerName, const std::string& password)
{
    HandleRecv toSend;
    toSend.SetContent("customer", customerName);
    toSend.SetContent("password", Encryption(password));
    std::string rt = toSend.Write(CommunicationType::REGISTER);
    ::send(socketClient_, rt.c_str(), rt.length(), 0);
    CLoginWait loginWaitDlg;
    loginWaitDlg.socketClient_ = socketClient_;
    loginWaitDlg.customerName_ = customerName;
    loginWaitDlg.mode_ = Mode::REGISTER;
    LoginResult ret = static_cast<LoginResult>(loginWaitDlg.DoModal());
    if (ret == LoginResult::SUCCEED)
    {
        std::ostringstream tmp;
        tmp << "注册成功，账号为 ：" << loginWaitDlg.id_;
        MessageBox(tmp.str().c_str(), _T("成功"), MB_ICONINFORMATION);
        EndDialog(0);
    }
    else
    {
        MessageBox(_T("注册失败,请重试"), _T("错误"), MB_ICONERROR);
        SetVerify();
    }
}
