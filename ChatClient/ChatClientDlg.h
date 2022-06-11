
// ChatClientDlg.h : ͷ�ļ�
//

#pragma once

#include <thread>
#include <mutex>
#include <unordered_map>
#include <queue>
#include "CMessage.h"
#include "public.h"

#define WM_SOCKET_CLIENT                 (WM_USER + 2001)
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
    virtual void OnOK();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnBnClickedSend();
    afx_msg void OnBnClickedTransferfile();
    afx_msg void OnSelchangeLoginPeople();
    afx_msg void OnSelchangeStatus();
    afx_msg LRESULT OnSocket(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
  public:
    std::string nickName;
    SOCKET socketClient{INVALID_SOCKET};
  private:
    std::unordered_map<std::string, std::string> userToChat;
    
    CListBox loginPeopleList;
    CComboBox statusCombo;
    bool is_service_open{ false };

    std::thread threadToFile;
    void threadTransFile();
    bool can_transfer_file{ false };
    std::mutex readFileMt;
    std::queue<std::pair<bool, cwy::File>> fileQue;
    
    
public:
    
    
    
    afx_msg void OnDestroy();
};
