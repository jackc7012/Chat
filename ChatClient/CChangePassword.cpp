// CChangePassword.cpp: 实现文件
//

#include "ChatClient.h"
#include "CChangePassword.h"
#include "CLoginWait.h"

#include "protocol.h"
using namespace cwy;

// CChangePassword 对话框

IMPLEMENT_DYNAMIC(CChangePassword, CDialogEx)

CChangePassword::CChangePassword(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_CHANGEPASSWORD, pParent)
{

}

CChangePassword::~CChangePassword()
{
}

void CChangePassword::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CChangePassword, CDialogEx)
    ON_BN_CLICKED(IDC_CHANGE_PASSWORD, &CChangePassword::OnBnClickedChangePassword)
END_MESSAGE_MAP()

// CChangePassword 消息处理程序

BOOL CChangePassword::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    ModifyStyleEx(0, WS_EX_APPWINDOW);
    SetDlgItemText(IDC_CP_ID, customerId_.c_str());

    srand((unsigned int)time(NULL));
    SetVerify();

    return TRUE;
}

void CChangePassword::OnOK()
{
    OnBnClickedChangePassword();
}

void CChangePassword::OnBnClickedChangePassword()
{
    CString strId, strOldPassword, strPassword, strPasswordConfirm, strVerify, strVerifyCode;
    GetDlgItemText(IDC_CP_ID, strId);
    GetDlgItemText(IDC_OLD_PASSWORD, strOldPassword);
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
        SendChangePasswordMessage(strId.GetBuffer(0), strOldPassword.GetBuffer(0), strPassword.GetBuffer(0));
    }
    strId.ReleaseBuffer();
    strOldPassword.ReleaseBuffer();
    strPassword.ReleaseBuffer();
    strPasswordConfirm.ReleaseBuffer();
    strVerify.ReleaseBuffer();
    strVerifyCode.ReleaseBuffer();
}

void CChangePassword::SetVerify()
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

void CChangePassword::SendChangePasswordMessage(const std::string& id, const std::string& oldPassword, const std::string& password)
{
    HandleRecv toSend;
    toSend.SetContent("id", id);
    toSend.SetContent("old_password", Encryption(oldPassword));
    toSend.SetContent("password", Encryption(password));
    std::string rt = toSend.Write(CommunicationType::CHANGEPASSWORD);
    ::send(socketClient_, rt.c_str(), rt.length(), 0);
    CLoginWait loginWaitDlg;
    loginWaitDlg.socketClient_ = socketClient_;
    loginWaitDlg.id_ = id;
    loginWaitDlg.mode_ = Mode::CHANGEPWD;
    LoginResult ret = static_cast<LoginResult>(loginWaitDlg.DoModal());
    if (ret == LoginResult::SUCCEED)
    {
        std::ostringstream tmp;
        tmp << id << " 修改密码成功!!!";
        MessageBox(tmp.str().c_str(), _T("成功"), MB_ICONINFORMATION);
        EndDialog(0);
    }
    else if (ret == LoginResult::PASSWORDERROR)
    {
        MessageBox(_T("原密码错误"), _T("错误"), MB_ICONERROR);
    }
    else
    {
        MessageBox(_T("密码修改失败,请重试"), _T("错误"), MB_ICONERROR);
        SetVerify();
    }
}
