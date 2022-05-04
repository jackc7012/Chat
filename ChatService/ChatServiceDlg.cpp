
// ChatServiceDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ChatService.h"
#include "ChatServiceDlg.h"
#include "afxdialogex.h"
#include <functional>
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

// CChatServiceDlg 对话框

CChatServiceDlg::CChatServiceDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_CHATSERVICE_DIALOG, pParent) {
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChatServiceDlg::DoDataExchange(CDataExchange* pDX) {
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, listLoginPeople);
}

BEGIN_MESSAGE_MAP(CChatServiceDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_START, &CChatServiceDlg::OnBnClickedStart)
    ON_MESSAGE(WM_SOCKET_TCP, OnSocketTcp)
    ON_MESSAGE(WM_SOCKET_UDP, OnSocketUdp)
    ON_BN_CLICKED(IDC_KICK, &CChatServiceDlg::OnBnClickedKick)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

// CChatServiceDlg 消息处理程序

BOOL CChatServiceDlg::OnInitDialog() {
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
    SetIcon(m_hIcon, TRUE);			// 设置大图标
    SetIcon(m_hIcon, FALSE);		// 设置小图标

    // TODO: 在此添加额外的初始化代码
    CFont font;
    font.CreatePointFont(150, _T("宋体"), NULL);
    GetDlgItem(IDC_STATUS)->SetFont(&font);
    GetDlgItem(IDC_START)->SetFont(&font);
    GetDlgItem(IDC_KICK)->SetFont(&font);
    GetDlgItem(IDC_STATIC_LOGIN_PEOPLE)->SetFont(&font);
    GetDlgItem(IDC_LIST_LOGIN_PEOPLE)->SetFont(&font);
    GetDlgItem(IDC_IPLIST)->SetFont(&font);

    logService.InitLog("../{time}/service");
    std::string ip = CNetWorkHandle::CreateInstance()->GetMainNetworkIp();
    Fun HandleBind = std::bind(&CChatServiceDlg::HandleAfter, this, std::placeholders::_1, std::placeholders::_2);
    CNetWorkHandle::CreateInstance()->StartThread(HandleBind);
    std::string showIp = ((ip == "get error") ? ("ip get error") : ("服务器ip : " + ip));
    SetDlgItemText(IDC_IPLIST, showIp.c_str());

    return TRUE;
}

BOOL CChatServiceDlg::DestroyWindow()
{
    return CDialogEx::DestroyWindow();
}

void CChatServiceDlg::OnSysCommand(UINT nID, LPARAM lParam) {
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

void CChatServiceDlg::OnPaint() {
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
HCURSOR CChatServiceDlg::OnQueryDragIcon() {
    return static_cast<HCURSOR>(m_hIcon);
}

void CChatServiceDlg::OnBnClickedStart() {
    CString str;
    GetDlgItemText(IDC_START, str);
    if (str == "启动") {
        if (StartTcp() && StartUdp()) {
            SetDlgItemText(IDC_STATUS, _T("服务器监听已经启动。。。"));
            SetDlgItemText(IDC_START, _T("停止"));
        }
    } else if (str == "停止") {
        ::closesocket(socketServiceTcp);
        ::closesocket(socketServiceUdp);
        SetDlgItemText(IDC_STATUS, _T("服务器未启动监听。。。"));
        SetDlgItemText(IDC_START, _T("启动"));
        logService << "服务器监听关闭";
        logService.PrintlogInfo(FILE_FORMAT);
    }
}

void CChatServiceDlg::OnBnClickedKick() {
    // TODO: 在此添加控件通知处理程序代码
}

bool CChatServiceDlg::StartTcp()
{
    socketServiceTcp = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socketServiceTcp == INVALID_SOCKET) {
        MessageBox(_T("创建TCP服务失败"), _T("错误"), MB_ICONERROR);
        return false;
    } else {
        addrServiceTcp.sin_family = AF_INET;
        addrServiceTcp.sin_port = htons(cwy::TCP_PORT);
        addrServiceTcp.sin_addr.S_un.S_addr = INADDR_ANY;
        if (::bind(socketServiceTcp, (sockaddr*)&addrServiceTcp, sizeof(addrServiceTcp)) == SOCKET_ERROR) {
            MessageBox(_T("绑定TCP服务失败"), _T("错误"), MB_ICONERROR);
            return false;
        }
        if (::listen(socketServiceTcp, SOMAXCONN) == SOCKET_ERROR) {
            MessageBox(_T("监听TCP服务失败"), _T("错误"), MB_ICONERROR);
            return false;
        }
        if (::WSAAsyncSelect(socketServiceTcp, this->m_hWnd, WM_SOCKET_TCP, FD_ACCEPT | FD_READ | FD_CLOSE) == SOCKET_ERROR) {
            MessageBox(_T("创建异步TCP服务失败"), _T("错误"), MB_ICONERROR);
            return false;
        }
        logService << "服务器TCP开启监听，端口号为：" << cwy::TCP_PORT;
    }
    logService.PrintlogInfo(FILE_FORMAT);
    return true;
}

bool CChatServiceDlg::StartUdp()
{
    /*socketServiceUdp = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (socketServiceUdp == INVALID_SOCKET) {
        MessageBox(_T("创建UDP服务失败"), _T("错误"), MB_ICONERROR);
        return false;
    } else {
        addrServiceUdp.sin_family = AF_INET;
        addrServiceUdp.sin_port = htons(cwy::UDP_PORT);
        addrServiceUdp.sin_addr.S_un.S_addr = INADDR_ANY;
        ::bind(socketServiceUdp, (sockaddr*)&addrServiceUdp, sizeof(addrServiceUdp));
        ::WSAAsyncSelect(socketServiceUdp, this->m_hWnd, WM_SOCKET_UDP, FD_READ);
        netWorkHandle->SetUdpSocket(socketServiceUdp);
        logService << "服务器UDP开启监听，端口号为：" << cwy::UDP_PORT;
    }
    logService.PrintlogInfo(FILE_FORMAT);*/
    return true;
}

void CChatServiceDlg::HandleAfter(int code, std::string msg)
{
    int aa = 0;
}

LRESULT CChatServiceDlg::OnSocketTcp(WPARAM wParam, LPARAM lParam) {
    CString str_text;
    switch (lParam)
    {
        case FD_ACCEPT:
        {
            SOCKADDR_IN addrClient = {0};
            int len = sizeof(SOCKADDR);
            SOCKET socket = ::accept(socketServiceTcp, (SOCKADDR*)&addrClient, &len);
            if (socket == SOCKET_ERROR) {
                break;
            }
            socket2IpMap.insert(std::make_pair(socket, inet_ntoa(addrClient.sin_addr)));
        }
        break;

        case FD_READ:
        {
            char strRecv[DATA_LENGTH] = {0};
            for (std::pair<SOCKET, std::string> itor : socket2IpMap) {
                int ret = ::recv(itor.first, strRecv, DATA_LENGTH, 0);
                if (ret > 0) {
                    break;
                }
            }
            CNetWorkHandle::CreateInstance()->PushEvent(strRecv);
        }
        break;

        case FD_CLOSE:
        {
            GetDlgItemText(IDC_TEXT, str_text);
            str_text += "\r\n";
            str_text += ::inet_ntoa(addrAccept.sin_addr);
            str_text += "已断开";
            SetDlgItemText(IDC_TEXT, str_text);
            //m_list_login_people.DeleteString(del_index);
        }
        break;

        default:
            break;
    }
    return 0;
}

LRESULT CChatServiceDlg::OnSocketUdp(WPARAM wParam, LPARAM lParam)
{
    switch (lParam)
    {
        case FD_READ:
        {

        }
        break;

        default:
            break;
    }
    return 0;
}

//
//void CChatServiceDlg::HandleRecv(const s_HandleRecv & handle_recv) {
//    //服务器处理事件
//    UpdateData(TRUE);
//    std::lock_guard<std::mutex> mt(mt_server_handle);
//    CString str_text;
//    std::string js_str_send;
//    s_HandleRecv to_send;
//    switch (handle_recv.type) {
//    case NULLCOMMUNICATION:
//        break;
//
//    case ERRORCOMMUNICATION:
//        break;
//
//    case REGISTER: {
//        char password[MAX_PATH] = { 0 };
//        GetPrivateProfileString(_T("login_info"), handle_recv.param.Register.customer, "", password, MAX_PATH, _T("./login.ini"));
//        logService << "customer " << handle_recv.param.Register.customer << " registere ";
//        if (strcmp(password, "") != 0) {
//            to_send.param.RegisterBack.customer = handle_recv.param.Register.customer;
//            to_send.param.RegisterBack.register_result = "failed";
//            to_send.param.RegisterBack.description = "user name is already have";
//            js_str_send = EncodeJson(REGISTERBACKFAILED, to_send);
//            logService << "result failed description is " << to_send.param.RegisterBack.description;
//            logService.PrintlogError(FILE_FORMAT);
//        } else {
//            to_send.param.RegisterBack.customer = handle_recv.param.Register.customer;
//            to_send.param.RegisterBack.register_result = "succeed";
//            WritePrivateProfileString(_T("login_info"), handle_recv.param.Register.customer, handle_recv.param.Register.password, ("./login.ini"));
//            js_str_send = EncodeJson(REGISTERBACKSUCCEED, to_send);
//            logService << "result succeed";
//            logService.PrintlogInfo(FILE_FORMAT);
//        }
//        ::send(handle_recv.socket.socket_accept_tcp, js_str_send.c_str(), js_str_send.length(), 0);
//        logService << "send message :" << js_str_send;
//        logService.PrintlogInfo(FILE_FORMAT);
//        delete[]handle_recv.param.Register.customer;
//        delete[]handle_recv.param.Register.password;
//    }
//    break;
//
//    case LOGIN: {
//        char password[MAX_PATH] = { 0 };
//        bool is_succeed = false;
//        logService << "customer " << handle_recv.param.Login.customer << " login ";
//        if (m_list_login_people.FindString(0, handle_recv.param.Login.customer) != LB_ERR) {
//            to_send.param.LoginBack.customer = handle_recv.param.Login.customer;
//            to_send.param.LoginBack.login_result = "failed";
//            to_send.param.LoginBack.description = "this people has already login";
//            js_str_send = EncodeJson(LOGINBACKFAILED, to_send);
//            logService << "result failed description is " << to_send.param.LoginBack.description;
//            logService.PrintlogError(FILE_FORMAT);
//        } else {
//            GetPrivateProfileString(_T("login_info"), handle_recv.param.Login.customer, "", password, MAX_PATH, _T("./login.ini"));
//            if (strcmp(password, "") == 0) {
//                to_send.param.LoginBack.customer = handle_recv.param.Login.customer;
//                to_send.param.LoginBack.login_result = "failed";
//                to_send.param.LoginBack.description = "no such people";
//                js_str_send = EncodeJson(LOGINBACKFAILED, to_send);
//                logService << "result failed description is " << to_send.param.LoginBack.description;
//                logService.PrintlogError(FILE_FORMAT);
//            } else {
//                //std::string decry_password = Decryption(password), decry_handle_recv = Decryption(handle_recv.Param.Login.password);
//                if (/*decry_password.compare(decry_handle_recv)*/strcmp(password, handle_recv.param.Login.password) == 0) {
//                    to_send.param.LoginBack.customer = handle_recv.param.Login.customer;
//                    to_send.param.LoginBack.login_result = "succeed";
//                    js_str_send = EncodeJson(LOGINBACKSUCCEED, to_send);
//                    is_succeed = true;
//                    logService << "result succeed";
//                    logService.PrintlogInfo(FILE_FORMAT);
//                } else {
//                    to_send.param.LoginBack.customer = handle_recv.param.Login.customer;
//                    to_send.param.LoginBack.login_result = "failed";
//                    to_send.param.LoginBack.description = "password error";
//                    js_str_send = EncodeJson(LOGINBACKFAILED, to_send);
//                    logService << "result failed description is " << to_send.param.LoginBack.description;
//                    logService.PrintlogError(FILE_FORMAT);
//                }
//            }
//        }
//        ::send(handle_recv.socket.socket_accept_tcp, js_str_send.c_str(), js_str_send.length(), 0);
//        logService << "send message :" << js_str_send;
//        logService.PrintlogInfo(FILE_FORMAT);
//        if (is_succeed) {
//            m_list_login_people.AddString(handle_recv.param.Login.customer);
//            name_to_socket_accept.insert(std::pair<std::string, SOCKET>(handle_recv.param.Login.customer, handle_recv.socket.socket_accept_tcp));
//            name_to_ip.insert(std::pair<std::string, std::string>(handle_recv.param.Login.customer, ip));
//            ve_accept_name.push_back(handle_recv.param.Login.customer);
//            GetDlgItemText(IDC_TEXT, str_text);
//            if (accpetCount != 1) {
//                str_text += "\r\n";
//            }
//            str_text += handle_recv.param.Login.customer;
//            str_text += "登录";
//            SetDlgItemText(IDC_TEXT, str_text);
//            to_send.param.ShowLogin.customer = new char *[ve_accept_name.size()];
//            for (unsigned int i = 0; i < ve_accept_name.size(); ++i) {
//                to_send.param.ShowLogin.customer[i] = const_cast<char *>(ve_accept_name[i].c_str());
//            }
//            to_send.param.ShowLogin.customer_num = ve_accept_name.size();
//            js_str_send = EncodeJson(SHOWLOGIN, to_send);
//            logService << "send message for all :" << js_str_send;
//            logService.PrintlogInfo(FILE_FORMAT);
//            delete[]to_send.param.ShowLogin.customer;
//            for (auto itor = name_to_socket_accept.cbegin(); itor != name_to_socket_accept.cend(); ++itor) {
//                ::send(itor->second, js_str_send.c_str(), js_str_send.length(), 0);
//            }
//        }
//        delete[]handle_recv.param.Login.customer;
//        delete[]handle_recv.param.Login.password;
//    }
//    break;
//
//    case DELETECUSTOMER: {
//        //int del_index = m_list_login_people.FindString(0, handle_recv.Param.DelCustomer.customer);
//        //m_list_login_people.DeleteString(del_index);
//        //name_to_socket_accept.erase(handle_recv.Param.DelCustomer.customer);
//        ////ip_to_name.erase()
//        //str_send = EncodeJson(DELETECUSTOMER, handle_recv.Param.DelCustomer.customer);
//        //for (auto itor = name_to_socket_accept.cbegin(); itor != name_to_socket_accept.cend(); ++itor)
//        //{
//        //	if (itor->first.compare(handle_recv.Param.DelCustomer.customer) != 0)
//        //	{
//        //		::send(itor->second, str_send.c_str(), str_send.length(), 0);
//        //	}
//        //}
//    }
//    break;
//
//    /*case CHAT: {
//        auto itor = name_to_socket_accept.find(handle_recv.Param.Chat.target);
//        to_send.Param.Chat.source = handle_recv.Param.Chat.source;
//        to_send.Param.Chat.target = handle_recv.Param.Chat.target;
//        to_send.Param.Chat.content = handle_recv.Param.Chat.content;
//        js_str_send = EncodeJson(CHAT, to_send);
//        ::send(itor->second, js_str_send.c_str(), js_str_send.length(), 0);
//        delete[]handle_recv.Param.Chat.source;
//        delete[]handle_recv.Param.Chat.target;
//        delete[]handle_recv.Param.Chat.content;
//    }
//    break;
//
//    case TRANSFERFILEREQUEST: {
//        auto itor = name_to_socket_accept.find(handle_recv.Param.TransferFileRequest.target);
//        to_send.Param.TransferFileRequest.source = handle_recv.Param.TransferFileRequest.source;
//        to_send.Param.TransferFileRequest.target = handle_recv.Param.TransferFileRequest.target;
//        to_send.Param.TransferFileRequest.fileName = handle_recv.Param.TransferFileRequest.fileName;
//        to_send.Param.TransferFileRequest.file_length = handle_recv.Param.TransferFileRequest.file_length;
//        js_str_send = EncodeJson(TRANSFERFILEREQUEST, to_send);
//        ::send(itor->second, js_str_send.c_str(), js_str_send.length(), 0);
//        delete[]handle_recv.Param.TransferFileRequest.source;
//        delete[]handle_recv.Param.TransferFileRequest.target;
//        delete[]handle_recv.Param.TransferFileRequest.fileName;
//        delete[]handle_recv.Param.TransferFileRequest.file_length;
//    }
//    break;
//
//    case TRANSFERFILERESPOND: {
//        auto itor = name_to_socket_accept.find(handle_recv.Param.TransferFileRespond.target);
//        to_send.Param.TransferFileRespond.source = handle_recv.Param.TransferFileRespond.source;
//        to_send.Param.TransferFileRespond.target = handle_recv.Param.TransferFileRespond.target;
//        to_send.Param.TransferFileRespond.fileName = handle_recv.Param.TransferFileRespond.fileName;
//        to_send.Param.TransferFileRespond.transfer_result = handle_recv.Param.TransferFileRespond.transfer_result;
//        js_str_send = EncodeJson(TRANSFERFILERESPOND, to_send);
//        ::send(itor->second, js_str_send.c_str(), js_str_send.length(), 0);
//        delete[]handle_recv.Param.TransferFileRespond.source;
//        delete[]handle_recv.Param.TransferFileRespond.target;
//        delete[]handle_recv.Param.TransferFileRespond.fileName;
//        delete[]handle_recv.Param.TransferFileRespond.transfer_result;
//    }
//    break;
//
//    case TRANSFERFILE: {
//        auto itor = name_to_socket_accept.find(handle_recv.Param.TransferFile.target);
//        to_send.Param.TransferFile.source = handle_recv.Param.TransferFile.source;
//        to_send.Param.TransferFile.target = handle_recv.Param.TransferFile.target;
//        to_send.Param.TransferFile.fileName = handle_recv.Param.TransferFile.fileName;
//        to_send.Param.TransferFile.file_length = handle_recv.Param.TransferFile.file_length;
//        to_send.Param.TransferFile.file_block = handle_recv.Param.TransferFile.file_block;
//        to_send.Param.TransferFile.file_content = handle_recv.Param.TransferFile.file_content;
//        to_send.Param.TransferFile.current_block = handle_recv.Param.TransferFile.current_block;
//        js_str_send = EncodeJson(TRANSFERFILE, to_send);
//        ::send(itor->second, js_str_send.c_str(), js_str_send.length(), 0);
//        delete[]handle_recv.Param.TransferFile.source;
//        delete[]handle_recv.Param.TransferFile.target;
//        delete[]handle_recv.Param.TransferFile.fileName;
//        delete[]handle_recv.Param.TransferFile.file_content;
//        delete[]handle_recv.Param.TransferFile.file_length;
//    }
//    break;*/
//
//    default:
//        break;
//    }
//    UpdateData(FALSE);
//
//    return;
//}

void CChatServiceDlg::OnDestroy()
{
    CNetWorkHandle::CreateInstance()->ExitThread();
}
