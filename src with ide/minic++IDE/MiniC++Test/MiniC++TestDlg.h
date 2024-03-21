// MiniC++TestDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CMiniCTestDlg dialog
class CMiniCTestDlg : public CDialog
{
// Construction
public:
	CMiniCTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MINICTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	BOOL optimizeCode;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton1();
	CListBox fileList;
	afx_msg void OnLbnDblclkList1();
	CEdit sourceEdit;
	CString sourceStr;
};
