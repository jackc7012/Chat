
// ChatServiceDlg.h : 头文件
//

#pragma once

#define WM_SOCKET_TCP               (WM_USER + 1000)
#define WM_SOCKET_UDP               (WM_USER + 1001)


#include "service.h"

using namespace cwy;

static const int LOGIN_PEOPLE_COL = 3;
static const std::string LOGIN_PEOPLE_LIST[] = {_T("账号"), _T("昵称"), _T("IP")};
static const std::string INSTANCE_NAME = _T("ChatService");

// CChatServiceDlg 对话框
class CChatServiceDlg : public CDialogEx {
    // 构造
public:
    CChatServiceDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_CHATSERVICE_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
    HICON m_hIcon;

    // 生成的消息映射函数
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedStart();
    SOCKADDR_IN addrServiceTcp{ 0 }, addrAccept{ 0 }, addrServiceUdp{ 0 };
    afx_msg LRESULT OnSocketTcp(WPARAM wParam, LPARAM lParam);
    std::vector<std::string> ve_accept_name;
    afx_msg void OnBnClickedKick();
    virtual BOOL DestroyWindow();
    CListCtrl m_listLoginPeople;

private:
    void InitControl();
    Service* service_{ nullptr };
    void EventHandle(const CallBackType callBackType, const std::string& message, bool isError);
};
