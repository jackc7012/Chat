
// ChatServiceDlg.h : ͷ�ļ�
//

#pragma once

#define WM_SOCKET                (WM_USER + 1000)

#include <vector>
#include <map>
#include <mutex>
#include <queue>

#include "public.h"
#include "CLog.h"

// CChatServiceDlg �Ի���
class CChatServiceDlg : public CDialogEx {
// ����
  public:
    CChatServiceDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_CHATSERVICE_DIALOG };
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
    afx_msg void OnBnClickedStart();
    SOCKET socket_service;
    SOCKADDR_IN addr_service, addr_accept;
    std::vector<SOCKET> socket_accept;
    DWORD accpet_count{ 0 };
    afx_msg LRESULT OnSocket(WPARAM wParam, LPARAM lParam);
    CListBox m_list_login_people;
    std::map<std::string, SOCKET> name_to_socket_accept;
    void HandleRecv(const s_HandleRecv &handle_recv);
    void HandleRecvRegister(const s_HandleRecv &handle_recv);
    void HandleRecvLogin(const s_HandleRecv &handle_recv);
    void HandleRecvDeleteCustomer(const s_HandleRecv &handle_recv);
    void HandleRecvChat(const s_HandleRecv &handle_recv);
    void HandleRecvTransferFileRequest(const s_HandleRecv &handle_recv);
    void HandleRecvTransferFileRespond(const s_HandleRecv &handle_recv);
    void HandleRecvTransferFile(const s_HandleRecv &handle_recv);
    std::mutex mt_server_handle;
    afx_msg void OnBnClickedKick();
    std::queue<std::string> task_queue;
    CLog g_log;
    void InitFont();
    /*int thread1();
    int thread2();
    int thread3();
    int thread4();
    int thread5();*/
    afx_msg void OnDestroy();
};
