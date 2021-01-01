
// ChatClientDlg.h : 头文件
//

#pragma once
#
#include "public.h"
#include "CLog.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include "CMessage.h"
#include <thread>
#include <mutex>
#include <queue>

#define WM_SOCKET                 (WM_USER + 1000)
#define WM_TRANSFERFILEACCEPT     (WM_USER + 1001)

// CChatClientDlg 对话框
class CChatClientDlg : public CDialogEx {
// 构造
  public:
    CChatClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_CHATCLIENT_DIALOG };
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
    afx_msg void OnBnClickedConnect();
    afx_msg LRESULT OnSocket(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnTransferFileAccept(WPARAM wParam, LPARAM lParam);
  public:
    SOCKET socket_client;
    SOCKADDR_IN addr;
    afx_msg void OnBnClickedSend();
    virtual void OnOK();
    CListBox m_list_login_people;
    std::string nick_name;
    afx_msg void OnBnClickedRegister();
    bool is_service_open{ false };
    afx_msg void OnBnClickedTransferfile();
    bool can_transfer_file{ false };
    int thread_TransferFile(const HWND &hwnd, const std::string &file_name, const std::string &target);
    int thread_TransferFileAccept(const HWND &hwnd, const std::string &file_name, const cwy::s_HandleRecv &handle_recv);
    std::mutex mt_read_file;
    std::queue<cwy::s_HandleRecv> q_transfer_file;
    cwy::CLog g_log;
};
