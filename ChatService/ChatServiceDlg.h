
// ChatServiceDlg.h : ͷ�ļ�
//

#pragma once

#define WM_SOCKET_TCP               (WM_USER + 1000)
#define WM_SOCKET_UDP               (WM_USER + 1001)

#include <vector>

#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>

#include "public.h"
#include "CLog.h"
#include "CNetWorkHandle.h"

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
    SOCKET socketServiceTcp{ 0 }, socketServiceUdp{ 0 };
    SOCKADDR_IN addrServiceTcp{ 0 }, addrAccept{ 0 }, addrServiceUdp{ 0 };
    afx_msg LRESULT OnSocketTcp(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSocketUdp(WPARAM wParam, LPARAM lParam);
    std::vector<std::string> ve_accept_name;
    afx_msg void OnBnClickedKick();
    bool StartTcp();
    bool StartUdp();
    virtual BOOL DestroyWindow();
    std::unordered_map<SOCKET, std::string> socket2IpMap;
    afx_msg void OnDestroy();
    std::unique_ptr<cwy::CNetWorkHandle> netWorkHandle{nullptr};
    CListBox listLoginPeople;
    LRESULT HandleControlUpdate(WPARAM wParam, LPARAM lParam);
};
