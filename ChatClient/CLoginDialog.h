#pragma once

// CLoginDialog 对话框

class CLoginDialog : public CDialogEx {
    DECLARE_DYNAMIC(CLoginDialog)

public:
    CLoginDialog(CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CLoginDialog();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum {
        IDD = IDD_LOGIN_DIALOG
    };
#endif

protected:
    HICON m_hIcon;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedLogin();
    afx_msg void OnBnClickedRegister();
    SOCKET socketClient{INVALID_SOCKET};
    virtual void OnOK();
};
