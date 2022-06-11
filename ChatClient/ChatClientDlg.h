
// ChatClientDlg.h : 头文件
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
