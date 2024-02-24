
// ChatClientDlg.cpp : 实现文件
//

#include "ChatClient.h"
#include "ChatClientDlg.h"
#include "CMessage.h"

#include <map>

#include "protocol.h"
using namespace cwy;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ABOUTBOX };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CChatClientDlg 对话框

CChatClientDlg::CChatClientDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_CHATCLIENT_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChatClientDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LOGIN_PEOPLE, m_list_login_people);
    DDX_Control(pDX, IDC_STATUS, m_status);
    DDX_Control(pDX, IDC_TRANSFILEPROGRESS, m_transfer_progress);
}

BEGIN_MESSAGE_MAP(CChatClientDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_CTLCOLOR()
    ON_WM_DROPFILES()
    ON_WM_DESTROY()
    ON_LBN_SELCHANGE(IDC_LOGIN_PEOPLE, &CChatClientDlg::OnLbnSelchangeLoginPeople)
    ON_CBN_SELCHANGE(IDC_STATUS, &CChatClientDlg::OnCbnSelchangeStatus)
    ON_BN_CLICKED(IDC_SEND, &CChatClientDlg::OnBnClickedSend)
    ON_BN_CLICKED(IDC_TRANSFERFILE, &CChatClientDlg::OnBnClickedTransferfile)
    ON_STN_CLICKED(IDC_INFO, &CChatClientDlg::OnStnClickedInfo)
    ON_MESSAGE(WM_SOCKET, OnSocket)
    ON_MESSAGE(WM_TRANSFERFILEPROGRESS, OnTransferFileProgress)
    ON_WM_SIZE()
END_MESSAGE_MAP()

// CChatClientDlg 消息处理程序

BOOL CChatClientDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 将“关于...”菜单项添加到系统菜单中。

    // IDM_ABOUTBOX 必须在系统命令范围内。
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon(m_hIcon, TRUE);			// 设置大图标
    SetIcon(m_hIcon, FALSE);		// 设置小图标
    SetWindowText(customerName_.c_str());
    ModifyStyleEx(0, WS_EX_APPWINDOW);

    InitControl();
    //InitTransferFileSocket();
    GetClientRect(&wndRect_);

    if (!loginFlag_)
    {
        if (MessageBox(_T("你的账号存在密码泄露风险,是否更改密码?"), _T("警告"), MB_YESNO | MB_ICONASTERISK) == IDYES)
        {
            CChangePassword changePasswordDlg;
            changePasswordDlg.socketClient_ = socketClient_;
            changePasswordDlg.customerId_ = customerId_;
            changePasswordDlg.DoModal();
        }
    }
    ::WSAAsyncSelect(socketClient_, this->m_hWnd, WM_SOCKET, FD_READ);

    // get customer
    HandleRecv toSend;
    toSend.SetContent("id", customerId_);
    toSend.SetContent("customer", customerName_);
    std::string result = toSend.Write(CommunicationType::GETANDSHOW);
    ::send(socketClient_, result.c_str(), result.length(), 0);

    /*int uploadThread = GetPrivateProfileInt("CommonInfo", "UploadThread", 2, "./chat.ini");
    int downloadThread = GetPrivateProfileInt("CommonInfo", "DownloadThread", 2, "./chat.ini");
    for (unsigned short i = 0; i < uploadThread; ++i)
    {
        fileTransUpload_.emplace_back(std::thread(&CChatClientDlg::ThreadHandlerUpload, this, i));
    }
    for (unsigned short i = 0; i < downloadThread; ++i)
    {
        fileTransDownload_.emplace_back(std::thread(&CChatClientDlg::ThreadHandlerDownload, this, i));
    }*/
    return TRUE;
}

void CChatClientDlg::OnOK()
{
    OnBnClickedSend();
}

void CChatClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CChatClientDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // 用于绘制的设备上下文

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // 使图标在工作区矩形中居中
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // 绘制图标
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CChatClientDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

HBRUSH CChatClientDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  在此更改 DC 的任何特性
    if (pWnd->GetDlgCtrlID() == IDC_INFO)
    {
        pDC->SetTextColor(RGB(255, 0, 0));
        pDC->SetBkMode(TRANSPARENT);
    }

    // TODO:  如果默认的不是所需画笔，则返回另一个画笔
    return hbr;
}

void CChatClientDlg::OnDropFiles(HDROP hDropInfo)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    TCHAR filePath[MAX_PATH] = { 0 };
    DragQueryFile(hDropInfo, 0, filePath, sizeof(filePath));
    SetDlgItemText(IDC_TRANSFERFILE, filePath);
    DragFinish(hDropInfo);
    CDialogEx::OnDropFiles(hDropInfo);
}

void CChatClientDlg::OnDestroy()
{
    CDialogEx::OnDestroy();

    HandleRecv toSend;
    toSend.SetContent("id", customerId_);
    std::string result = toSend.Write(CommunicationType::DELCUSTOMER);
    ::send(socketClient_, result.c_str(), result.length(), 0);
    /*int uploadThread = GetPrivateProfileInt("CommonInfo", "UploadThread", 2, "./chat.ini");
    int downloadThread = GetPrivateProfileInt("CommonInfo", "DownloadThread", 2, "./chat.ini");
    threadExit_ = true;
    for (int i = 0; i < uploadThread; ++i)
    {
        if (fileTransUpload_[i].joinable())
        {
            fileTransUpload_[i].join();
        }
    }
    for (int i = 0; i < downloadThread; ++i)
    {
        if (fileTransDownload_[i].joinable())
        {
            fileTransDownload_[i].join();
        }
    }*/
}

void CChatClientDlg::OnLbnSelchangeLoginPeople()
{
    UpdateData();
    SetDlgItemText(IDC_INFO, _T(""));
    SetDlgItemText(IDC_SENDTEXT, _T(""));
    SetDlgItemText(IDC_FILENAME, _T(""));
    m_transfer_progress.SetPos(0);
    CString strTarget;
    int curSel = m_list_login_people.GetCurSel();
    m_list_login_people.GetText(curSel, strTarget);
    int pos = strTarget.Find(' ', 0);
    if (pos != -1)
    {
        CString newString = strTarget.Right(3);
        if (strcmp(newString, "new") == 0)
        {
            CString newShow = strTarget.Left(strTarget.GetLength() - 3);
            UpdateListBox(curSel, newShow.GetBuffer(0));
            m_list_login_people.SetCurSel(curSel);
            newShow.ReleaseBuffer();
        }
        UINT64 id = strtoull(strTarget.Left(pos).GetBuffer(0), nullptr, 10);
        auto chatMessageItor = chatMessage_.find(id);
        if (chatMessageItor != chatMessage_.end())
        {
            if (chatMessageItor->second.str().empty())
            {
                HandleRecv toSend;
                toSend.SetContent("requestid", customerId_);
                toSend.SetContent("id", strTarget.Left(pos).GetBuffer(0));
                std::string jsSend = toSend.Write(CommunicationType::INITCUSTOMERCHAT);
                ::send(socketClient_, jsSend.c_str(), jsSend.length(), 0);
            }
            else
            {
                SetDlgItemText(IDC_TEXT, chatMessageItor->second.str().c_str());
            }
        }
        auto progressNumItor = progressNum_.find(id);
        if (progressNumItor != progressNum_.end())
        {
            m_transfer_progress.SetPos(progressNumItor->second);
        }
    }
    strTarget.ReleaseBuffer();
    UpdateData(FALSE);
}

void CChatClientDlg::OnCbnSelchangeStatus()
{
    // TODO: 在此添加控件通知处理程序代码

}

void CChatClientDlg::OnBnClickedSend()
{
    int curSel = m_list_login_people.GetCurSel();
    if (curSel == CB_ERR)
    {
        MessageBox(_T("请选择一个用户！"), _T("错误"), MB_ICONERROR);
    }
    else
    {
        CString strSend, strSendTarget;
        m_list_login_people.GetText(curSel, strSendTarget);
        GetDlgItemText(IDC_SENDTEXT, strSend);
        if (strSend == "")
        {
            MessageBox(_T("发送内容不能为空...."), _T("错误"), MB_ICONERROR);
        }
        else
        {
            int pos = strSendTarget.Find(' ', 0);
            if (pos != -1)
            {
                HandleRecv toSend;
                toSend.SetContent("sourceid", customerId_);
                toSend.SetContent("targetid", strSendTarget.Left(pos).GetBuffer(0));
                toSend.SetContent("content", strSend.GetBuffer(0));
                std::string sysTime = GetSystemTime();
                toSend.SetContent("time", sysTime);
                std::string jsSend = toSend.Write(CommunicationType::CHAT);
                ::send(socketClient_, jsSend.c_str(), jsSend.length(), 0);
                UINT64 targetId = strtoull(toSend.GetContent("targetid").c_str(), nullptr, 10);
                chatMessage_[targetId] << sysTime << "  /send\r\n" << toSend.GetContent("content") << "\r\n\r\n";
                SetDlgItemText(IDC_TEXT, chatMessage_[targetId].str().c_str());
            }
            else
            {
                MessageBox(_T("内部错误,请重试!!!"), _T("错误"), MB_ICONERROR);
            }
        }
        strSend.ReleaseBuffer();
        strSendTarget.ReleaseBuffer();
        SetDlgItemText(IDC_SENDTEXT, "");
    }
}

void CChatClientDlg::OnBnClickedTransferfile()
{
    int curSel = m_list_login_people.GetCurSel();
    if (curSel == CB_ERR)
    {
        MessageBox(_T("请选择一个用户！"), _T("错误"), MB_ICONERROR);
    }
    else
    {
        CString strSendTarget, strFilePath;
        m_list_login_people.GetText(curSel, strSendTarget);
        GetDlgItemText(IDC_FILENAME, strFilePath);
        if (strFilePath.IsEmpty())
        {
            TCHAR szFilter[] = _T("文本文件(*.txt)|*.txt|所有文件(*.*)|*.*||");
            CFileDialog fileDlg(TRUE, _T("txt"), NULL, 0, szFilter, this);
            if (IDOK == fileDlg.DoModal())
            {
                strFilePath = fileDlg.GetPathName();
                SetDlgItemText(IDC_FILENAME, strFilePath);
            }
        }
        int pos = strSendTarget.Find('\t', 0);
        if (pos != -1)
        {
            std::lock_guard<std::mutex> lg(quMuUp_);
            transferFileUpload_.push(TransFile(strFilePath.GetBuffer(0), strtoull(strSendTarget.Left(pos).GetBuffer(0), nullptr, 10)));
        }
        strFilePath.ReleaseBuffer();
        strSendTarget.ReleaseBuffer();
    }
}

void CChatClientDlg::OnStnClickedInfo()
{
    CString strInfo;
    GetDlgItemText(IDC_INFO, strInfo);
    if (strInfo != "")
    {
        int pos1 = strInfo.Find(' ');
        int pos2 = strInfo.ReverseFind(' ');
        CString strId = strInfo.Mid(pos1 + 1, pos2 - pos1 - 1);
        UINT64 id = strtoull(strId.GetBuffer(0), nullptr, 10);
        auto itor1 = loginInfo_.find(id);
        int distance = std::distance(loginInfo_.begin(), itor1);
        char temp[100];
        memset(temp, 0, 100);
        sprintf_s(temp, 100, "%-20llu%-30s%s", id, loginInfo_[id].first.c_str(), loginInfo_[id].second.c_str());
        UpdateListBox(distance, temp);
        m_list_login_people.SetCurSel(distance);
        SetDlgItemText(IDC_TEXT, chatMessage_[id].str().c_str());
        SetDlgItemText(IDC_INFO, _T(""));
        strId.ReleaseBuffer();
    }
    strInfo.ReleaseBuffer();
}

LRESULT CChatClientDlg::OnSocket(WPARAM wParam, LPARAM lParam)
{
    char* strRecv = new char[DATA_LENGTH];
    memset(strRecv, 0, DATA_LENGTH);
    if (lParam == FD_READ)
    {
        UpdateData(TRUE);
        ::recv(socketClient_, strRecv, DATA_LENGTH, 0);
        HandleRecv handleRecv(socketClient_, strRecv);
        switch (handleRecv.GetType())
        {
            case CommunicationType::SHOWLOGIN:
            {
                if (handleRecv.GetContent("show_login_type") == "0")
                {
                    std::vector<std::string> peopleList;
                    SplitString(handleRecv.GetContent("customer"), COMBINE_INFOS, peopleList);
                    for (size_t i = 0; i < peopleList.size(); ++i)
                    {
                        std::vector<std::string> peopleInfo;
                        SplitString(peopleList.at(i), COMBINE_ONE_INFO, peopleInfo);
                        if (peopleInfo.size() != 3 || (peopleInfo.at(0) == customerId_))
                        {
                            continue;
                        }
                        loginInfo_[strtoull(peopleInfo.at(0).c_str(), nullptr, 10)] =
                            std::make_pair(peopleInfo.at(1), (peopleInfo.at(2) == "1") ? "在线" : "离线");
                    }
                    for (auto itor = loginInfo_.begin(); itor != loginInfo_.end(); ++itor)
                    {
                        char temp[100];
                        memset(temp, 0, 100);
                        sprintf_s(temp, 100, "%-20llu%-30s%s", itor->first, itor->second.first.c_str(), itor->second.second.c_str());
                        m_list_login_people.AddString(temp);
                        chatMessage_.insert(std::make_pair(itor->first, ""));
                    }
                }
                else if ((handleRecv.GetContent("show_login_type") == "1") || (handleRecv.GetContent("show_login_type") == "-1"))
                {
                    std::vector<std::string> peopleInfo;
                    SplitString(handleRecv.GetContent("customer"), COMBINE_ONE_INFO, peopleInfo);
                    if (peopleInfo.size() != 3 || (peopleInfo.at(0) == customerId_))
                    {
                        break;
                    }
                    UINT64 id = strtoull(peopleInfo.at(0).c_str(), nullptr, 10);
                    loginInfo_[id] = std::make_pair(peopleInfo.at(1), (handleRecv.GetContent("show_login_type") == "1") ? "在线" : "离线");
                    auto itor = loginInfo_.find(id);
                    int pos = std::distance(loginInfo_.begin(), itor);
                    char temp[100];
                    memset(temp, 0, 100);
                    sprintf_s(temp, 100, "%-20llu%-30s%s", id, loginInfo_[id].first.c_str(), loginInfo_[id].second.c_str());
                    UpdateListBox(pos, temp);
                }
            }
            break;

            case CommunicationType::CHAT:
            {
                if (handleRecv.GetContent("targetid") == customerId_)
                {
                    std::ostringstream chatResult;
                    // 消息添加
                    UINT64 sourceId = strtoull(handleRecv.GetContent("sourceid").c_str(), nullptr, 10);
                    chatMessage_[sourceId] << handleRecv.GetContent("time") << "  /recv\r\n" << handleRecv.GetContent("content") << "\r\n\r\n";
                    // 新消息显示
                    auto loginInfoItor = loginInfo_.find(sourceId);
                    if (loginInfoItor != loginInfo_.end())
                    {
                        int distance = std::distance(loginInfo_.begin(), loginInfoItor);
                        int pos = m_list_login_people.GetCurSel();
                        if (distance != pos)
                        {
                            char temp[100];
                            memset(temp, 0, 100);
                            sprintf_s(temp, 100, "%-20llu%-30s%s new", loginInfoItor->first, loginInfoItor->second.first.c_str(), loginInfoItor->second.second.c_str());
                            UpdateListBox(distance, temp);
                            memset(temp, 0, 100);
                            sprintf_s(temp, 100, "收到来自 %llu %s的新消息", loginInfoItor->first, loginInfoItor->second.first.c_str());
                            SetDlgItemText(IDC_INFO, temp);
                        }
                        else
                        {
                            SetDlgItemText(IDC_TEXT, chatMessage_[sourceId].str().c_str());
                        }
                    }
                }
            }
            break;

            case CommunicationType::FORCEDELETE:
            {
                if (handleRecv.GetContent("id") == customerId_)
                {
                    MessageBox(_T("您的账号已在其他地点登录,请确认!!!"), _T("错误"), MB_ICONERROR);
                    EndDialog(-1);
                }
            }
            break;

            case CommunicationType::TRANSFERFILEINFO:
            {
                /*if (handleRecv.Param.transferFileInfo_.targetid == customerId_)
                {
                    std::lock_guard<std::mutex> lg(quMuDown_);
                    transferFileDownload_.push(TransFile(handleRecv.Param.transferFileInfo_.file_name, handleRecv.Param.transferFileInfo_.sourceid,
                        handleRecv.Param.transferFileInfo_.file_length, handleRecv.Param.transferFileInfo_.file_block));
                }*/
            }
            break;

            case CommunicationType::INITCUSTOMERCHATBACK:
            {
                if (handleRecv.GetContent("id") == customerId_)
                {
                    UINT64 contentId = strtoull(handleRecv.GetContent("contentid").c_str(), nullptr, 10);
                    std::string sourceContent = handleRecv.GetContent("sourcecontent");
                    std::string targetContent = handleRecv.GetContent("targetcontent");
                    if ((sourceContent.length() != 0) || (targetContent.length() != 0))
                    {
                        std::vector<std::string> sourceContentVec, targetContentVec, allContentVec;
                        SplitString(sourceContent, COMBINE_INFOS, sourceContentVec);
                        SplitString(targetContent, COMBINE_INFOS, targetContentVec);
                        std::sort(sourceContentVec.begin(), sourceContentVec.end());
                        std::sort(targetContentVec.begin(), targetContentVec.end());
                        allContentVec.resize(sourceContentVec.size() + targetContentVec.size());
                        std::merge(sourceContentVec.begin(), sourceContentVec.end(), targetContentVec.begin(), targetContentVec.end(), allContentVec.begin());
                        std::ostringstream ss;
                        for (size_t i = 0; i < allContentVec.size(); ++i)
                        {
                            std::vector<std::string> contentVec;
                            SplitString(allContentVec.at(i), COMBINE_ONE_INFO, contentVec);
                            if (contentVec.size() == 3) // source : time&content&type
                            {
                                ss << contentVec.at(0) << "  /send\r\n" << contentVec.at(1) << "\r\n\r\n";
                            }
                            else if (contentVec.size() == 4) // target : time&content&isRead&type
                            {
                                ss << contentVec.at(0) << "  /recv\r\n" << contentVec.at(1) << "\r\n\r\n";
                            }
                        }
                        chatMessage_[contentId] << ss.str();
                        SetDlgItemText(IDC_TEXT, ss.str().c_str());
                        GetDlgItem(IDC_TEXT)->align
                    }
                }
            }
            break;
        }
        UpdateData(FALSE);
    }
    delete[]strRecv;
    strRecv = nullptr;

    return 0;
}

LRESULT CChatClientDlg::OnTransferFileProgress(WPARAM wParam, LPARAM lParam)
{
    UpdateData(TRUE);
    double progress = (double)wParam;
    UINT64 id = (UINT64)lParam;
    int pos = static_cast<int>(progress * 100);
    m_transfer_progress.SetPos(pos);
    progressNum_[id] = ((pos == 100) ? 0 : pos);
    UpdateData(FALSE);

    return 0;
}

void CChatClientDlg::InitControl()
{
    CFont font, fontLogin;
    font.CreatePointFont(150, _T("宋体"), NULL);
    UINT32 itmNum = sizeof(ALL_ITM) / sizeof(int);
    for (UINT32 i = 0; i < itmNum - 1; ++i)
    {
        GetDlgItem(ALL_ITM[i])->SetFont(&font);
    }

    // 登录信息
    SetDlgItemText(IDC_ID, customerId_.c_str());
    SetDlgItemText(IDC_NAME, customerName_.c_str());

    // 聊天
    SetDlgItemText(IDC_INFO, _T(""));

    // 消息框 进度条
    GetDlgItem(IDC_TRANSFILEPROGRESS)->ShowWindow(false);
    m_transfer_progress.SetRange(0, 100);
    m_transfer_progress.SetPos(0);

    fontLogin.CreateStockObject(SYSTEM_FIXED_FONT);
    GetDlgItem(IDC_LOGIN_PEOPLE)->SetFont(&fontLogin);
    fontLogin.DeleteObject();

    // 状态栏
    m_status.AddString(_T("在线"));
    m_status.AddString(_T("忙碌"));
    m_status.AddString(_T("离开"));
    m_status.AddString(_T("离线"));
    m_status.AddString(_T("自定义"));
    m_status.SetCurSel(0);

    DragAcceptFiles(TRUE);
}

void CChatClientDlg::InitTransferFileSocket()
{
    socketTranserFile_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == socketTranserFile_)
    {
        MessageBox(_T("创建文件传输服务失败,请重试！"), _T("错误"), MB_ICONERROR);
        return;
    }

    char ip[20];
    memset(ip, 0, 20);
    GetPrivateProfileString("CommonInfo", "ServerIp", "127.0.0.1", ip, 20, "./chat.ini");
    unsigned int port = GetPrivateProfileInt("CommonInfo", "TransferPort", 6004, "./chat.ini");

    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(port);
    addr.sin_addr.S_un.S_addr = inet_addr(ip);

    if (::connect(socketTranserFile_, (SOCKADDR*)&addr, sizeof(addr)) != 0)
    {
        MessageBox(_T("连接文件传输端口失败，请重试！"), _T("错误"), MB_ICONERROR);
        return;
    }
    ::WSAAsyncSelect(socketTranserFile_, this->m_hWnd, WM_SOCKET_FILE, FD_READ);
}

inline void CChatClientDlg::UpdateListBox(int pos, const std::string& newMessage)
{
    m_list_login_people.DeleteString(pos);
    m_list_login_people.InsertString(pos, newMessage.c_str());
}

void CChatClientDlg::ThreadHandlerUpload(const unsigned short threadNum)
{
    /*while (1)
    {
        if (threadExit_)
        {
            break;
        }
        TransFile transFile;
        {
            std::lock_guard<std::mutex> lg(quMuUp_);
            if (transferFileUpload_.empty())
            {
                Sleep(1);
                continue;
            }
            transFile = transferFileUpload_.front();
            transferFileUpload_.pop();
        }
        GetDlgItem(IDC_TRANSFILEPROGRESS)->ShowWindow(true);
        CFile file;
        file.Open(transFile.fileName_.c_str(), CFile::modeRead | CFile::typeBinary);
        UINT64 len = file.GetLength();
        s_HandleRecv toSend;
        toSend.Param.transferFileInfo_.sourceid = this->customerId_;
        toSend.Param.transferFileInfo_.targetid = transFile.id_;
        std::string fileName = transFile.fileName_.substr(transFile.fileName_.find_last_of('\\') + 1);
        RegisterSpace(&toSend.Param.transferFileInfo_.file_name, fileName);
        toSend.Param.transferFileInfo_.file_length = len;
        UINT64 fileSize = (UINT64)ceil((double)((float)len / FILE_DATA_LENGTH));
        toSend.Param.transferFileInfo_.file_block = fileSize;
        RegisterSpace(&toSend.Param.transferFileInfo_.transfer_time, GetSystemTime());
        std::string jsSend = EncodeJson(CommunicationType::TRANSFERFILEINFO, toSend);
        UnregisterSpace(CommunicationType::TRANSFERFILEINFO, toSend);
        ::send(socketClient_, jsSend.c_str(), jsSend.length(), 0);
        Sleep(100);
        toSend.Clear();
        char* getStr = new char[FILE_DATA_LENGTH];
        for (unsigned int nowBlock = 0; nowBlock < fileSize; ++nowBlock)
        {
            memset(getStr, 0, FILE_DATA_LENGTH);
            file.Read(getStr, FILE_DATA_LENGTH);
            RegisterSpace(&toSend.Param.transferFileContent_.file_content, getStr);
            toSend.Param.transferFileContent_.now_block = nowBlock++;
            std::string jsSend = EncodeJson(CommunicationType::TRANSFERFILECONTENT, toSend);
            UnregisterSpace(CommunicationType::TRANSFERFILECONTENT, toSend);
            ::send(socketClient_, jsSend.c_str(), jsSend.length(), 0);
            file.Seek(FILE_DATA_LENGTH * toSend.Param.transferFileContent_.now_block, CFile::begin);
            double progress = static_cast<double>(nowBlock / static_cast<double>(fileSize));
            PostMessage(WM_TRANSFERFILEPROGRESS, (WPARAM)progress, 0);
            Sleep(500);
        }

        delete[]getStr;
        getStr = nullptr;
        file.Close();
        Sleep(1000);
        GetDlgItem(IDC_TRANSFILEPROGRESS)->ShowWindow(false);

        Sleep(1);
    }*/

    return;
}

void CChatClientDlg::ThreadHandlerDownload(const unsigned short threadNum)
{
    /*while (1)
    {
        if (threadExit_)
        {
            break;
        }
        TransFile transFile;
        {
            std::lock_guard<std::mutex> lg(quMuDown_);
            if (transferFileDownload_.empty())
            {
                Sleep(1);
                continue;
            }
            transFile = transferFileDownload_.front();
            transferFileDownload_.pop();
        }
        CString pathName;
        TCHAR szFilter[] = _T("所有文件(*.*)|*.*||");
        CFileDialog fileDlg(FALSE, transFile.fileName_.substr(transFile.fileName_.find_last_of('.') + 1).c_str(), transFile.fileName_.c_str(),
            0, szFilter, this);
        if (IDOK == fileDlg.DoModal())
        {
            pathName = fileDlg.GetPathName();
        }
        GetDlgItem(IDC_TRANSFILEPROGRESS)->ShowWindow(true);
        std::vector<std::string> fileSequence(transFile.fileBlock_);
        char* getStr = new char[FILE_DATA_LENGTH];
        for (UINT64 i = 0; i < transFile.fileBlock_; ++i)
        {
            memset(getStr, 0, FILE_DATA_LENGTH);
            ::recv(socketTranserFile_, getStr, FILE_DATA_LENGTH, 0);
            s_HandleRecv handleRecv;
            if (!DecodeJson(getStr, handleRecv))
            {
                continue;
            }
            fileSequence[handleRecv.Param.transferFileContent_.now_block] = std::string(handleRecv.Param.transferFileContent_.file_content);
            UnregisterSpace(CommunicationType::TRANSFERFILECONTENT, handleRecv);
            double progress = static_cast<double>(i / static_cast<double>(transFile.fileBlock_ * 2));
            PostMessage(WM_TRANSFERFILEPROGRESS, (WPARAM)progress, 0);
            Sleep(500);
        }
        delete[]getStr;
        getStr = nullptr;
        CFile file;
        file.Open(transFile.fileName_.c_str(), CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
        for (size_t i = 0; i < fileSequence.size(); ++i)
        {
            file.Write(fileSequence.at(i).c_str(), fileSequence.at(i).length());
            file.SeekToEnd();
            double progress = static_cast<double>((i + transFile.fileBlock_) / static_cast<double>(transFile.fileBlock_ * 2));
            PostMessage(WM_TRANSFERFILEPROGRESS, (WPARAM)progress, 0);
        }
        file.Close();
        Sleep(1);
        GetDlgItem(IDC_TRANSFILEPROGRESS)->ShowWindow(false);
    }*/

    return;
}

void CChatClientDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);

    UINT32 itmNum = sizeof(ALL_ITM) / sizeof(int);
    for (UINT32 i = 0; i < itmNum; i++)
    {
        CWnd* pWnd = GetDlgItem(ALL_ITM[i]);

        if (pWnd && nType != 1 && wndRect_.Width() && wndRect_.Height())
        {
            CRect rect;
            pWnd->GetWindowRect(&rect);
            ScreenToClient(&rect);
            rect.left = rect.left * cx / wndRect_.Width();
            rect.right = rect.right * cx / wndRect_.Width();
            rect.top = rect.top * cy / wndRect_.Height();
            rect.bottom = rect.bottom * cy / wndRect_.Height();
            pWnd->MoveWindow(rect);
        }
    }
    GetClientRect(&wndRect_);
}
