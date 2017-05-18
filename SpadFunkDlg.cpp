
// SpadFunkDlg.cpp : implementation file
//
//	2016-02-14	Infört dubbla datalagringar; db och filer.
//				Spadfunk hämtar info (DataMode) via admi-1000-param1 som sedan spaddata använder
//

#include "stdafx.h"
#include "SpadFunk.h"
#include "SpadFunkDlg.h"
#include "SpadData.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// globaler, löses vid länkning för användare i andra cpp:er genom extern...
// värdet för datamode uppdateras via admi-funktionspost param1
// path får ändras hårdkodat

int     ge_DataMode = 1;
CString ge_Path = "c:\\SpadFiler\\";

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
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

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSpadFunkDlg dialog


// Konstruktor
CSpadFunkDlg::CSpadFunkDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSpadFunkDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_psd = NULL;
	m_psd = new CSpadData;

	// hämta datamode från gemensan inifil spad.ini
	char wstr[5];
	// ge-Path är hårdkodad, se ovan
	GetPrivateProfileString("Y", "DataMode", "", wstr, 5, ge_Path+"spad.ini");
	ge_DataMode = atoi(wstr);

	m_funktionsnummer = 1001;
	if ( __argc > 1 && memcmp(*(__argv+1), "10", 2) == 0)
		m_funktionsnummer = atoi(*(__argv+1));

	CSpadData::typ_SpadBest sb;
	memset(&sb, 0, sizeof(CSpadData::typ_SpadBest));
	m_psd->HamtaDatumKlocka(&sb.SenastDatum, &sb.SenastKlock);
	sb.FunktionsNummer = m_funktionsnummer;
	sb.Kommando = 526; // programmet startar
	m_psd->InsertSpadBest(&sb); 
}

// Destruktor
CSpadFunkDlg::~CSpadFunkDlg()
{
	CSpadData::typ_SpadBest sb;
	memset(&sb, 0, sizeof(CSpadData::typ_SpadBest));
	m_psd->HamtaDatumKlocka(&sb.SenastDatum, &sb.SenastKlock);
	sb.FunktionsNummer = m_funktionsnummer;
	sb.Kommando = 528; // programmet slutar
	m_psd->InsertSpadBest(&sb);

	if (m_psd != NULL)
		delete m_psd;
}

void CSpadFunkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSpadFunkDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, &CSpadFunkDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CSpadFunkDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSpadFunkDlg message handlers

BOOL CSpadFunkDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	m_iTimerId = 1;

	CSpadData::typ_SpadAdmi sa;
	m_psd->SelectSpadAdmiFunk(1000, &sa);	// datamode finns i 1000-posten
	if (sa.Param1 == 1 || sa.Param1 == 2)	// db eller filer
		ge_DataMode = sa.Param1;

	m_psd->SelectSpadAdmiFunk(m_funktionsnummer, &sa);
	m_sniffintervall  = sa.SniffIntervallSekunder;
	m_korvarvsekunder = sa.KorVarvSekunder;
	m_aliverapp       = sa.AliveRapp;

	CString temp;
	temp.Format("Funktion %d", m_funktionsnummer);
	AfxGetMainWnd()->SetWindowTextA(temp);

	m_iOntimerAnt = 0;
	TimerOn();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSpadFunkDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSpadFunkDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSpadFunkDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSpadFunkDlg::OnBnClickedCancel()
{
	CDialogEx::OnOK();
}

//------------------------------------------------------------------------------

void CSpadFunkDlg::OnTimer(UINT nIDEvent) 
{
	TimerOff();

	m_iOntimerAnt++;
	CString temp, temp2, wtitel;
	CSpadData::typ_SpadBest sb;

	CSpadData::typ_SpadAdmi sa;
	m_psd->SelectSpadAdmiFunk(1000, &sa);   // datamode ligger 1000-posten
	if (sa.Param1 == 1 || sa.Param1 == 2)	// db eller filer
		ge_DataMode = sa.Param1;
	m_psd->SelectSpadAdmiFunk(m_funktionsnummer, &sa);
	m_sniffintervall  = sa.SniffIntervallSekunder;
	m_korvarvsekunder = sa.KorVarvSekunder;
	m_aliverapp       = sa.AliveRapp; 

	// om avbryt exe så EndDialog(99) och return, innan fixa bort avbrytkoden (99) i kommando i admi
	// sb kommando 528 skapas ju sedan redan, som för avslutaknappen, i destruktorn
	if (m_psd->KollaBegaranAvbrytExekveringFinns(m_funktionsnummer))
	{
		m_psd->NollstallBegaran(m_funktionsnummer);
		EndDialog(99);
		return;
	}

	temp.Format("sniff-sniff-sniff %d, funk %d, sniff %d sek, varv %d sek, rapp %d, datamode %d", m_iOntimerAnt, 
		 m_funktionsnummer, m_sniffintervall, m_korvarvsekunder, m_aliverapp, ge_DataMode);
	_Messa(temp, 0);

	std::vector<CSpadData::typ_SpadBest>  bestvector;
	bestvector.clear();
	// Hämta alla ettor som har returkod noll
	m_psd->SelectSpadBestNya(&bestvector, m_funktionsnummer);

	if (bestvector.size() == 0 && m_aliverapp == 1) // alive and idle
	{
		memset(&sb, 0, sizeof(sb));
		sb.FunktionsNummer = m_funktionsnummer;
		sb.Kommando = 77;
		m_psd->HamtaDatumKlocka(&sb.SenastDatum, &sb.SenastKlock);
		m_psd->InsertSpadBest(&sb);
	}

	if (bestvector.size() > 0)
	{
		sb = bestvector[0];

		wtitel = sb.BestTitel;
		wtitel.Trim();
		temp.Format("Beställning nr %d: '%s' %d varv körs igång %d %d", 
			sb.BestNummer, wtitel,
			sb.BestAntal,
			sb.BestDatum,
			sb.BestKlock);
		_Messa(temp, 0);

		// Markera att beställningen, kommando 1, är omhändertagen av köraren
		// genom att märka den i returkod
		m_psd->UpdateraSpadBestBestIDReturkod(sb.BestID, 181);

		m_psd->HamtaDatumKlocka(&sb.KorstartDatum, &sb.KorstartKlock);
		sb.SenastDatum = sb.KorstartDatum;
		sb.SenastKlock = sb.KorstartKlock;
		sb.Kommando  = 2;
		m_psd->InsertSpadBest(&sb);

		int retur = Korning_0000(&sb, m_korvarvsekunder, m_funktionsnummer);

		m_psd->HamtaDatumKlocka(&sb.KorslutDatum, &sb.KorslutKlock);
		sb.SenastDatum = sb.KorslutDatum;
		sb.SenastKlock = sb.KorslutKlock;
		if (retur > 0) // avbruten
		{
			sb.Kommando = 7;
			temp2.Format("AVBRUTEN");
		}
		else
		{
			sb.Kommando = 4;
			temp2.Format("KLAR");
		}
		sb.ProgressNummer = 999999999;
		m_psd->InsertSpadBest(&sb);
			
		temp.Format("Beställning nr %d: '%s' %d varv %s %d %d", 
			sb.BestNummer, wtitel,
			sb.BestAntal,
			temp2,
			sb.KorslutDatum,
			sb.KorslutKlock);
		_Messa(temp, 0);
	}

	TimerOn();
}

void CSpadFunkDlg::TimerOn()
{
	m_WinTimerId = SetTimer(m_iTimerId, m_sniffintervall*1000, NULL);
	m_timeron = true;
}

void CSpadFunkDlg::TimerOff()
{
    KillTimer(m_WinTimerId);
	m_timeron = false;
}

int CSpadFunkDlg::Korning_0000(CSpadData::typ_SpadBest *sb, int korvarvsekunder, int funktionsnummer)
{
	for (int varv = 1; varv <= sb->BestAntal; varv++)
	{
		Sleep(korvarvsekunder * 1000);
		sb->ProgressNummer = varv;
		sb->Kommando = 3;
		m_psd->HamtaDatumKlocka(&sb->ProgressDatum, &sb->ProgressKlock);
		sb->SenastDatum = sb->ProgressDatum;
		sb->SenastKlock = sb->ProgressKlock;

		sb->KorPostAntal = varv*7000;
		sb->ResultatBelopp1 = varv*100;
		sb->ResultatBelopp2 = varv*200;
		sb->ResultatBelopp3 = varv*300;
		m_psd->InsertSpadBest(sb);

		while ( m_psd->KollaBegaranPausaKorningFinns(funktionsnummer) == TRUE ) // så länge admikommando = 6
		{
			m_psd->HamtaDatumKlocka(&sb->KorslutDatum, &sb->KorslutKlock);
			sb->SenastDatum = sb->ProgressDatum;
			sb->SenastKlock = sb->ProgressKlock;
			sb->Kommando = 16;
			m_psd->InsertSpadBest(sb);
			Sleep(korvarvsekunder * 1000);
		}

		if (sb->Kommando == 16) // om vi har haft paus så berätta att vi fortsätter
		{
			sb->Kommando = 17;
			m_psd->HamtaDatumKlocka(&sb->ProgressDatum, &sb->ProgressKlock);
			sb->SenastDatum = sb->ProgressDatum;
			sb->SenastKlock = sb->ProgressKlock;
			m_psd->InsertSpadBest(sb);
		}

		if (m_psd->KollaBegaranAvbrytKorningFinns(funktionsnummer)) // har pausen brutits pga avbrottsönskan eller fortsätt
			                                                        // eller bara vanligt varvslut och avbrytkoll
		{
			m_psd->NollstallBegaran(funktionsnummer); // Admi kom = 0
			// gör mera
			return 7;
		}

	}
	return 0;
}

int CSpadFunkDlg::_Messa(CString csmess, char how)
{

    CTime time;
    CString temp;

    if (how != 14)
    {
        time = CTime::GetCurrentTime();
        temp = time.Format("%y-%m-%d  %H.%M.%S  ");
        csmess.Insert(0, temp);
    }

    csmess.Append("\n"); 
//    fputs(csmess, m_loggfil);
    int iRad = SendDlgItemMessage(IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)(LPCSTR) csmess);
    SendDlgItemMessage(IDC_LIST1, LB_SETTOPINDEX, iRad, 0);  

    GetDlgItem(IDC_LIST1)->UpdateWindow();
    _PumpMessages();
    return 0;
}

int CSpadFunkDlg::_PumpMessages()
{
    // Must call Create() before using the dialog
    ASSERT(m_hWnd!=NULL);

    MSG msg;
    // Handle dialog messages
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if(!IsDialogMessage(&msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);  
        }
    }
    return 0;
}


void CSpadFunkDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}
