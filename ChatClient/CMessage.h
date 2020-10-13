#pragma once


// CMessage 对话框
#include <string>

class CMessage : public CDialogEx {
    DECLARE_DYNAMIC(CMessage)

  public:
    CMessage(CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CMessage();

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_REQUEST };
#endif

  protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
  public:
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    std::string message;
    afx_msg void OnBnClickedAgree();
    afx_msg void OnBnClickedRefuse();
};
