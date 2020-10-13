
// ChatClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ChatClient.h"
#include "ChatClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

static const char *s_verify_code = "abcdefghijkmnpqrstuvwxyzABCDEFGHJKLMNPQRSTUVWXYZ123456789";

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
    : CDialogEx(IDD_CHATCLIENT_DIALOG, pParent) {
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChatClientDlg::DoDataExchange(CDataExchange* pDX) {
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LOGIN_PEOPLE, m_list_login_people);
}

BEGIN_MESSAGE_MAP(CChatClientDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_CONNECT, &CChatClientDlg::OnBnClickedConnect)
    ON_BN_CLICKED(IDC_SEND, &CChatClientDlg::OnBnClickedSend)
    ON_MESSAGE(WM_SOCKET, OnSocket)
    ON_BN_CLICKED(IDC_REGISTER, &CChatClientDlg::OnBnClickedRegister)
    ON_BN_CLICKED(IDC_TRANSFERFILE, &CChatClientDlg::OnBnClickedTransferfile)
    ON_MESSAGE(WM_TRANSFERFILEACCEPT, OnTransferFileAccept)
    ON_WM_DESTROY()
    ON_WM_DESTROY()
END_MESSAGE_MAP()

// CChatClientDlg 消息处理程序

BOOL CChatClientDlg::OnInitDialog() {
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

    // 设置随机数种子
    srand((unsigned int)time(NULL));

    // 设置字体
    InitFont();

    // 禁用聊天组框
    EnableWindowChat(FALSE);

    GetDlgItem(IDC_REQUESTFILENAME)->ShowWindow(SW_HIDE);

    SetDlgItemText(IDC_IP, _T("127.0.0.1"));

    SetVerify();

    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CChatClientDlg::ConnectServer(const std::string & ip/* = "127.0.0.1"*/) {
    socket_client = ::socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == socket_client) {
        MessageBox(_T("创建服务失败,请重试！"), _T("错误"), MB_ICONERROR);
        return;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(SOCKET_PORT);
    addr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());

    SetDlgItemText(IDC_TEXT, _T("正在连接服务器.......\r\n"));
    UpdateWindow();

    if (0 != ::connect(socket_client, (SOCKADDR *)&addr, sizeof(addr))) {
        MessageBox(_T("连接失败，请重试！"), _T("错误"), MB_ICONERROR);
        return;
    }
    ::WSAAsyncSelect(socket_client, this->m_hWnd, WM_SOCKET, FD_READ);
    is_service_open = true;

    return;
}

void CChatClientDlg::InitFont() {
    CFont font;
    font.CreatePointFont(150, _T("宋体"), NULL);
    // 组框
    GetDlgItem(IDC_STATICINFO)->SetFont(&font);
    GetDlgItem(IDC_STATICLOGIN)->SetFont(&font);
    // 登录信息
    GetDlgItem(IDC_STATICIP)->SetFont(&font);
    GetDlgItem(IDC_IP)->SetFont(&font);
    GetDlgItem(IDC_STATICNAME)->SetFont(&font);
    GetDlgItem(IDC_NAME)->SetFont(&font);
    GetDlgItem(IDC_STATICPASSWORD)->SetFont(&font);
    GetDlgItem(IDC_PASSWORD)->SetFont(&font);
    GetDlgItem(IDC_STATICCODE)->SetFont(&font);
    GetDlgItem(IDC_CODE)->SetFont(&font);
    GetDlgItem(IDC_CODE_VERIFY)->SetFont(&font);
    GetDlgItem(IDC_REGISTER)->SetFont(&font);
    GetDlgItem(IDC_CONNECT)->SetFont(&font);
    GetDlgItem(IDC_LOGIN_PEOPLE)->SetFont(&font);
    // 聊天
    GetDlgItem(IDC_TEXT)->SetFont(&font);
    GetDlgItem(IDC_SENDTEXT)->SetFont(&font);
    GetDlgItem(IDC_SEND)->SetFont(&font);
    GetDlgItem(IDC_FILENAME)->SetFont(&font);
    GetDlgItem(IDC_TRANSFERFILE)->SetFont(&font);

    return;
}

void CChatClientDlg::EnableWindowInfo(const BOOL flag/* = TRUE*/) {
    GetDlgItem(IDC_IP)->EnableWindow(flag);
    GetDlgItem(IDC_NAME)->EnableWindow(flag);
    GetDlgItem(IDC_PASSWORD)->EnableWindow(flag);
    GetDlgItem(IDC_CODE)->EnableWindow(flag);
    GetDlgItem(IDC_CODE_VERIFY)->EnableWindow(flag);
}

void CChatClientDlg::EnableWindowChat(const BOOL flag/* = TRUE*/) {
    GetDlgItem(IDC_SENDTEXT)->EnableWindow(flag);
    GetDlgItem(IDC_SEND)->EnableWindow(flag);
    GetDlgItem(IDC_FILENAME)->EnableWindow(flag);
    GetDlgItem(IDC_TRANSFERFILE)->EnableWindow(flag);
}

void CChatClientDlg::SetVerify() {
    int code_rand = 0;
    std::string verify_code("");
    for (int i = 0; i < 4; ++i) {
        code_rand = rand() % strlen(s_verify_code);
        verify_code += s_verify_code[code_rand];
    }
    SetDlgItemText(IDC_CODE_VERIFY, verify_code.c_str());
}

void CChatClientDlg::HandleRegister(const int flag, const s_HandleRecv & handle_recv) {
    CString str_text;
    switch (flag) {
    case 0: {
        GetDlgItemText(IDC_TEXT, str_text);
        if (str_text.IsEmpty()) {
            str_text += "注册成功！";
        } else {
            str_text += "\r\n注册成功！";
        }
        SetDlgItemText(IDC_TEXT, str_text);
        g_log << "register " << handle_recv.Param.RegisterBack.customer << " succeed";
        g_log.PrintlogCustom(FILE_FORMAT);
        delete[]handle_recv.Param.RegisterBack.customer;
        delete[]handle_recv.Param.RegisterBack.register_result;
    }
    break;

    case 1: {
        MessageBox(handle_recv.Param.RegisterBack.description, _T("错误"), MB_ICONERROR);
        g_log << "register " << handle_recv.Param.RegisterBack.customer << " failed , description " << handle_recv.Param.RegisterBack.description;
        g_log.PrintlogCustom(FILE_FORMAT);
        delete[]handle_recv.Param.RegisterBack.customer;
        delete[]handle_recv.Param.RegisterBack.register_result;
        delete[]handle_recv.Param.RegisterBack.description;
    }
    break;

    default:
        break;
    }
	::closesocket(socket_client);

    return;
}

void CChatClientDlg::HandleLogin(const int flag, const s_HandleRecv & handle_recv) {
    CString str_text;
    switch (flag) {
    case 0: {
        if (0 == nick_name.compare(handle_recv.Param.LoginBack.customer)) {
            GetDlgItemText(IDC_TEXT, str_text);
            if (str_text.IsEmpty()) {
                str_text += "连接服务器成功！";
            } else {
                str_text += "\r\n连接服务器成功！";
            }
            EnableWindowInfo(FALSE);
            EnableWindowChat(TRUE);
            SetDlgItemText(IDC_TEXT, str_text);
            SetDlgItemText(IDC_CONNECT, _T("断开"));
        }
        delete[]handle_recv.Param.LoginBack.customer;
        delete[]handle_recv.Param.LoginBack.login_result;
    }
    break;

    case 1: {
        if (0 == nick_name.compare(handle_recv.Param.LoginBack.customer)) {
            MessageBox(handle_recv.Param.LoginBack.description, _T("错误"), MB_ICONERROR);
        }
        delete[]handle_recv.Param.LoginBack.customer;
        delete[]handle_recv.Param.LoginBack.login_result;
        delete[]handle_recv.Param.LoginBack.description;
    }
    break;

    default:
        break;
    }

    return;
}

void CChatClientDlg::HandleDeleteCustomer(const s_HandleRecv & handle_recv) {
    CString str_text;
    int del_index = m_list_login_people.FindString(0, handle_recv.Param.DelCustomer.customer);
    m_list_login_people.DeleteString(del_index);
    GetDlgItemText(IDC_TEXT, str_text);
    char content[50] = { 0 };
    sprintf_s(content, 50, "\r\n%s已退出", handle_recv.Param.DelCustomer.customer);
    str_text += content;
    SetDlgItemText(IDC_TEXT, str_text);

    return;
}

void CChatClientDlg::HandleShowLogin(const s_HandleRecv & handle_recv) {
    for (unsigned int i = 0; i < handle_recv.Param.ShowLogin.customer_num; ++i) {
        if (nick_name.compare(handle_recv.Param.ShowLogin.customer[i]) != 0) {
            m_list_login_people.AddString(handle_recv.Param.ShowLogin.customer[i]);
        }
        delete[]handle_recv.Param.ShowLogin.customer[i];
    }
    delete[]handle_recv.Param.ShowLogin.customer;
}

void CChatClientDlg::HandleChat(const s_HandleRecv & handle_recv) {
    CString str_text;
    if (nick_name.compare(handle_recv.Param.Chat.target) == 0) {
        GetDlgItemText(IDC_TEXT, str_text);
        str_text += "\r\n";
        char content[DATA_LENGTH + 10] = { 0 };
        sprintf_s(content, DATA_LENGTH + 10, "%s说：%s", handle_recv.Param.Chat.source, handle_recv.Param.Chat.content);
        str_text += content;
        SetDlgItemText(IDC_TEXT, str_text);
    }
    delete[]handle_recv.Param.Chat.content;
    delete[]handle_recv.Param.Chat.source;
    delete[]handle_recv.Param.Chat.target;
}

void CChatClientDlg::HandleTransferfile(const int flag, const s_HandleRecv & handle_recv) {
}

void CChatClientDlg::OnSysCommand(UINT nID, LPARAM lParam) {
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

void CChatClientDlg::OnPaint() {
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
HCURSOR CChatClientDlg::OnQueryDragIcon() {
    return static_cast<HCURSOR>(m_hIcon);
}

void CChatClientDlg::OnBnClickedRegister() {
    CString str_ip, str_name, str_password, str_code, str_code_verify;
    GetDlgItemText(IDC_IP, str_ip);
    GetDlgItemText(IDC_NAME, str_name);
    GetDlgItemText(IDC_PASSWORD, str_password);
    GetDlgItemText(IDC_CODE, str_code);
    GetDlgItemText(IDC_CODE_VERIFY, str_code_verify);
    if ("" == str_ip) {
        MessageBox(_T("IP不能为空"), _T("错误"), MB_ICONERROR);
    } else if ("" == str_name) {
        MessageBox(_T("昵称不能为空"), _T("错误"), MB_ICONERROR);
    } /*else if ("" == str_code) {
        MessageBox(_T("请输入验证码"), _T("错误"), MB_ICONERROR);
    } else if (!VerifyCode(str_code, str_code_verify)) {
        MessageBox(_T("验证码错误"), _T("错误"), MB_ICONERROR);
        SetVerify();
    }*/ else {
        s_HandleRecv to_send;
        const char *ip = str_ip.GetBuffer();
        const char *name = str_name.GetBuffer();
        const char *password = str_password.GetBuffer();
        if (!is_service_open) {
            ConnectServer(ip);
        }
        //std::string encry_psd = Encryption(password);
        to_send.Param.Register.customer = const_cast<char *>(name);
        to_send.Param.Register.password = const_cast<char *>(password);
        std::string send_data = EncodeJson(REGISTER, to_send);
        ::send(socket_client, send_data.c_str(), send_data.length(), 0);
    }
    str_ip.ReleaseBuffer();
    str_name.ReleaseBuffer();
    str_password.ReleaseBuffer();
    str_code.ReleaseBuffer();
    str_code_verify.ReleaseBuffer();

    return;
}

void CChatClientDlg::OnBnClickedConnect() {
    CString str_ip, str_name, str_password, str_code, str_code_verify, str_connect, str_text;
    GetDlgItemText(IDC_IP, str_ip);
    GetDlgItemText(IDC_NAME, str_name);
    GetDlgItemText(IDC_PASSWORD, str_password);
    GetDlgItemText(IDC_CODE, str_code);
    GetDlgItemText(IDC_CODE_VERIFY, str_code_verify);
    GetDlgItemText(IDC_CONNECT, str_connect);
    s_HandleRecv to_send;
    if ("连接" == str_connect) {
        if ("" == str_ip) {
            MessageBox(_T("IP不能为空"), _T("错误"), MB_ICONERROR);
        } else if ("" == str_name) {
            MessageBox(_T("昵称不能为空"), _T("错误"), MB_ICONERROR);
        } /*else if ("" == str_code) {
            MessageBox(_T("请输入验证码"), _T("错误"), MB_ICONERROR);
        } else if (!VerifyCode(str_code, str_code_verify)) {
            MessageBox(_T("验证码错误"), _T("错误"), MB_ICONERROR);
            SetVerify();
        }*/ else {
            const char *ip = str_ip.GetBuffer();
            const char *name = str_name.GetBuffer();
            const char *password = str_password.GetBuffer();
            if (!is_service_open) {
                ConnectServer(ip);
            }
            //std::string encry_psd = Encryption(password);
            to_send.Param.Login.customer = const_cast<char *>(name);
            to_send.Param.Login.password = const_cast<char *>(password);
            std::string send_data = EncodeJson(LOGIN, to_send);
            ::send(socket_client, send_data.c_str(), send_data.length(), 0);
            nick_name = str_name;
        }
        char file_path[256] = { 0 };
        sprintf_s(file_path, 256, "./%s", nick_name.c_str());
        g_log.InitLog(file_path, DEBUG_LEVEL, true);
    } else if ("断开" == str_connect) {
        to_send.Param.DelCustomer.customer = const_cast<char *>(nick_name.c_str());
        std::string send_data = EncodeJson(DELETECUSTOMER, to_send);
        ::send(socket_client, send_data.c_str(), send_data.length(), 0);
        ::closesocket(socket_client);
        EnableWindowInfo(TRUE);
        EnableWindowChat(FALSE);
        SetDlgItemText(IDC_CONNECT, _T("连接"));
        nick_name = "";
        is_service_open = false;
        SetVerify();

        g_log.UnitLog();
    }
    str_ip.ReleaseBuffer();
    str_name.ReleaseBuffer();
    str_password.ReleaseBuffer();
    str_code.ReleaseBuffer();
    str_code_verify.ReleaseBuffer();
	SetDlgItemText(IDC_SENDTEXT, "");

    return;
}

void CChatClientDlg::OnBnClickedSend() {
    CString str_text, str_send, str_send_target;
    s_HandleRecv to_send;
    GetDlgItemText(IDC_SENDTEXT, str_send);
    int cur_sel = m_list_login_people.GetCurSel();
    if (cur_sel == CB_ERR) {
        MessageBox(_T("请选择一个用户！"), _T("错误"), MB_ICONERROR);
    } else {
        m_list_login_people.GetText(cur_sel, str_send_target);
        if ("" == str_send) {
            MessageBox(_T("发送内容不能为空...."), _T("错误"), MB_ICONERROR);
        } else {
            to_send.Param.Chat.content = str_send.GetBuffer();
            to_send.Param.Chat.source = const_cast<char *>(nick_name.c_str());
            to_send.Param.Chat.target = str_send_target.GetBuffer(0);
            std::string js_str_send = EncodeJson(CHAT, to_send);
            g_log << "send : " <<js_str_send;
            g_log.PrintlogDebug(FILE_FORMAT);
            ::send(socket_client, js_str_send.c_str(), js_str_send.length(), 0);
            GetDlgItemText(IDC_TEXT, str_text);
            str_text += "\r\n";
            str_text += str_send;
            SetDlgItemText(IDC_TEXT, str_text);
            SetDlgItemText(IDC_SENDTEXT, "");
            str_send_target.ReleaseBuffer();
        }
    }

    return;
}

void CChatClientDlg::OnBnClickedTransferfile() {
    CString str_text, str_send_target;
    int cur_sel = m_list_login_people.GetCurSel();
    if (cur_sel == CB_ERR) {
        MessageBox(_T("请选择一个用户！"), _T("错误"), MB_ICONERROR);
    } else {
        m_list_login_people.GetText(cur_sel, str_send_target);
        TCHAR szFilter[] = _T("文本文件(*.txt)|*.txt|所有文件(*.*)|*.*||");
        CFileDialog fileDlg(TRUE, _T("txt"), NULL, 0, szFilter, this);
        CString strFilePath;
        if (IDOK == fileDlg.DoModal()) {
            strFilePath = fileDlg.GetPathName();
            SetDlgItemText(IDC_FILENAME, strFilePath);
        }
        s_HandleRecv to_send;
        CFile m_file;
        m_file.Open(strFilePath, CFile::modeRead | CFile::typeBinary);
        unsigned long long len = m_file.GetLength(), now_length = 0, now_block = 1;
        CString filename = fileDlg.GetFileName();
        char c_file_length[256] = { 0 };
        sprintf_s(c_file_length, 256, "%lld", (long long)len);
        to_send.Param.TransferFileRequest.file_name = filename.GetBuffer(0);;
        to_send.Param.TransferFileRequest.file_length = c_file_length;
        to_send.Param.TransferFileRequest.source = const_cast<char *>(nick_name.c_str());
        to_send.Param.TransferFileRequest.target = str_send_target.GetBuffer(0);
        std::string js_str_send = EncodeJson(TRANSFERFILEREQUEST, to_send);
        ::send(socket_client, js_str_send.c_str(), js_str_send.length(), 0);
        strFilePath.ReleaseBuffer();
        str_send_target.ReleaseBuffer();
        filename.ReleaseBuffer(0);
        m_file.Close();
    }
}

void CChatClientDlg::OnOK() {
    OnBnClickedSend();
}

LRESULT CChatClientDlg::OnSocket(WPARAM wParam, LPARAM lParam) {
    char *str_recv = new char[DATA_LENGTH];
    CString str_text;
    memset(str_recv, 0, DATA_LENGTH);
    if (lParam == FD_READ) {
        UpdateData(TRUE);
        ::recv(socket_client, str_recv, DATA_LENGTH, 0);
        s_HandleRecv handle_recv, to_send;
        DecodeJson(str_recv, handle_recv);
        switch (handle_recv.type) {
        case NULLCOMMUNICATION:
            break;

        case ERRORCOMMUNICATION:
            break;

        case REGISTERBACKSUCCEED: {
            HandleRegister(0, handle_recv);
        }
        break;

        case REGISTERBACKFAILED: {
            HandleRegister(1, handle_recv);
        }
        break;

        case LOGINBACKSUCCEED: {
            HandleLogin(0, handle_recv);
        }
        break;

        case LOGINBACKFAILED: {
            HandleLogin(1, handle_recv);
        }
        break;

        case DELETECUSTOMER: {
            HandleDeleteCustomer(handle_recv);

        }
        break;

        case SHOWLOGIN: {
            HandleShowLogin(handle_recv);
        }
        break;

        case CHAT: {
            HandleChat(handle_recv);
        }
        break;

        case TRANSFERFILEREQUEST: {
            if (nick_name.compare(handle_recv.Param.TransferFileRequest.target) == 0) {
                CMessage m_dlg;
                char temp[DATA_LENGTH] = { 0 };
                sprintf_s(temp, DATA_LENGTH, "%s向你发送%s文件，大小为%s字节，\r\n是否接受？",
                          handle_recv.Param.TransferFileRequest.source,handle_recv.Param.TransferFileRequest.file_name, handle_recv.Param.TransferFileRequest.file_length);
                m_dlg.message = temp;
                int i_return = m_dlg.DoModal();
                to_send.Param.TransferFileRespond.source = handle_recv.Param.TransferFileRequest.target;
                to_send.Param.TransferFileRespond.target = handle_recv.Param.TransferFileRequest.source;
                to_send.Param.TransferFileRespond.file_name = handle_recv.Param.TransferFileRequest.file_name;
                if (i_return == 1) {
                    to_send.Param.TransferFileRespond.transfer_result = "agree";
                    can_transfer_file = true;
                    std::string filename(handle_recv.Param.TransferFileRequest.file_name);
                    std::string filetitle = filename.substr(0, filename.find('.'));
                    std::string filesuffix = filename.substr(filename.find('.') + 1);
                    TCHAR szFilter[] = _T("所有文件(*.*)|*.*||");
                    CFileDialog fileDlg(FALSE, filesuffix.c_str(), filename.c_str(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
                    CString strFilePath;
                    if (IDOK == fileDlg.DoModal()) {
                        strFilePath = fileDlg.GetPathName();
                        SetDlgItemText(IDC_REQUESTFILENAME, strFilePath);
                    }
                    std::thread c_thread_TransferAccept(&CChatClientDlg::thread_TransferFileAccept, this, m_hWnd, strFilePath.GetBuffer(0), handle_recv);
                    c_thread_TransferAccept.detach();
                    strFilePath.ReleaseBuffer();
                } else if (i_return == -1) {
                    to_send.Param.TransferFileRespond.transfer_result = "refuse";
                    can_transfer_file = false;
                }
                std::string js_str_send = EncodeJson(TRANSFERFILERESPOND, to_send);
                ::send(socket_client, js_str_send.c_str(), js_str_send.length(), 0);
            }
            delete[]handle_recv.Param.TransferFileRequest.target;
            delete[]handle_recv.Param.TransferFileRequest.source;
            delete[]handle_recv.Param.TransferFileRequest.file_name;
            delete[]handle_recv.Param.TransferFileRequest.file_length;
        }
        break;

        case TRANSFERFILERESPOND: {
            CString str_send_target;
            int cur_sel = m_list_login_people.GetCurSel();
            m_list_login_people.GetText(cur_sel, str_send_target);
            if (nick_name.compare(handle_recv.Param.TransferFileRespond.target) == 0 &&
                    str_send_target.Compare(handle_recv.Param.TransferFileRespond.source) == 0) {
                if (strcmp(handle_recv.Param.TransferFileRespond.transfer_result, "refuse") == 0) {
                    MessageBox(_T("对方拒绝了你的请求"), _T("提示"), MB_ICONASTERISK);
                } else if (strcmp(handle_recv.Param.TransferFileRespond.transfer_result, "agree") == 0) {
                    CString cstr_file_name;
                    GetDlgItemText(IDC_FILENAME, cstr_file_name);
                    std::thread c_thread_TransferFile(&CChatClientDlg::thread_TransferFile, this, m_hWnd, cstr_file_name.GetBuffer(0), str_send_target.GetBuffer(0));
                    c_thread_TransferFile.detach();
                    cstr_file_name.ReleaseBuffer();
                    str_send_target.ReleaseBuffer();
                }
            }
            delete[]handle_recv.Param.TransferFileRespond.target;
            delete[]handle_recv.Param.TransferFileRespond.source;
            delete[]handle_recv.Param.TransferFileRespond.file_name;
            delete[]handle_recv.Param.TransferFileRespond.transfer_result;
        }
        break;

        case TRANSFERFILE: {
            if (nick_name.compare(handle_recv.Param.TransferFile.target) == 0 && can_transfer_file) {
                q_transfer_file.push(handle_recv);
            }
            delete[]handle_recv.Param.TransferFile.target;
            delete[]handle_recv.Param.TransferFile.source;
            delete[]handle_recv.Param.TransferFile.file_name;
            delete[]handle_recv.Param.TransferFile.file_length;
            delete[]handle_recv.Param.TransferFile.file_content;
        }
        break;

        default:
            break;
        }
        UpdateData(FALSE);
    }
    delete []str_recv;

    return 0;
}

int CChatClientDlg::thread_TransferFile(const HWND & hwnd, const std::string & file_name, const std::string & target) {
    s_HandleRecv to_send;
    CFile m_file;
    m_file.Open(file_name.c_str(), CFile::modeRead | CFile::typeBinary);
    unsigned long long len = m_file.GetLength();
    unsigned int now_block = 1;
    char c_file_length[256] = { 0 };
    sprintf_s(c_file_length, 256, "%lld", (long long)len);
    char *getStr = new char[DATA_LENGTH];
    to_send.Param.TransferFile.file_name = const_cast<char *>(file_name.c_str());
    to_send.Param.TransferFile.file_length = c_file_length;
    to_send.Param.TransferFile.source = const_cast<char *>(nick_name.c_str());
    to_send.Param.TransferFile.target = const_cast<char *>(target.c_str());
    to_send.Param.TransferFile.file_block = (unsigned int)ceil((double)((float)len / DATA_LENGTH));
    while (now_block <= to_send.Param.TransferFile.file_block) {
        memset(getStr, 0, DATA_LENGTH);
        m_file.Read(getStr, DATA_LENGTH);
        to_send.Param.TransferFile.file_content = getStr;
        to_send.Param.TransferFile.current_block = now_block++;
        std::string js_str_send = EncodeJson(TRANSFERFILE, to_send);
        ::send(socket_client, js_str_send.c_str(), js_str_send.length(), 0);
        m_file.Seek(DATA_LENGTH * to_send.Param.TransferFile.current_block, CFile::begin);
    }
    delete[]getStr;
    m_file.Close();
    char str_text[DATA_LENGTH] = { 0 };
    ::GetDlgItemText(hwnd, IDC_TEXT, str_text, DATA_LENGTH);
    strcat_s(str_text, "\r\n传输完成！");
    ::SetDlgItemText(hwnd, IDC_TEXT, str_text);

    return 0;
}

int CChatClientDlg::thread_TransferFileAccept(const HWND & hwnd, const std::string & file_name, const s_HandleRecv & handle_recv) {
    CFile file_accept;
    s_HandleRecv transfer_file_accept;
    while (1) {
        if (q_transfer_file.size()) {
            transfer_file_accept = q_transfer_file.front();
            q_transfer_file.pop();
            if (transfer_file_accept.Param.TransferFile.current_block == 1) {
                file_accept.Open(file_name.c_str(), CFile::modeWrite | CFile::modeCreate | CFile::typeBinary);
            }
            if (transfer_file_accept.Param.TransferFile.current_block != transfer_file_accept.Param.TransferFile.file_block) {
                file_accept.Write(transfer_file_accept.Param.TransferFile.file_content, DATA_LENGTH);
                file_accept.Flush();
                file_accept.Seek(DATA_LENGTH * transfer_file_accept.Param.TransferFile.current_block, CFile::begin);
            } else if (transfer_file_accept.Param.TransferFile.current_block == transfer_file_accept.Param.TransferFile.file_block) {
                file_accept.Write(transfer_file_accept.Param.TransferFile.file_content, atoll(transfer_file_accept.Param.TransferFile.file_length) - (transfer_file_accept.Param.TransferFile.file_block - 1) * DATA_LENGTH);
                file_accept.Close();
                can_transfer_file = false;
                char str_text[DATA_LENGTH] = { 0 };
                ::GetDlgItemText(hwnd, IDC_TEXT, str_text, DATA_LENGTH);
                strcat_s(str_text, "\r\n接收完成！");
                ::SetDlgItemText(hwnd, IDC_TEXT, str_text);
                break;
            }
        }
    }

    return 0;
}

LRESULT CChatClientDlg::OnTransferFileAccept(WPARAM wParam, LPARAM lParam) {
    s_HandleRecv *transfer_file_accept = (s_HandleRecv*)wParam;
    CFile file_accept;
    CString cstr_file_name, str_text;
    GetDlgItemText(IDC_REQUESTFILENAME, cstr_file_name);
    if (transfer_file_accept->Param.TransferFile.file_block == 1) {
        file_accept.Open(cstr_file_name, CFile::modeWrite | CFile::modeCreate | CFile::typeBinary);
        file_accept.Write(transfer_file_accept->Param.TransferFile.file_content,
                          atoi(transfer_file_accept->Param.TransferFile.file_length));
        file_accept.Close();
        GetDlgItemText(IDC_TEXT, str_text);
        str_text += "\r\n接收完成！";
        SetDlgItemText(IDC_TEXT, str_text);
    } else {
        if (transfer_file_accept->Param.TransferFile.current_block == 1) {
            file_accept.Open(cstr_file_name, CFile::modeWrite | CFile::modeCreate | CFile::typeBinary);
        }
        if (transfer_file_accept->Param.TransferFile.current_block != transfer_file_accept->Param.TransferFile.file_block) {
            file_accept.Write(transfer_file_accept->Param.TransferFile.file_content, DATA_LENGTH);
            file_accept.Flush();
            file_accept.Seek(DATA_LENGTH * transfer_file_accept->Param.TransferFile.current_block, CFile::begin);
        } else if (transfer_file_accept->Param.TransferFile.current_block == transfer_file_accept->Param.TransferFile.file_block) {
            file_accept.Write(transfer_file_accept->Param.TransferFile.file_content, atoll(transfer_file_accept->Param.TransferFile.file_length) - (transfer_file_accept->Param.TransferFile.file_block - 1) * DATA_LENGTH);
            file_accept.Close();
            can_transfer_file = false;
            GetDlgItemText(IDC_TEXT, str_text);
            str_text += "\r\n接收完成！";
            SetDlgItemText(IDC_TEXT, str_text);
        }
    }

    return 0;
}


void CChatClientDlg::OnDestroy() {
    CDialogEx::OnDestroy();
    WSACleanup();
}
