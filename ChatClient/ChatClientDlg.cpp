
// ChatClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ChatClient.h"
#include "ChatClientDlg.h"
#include "CMessage.h"
#include "afxdialogex.h"

#include "protocol.h"
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

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CChatClientDlg �Ի���

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
    ON_BN_CLICKED(IDC_SEND, &CChatClientDlg::OnBnClickedSend)
    ON_MESSAGE(WM_SOCKET, OnSocket)
    ON_BN_CLICKED(IDC_TRANSFERFILE, &CChatClientDlg::OnBnClickedTransferfile)
    ON_MESSAGE(WM_TRANSFERFILEPROGRESS, OnTransferFileProgress)
    ON_LBN_SELCHANGE(IDC_LOGIN_PEOPLE, &CChatClientDlg::OnLbnSelchangeLoginPeople)
    ON_WM_CTLCOLOR()
    ON_CBN_SELCHANGE(IDC_STATUS, &CChatClientDlg::OnCbnSelchangeStatus)
    ON_WM_DROPFILES()
    ON_WM_DESTROY()
    ON_STN_CLICKED(IDC_INFO, &CChatClientDlg::OnStnClickedInfo)
END_MESSAGE_MAP()

// CChatClientDlg ��Ϣ�������

BOOL CChatClientDlg::OnInitDialog()
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
    InitControl();
    ::WSAAsyncSelect(socketClient_, this->m_hWnd, WM_SOCKET, FD_READ);

    if (!loginFlag_)
    {
        if (MessageBox(_T("����˺Ŵ�������й¶����,�Ƿ��������?"), _T("����"), MB_YESNO | MB_ICONASTERISK) == IDYES)
        {
            CChangePassword dlg;
            dlg.socketClient_ = socketClient_;
            dlg.customerId_ = customerId_;
            dlg.DoModal();
        }
    }
    for (unsigned short i = 0; i < 5; ++i)
    {
        fileTrans_.emplace_back(std::thread(&CChatClientDlg::ThreadHandler, this, i));
    }
    return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CChatClientDlg::OnPaint()
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
HCURSOR CChatClientDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CChatClientDlg::OnBnClickedSend()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    CString strText, strSend, strSendTarget;
    s_HandleRecv toSend;
    int curSel = m_list_login_people.GetCurSel();
    if (curSel == CB_ERR)
    {
        MessageBox(_T("��ѡ��һ���û���"), _T("����"), MB_ICONERROR);
    }
    else
    {
        m_list_login_people.GetText(curSel, strSendTarget);
        GetDlgItemText(IDC_SENDTEXT, strSend);
        if (strSend == "")
        {
            MessageBox(_T("�������ݲ���Ϊ��...."), _T("����"), MB_ICONERROR);
        }
        else
        {
            int pos = strSendTarget.Find('\t', 0);
            if (pos != -1)
            {
                std::string sysTime = GetSystemTime();
                toSend.Param.chat_.sourceid = customerId_;
                toSend.Param.chat_.targetid = strtoull(strSendTarget.Left(pos).GetBuffer(0), nullptr, 10);
                RegisterSpace(&toSend.Param.chat_.content, strSend.GetBuffer(0));
                RegisterSpace(&toSend.Param.chat_.chat_time, sysTime);
                std::string jsSend = EncodeJson(CommunicationType::CHAT, toSend);
                ::send(socketClient_, jsSend.c_str(), jsSend.length(), 0);
                auto itor = chatMessage_.find(toSend.Param.chat_.targetid);
                if (itor != chatMessage_.end())
                {
                    std::ostringstream chatResult;
                    chatResult << itor->second << sysTime << "  /send\r\n" << toSend.Param.chat_.content << "\r\n\r\n";
                    chatMessage_[toSend.Param.chat_.targetid] = chatResult.str();
                    SetDlgItemText(IDC_TEXT, chatResult.str().c_str());
                }
                UnregisterSpace(CommunicationType::CHAT, toSend);
            }
            else
            {
                MessageBox(_T("�ڲ�����,������!!!"), _T("����"), MB_ICONERROR);
            }
        }
    }
    strText.ReleaseBuffer();
    strSend.ReleaseBuffer();
    strSendTarget.ReleaseBuffer();
}

void CChatClientDlg::OnBnClickedTransferfile()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    int curSel = m_list_login_people.GetCurSel();
    if (curSel == CB_ERR)
    {
        MessageBox(_T("��ѡ��һ���û���"), _T("����"), MB_ICONERROR);
    }
    else
    {
        CString strSendTarget, strFilePath;
        m_list_login_people.GetText(curSel, strSendTarget);
        GetDlgItemText(IDC_FILENAME, strFilePath);
        if (strFilePath.IsEmpty())
        {
            TCHAR szFilter[] = _T("�ı��ļ�(*.txt)|*.txt|�����ļ�(*.*)|*.*||");
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
            std::lock_guard<std::mutex> lg(quMu_);
            transferFile_.push(TransFile(0, strFilePath.GetBuffer(0), strtoull(strSendTarget.Left(pos).GetBuffer(0), nullptr, 10)));
        }
        strFilePath.ReleaseBuffer();
        strSendTarget.ReleaseBuffer();
    }
}

void CChatClientDlg::OnOK()
{
    // TODO: �ڴ����ר�ô����/����û���
    OnBnClickedSend();
}

LRESULT CChatClientDlg::OnSocket(WPARAM wParam, LPARAM lParam)
{
    char* strRecv = new char[DATA_LENGTH];
    CString strText;
    memset(strRecv, 0, DATA_LENGTH);
    if (lParam == FD_READ)
    {
        UpdateData(TRUE);
        ::recv(socketClient_, strRecv, DATA_LENGTH, 0);
        s_HandleRecv handleRecv, toSend;
        if (!DecodeJson(strRecv, handleRecv))
        {
            delete[]strRecv;
            strRecv = nullptr;
            return -1;
        }
        switch (handleRecv.type_)
        {
        case CommunicationType::SHOWLOGIN:
        {
            if (handleRecv.Param.showLogin_.show_login_type == 0)
            {
                std::vector<std::string> peopleList;
                SplitString(handleRecv.Param.showLogin_.customer, '|', peopleList);
                for (size_t i = 0; i < peopleList.size(); ++i)
                {
                    std::vector<std::string> peopleInfo;
                    SplitString(peopleList.at(i), ':', peopleInfo);
                    if (peopleInfo.size() != 3 || (peopleInfo.at(0) == std::to_string(customerId_)))
                    {
                        continue;
                    }
                    loginInfo_[strtoull(peopleInfo.at(0).c_str(), nullptr, 10)] =
                        std::make_pair(peopleInfo.at(1), (peopleInfo.at(2) == "1") ? "����" : "����");
                }
                for (auto itor = loginInfo_.begin(); itor != loginInfo_.end(); ++itor)
                {
                    char temp[50];
                    memset(temp, 0, 50);
                    sprintf_s(temp, 50, "%llu\t\t%s\t\t\t%s", itor->first, itor->second.first.c_str(), itor->second.second.c_str());
                    m_list_login_people.AddString(temp);
                    chatMessage_.insert(std::make_pair(itor->first, ""));
                }
            }
            else if ((handleRecv.Param.showLogin_.show_login_type == 1) || (handleRecv.Param.showLogin_.show_login_type == -1))
            {
                std::vector<std::string> peopleInfo;
                SplitString(handleRecv.Param.showLogin_.customer, ':', peopleInfo);
                if (peopleInfo.size() != 3 || (peopleInfo.at(0) == std::to_string(customerId_)))
                {
                    break;
                }
                UINT64 id = strtoull(peopleInfo.at(0).c_str(), nullptr, 10);
                loginInfo_[id] =
                    std::make_pair(peopleInfo.at(1), (handleRecv.Param.showLogin_.show_login_type == 1) ? "����" : "����");
                auto itor = loginInfo_.find(id);
                int pos = std::distance(loginInfo_.begin(), itor);
                char temp[50];
                memset(temp, 0, 50);
                sprintf_s(temp, 50, "%llu\t\t%s\t\t\t%s", id, loginInfo_[id].first.c_str(), loginInfo_[id].second.c_str());
                UpdateListBox(pos, temp);
            }
        }
        break;

        case CommunicationType::CHAT:
        {
            if (handleRecv.Param.chat_.targetid == customerId_)
            {
                std::ostringstream chatResult;
                // ��Ϣ���
                auto itor1 = chatMessage_.find(handleRecv.Param.chat_.sourceid);
                if (itor1 != chatMessage_.end())
                {
                    chatResult << itor1->second;
                }
                chatResult << handleRecv.Param.chat_.chat_time << "  /recv\r\n" << handleRecv.Param.chat_.content << "\r\n\r\n";
                if (itor1 == chatMessage_.end())
                {
                    chatMessage_.insert(std::make_pair(handleRecv.Param.chat_.sourceid, chatResult.str()));
                }
                else
                {
                    chatMessage_[handleRecv.Param.chat_.sourceid] = chatResult.str();
                }
                // ����Ϣ��ʾ
                auto itor2 = loginInfo_.find(handleRecv.Param.chat_.sourceid);
                if (itor2 != loginInfo_.end())
                {
                    int distance = std::distance(loginInfo_.begin(), itor2);
                    int pos = m_list_login_people.GetCurSel();
                    if (distance != pos)
                    {
                        char temp[50];
                        memset(temp, 0, 50);
                        sprintf_s(temp, 50, "%llu\t\t%s\t\t\t%s new", itor2->first, itor2->second.first.c_str(), itor2->second.second.c_str());
                        UpdateListBox(distance, temp);
                        memset(temp, 0, 50);
                        sprintf_s(temp, 50, "�յ����� %llu %s������Ϣ", itor2->first, itor2->second.first.c_str());
                        SetDlgItemText(IDC_INFO, temp);
                    }
                    else
                    {
                        SetDlgItemText(IDC_TEXT, chatMessage_[handleRecv.Param.chat_.sourceid].c_str());
                    }
                }
            }
        }
        break;

        case CommunicationType::FORCEDELETE:
        {
            if (handleRecv.Param.forceDelete_.id == customerId_)
            {
                MessageBox(_T("�����˺����������ص��¼,��ȷ��!!!"), _T("����"), MB_ICONERROR);
                EndDialog(-1);
            }
        }
        break;

        case CommunicationType::TRANSFERFILEINFO:
        {
            if (handleRecv.Param.transferFileInfo_.targetid == customerId_)
            {
                std::lock_guard<std::mutex> lg(quMu_);
                transferFile_.push(TransFile(1, handleRecv.Param.transferFileInfo_.file_name, handleRecv.Param.transferFileInfo_.sourceid,
                    handleRecv.Param.transferFileInfo_.file_length, handleRecv.Param.transferFileInfo_.file_block));
            }
        }
        break;

        }
        UnregisterSpace(handleRecv.type_, handleRecv);
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
    m_transfer_progress.SetPos(static_cast<int>(progress * 100));
    UpdateData(FALSE);

    return 0;
}

void CChatClientDlg::OnLbnSelchangeLoginPeople()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    UpdateData(TRUE);
    SetDlgItemText(IDC_INFO, _T(""));
    SetDlgItemText(IDC_SENDTEXT, _T(""));
    SetDlgItemText(IDC_FILENAME, _T(""));
    m_transfer_progress.SetPos(0);
    CString strTarget;
    int curSel = m_list_login_people.GetCurSel();
    m_list_login_people.GetText(curSel, strTarget);
    int pos = strTarget.Find('\t', 0);
    if (pos != -1)
    {
        CString newString = strTarget.Right(3);
        if (strcmp(newString, "new") == 0)
        {
            CString newShow = strTarget.Left(strTarget.GetLength() - 3);
            UpdateListBox(pos, newShow.GetBuffer(0));
            m_list_login_people.SetCurSel(curSel);
            newShow.ReleaseBuffer();
        }
        UINT64 id = strtoull(strTarget.Left(pos).GetBuffer(0), nullptr, 10);
        auto itor1 = chatMessage_.find(id);
        if (itor1 != chatMessage_.end())
        {
            SetDlgItemText(IDC_TEXT, itor1->second.c_str());
        }
        auto itor2 = progressNum_.find(id);
        if (itor2 != progressNum_.end())
        {
            m_transfer_progress.SetPos(itor2->second);
        }
    }
    strTarget.ReleaseBuffer();
    UpdateData(FALSE);
}

HBRUSH CChatClientDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  �ڴ˸��� DC ���κ�����
    if (pWnd->GetDlgCtrlID() == IDC_INFO)
    {
        pDC->SetTextColor(RGB(255, 0, 0));
        pDC->SetBkMode(TRANSPARENT);
    }

    // TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
    return hbr;
}

void CChatClientDlg::OnCbnSelchangeStatus()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
}

void CChatClientDlg::InitControl()
{
    CFont font;
    font.CreatePointFont(150, _T("����"), NULL);
    // ���
    GetDlgItem(IDC_STATICINFO)->SetFont(&font);
    GetDlgItem(IDC_STATICLOGIN)->SetFont(&font);
    // ��¼��Ϣ
    GetDlgItem(IDC_STATICID_LOGIN)->SetFont(&font);
    GetDlgItem(IDC_ID)->SetFont(&font);
    SetDlgItemText(IDC_ID, std::to_string(customerId_).c_str());
    GetDlgItem(IDC_STATICNAME)->SetFont(&font);
    GetDlgItem(IDC_NAME)->SetFont(&font);
    SetDlgItemText(IDC_NAME, customerName_.c_str());
    GetDlgItem(IDC_LOGIN_PEOPLE)->SetFont(&font);
    // ����
    GetDlgItem(IDC_TEXT)->SetFont(&font);
    GetDlgItem(IDC_SENDTEXT)->SetFont(&font);
    GetDlgItem(IDC_SEND)->SetFont(&font);
    GetDlgItem(IDC_FILENAME)->SetFont(&font);
    GetDlgItem(IDC_TRANSFERFILE)->SetFont(&font);
    GetDlgItem(IDC_INFO)->SetFont(&font);

    SetDlgItemText(IDC_INFO, _T(""));
    GetDlgItem(IDC_TRANSFILEPROGRESS)->ShowWindow(false);
    m_transfer_progress.SetRange(0, 100);
    m_transfer_progress.SetPos(0);

    m_status.AddString(_T("����"));
    m_status.AddString(_T("æµ"));
    m_status.AddString(_T("�뿪"));
    m_status.AddString(_T("����"));
    m_status.AddString(_T("�Զ���"));
    m_status.SetCurSel(0);

    DragAcceptFiles(TRUE);
}

inline void CChatClientDlg::UpdateListBox(int pos, const std::string& newMessage)
{
    m_list_login_people.DeleteString(pos);
    m_list_login_people.InsertString(pos, newMessage.c_str());
}

void CChatClientDlg::ThreadHandler(const unsigned short threadNum)
{
    while (1)
    {
        if (threadExit_)
        {
            break;
        }
        TransFile transFile;
        {
            std::lock_guard<std::mutex> lg(quMu_);
            if (transferFile_.empty())
            {
                Sleep(1);
                continue;
            }
            transFile = transferFile_.front();
            transferFile_.pop();
        }
        GetDlgItem(IDC_TRANSFILEPROGRESS)->ShowWindow(true);
        if (transFile.mode_ == 0)
        {
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
        }
        else if (transFile.mode_ == 1)
        {
            CString pathName;
            TCHAR szFilter[] = _T("�ı��ļ�(*.txt)|*.txt|�����ļ�(*.*)|*.*||");
            CFileDialog fileDlg(FALSE, transFile.fileName_.substr(transFile.fileName_.find_last_of('.') + 1).c_str(), transFile.fileName_.c_str(),
                0, szFilter, this);
            if (IDOK == fileDlg.DoModal())
            {
                pathName = fileDlg.GetPathName();
            }
            Sleep(100);
            CFile file;
            file.Open(pathName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
            char* getStr = new char[FILE_DATA_LENGTH];
            s_HandleRecv handleRecv;
            for (unsigned int nowBlock = 0; nowBlock < transFile.fileBlock_; ++nowBlock)
            {
               /* memset(getStr, 0, FILE_DATA_LENGTH);
                ::recv(socketClient_, getStr, FILE_DATA_LENGTH, 0);
                if (!DecodeJson(getStr, handleRecv))
                {
                    continue;
                }
                file.Write(getStr, FILE_DATA_LENGTH);
                RegisterSpace(&toSend.Param.transferFileContent_.file_content, getStr);
                toSend.Param.transferFileContent_.now_block = nowBlock++;
                std::string jsSend = EncodeJson(CommunicationType::TRANSFERFILECONTENT, toSend);
                UnregisterSpace(CommunicationType::TRANSFERFILECONTENT, toSend);
                ::send(socketClient_, jsSend.c_str(), jsSend.length(), 0);
                m_file.Seek(FILE_DATA_LENGTH * toSend.Param.transferFileContent_.now_block, CFile::begin);
                double progress = static_cast<double>(nowBlock / static_cast<double>(fileSize));
                PostMessage(WM_TRANSFERFILEPROGRESS, (WPARAM)progress, 0);*/
                Sleep(500);
            }
            delete[]getStr;
            getStr = nullptr;
            file.Close();
            Sleep(1000);
        }
        GetDlgItem(IDC_TRANSFILEPROGRESS)->ShowWindow(false);

        Sleep(1);
    }
}

void CChatClientDlg::OnDropFiles(HDROP hDropInfo)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    TCHAR filePath[MAX_PATH] = { 0 };
    DragQueryFile(hDropInfo, 0, filePath, sizeof(filePath));
    SetDlgItemText(IDC_TRANSFERFILE, filePath);
    DragFinish(hDropInfo);
    CDialogEx::OnDropFiles(hDropInfo);
}


void CChatClientDlg::OnDestroy()
{
    CDialogEx::OnDestroy();

    // TODO: �ڴ˴������Ϣ����������
    threadExit_ = true;
    for (int i = 0; i < 5; ++i)
    {
        if (fileTrans_[i].joinable())
        {
            fileTrans_[i].join();
        }
    }
}

void CChatClientDlg::OnStnClickedInfo()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
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
        char temp[50];
        memset(temp, 0, 50);
        sprintf_s(temp, 50, "%llu\t\t%s\t\t\t%s", id, loginInfo_[id].first.c_str(), loginInfo_[id].second.c_str());
        UpdateListBox(distance, temp);
        m_list_login_people.SetCurSel(distance);
        SetDlgItemText(IDC_TEXT, chatMessage_[id].c_str());
        SetDlgItemText(IDC_INFO, _T(""));
    }
    strInfo.ReleaseBuffer();
}
