// CLoginWait.cpp: 实现文件
//

#include "stdafx.h"
#include "ChatClient.h"
#include "CLoginWait.h"
#include "afxdialogex.h"


// CLoginWait 对话框

IMPLEMENT_DYNAMIC(CLoginWait, CDialogEx)

CLoginWait::CLoginWait(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOGIN_WAIT, pParent)
{

}

CLoginWait::~CLoginWait()
{
}

void CLoginWait::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLoginWait, CDialogEx)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CLoginWait 消息处理程序


BOOL CLoginWait::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  在此添加额外的初始化
    CFont font;
    font.CreatePointFont(150, _T("宋体"), NULL);
    GetDlgItem(IDC_LOGIN_TIME_OUT)->SetFont(&font);
    GetDlgItem(IDC_LOGIN)->SetFont(&font);

    SetDlgItemInt(IDC_LOGIN_TIME_OUT, 30);
    SetTimer(1, 1000, nullptr);
    SetTimer(2, 50, nullptr);

    return TRUE;
}

void CLoginWait::OnTimer(UINT_PTR nIDEvent)
{
    switch (nIDEvent) {
    case 1: {
        int time = GetDlgItemInt(IDC_LOGIN_TIME_OUT);
        --time;
        if (time == 0) {
            EndDialog(-1);
        }
        SetDlgItemInt(IDC_LOGIN_TIME_OUT, time);
    }
          break;

    case 2: {
        if (flag == 1) {
            EndDialog(1);
        }
        else if (flag == 2) {
            EndDialog(2);
        }
    }
          break;
    default:
        break;
    }

    CDialogEx::OnTimer(nIDEvent);
}

void CLoginWait::socketRecvThread(void* lParam)
{
    while (1) {
        CLoginWait* pThis = (CLoginWait*)lParam;
        char* buf = new char[DATA_LENGTH];
        memset(buf, 0, DATA_LENGTH);
        ::recv(pThis->socketClient, buf, DATA_LENGTH, 0);
        if (buf != '\0') {
            s_HandleRecv rt;
            DecodeJson(buf, rt);
            if (rt.type == CommunicationType::LOGINBACKSUCCEED) {
                pThis->flag = 1;
            }
            else if (rt.type == CommunicationType::LOGINBACKFAILED) {
                pThis->flag = 2;
            }
        }
        delete[]buf;
    }
}
