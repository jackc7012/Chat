
// ChatClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ChatClient.h"
#include "ChatClientDlg.h"
#include "afxdialogex.h"

#include <sstream>
#include <math.h>
#include "public.h"
#include "CLog.h"
using namespace cwy;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx {
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

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX) {
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
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
    DDX_Control(pDX, IDC_LOGIN_PEOPLE, loginPeopleList);
    DDX_Control(pDX, IDC_STATUS, statusCombo);
}

BEGIN_MESSAGE_MAP(CChatClientDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_SEND, &CChatClientDlg::OnBnClickedSend)
    ON_BN_CLICKED(IDC_TRANSFERFILE, &CChatClientDlg::OnBnClickedTransferfile)
    ON_LBN_SELCHANGE(IDC_LOGIN_PEOPLE, &CChatClientDlg::OnSelchangeLoginPeople)
    ON_MESSAGE(WM_SOCKET_CLIENT, OnSocket)
    ON_CBN_SELCHANGE(IDC_STATUS, &CChatClientDlg::OnSelchangeStatus)
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
    if (pSysMenu != NULL) {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty()) {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon(m_hIcon, TRUE);         // 设置大图标
    SetIcon(m_hIcon, FALSE);        // 设置小图标

    CFont font;
    font.CreatePointFont(150, _T("宋体"), NULL);
    // 组框
    GetDlgItem(IDC_STATICINFO)->SetFont(&font);
    GetDlgItem(IDC_STATICLOGIN)->SetFont(&font);
    GetDlgItem(IDC_STATICCHATINFO)->SetFont(&font);
    GetDlgItem(IDC_STATICFILEINFO)->SetFont(&font);
    // 用户信息
    GetDlgItem(IDC_STATICNAME)->SetFont(&font);
    GetDlgItem(IDC_NAME)->SetFont(&font);
    GetDlgItem(IDC_STATICSTATUS)->SetFont(&font);
    GetDlgItem(IDC_STATUS)->SetFont(&font);
    GetDlgItem(IDC_LOGIN_PEOPLE)->SetFont(&font);
    // 聊天
    GetDlgItem(IDC_TEXT)->SetFont(&font);
    GetDlgItem(IDC_SENDTEXT)->SetFont(&font);
    GetDlgItem(IDC_SEND)->SetFont(&font);
    GetDlgItem(IDC_FILENAME)->SetFont(&font);
    GetDlgItem(IDC_TRANSFERFILE)->SetFont(&font);
    GetDlgItem(IDC_FILERECV)->SetFont(&font);

    statusCombo.SetCurSel(0);

    SetDlgItemText(IDC_NAME, nickName.c_str());

    if (::WSAAsyncSelect(socketClient, this->m_hWnd, WM_SOCKET_CLIENT, FD_READ) == SOCKET_ERROR) {
        MessageBox(_T("创建异步TCP服务失败，请重试"), _T("错误"), MB_ICONERROR);
    }
    logClient.InitLog(nickName);

    return TRUE;
}

void CChatClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    } else {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CChatClientDlg::OnPaint()
{
    if (IsIconic()) {
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
    } else {
        CDialogEx::OnPaint();
    }
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CChatClientDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CChatClientDlg::OnBnClickedSend()
{
    CString strText, strSend, strSendTarget;
    s_HandleRecv toSend;
    int curSel = loginPeopleList.GetCurSel();
    if (curSel == CB_ERR) {
        MessageBox(_T("请选择一个用户！"), _T("错误"), MB_ICONERROR);
    } else {
        GetDlgItemText(IDC_SENDTEXT, strSend);
        loginPeopleList.GetText(curSel, strSendTarget);
        if (strSend == "") {
            MessageBox(_T("发送内容不能为空...."), _T("错误"), MB_ICONERROR);
        } else {
            RegisterSpace(&toSend.Param.chat_.source, nickName);
            RegisterSpace(&toSend.Param.chat_.target, strSendTarget.GetBuffer(0));
            RegisterSpace(&toSend.Param.chat_.content, strSend.GetBuffer(0));
            std::string jsSend = EncodeJson(CommunicationType::CHAT, toSend);
            UnregisterSpace(CommunicationType::CHAT, toSend);
            logClient << "send : " << jsSend;
            logClient.PrintlogDebug(FILE_FORMAT);
            ::send(socketClient, jsSend.c_str(), jsSend.length(), 0);
            GetDlgItemText(IDC_TEXT, strText);
            std::ostringstream strTextIn;
            strTextIn << strText.GetBuffer(0) << nickName << "  " << GetTime() << " send\r\n" << strSend << "\r\n\r\n";
            SetDlgItemText(IDC_TEXT, strTextIn.str().c_str());
            SetDlgItemText(IDC_SENDTEXT, "");
            auto itor = userToChat.find(strSendTarget.GetBuffer(0));
            if (itor != userToChat.end()) {
                itor->second = strTextIn.str();
            }
        }
    }
}

void CChatClientDlg::OnBnClickedTransferfile()
{
    CString strSendTarget;
    int curSel = loginPeopleList.GetCurSel();
    if (curSel == CB_ERR) {
        MessageBox(_T("请选择一个用户！"), _T("错误"), MB_ICONERROR);
    } else {
        loginPeopleList.GetText(curSel, strSendTarget);
        TCHAR szFilter[] = _T("所有文件(*.*)|*.*||");
        CFileDialog fileDlg(TRUE, _T("*"), NULL, 0, szFilter, this);
        CString strFilePath, strFileName;
        if (IDOK == fileDlg.DoModal()) {
            strFilePath = fileDlg.GetPathName();
            strFileName = fileDlg.GetFileName();
            SetDlgItemText(IDC_FILENAME, strFilePath);
            s_HandleRecv toSend;
            CFile fileToTrans;
            fileToTrans.Open(strFilePath, CFile::modeRead | CFile::typeBinary);
            RegisterSpace(&toSend.Param.transferFileRequest_.source, nickName);
            RegisterSpace(&toSend.Param.transferFileRequest_.target, strSendTarget.GetBuffer());
            RegisterSpace(&toSend.Param.transferFileRequest_.file_name, strFileName.GetBuffer());
            toSend.Param.transferFileRequest_.file_length = fileToTrans.GetLength();
            std::string jsSend = EncodeJson(CommunicationType::TRANSFERFILEREQUEST, toSend);
            UnregisterSpace(CommunicationType::TRANSFERFILEREQUEST, toSend);
            logClient << "trans file send : " << jsSend;
            logClient.PrintlogDebug(FILE_FORMAT);
            ::send(socketClient, jsSend.c_str(), jsSend.length(), 0);
            fileToTrans.Close();
            std::thread threadToSe(&CChatClientDlg::threadTransFile, this, strFilePath.GetBuffer(), strSendTarget.GetBuffer(), true);
            threadToSe.join();
        }
    }
}

void CChatClientDlg::OnOK()
{
    OnBnClickedSend();
}

LRESULT CChatClientDlg::OnSocket(WPARAM wParam, LPARAM lParam)
{
    UpdateData(TRUE);
    switch (lParam) {
        case FD_READ:
        {
            char strRecv[DATA_LENGTH] = {0};
            int ret = ::recv(socketClient, strRecv, DATA_LENGTH, 0);
            if (ret < 0) {
                break;
            }
            logClient << "recv : " << strRecv;
            logClient.PrintlogDebug(FILE_FORMAT);
            s_HandleRecv handleRecv;
            DecodeJson(strRecv, handleRecv);
            switch (handleRecv.type_) {
                case CommunicationType::LOGINBOARDCAST: {
                    std::vector<std::string> customers;
                    SplitString(handleRecv.Param.loginBoardcast_.customer, '|', customers);
                    for (const auto& itor : customers) {
                        loginPeopleList.AddString(itor.c_str());
                        userToChat.insert(std::make_pair(itor, ""));
                    }
                    break;
                }

                case CommunicationType::CHAT: {
                    std::string source(handleRecv.Param.chat_.source);
                    auto itor = userToChat.find(source);
                    std::ostringstream ss;
                    if (itor != userToChat.end()) {
                        ss << source.substr(0, source.find_first_of('-')) << "  "
                           << GetTime() << " recv\r\n" << handleRecv.Param.chat_.content << "\r\n\r\n";
                        itor->second += ss.str();
                    }
                    int nowIndex = loginPeopleList.GetCurSel();
                    int index = loginPeopleList.FindString(-1, source.c_str());
                    if (index >= 0 && nowIndex != index) {
                        CString userName;
                        loginPeopleList.GetText(index, userName);
                        userName += "*";
                        loginPeopleList.DeleteString(index);
                        loginPeopleList.InsertString(index, userName);
                    }
                    if (nowIndex == index) {
                        CString strMon;
                        GetDlgItemText(IDC_TEXT, strMon);
                        strMon += ss.str().c_str();
                        SetDlgItemText(IDC_TEXT, strMon);
                    }
                    break;
                }

                case CommunicationType::TRANSFERFILEREQUEST: {
                    s_HandleRecv toSend;
                    std::string targetName = std::string(handleRecv.Param.transferFileRequest_.target);
                    if (nickName.compare(targetName.substr(0, targetName.find_first_of('-'))) != 0) {
                        break;
                    }
                    CMessage confirmDlg;
                    std::ostringstream ss;
                    ss << handleRecv.Param.transferFileRequest_.source << "向你发送"
                        << handleRecv.Param.transferFileRequest_.file_name << "文件，大小为"
                        << handleRecv.Param.transferFileRequest_.file_length << "字节，\r\n是否现在保存？";
                    RegisterSpace(&toSend.Param.transferFileRespond_.source, handleRecv.Param.transferFileRequest_.source);
                    RegisterSpace(&toSend.Param.transferFileRespond_.target, handleRecv.Param.transferFileRequest_.target);
                    RegisterSpace(&toSend.Param.transferFileRespond_.file_name, handleRecv.Param.transferFileRequest_.file_name);
                    std::ostringstream filePath("../");
                    filePath << nickName << "/recv/" << handleRecv.Param.transferFileRequest_.source;
                    confirmDlg.message = ss.str();
                    confirmDlg.filePath = filePath.str();
                    confirmDlg.fileName = handleRecv.Param.transferFileRequest_.file_name;
                    int ret = confirmDlg.DoModal();
                    if ((ret == 0) || (ret == 1)) {
                        RegisterSpace(&toSend.Param.transferFileRespond_.transfer_result, "agree");
                        std::thread threadToAcc(&CChatClientDlg::threadTransFile, this, confirmDlg.filePath, "", false);
                        threadToAcc.join();
                    } else if (ret == -1) {
                        RegisterSpace(&toSend.Param.transferFileRespond_.transfer_result, "refuse");
                    }
                    std::string jsSend = EncodeJson(CommunicationType::TRANSFERFILERESPOND, toSend);
                    UnregisterSpace(CommunicationType::TRANSFERFILERESPOND, toSend);
                    ::send(socketClient, jsSend.c_str(), jsSend.length(), 0);
                }
                break;

                case CommunicationType::TRANSFERFILERESPOND: {


                    break;
                }
            }
            UnregisterSpace(handleRecv.type_, handleRecv);
        }
        break;

        default:
            break;
    }
    UpdateData(FALSE);
    return 0;
}

// flag : true 传送文件 false 接受文件
void CChatClientDlg::threadTransFile(const std::string& filePath, const std::string& target, const bool flag)
{
    s_HandleRecv toSend;
    CFile fileToTrans;
    if (flag) {
        fileToTrans.Open(filePath.c_str(), CFile::modeRead | CFile::typeBinary);
        unsigned long long len = fileToTrans.GetLength();
        unsigned int now_block = 1;
        char* strSend = new char[DATA_LENGTH];
        RegisterSpace(&toSend.Param.transferFile_.source, nickName);
        RegisterSpace(&toSend.Param.transferFile_.target, target);
        RegisterSpace(&toSend.Param.transferFile_.file_name, fileToTrans.GetFileName().GetBuffer());
        toSend.Param.transferFile_.file_length = len;
        toSend.Param.transferFile_.file_block = static_cast<unsigned int>(ceil(static_cast<double>(len / DATA_LENGTH)));
        while (now_block <= toSend.Param.transferFile_.file_block) {
            memset(strSend, 0, DATA_LENGTH);
            fileToTrans.Read(strSend, DATA_LENGTH);
            RegisterSpace(&toSend.Param.transferFile_.file_content, strSend);
            toSend.Param.transferFile_.current_block = now_block++;
            std::string jsSend = EncodeJson(CommunicationType::TRANSFERFILE, toSend);
            ::send(socketClient, jsSend.c_str(), jsSend.length(), 0);
            fileToTrans.Seek(static_cast<long long>(DATA_LENGTH) * toSend.Param.transferFile_.current_block, CFile::begin);
            Sleep(1);
        }
        UnregisterSpace(CommunicationType::TRANSFERFILE, toSend);
        delete[]strSend;
    } else {
        fileToTrans.Open(filePath.c_str(), CFile::modeWrite | CFile::modeCreate | CFile::typeBinary);
    }
    fileToTrans.Close();
}

void CChatClientDlg::OnSelchangeLoginPeople()
{
    CString userName;
    int index = loginPeopleList.GetCurSel();
    if (index < 0) {
        return;
    }
    loginPeopleList.GetText(index, userName);
    std::string user(userName.GetBuffer());
    if (user.find('*') != std::string::npos) {
        user = user.substr(0, user.length() - 1);
        loginPeopleList.DeleteString(index);
        loginPeopleList.InsertString(index, user.c_str());
    }
    auto itor = userToChat.find(user);
    if (itor != userToChat.end()) {
        SetDlgItemText(IDC_TEXT, itor->second.c_str());
    }
}

void CChatClientDlg::OnSelchangeStatus()
{
    int index = statusCombo.GetCurSel();
    if (index < 0) {
        return;
    }
    switch (index) {
        case 0: {
            
            break;
        }

        case 1: {

            break;
        }

        case 2: {

            break;
        }
    }
}
