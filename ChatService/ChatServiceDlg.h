
// ChatServiceDlg.h : 头文件
//

#pragma once

#define WM_SOCKET                (WM_USER + 1000)

#include <vector>
#include <map>
#include <mutex>
#include <queue>

#include "public.h"
#include "CLog.h"
#include "CNetWorkHandle.h"

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
    SOCKADDR_IN addrServiceTcp{ 0 }, addr_accept{ 0 }, addrServiceUdp{ 0 };
    std::vector<SOCKET> socket_accept;
    std::string ip;
    DWORD accpet_count{ 0 };
    afx_msg LRESULT OnSocket(WPARAM wParam, LPARAM lParam);
    CListBox m_list_login_people;
    std::multimap<std::string, SOCKET> name_to_socket_accept;
    std::multimap<std::string, std::string> name_to_ip;
    void HandleRecv(const s_HandleRecv &handle_recv);
    std::vector<std::string> ve_accept_name;
    std::mutex mt_server_handle;
    afx_msg void OnBnClickedKick();
    std::queue<std::string> task_queue;
    mychat::CLog logService;
    mychat::CNetWorkHandle netWorkHandle;
    bool StartTcp();
    bool StartUdp();
};
