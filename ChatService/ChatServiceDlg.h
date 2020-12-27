
// ChatServiceDlg.h : 头文件
//

#pragma once

#define WM_SOCKET_TCP               (WM_USER + 1000)
#define WM_SOCKET_UDP               (WM_USER + 1001)

#include <vector>
#include <map>
#include <mutex>
#include <queue>
#include <thread>

#include "public.h"
#include "CLog.h"
#include "CNetWorkHandle.h"

using IpName = std::pair<std::string, std::string>;

const int THREAD_NUM = 5;

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
    SOCKET socketServiceTcp{ 0 }, socketServiceUdp{ 0 };
    SOCKADDR_IN addrServiceTcp{ 0 }, addrAccept{ 0 }, addrServiceUdp{ 0 };
    std::vector<SOCKET> socketAccept;
    DWORD accpetCount{ 0 };
    afx_msg LRESULT OnSocketTcp(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSocketUdp(WPARAM wParam, LPARAM lParam);
    CListBox m_list_login_people;
    std::multimap<std::string, SOCKET> name_to_socket_accept;
    std::multimap<std::string, std::string> name_to_ip;
    std::multimap<SOCKET, IpName> socketToIpName;
    std::vector<std::string> ve_accept_name;
    std::mutex mtServerHandle;
    afx_msg void OnBnClickedKick();
    std::queue<mychat::s_TaskQueue> taskQueue;
    mychat::CLog logService;
    mychat::CNetWorkHandle* netWorkHandle;
    bool StartTcp();
    bool StartUdp();
    std::thread myHandleThread[THREAD_NUM];
    void threadTask(int taskNum);
};
