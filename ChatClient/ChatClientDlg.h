
// ChatClientDlg.h : 头文件
//

#pragma once
#include <vector>
#include <queue>
#include <unordered_map>

#include "CLog.h"
#include "CChangePassword.h"


#define WM_SOCKET                 (WM_USER + 1000)
#define WM_TRANSFERFILEPROGRESS   (WM_USER + 1001)

struct TransFile {
    TransFile()
        : mode_(0), id_(0), fileLength_(0), fileBlock_(0)
    {

    }
    TransFile(const int mode, const std::string& fileName, const UINT64 id, const UINT64 fileLength = 0,
        const unsigned long long fileBlock = 0)
        : mode_(mode), fileName_(fileName), id_(id), fileLength_(fileLength), fileBlock_(fileBlock)
    {

    }
    int mode_; // 0 upload 1 download
    std::string fileName_;
    UINT64 id_;
    UINT64 fileLength_;
    UINT64 fileBlock_;
};

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

    DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    virtual void OnOK();

public:
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnDropFiles(HDROP hDropInfo);
    afx_msg void OnDestroy();

    afx_msg void OnLbnSelchangeLoginPeople();
    afx_msg void OnCbnSelchangeStatus();
    afx_msg void OnBnClickedSend();
    afx_msg void OnBnClickedTransferfile();
    afx_msg void OnStnClickedInfo();

public:
    afx_msg LRESULT OnSocket(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnTransferFileProgress(WPARAM wParam, LPARAM lParam);

private:
    void InitControl();
    inline void UpdateListBox(int pos, const std::string& newMessage);

    void ThreadHandler(const unsigned short threadNum);

public:
    UINT64 customerId_{ 0 };
    std::string customerName_;
    SOCKET socketClient_{ 0 };
    bool loginFlag_{ true }; // true 登录 false 其他ip已登录

protected:
    HICON m_hIcon;
    CListBox m_list_login_people;
    CComboBox m_status;
    CProgressCtrl m_transfer_progress;

private:
    std::unordered_map<UINT64, std::string> chatMessage_; // id : chatmessage
    std::unordered_map<UINT64, std::pair<std::string, std::string>> loginInfo_; // id : {nick_name, status}
    std::unordered_map<UINT64, long> progressNum_;
    std::vector<std::thread> fileTrans_;
    std::queue<TransFile> transferFile_;
    std::mutex quMu_;
    bool threadExit_{ false };

    cwy::CLog g_log;
};
