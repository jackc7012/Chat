#pragma once

// CLoginDialog 对话框

class CLoginDialog : public CDialogEx {
    DECLARE_DYNAMIC(CLoginDialog)

public:
    CLoginDialog(CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CLoginDialog();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_LOGIN_DIALOG };
#endif

protected:
    HICON m_hIcon;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    virtual void OnOK();

public:
    afx_msg void OnBnClickedLogin();
    afx_msg void OnBnClickedRegister();
    afx_msg void OnDestroy();

private:
    SOCKET socketClient_{ INVALID_SOCKET };
};
