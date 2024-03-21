// MiniC++TestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MiniC++Test.h"
#include "MiniC++TestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString source="";
CString folder="";
char name_buffer[255];

// CAboutDlg dialog used for App About
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMiniCTestDlg dialog



CMiniCTestDlg::CMiniCTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMiniCTestDlg::IDD, pParent)
	, optimizeCode(FALSE)
	, sourceStr(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMiniCTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK1, optimizeCode);
	DDX_Control(pDX, IDC_LIST1, fileList);
	DDX_Control(pDX, IDC_EDIT2, sourceEdit);
	DDX_Text(pDX, IDC_EDIT2, sourceStr);
}

BEGIN_MESSAGE_MAP(CMiniCTestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_LBN_DBLCLK(IDC_LIST1, OnLbnDblclkList1)
END_MESSAGE_MAP()


// CMiniCTestDlg message handlers

BOOL CMiniCTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	
	fileList.AddString("errors.cpp");
	fileList.AddString("fact.cpp");
	fileList.AddString("misc.cpp");
	fileList.AddString("stack.cpp");
	fileList.AddString("OOP.cpp");
//	fileList.AddString("errors.cpp");
//	fileList.AddString("errors.cpp");

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMiniCTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMiniCTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMiniCTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//select new file
void CMiniCTestDlg::OnBnClickedButton2()
{
	CFileDialog openDlg(TRUE,0,source,OFN_EXPLORER);
	if(openDlg.DoModal()==IDOK){
		source=openDlg.GetPathName();
	//	MessageBox(source);
		CString cmd2="notepad ";
		cmd2+=source;
		WinExec(cmd2,true);
	}
}


// compile-run selected file
void CMiniCTestDlg::OnBnClickedButton1()
{
	UpdateData(TRUE);
	CString command="minic++ ";
	if(optimizeCode)
		command+=" -optimize ";
	command+=source;

//compile-build
	int res=WinExec(command,true);
//	if(res=-1)
//		MessageBox("failed to build due compilation errors");
//	else
	Sleep(500);

	CString cmd2="notepad c:\\1.vm";
	WinExec(cmd2,true);
	Sleep(100);	
	WinExec("vm.exe c:\\1.vm",true);
	

}

void CMiniCTestDlg::OnLbnDblclkList1()
{
	int idx=fileList.GetCurSel();
	sourceStr="";
	CFile file;
	switch(idx){
		case 0:
			source="errors.cpp";
			break;
		case 1:
			source="fact.cpp";
			break;
		case 2:
			source="misc.cpp";
			break;
		case 3:
			source="stack.cpp";
			break;
		case 4:
			source="OOP.cpp";
			break;
	}
	if(!file.Open("fact.cpp",CFile::modeRead)){
		MessageBox("file not found");
		return;
	}
	char buffer[1024];
	int count=1024;
	file.Read(buffer,count);
	sourceStr+=buffer;
	MessageBox(sourceStr);
	UpdateData(FALSE);
	
}
