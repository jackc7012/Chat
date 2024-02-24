
// ChatClientDlg.h : 头文件
//

#pragma once
#include <vector>
#include <mutex>
#include <queue>
#include <unordered_map>

#include "CChangePassword.h"

#include "info.h"

/*
   * message
   * sysTime << "  /send(recv)\r\n" << content << "\r\n\r\n";
 */

#define WM_SOCKET                   (WM_USER + 1000)
#define WM_SOCKET_FILE              (WM_SOCKET + 1)
#define WM_TRANSFERFILEPROGRESS     (WM_SOCKET_FILE + 1)

static const int ALL_ITM[] = { IDC_STATICINFO, IDC_STATICLOGIN, // 组框
                               IDC_STATICID_LOGIN, IDC_ID, IDC_STATICNAME, IDC_NAME, // 登录信息
                               IDC_TEXT, IDC_SENDTEXT, IDC_SEND, IDC_FILENAME, IDC_TRANSFERFILE, IDC_INFO, //聊天
                               IDC_TRANSFILEPROGRESS, // 进度条
                               IDC_LOGIN_PEOPLE // 人员信息
};

struct TransFile
{
    TransFile()
        : id_(0), fileLength_(0), fileBlock_(0)
    {

    }
    TransFile(const std::string& fileName, const UINT64 id, const UINT64 fileLength = 0, const UINT64 fileBlock = 0)
        : fileName_(fileName), id_(id), fileLength_(fileLength), fileBlock_(fileBlock)
    {

    }
    std::string fileName_;
    UINT64 id_;
    UINT64 fileLength_;
    UINT64 fileBlock_;
};

// CChatClientDlg 对话框
class CChatClientDlg : public CDialogEx
{
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
    void InitTransferFileSocket();
    inline void UpdateListBox(int pos, const std::string& newMessage);

    void ThreadHandlerUpload(const unsigned short threadNum);
    void ThreadHandlerDownload(const unsigned short threadNum);

public:
    std::string customerId_;
    std::string customerName_;
    SOCKET socketClient_{ INVALID_SOCKET };
    bool loginFlag_{ true }; // true 登录 false 其他ip已登录
    cwy::Info info_;

protected:
    HICON m_hIcon;
    CListBox m_list_login_people;
    CComboBox m_status;
    CProgressCtrl m_transfer_progress;
    CRect wndRect_;

private:
    SOCKET socketTranserFile_{ INVALID_SOCKET };
    std::unordered_map<UINT64, std::ostringstream> chatMessage_; // id(send : targetid, recv : sourceid) : chatmessage
    std::unordered_map<UINT64, std::pair<std::string, std::string>> loginInfo_; // id : {nick_name, status}
    std::unordered_map<UINT64, int> progressNum_;
    std::vector<std::thread> fileTransUpload_, fileTransDownload_;
    std::queue<TransFile> transferFileUpload_, transferFileDownload_;
    std::mutex quMuUp_, quMuDown_;
    bool threadExit_{ false };
public:
    afx_msg void OnSize(UINT nType, int cx, int cy);
};
