// CMessage.cpp: 实现文件
//

#include "stdafx.h"
#include "ChatClient.h"
#include "CMessage.h"
#include "afxdialogex.h"

// CMessage 对话框

IMPLEMENT_DYNAMIC(CMessage, CDialogEx)

CMessage::CMessage(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_REQUEST, pParent)
{

}

CMessage::~CMessage()
{
}

void CMessage::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMessage, CDialogEx)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_AGREE, &CMessage::OnBnClickedAgree)
    ON_BN_CLICKED(IDC_REFUSE, &CMessage::OnBnClickedRefuse)
END_MESSAGE_MAP()


// CMessage 消息处理程序

BOOL CMessage::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  在此添加额外的初始化
    CFont font;
    font.CreatePointFont(150, _T("宋体"), NULL);
    GetDlgItem(IDC_MESSAGE)->SetFont(&font);
    GetDlgItem(IDC_AGREE)->SetFont(&font);
    GetDlgItem(IDC_REFUSE)->SetFont(&font);
    GetDlgItem(IDC_TIME)->SetFont(&font);

    SetDlgItemInt(IDC_TIME, 30);
    SetDlgItemText(IDC_MESSAGE, message.c_str());
    SetTimer(1, 1000, nullptr);

    return TRUE;
}

void CMessage::OnTimer(UINT_PTR nIDEvent)
{
    switch (nIDEvent) {
        case 1: {
            int time = GetDlgItemInt(IDC_TIME);
            --time;
            if (time == 0) {
                EndDialog(-1);
            }
            SetDlgItemInt(IDC_TIME, time);
        }
        break;

        default:
            break;
    }
    CDialogEx::OnTimer(nIDEvent);
}

void CMessage::OnBnClickedAgree()
{
    EndDialog(1);
}

void CMessage::OnBnClickedRefuse()
{
    EndDialog(-1);
}
