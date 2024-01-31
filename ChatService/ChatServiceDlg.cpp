
// ChatServiceDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ChatService.h"
#include "ChatServiceDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
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

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CChatServiceDlg �Ի���

CChatServiceDlg::CChatServiceDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_CHATSERVICE_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChatServiceDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_LOGIN_PEOPLE, m_listLoginPeople);
}

BEGIN_MESSAGE_MAP(CChatServiceDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_START, &CChatServiceDlg::OnBnClickedStart)
    ON_MESSAGE(WM_SOCKET_TCP, OnSocketTcp)
    ON_BN_CLICKED(IDC_KICK, &CChatServiceDlg::OnBnClickedKick)
END_MESSAGE_MAP()

// CChatServiceDlg ��Ϣ�������

BOOL CChatServiceDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // ��������...���˵�����ӵ�ϵͳ�˵��С�

    // IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

    // ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
    //  ִ�д˲���
    SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
    SetIcon(m_hIcon, FALSE);		// ����Сͼ��

    // TODO: �ڴ���Ӷ���ĳ�ʼ������
    HANDLE chatServiceMutex = CreateMutex(nullptr, TRUE, INSTANCE_NAME.c_str());
    if (chatServiceMutex == nullptr)
    {
        MessageBox(_T("����������ʧ�ܣ������˳�!!!"));
        return FALSE;
    }
    else if (chatServiceMutex != nullptr && GetLastError() == ERROR_ALREADY_EXISTS)
    {
        MessageBox(_T("�Ѿ������������������У���رմ˳���!!!"));
        return FALSE;
    }
    CloseHandle(chatServiceMutex);

    InitControl();

    service_ = new Service();
    if (service_ == nullptr)
    {
        MessageBox(_T("��ȡ��������ʵ��ʧ��....������!!!"));
        return FALSE;
    }
    CallBack callBack = std::bind(&CChatServiceDlg::EventHandle, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    int ret = service_->Init(callBack);
    if (ret == -1)
    {
        MessageBox(_T("��������ʧ��....������!!!"));
        return FALSE;
    }

    std::string ip = service_->GetServerIp();
    SetDlgItemText(IDC_IPLIST, ip.c_str());

    return TRUE;
}

void CChatServiceDlg::EventHandle(const CallBackType callBackType, const std::string& message, bool isError)
{
    switch (callBackType)
    {
        case CallBackType::INIT:
        {

            break;
        }

        case CallBackType::LOGIN:
        {
            std::vector<std::string> des;
            SplitString(message, COMBINE_ONE_CUSTOMER, des);
            if (des.size() == LOGIN_PEOPLE_COL)
            {
                int cou = m_listLoginPeople.GetItemCount();
                m_listLoginPeople.InsertItem(cou, "");
                for (int i = 0; i < LOGIN_PEOPLE_COL; ++i)
                {
                    m_listLoginPeople.SetItemText(cou, i, des.at(i).c_str());
                }
            }
            break;
        }

        case CallBackType::LOGOUT:
        {
            int cou = m_listLoginPeople.GetItemCount(), index = 0;
            for (; index < cou; ++index)
            {
                CString id = m_listLoginPeople.GetItemText(index, 0);
                if (id.GetBuffer() == message)
                {
                    break;
                }
            }
            m_listLoginPeople.DeleteItem(index);
            break;
        }

        case CallBackType::SEND:
        {

            break;
        }

        default:
            break;
    }
}

void CChatServiceDlg::InitControl()
{
    CFont font;
    font.CreatePointFont(150, _T("����"), NULL);
    GetDlgItem(IDC_STATUS)->SetFont(&font);
    GetDlgItem(IDC_START)->SetFont(&font);
    GetDlgItem(IDC_KICK)->SetFont(&font);
    GetDlgItem(IDC_STATIC_LOGIN_PEOPLE)->SetFont(&font);
    GetDlgItem(IDC_LIST_LOGIN_PEOPLE)->SetFont(&font);
    GetDlgItem(IDC_IPLIST)->SetFont(&font);

    m_listLoginPeople.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    CRect rect;
    m_listLoginPeople.GetWindowRect(rect);
    int perWidth = (rect.right - rect.left) / LOGIN_PEOPLE_COL;
    for (int i = 0; i < LOGIN_PEOPLE_COL; ++i)
    {
        m_listLoginPeople.InsertColumn(i, LOGIN_PEOPLE_LIST[i].c_str(), LVCFMT_CENTER, perWidth);
    }
}

BOOL CChatServiceDlg::DestroyWindow()
{
    if (service_ != nullptr)
    {
        delete service_;
        service_ = nullptr;
    }
    return CDialogEx::DestroyWindow();
}

void CChatServiceDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CChatServiceDlg::OnPaint()
{
    if (IsIconic())
    {
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
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CChatServiceDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CChatServiceDlg::OnBnClickedStart()
{
    CString str;
    GetDlgItemText(IDC_START, str);
    if (str == "����")
    {
        SOCKET socketServiceTcp = service_->StartTcp();
        if (socketServiceTcp != INVALID_SOCKET)
        {
            ::WSAAsyncSelect(socketServiceTcp, this->m_hWnd, WM_SOCKET_TCP, FD_ACCEPT | FD_READ | FD_CLOSE);
            SetDlgItemText(IDC_STATUS, _T("�����������Ѿ�����������"));
            SetDlgItemText(IDC_START, _T("ֹͣ"));
        }
        else
        {
            SetDlgItemText(IDC_STATUS, _T("�����������޷�����,��������״̬�����ԡ�����"));
        }
    }
    else if (str == "ֹͣ")
    {
        service_->StopTcp();
        SetDlgItemText(IDC_STATUS, _T("������δ��������������"));
        SetDlgItemText(IDC_START, _T("����"));
    }
}

void CChatServiceDlg::OnBnClickedKick()
{
    int index = m_listLoginPeople.GetSelectionMark();
    if (index < 0)
    {
        return;
    }
    CString id = m_listLoginPeople.GetItemText(index, 0);
    service_->HandleKick(strtoull(id, nullptr, 10));
    m_listLoginPeople.DeleteItem(index);
}

LRESULT CChatServiceDlg::OnSocketTcp(WPARAM wParam, LPARAM lParam)
{
    CString str_text;
    switch (lParam)
    {
        case FD_ACCEPT:
        {
            SOCKET socketClient = service_->ClientAccept();
            if (socketClient != INVALID_SOCKET)
            {
                ::WSAAsyncSelect(socketClient, this->m_hWnd, WM_SOCKET_TCP, FD_ACCEPT | FD_READ | FD_CLOSE);
            }
            break;
        }

        case FD_READ:
        {
            service_->ClientRecv(static_cast<SOCKET>(wParam));
            break;
        }

        case FD_CLOSE:
        {
            GetDlgItemText(IDC_TEXT, str_text);
            str_text += "\r\n";
            str_text += ::inet_ntoa(addrAccept.sin_addr);
            str_text += "�ѶϿ�";
            SetDlgItemText(IDC_TEXT, str_text);
            //m_list_login_people.DeleteString(del_index);
            break;
        }

        default:
            break;
    }
    return 0;
}