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
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnBnClickedYes();
    afx_msg void OnBnClickedRefuse();
    afx_msg void OnBnClickedSaveas();
    DECLARE_MESSAGE_MAP()
  public:
    std::string message;
    std::string filePath;
    std::string fileName;
};
