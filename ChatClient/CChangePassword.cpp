// CChangePassword.cpp: 实现文件
//

#include "ChatClient.h"
#include "CChangePassword.h"
#include "afxdialogex.h"
#include <string>

#include "public.h"

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

    // TODO:  在此添加额外的初始化
    ModifyStyleEx(0, WS_EX_APPWINDOW);
    SetDlgItemText(IDC_CP_ID, std::to_string(customerId_).c_str());
    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}

void CChangePassword::OnBnClickedChangePassword()
{
    // TODO: 在此添加控件通知处理程序代码
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
    else if (!VerifyCode(strVerify.GetBuffer(0), strVerifyCode.GetBuffer(0)))
    {
        MessageBox(_T("验证码错误"), _T("错误"), MB_ICONERROR);
        SetVerify();
    }
    else
    {
        s_HandleRecv toSend;
        RegisterSpace(&toSend.Param.changePassword_.old_password, Encryption(strOldPassword.GetBuffer(0)));
        RegisterSpace(&toSend.Param.changePassword_.password, Encryption(strPassword.GetBuffer(0)));
        SendChangePasswordMessage(toSend);
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

void CChangePassword::SendChangePasswordMessage(s_HandleRecv& toSend)
{
    std::string rt = EncodeJson(CommunicationType::CHANGEPASSWORD, toSend);
    UnregisterSpace(CommunicationType::CHANGEPASSWORD, toSend);
    ::send(socketClient_, rt.c_str(), rt.length(), 0);
    CLoginWait dlg1;
    dlg1.socketClient_ = socketClient_;
    dlg1.customerName_ = std::to_string(customerId_);
    dlg1.mode_ = 2;
    int ret = dlg1.DoModal();
    if (ret == 0)
    {
        std::ostringstream tmp;
        tmp << customerId_ << " 修改密码成功!!!";
        MessageBox(tmp.str().c_str(), _T("成功"), MB_ICONINFORMATION);
        EndDialog(0);
    }
    else
    {
        MessageBox(_T("密码修改失败,请重试"), _T("错误"), MB_ICONERROR);
        SetVerify();
    }
}
