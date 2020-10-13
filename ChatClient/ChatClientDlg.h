
// ChatClientDlg.h : ͷ�ļ�
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

// CChatClientDlg �Ի���
class CChatClientDlg : public CDialogEx {
// ����
  public:
    CChatClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_CHATCLIENT_DIALOG };
#endif

  protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
  protected:
    HICON m_hIcon;

    // ���ɵ���Ϣӳ�亯��
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
    int thread_TransferFileAccept(const HWND &hwnd, const std::string &file_name, const s_HandleRecv &handle_recv);
    std::mutex mt_read_file;
    std::queue<s_HandleRecv> q_transfer_file;
    CLog g_log;
    void ConnectServer(const std::string &ip = "127.0.0.1");
    void InitFont();
    void EnableWindowInfo(const BOOL flag = TRUE);
    void EnableWindowChat(const BOOL flag = TRUE);
    void SetVerify();
    void HandleRegister(const int flag, const s_HandleRecv &handle_recv);
    void HandleLogin(const int flag, const s_HandleRecv &handle_recv);
    void HandleDeleteCustomer(const s_HandleRecv &handle_recv);
    void HandleShowLogin(const s_HandleRecv &handle_recv);
    void HandleChat(const s_HandleRecv &handle_recv);
    void HandleTransferfile(const int flag, const s_HandleRecv &handle_recv);
    afx_msg void OnDestroy();
};
