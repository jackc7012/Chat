
// ChatServiceDlg.h : 头文件
//

#pragma once

#define WM_SOCKET_TCP               (WM_USER + 1000)
#define WM_SOCKET_UDP               (WM_USER + 1001)

#include "CNetWorkHandle.h"

// CChatServiceDlg 对话框
class CChatServiceDlg : public CDialogEx {
    // 构造
public:
    CChatServiceDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum {
        IDD = IDD_CHATSERVICE_DIALOG
    };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
    HICON m_hIcon;

    // 生成的消息映射函数
    virtual BOOL OnInitDialog();
    virtual BOOL DestroyWindow();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnBnClickedStart();
    afx_msg void OnBnClickedKick();
    afx_msg void OnDestroy();
    afx_msg LRESULT OnSocketTcp(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSocketUdp(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT HandleControlUpdate(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
private:
    bool StartTcp();
    bool StartUdp();

    CListBox listLoginPeople;
    SOCKET socketServiceTcp{0}, socketServiceUdp{0};
    std::unordered_map<SOCKET, std::string> socket2IpMap;
    std::unique_ptr<cwy::CNetWorkHandle> netWorkHandle{nullptr};
};
