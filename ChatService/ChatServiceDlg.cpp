
// ChatServiceDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ChatService.h"
#include "ChatServiceDlg.h"
#include "afxdialogex.h"
using namespace cwy;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx {
  public:
    CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ABOUTBOX };
#endif

  protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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

// CChatServiceDlg �Ի���

CChatServiceDlg::CChatServiceDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_CHATSERVICE_DIALOG, pParent) {
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChatServiceDlg::DoDataExchange(CDataExchange* pDX) {
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_LOGIN_PEOPLE, listLoginPeople);
}

BEGIN_MESSAGE_MAP(CChatServiceDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_START, &CChatServiceDlg::OnBnClickedStart)
    ON_BN_CLICKED(IDC_KICK, &CChatServiceDlg::OnBnClickedKick)
    ON_WM_DESTROY()
    ON_MESSAGE(WM_SOCKET_TCP, OnSocketTcp)
    ON_MESSAGE(WM_SOCKET_UDP, OnSocketUdp)
    ON_MESSAGE(THREAD_CONTROL_UPDATE, HandleControlUpdate)
END_MESSAGE_MAP()

// CChatServiceDlg ��Ϣ�������

BOOL CChatServiceDlg::OnInitDialog() {
    CDialogEx::OnInitDialog();

    // ��������...���˵�����ӵ�ϵͳ�˵��С�

    // IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

    // ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
    //  ִ�д˲���
    SetIcon(m_hIcon, TRUE);         // ���ô�ͼ��
    SetIcon(m_hIcon, FALSE);        // ����Сͼ��

    // TODO: �ڴ���Ӷ���ĳ�ʼ������
    CFont font;
    font.CreatePointFont(150, _T("����"), NULL);
    GetDlgItem(IDC_STATUS)->SetFont(&font);
    GetDlgItem(IDC_START)->SetFont(&font);
    GetDlgItem(IDC_KICK)->SetFont(&font);
    GetDlgItem(IDC_STATIC_LOGIN_PEOPLE)->SetFont(&font);
    GetDlgItem(IDC_LIST_LOGIN_PEOPLE)->SetFont(&font);
    GetDlgItem(IDC_IPLIST)->SetFont(&font);

    logService.InitLog("../{time}/service");
    netWorkHandle = std::make_unique<CNetWorkHandle>();
    std::string ip = netWorkHandle->GetMainNetworkIp();
    netWorkHandle->StartThread(m_hWnd);
    std::string showIp = ((ip == "get error") ? ("ip get error") : ("������ip : " + ip));
    SetDlgItemText(IDC_IPLIST, showIp.c_str());

    return TRUE;
}

void CChatServiceDlg::OnDestroy()
{
    netWorkHandle->ExitThread();
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CChatServiceDlg::OnPaint() {
    if (IsIconic()) {
        CPaintDC dc(this); // ���ڻ��Ƶ��豸������

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // ʹͼ���ڹ����������о���
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // ����ͼ��
        dc.DrawIcon(x, y, m_hIcon);
    } else {
        CDialogEx::OnPaint();
    }
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CChatServiceDlg::OnQueryDragIcon() {
    return static_cast<HCURSOR>(m_hIcon);
}

void CChatServiceDlg::OnBnClickedStart() {
    CString str;
    GetDlgItemText(IDC_START, str);
    if (str == "����") {
        if (StartTcp() && StartUdp()) {
            SetDlgItemText(IDC_STATUS, _T("�����������Ѿ�����������"));
            SetDlgItemText(IDC_START, _T("ֹͣ"));
            logService << "��������������";
        }
    } else if (str == "ֹͣ") {
        ::closesocket(socketServiceTcp);
        ::closesocket(socketServiceUdp);
        SetDlgItemText(IDC_STATUS, _T("������δ��������������"));
        SetDlgItemText(IDC_START, _T("����"));
        logService << "�����������ر�";
    }
    logService.PrintlogInfo(FILE_FORMAT);
}

void CChatServiceDlg::OnBnClickedKick() {
    // TODO: �ڴ���ӿؼ�֪ͨ����������
}

bool CChatServiceDlg::StartTcp()
{
    socketServiceTcp = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socketServiceTcp == INVALID_SOCKET) {
        MessageBox(_T("����TCP����ʧ��"), _T("����"), MB_ICONERROR);
        return false;
    } else {
        SOCKADDR_IN addrServiceTcp{0};
        addrServiceTcp.sin_family = AF_INET;
        addrServiceTcp.sin_port = htons(TCP_PORT);
        addrServiceTcp.sin_addr.S_un.S_addr = INADDR_ANY;
        if (::bind(socketServiceTcp, (sockaddr*)&addrServiceTcp, sizeof(addrServiceTcp)) == SOCKET_ERROR) {
            MessageBox(_T("��TCP����ʧ��"), _T("����"), MB_ICONERROR);
            return false;
        }
        if (::listen(socketServiceTcp, SOMAXCONN) == SOCKET_ERROR) {
            MessageBox(_T("����TCP����ʧ��"), _T("����"), MB_ICONERROR);
            return false;
        }
        if (::WSAAsyncSelect(socketServiceTcp, this->m_hWnd, WM_SOCKET_TCP, FD_ACCEPT | FD_READ | FD_CLOSE) == SOCKET_ERROR) {
            MessageBox(_T("�����첽TCP����ʧ��"), _T("����"), MB_ICONERROR);
            return false;
        }
        logService << "������TCP�����������˿ں�Ϊ��" << TCP_PORT;
    }
    logService.PrintlogInfo(FILE_FORMAT);
    return true;
}

bool CChatServiceDlg::StartUdp()
{
    /*socketServiceUdp = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (socketServiceUdp == INVALID_SOCKET) {
        MessageBox(_T("����UDP����ʧ��"), _T("����"), MB_ICONERROR);
        return false;
    } else {
        addrServiceUdp.sin_family = AF_INET;
        addrServiceUdp.sin_port = htons(cwy::UDP_PORT);
        addrServiceUdp.sin_addr.S_un.S_addr = INADDR_ANY;
        ::bind(socketServiceUdp, (sockaddr*)&addrServiceUdp, sizeof(addrServiceUdp));
        ::WSAAsyncSelect(socketServiceUdp, this->m_hWnd, WM_SOCKET_UDP, FD_READ);
        netWorkHandle->SetUdpSocket(socketServiceUdp);
        logService << "������UDP�����������˿ں�Ϊ��" << cwy::UDP_PORT;
    }
    logService.PrintlogInfo(FILE_FORMAT);*/
    return true;
}

LRESULT CChatServiceDlg::OnSocketTcp(WPARAM wParam, LPARAM lParam) {
    switch (lParam) {
        case FD_ACCEPT: {
            SOCKADDR_IN addrClient = {0};
            int len = sizeof(SOCKADDR);
            SOCKET socket = ::accept(socketServiceTcp, (SOCKADDR*)&addrClient, &len);
            if (socket == SOCKET_ERROR) {
                break;
            }
            socket2IpMap.insert(std::make_pair(socket, inet_ntoa(addrClient.sin_addr)));
            break;
        }

        case FD_READ: {
            std::string ip;
            SOCKET socket = INVALID_SOCKET;
            char strRecv[DATA_LENGTH] = {0};
            for (std::pair<SOCKET, std::string> itor : socket2IpMap) {
                int ret = ::recv(itor.first, strRecv, DATA_LENGTH, 0);
                if (ret > 0) {
                    socket = itor.first;
                    ip = itor.second;
                    break;
                }
            }
            netWorkHandle->PushEvent(ClientInfoTcp(CommunicationType::NULLCOMMUNICATION, ip, strRecv, socket));
            break;
        }

        case FD_CLOSE: {
            int c = 3;
        }

        default:
            break;
    }
    return 0;
}

LRESULT CChatServiceDlg::OnSocketUdp(WPARAM wParam, LPARAM lParam)
{
    switch (lParam) {
        case FD_READ: {

            break;
        }

        default:
            break;
    }
    return 0;
}

LRESULT CChatServiceDlg::HandleControlUpdate(WPARAM wParam, LPARAM lParam)
{
    ClientInfoTcp* clientInfoTcp = (ClientInfoTcp*)wParam;
    UpdateData(TRUE);

    if (clientInfoTcp == nullptr) {
        return -1;
    }

    // ������Ϣ
    if (clientInfoTcp->GetType() == CommunicationType::ERRORCOMMUNICATION) {
        ::send(clientInfoTcp->GetSocket(), "unknown error", std::string("unknown error").length(), 0);
    } else if ((clientInfoTcp->GetType() != CommunicationType::NULLCOMMUNICATION) &&
               (clientInfoTcp->GetType() != CommunicationType::DELETECUSTOMER)) {
        ::send(clientInfoTcp->GetSocket(), clientInfoTcp->GetContent().c_str(), clientInfoTcp->GetContent().length(), 0);
    }

    // �������
    switch (clientInfoTcp->GetType()) {
        case CommunicationType::LOGINBACKSUCCEED: {
            Json::Value js_value;
            Json::Reader js_reader;
            if (js_reader.parse(clientInfoTcp->GetContent(), js_value)) {
                std::string userName = js_value["customer"].asString();
                listLoginPeople.AddString(userName.c_str());
            }
            break;
        }

        case CommunicationType::DELETECUSTOMER: {
            int index = listLoginPeople.FindString(-1, clientInfoTcp->GetContent().c_str());
            if (index != LB_ERR) {
                listLoginPeople.DeleteString(index);
            }
            break;
        }
    }

    UpdateData(FALSE);
    return 0;
}
