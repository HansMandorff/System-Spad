
// SpadFunkTVrapDlg.cpp : implementation file
//
// 2016-08-04
// Programmet skapas.
// Jag har tänkt att inom "spad-ramen" skapa ytterligare en funktion: rapporter som i testerna av
// nya siffergeneratorn, vecko, månad, år och reachutv.
// Nytt blir att hantera resultat, nog lagra rapportrader i basen.
//  
// Först lägga in ramverket från spadfunkdlg, ok.
// Lägg in en fjärde funktion i spad, 1004: 
// - rapptyp 1-4 i fältet för antal
// - callback
// 2016-08-17
// lägger till dagdelsrapport nummer5
//

#include "stdafx.h"
#include "SpadFunkTVrap.h"
#include "SpadFunkTVrapDlg.h"
//#include "SpadData.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// globaler, löses vid länkning för användare i andra cpp:er genom extern...
// värdet för datamode uppdateras via admi-funktionspost param1
// path får ändras hårdkodat
int     ge_DataMode = 1;
CString ge_Path = "c:\\SpadFiler\\";

// diverse utanför klassen
int _callback(int *x);
int g_bestnr;
CSpadFunkTVrapDlg	*g_pThis = NULL;	// tittat i hotslot och lärt mig
int hots_z_HamtaVeckonummer(int iDatum, int *iVeckodag);
int hots_z_Datum68(int iDatum);
int hots_z_Datum86(int iDatum);

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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

// CSpadFunkTVrapDlg dialog

// Konstruktor
CSpadFunkTVrapDlg::CSpadFunkTVrapDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SPADFUNKTVRAP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pSGu = new CSGupper;	// här är etablering av klassen CSGupper
	m_psd = new CSpadData;
//	g_psd = m_psd;

	// hämta datamode från gemensan inifil spad.ini
	char wstr[5];
	// ge-Path är hårdkodad, se ovan
	GetPrivateProfileString("Y", "DataMode", "", wstr, 5, ge_Path + "spad.ini");
	ge_DataMode = atoi(wstr);

	m_funktionsnummer = 1004;
	if (__argc > 1 && memcmp(*(__argv + 1), "10", 2) == 0)
		m_funktionsnummer = atoi(*(__argv + 1));

	CSpadData::typ_SpadBest sb;
	memset(&sb, 0, sizeof(CSpadData::typ_SpadBest));
	m_psd->HamtaDatumKlocka(&sb.SenastDatum, &sb.SenastKlock);
	sb.FunktionsNummer = m_funktionsnummer;
	sb.Kommando = 526; // programmet startar
	m_psd->InsertSpadBest(&sb);
}

// Destruktor
CSpadFunkTVrapDlg::~CSpadFunkTVrapDlg()
{
	CSpadData::typ_SpadBest sb;
	memset(&sb, 0, sizeof(CSpadData::typ_SpadBest));
	m_psd->HamtaDatumKlocka(&sb.SenastDatum, &sb.SenastKlock);
	sb.FunktionsNummer = m_funktionsnummer;
	sb.Kommando = 528; // programmet slutar
	m_psd->InsertSpadBest(&sb);

	if (m_psd != NULL)
		delete m_psd;
	if (m_pSGu != NULL)
		delete m_pSGu;
}

void CSpadFunkTVrapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSpadFunkTVrapDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CSpadFunkTVrapDlg message handlers

BOOL CSpadFunkTVrapDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	g_pThis = this;	// lärt från hotslot

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
	m_sniffintervall = sa.SniffIntervallSekunder;
	m_korvarvsekunder = sa.KorVarvSekunder;
	m_aliverapp = sa.AliveRapp;

	CString temp;
	temp.Format("Funktion %d", m_funktionsnummer);
	AfxGetMainWnd()->SetWindowTextA(temp);

	m_iOntimerAnt = 0;
	TimerOn();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSpadFunkTVrapDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSpadFunkTVrapDlg::OnPaint()
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
HCURSOR CSpadFunkTVrapDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//------------------------------------------------------------------------------

void CSpadFunkTVrapDlg::OnTimer(UINT nIDEvent)
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
	m_sniffintervall = sa.SniffIntervallSekunder;
	m_korvarvsekunder = sa.KorVarvSekunder;
	m_aliverapp = sa.AliveRapp;

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
		sb.Kommando = 2;
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

int CSpadFunkTVrapDlg::Korning_0000(CSpadData::typ_SpadBest *sb, int korvarvsekunder, int funktionsnummer)
{

	// Här ska vi lägga in fyra fasta rapporter från sgtestdlg.cpp 
	// färdig rapport veckosnitt

	g_bestnr = sb->BestNummer;
	if (sb->BestAntal == 1)
		VeckoRapport(sb);
	if (sb->BestAntal == 2)
		ManadsRapport(sb);
	if (sb->BestAntal == 3)
		ArsRapport(sb);
	if (sb->BestAntal == 4)
		ReachutvRapport(sb);
	if (sb->BestAntal == 5)
		DagdelsRapport(sb);

	return 0;

	// låt nedan stå, ev lägga in callback i upper och sen progressrapp i arsrapport
	for (int varv = 1; varv <= sb->BestAntal; varv++)
	{
		Sleep(korvarvsekunder * 1000);
		sb->ProgressNummer = varv;
		sb->Kommando = 3;
		m_psd->HamtaDatumKlocka(&sb->ProgressDatum, &sb->ProgressKlock);
		sb->SenastDatum = sb->ProgressDatum;
		sb->SenastKlock = sb->ProgressKlock;

		sb->KorPostAntal = varv * 7000;
		sb->ResultatBelopp1 = varv * 100;
		sb->ResultatBelopp2 = varv * 200;
		sb->ResultatBelopp3 = varv * 300;
		m_psd->InsertSpadBest(sb);

		while (m_psd->KollaBegaranPausaKorningFinns(funktionsnummer) == TRUE) // så länge admikommando = 6
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

void CSpadFunkTVrapDlg::TimerOn()
{
	m_WinTimerId = SetTimer(m_iTimerId, m_sniffintervall * 1000, NULL);
	m_timeron = true;
}

void CSpadFunkTVrapDlg::TimerOff()
{
	KillTimer(m_WinTimerId);
	m_timeron = false;
}

int CSpadFunkTVrapDlg::_Messa(CString csmess, char how)
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
	int iRad = SendDlgItemMessage(IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)csmess);
	SendDlgItemMessage(IDC_LIST1, LB_SETTOPINDEX, iRad, 0);

	GetDlgItem(IDC_LIST1)->UpdateWindow();
	_PumpMessages();
	return 0;
}

int CSpadFunkTVrapDlg::_PumpMessages()
{
	// Must call Create() before using the dialog
	ASSERT(m_hWnd != NULL);

	MSG msg;
	// Handle dialog messages
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (!IsDialogMessage(&msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return 0;
}

void CSpadFunkTVrapDlg::VeckoRapport(CSpadData::typ_SpadBest *sb)
{
	int retur;
	CString temp;

	int startdatum = sb->ResultatBelopp1;
	int dagantal   = sb->ResultatBelopp2;
	int mstd       = sb->ResultatBelopp3;

	m_pSGu->sgu_init();
	int  kanaler[] = { 1,2,28,29,15,74,61,197,43,683,50,108,901,-1 };
	char tkanal[][13] = { "SVT1","SVT2","TV3","TV4","Kanal 5","TV6","Sjuan","TV8","Kanal 9","TV10","SVT barn","Nickelod","Totalt" };
	m_pSGu->add_slot(startdatum, 120, 1440, kanaler, 0, 5, ',', 'm', 1, dagantal);
	m_pSGu->add_mstd(mstd);
	m_pSGu->add_matt(SGU_KAL_KAN, SGU_TYP_RAT, SGU_FOR_ABS, SGU_RED_TUS, 0);
	m_pSGu->add_matt(SGU_KAL_KAN, SGU_TYP_RAT, SGU_FOR_REL, SGU_RED_DC3, 0);
	m_pSGu->add_matt(SGU_KAL_KAN, SGU_TYP_REA, SGU_FOR_ABS, SGU_RED_TUS, 0);
	m_pSGu->add_matt(SGU_KAL_KAN, SGU_TYP_REA, SGU_FOR_REL, SGU_RED_DC3, 0);
	m_pSGu->add_matt(SGU_KAL_KAN, SGU_TYP_SHR, SGU_FOR_REL, SGU_RED_DC1, 0);
	m_pSGu->add_matt(SGU_KAL_KAN, SGU_TYP_TID, SGU_FOR_ABS, SGU_RED_DC3, 0);
	m_pSGu->add_matt(SGU_KAL_KAN, SGU_TYP_VRE, SGU_FOR_REL, SGU_RED_DC1, 0);

	std::vector<CSpadData::typ_SpadRapp>  rappvector;
	rappvector.clear();
	CSpadData::typ_SpadRapp sr;
	memset(&sr, 0, sizeof(CSpadData::typ_SpadRapp));
	sr.BestNummer = sb->BestNummer;
	sr.RappTyp = 1;

	_Messa("Körning startar", 0);
	retur = m_pSGu->sgu_run();
	temp.Format("Körning klar, %d anrop på basic() och werea()", retur);
	_Messa(temp, 0);

	CTime time;
	CString temp2;
	time = CTime::GetCurrentTime();
	temp2 = time.Format("%y-%m-%d  %H.%M.%S  ");

	CSGupper::typ_cell cell;
	int vardind = 0;
	cell = m_pSGu->res_cell(&vardind);	// hämtning av första cellen, ska finnas 36 st
	if (cell.slotind < 0)
	{
		_Messa("Resultatet tomt", 0);
		return;
	}
	
	char wmstdnamn[20];
	memset(wmstdnamn, 0, sizeof(wmstdnamn));
	m_pSGu->HamtaMgrpNamn(wmstdnamn, cell.mstd_nr);

	temp.Format(" Veckorapport genomsnittlig dag under en vecka: %06d och %d efterföljande, totalt %d dagar\n", cell.datum6, cell.antal - 1, cell.antal);
	sr.RadNummer++;
	strcpy_s(sr.Rad, temp);
	rappvector.push_back(sr);

	temp.Format(" Räckvidd dag: sett minst %d sammanhängande minuter, vecka: minst 15 minuter, målgrupp: nr %03d %s, timeshift: %s",
		cell.remimi, cell.mstd_nr, wmstdnamn, cell.ts_text);
	sr.RadNummer++;
	strcpy_s(sr.Rad, temp);
	rappvector.push_back(sr);

	temp = " Framställd " + temp2.Trim();
	temp.AppendFormat(" (bnr %d)", sr.BestNummer);
	sr.RadNummer++;
	strcpy_s(sr.Rad, temp);
	rappvector.push_back(sr);

	sr.RadNummer++;
	strcpy_s(sr.Rad, " ");
	rappvector.push_back(sr);

	sr.RadNummer++;
	strcpy_s(sr.Rad, " kanal\t\t\trating (000) \trating %\treach (000) \treach %\t\tshare\t\ttid\t\tvreach %");
	rappvector.push_back(sr);

	int rad = 0;
	while (cell.slotind >= 0) // snurra igenom från 0
	{
		int kol = 0;
		int kanal_nr = cell.kanal_nr;
		char wstr[7][20];
		while (kol < 7)	// 7 värden per kanalrad i rapporten, jämför med dagdelsrapport där kolumnvärden är olika slottar, annorlunda!
		{
			strcpy_s(wstr[kol], cell.vardred);
			vardind++;
			cell = m_pSGu->res_cell(&vardind);	// nästa cell
			kol++;
		}
		temp.Format(" %03d %s  \t\t%7s\t\t%7s\t\t%7s\t\t%7s\t\t%7s\t\t%7s\t\t%7s", kanal_nr, tkanal[rad], wstr[0], wstr[1], wstr[2], wstr[3], wstr[4], wstr[5], wstr[6]);
		sr.RadNummer++;
		strcpy_s(sr.Rad, temp);
		rappvector.push_back(sr);

		rad++;
	}

	sr.RadNummer++;
	strcpy_s(sr.Rad, " ");
	rappvector.push_back(sr);

	// uppdatera spaddata med sr-vektorn
	m_psd->InsertSpadRappAll(&rappvector);

}

void CSpadFunkTVrapDlg::ManadsRapport(CSpadData::typ_SpadBest *sb) 
{
	// färdig rapport månad 

	int retur;
	CString temp;
	int startdatum = sb->ResultatBelopp1;
	int dagantal = sb->ResultatBelopp2;
	int mstd = sb->ResultatBelopp3;

	int  kanaler[] = { 1,2,28,29,15,74,61,197,43,683,50,108,901, -1 };
	char tkanal[][13] = { "SVT1","SVT2","TV3","TV4","Kanal 5","TV6","Sjuan","TV8","Kanal 9","TV10","SVT barn","Nickelod","Totalt" };

	m_pSGu->sgu_init();
	m_pSGu->add_slot(startdatum, 120, 1440, kanaler, 0, 5, ',', 'm', 1, dagantal);
	m_pSGu->add_mstd(mstd);
	m_pSGu->add_matt(SGU_KAL_KAN, SGU_TYP_REA, SGU_FOR_REL, SGU_RED_DC1, 0);
	m_pSGu->add_matt(SGU_KAL_KAN, SGU_TYP_REA, SGU_FOR_ABS, SGU_RED_TUS, 0);
	m_pSGu->add_matt(SGU_KAL_KAN, SGU_TYP_TID, SGU_FOR_ABS, SGU_RED_DC1, 0);
	m_pSGu->add_matt(SGU_KAL_KAN, SGU_TYP_SHR, SGU_FOR_REL, SGU_RED_DC1, 0);
	m_pSGu->add_matt(SGU_KAL_KAN, SGU_TYP_VRE, SGU_FOR_REL, SGU_RED_DC1, 0);

	std::vector<CSpadData::typ_SpadRapp>  rappvector;
	rappvector.clear();
	CSpadData::typ_SpadRapp sr;
	memset(&sr, 0, sizeof(CSpadData::typ_SpadRapp));
	sr.BestNummer = sb->BestNummer;
	sr.RappTyp = 2;

	_Messa("Körning startar", 0);
	retur = m_pSGu->sgu_run();
	temp.Format("Körning klar, %d anrop på basic() och werea()", retur);
	_Messa(temp, 0);

	CTime time;
	CString temp2;
	time = CTime::GetCurrentTime();
	temp2 = time.Format("%y-%m-%d  %H.%M.%S  ");

	CSGupper::typ_cell cell;
	int vardind = 0;
	cell = m_pSGu->res_cell(&vardind);	// hämtning av första cellen
	if (cell.slotind < 0)
	{
		_Messa("Resultatet tomt", 0);
		return;
	}

	char wmstdnamn[20];
	memset(wmstdnamn, 0, sizeof(wmstdnamn));
	m_pSGu->HamtaMgrpNamn(wmstdnamn, cell.mstd_nr);

	temp.Format(" Månadsrapport genomsnittlig dag viss månad: %06d och %d efterföljande, totalt %d dagar", cell.datum6, cell.antal - 1, cell.antal);
	sr.RadNummer++;
	strcpy_s(sr.Rad, temp);
	rappvector.push_back(sr);

	temp.Format(" Räckviddskriterium: sett minst %d sammanhängande minuter, vecka: minst 15 minuter, målgrupp: nr %03d %s, timeshift: %s",
		cell.remimi, cell.mstd_nr, wmstdnamn, cell.ts_text);
	sr.RadNummer++;
	strcpy_s(sr.Rad, temp);
	rappvector.push_back(sr);

	temp = " Framställd " + temp2.Trim();
	temp.AppendFormat(" (bnr %d)", sr.BestNummer);
	sr.RadNummer++;
	strcpy_s(sr.Rad, temp);
	rappvector.push_back(sr);

	sr.RadNummer++;
	strcpy_s(sr.Rad, " ");
	rappvector.push_back(sr);

	sr.RadNummer++;
	strcpy_s(sr.Rad, " kanal\t\t\treach %\t\treach (000)\t   ttid\t\t share \t\tvreach %");
	rappvector.push_back(sr);

	int rad = 0;
	while (cell.slotind >= 0) // snurra igenom från 0
	{
		int kol = 0;
		int kanal_nr = cell.kanal_nr;
		char wstr[5][20];
		while (kol < 5)	// 5 värden per kanalrad i rapporten
		{
			strcpy_s(wstr[kol], cell.vardred);
			vardind++;
			cell = m_pSGu->res_cell(&vardind);	// nästa cell
			kol++;
		}
		// testar högerställda strängar (%7s), nja verkar bli centrerat högerställda
		temp.Format(" %03d %s  \t\t%7s\t\t%7s\t\t%7s\t\t%7s\t\t%7s", kanal_nr, tkanal[rad], wstr[0], wstr[1], wstr[2], wstr[3], wstr[4]);
		sr.RadNummer++;
		strcpy_s(sr.Rad, temp);
		rappvector.push_back(sr);

		rad++;
	}

	sr.RadNummer++;
	strcpy_s(sr.Rad, " ");
	rappvector.push_back(sr);

	// uppdatera spaddata med sr-vektorn
	m_psd->InsertSpadRappAll(&rappvector);
}

void CSpadFunkTVrapDlg::ArsRapport(CSpadData::typ_SpadBest *sb)
{
	// årsrapport
	// testar att utnyttja ny funktion för "viktning" av universumtal över skarv
	// dito för summa vikter (ändras varje dag)

	int retur;
	CString temp;
	int startdatum = sb->ResultatBelopp1;
	int dagantal = sb->ResultatBelopp2;
	int mstd = sb->ResultatBelopp3;

	int  kanaler[] = { 1,2,28,29,15,74,61,197,43,683,50,108,901, -1 };
	char tkanal[][13] = { "SVT1","SVT2","TV3","TV4","Kanal 5","TV6","Sjuan","TV8","Kanal 9","TV10","SVT barn","Nickelod","Totalt" };

	m_pSGu->sgu_init();
	m_pSGu->ange_refr_callback(_callback);
	m_pSGu->add_slot(startdatum, 120, 1440, kanaler, 0, 5, ',', 'm', 1, dagantal);
	m_pSGu->add_mstd(mstd);
	m_pSGu->add_matt(SGU_KAL_KAN, SGU_TYP_REA, SGU_FOR_REL, SGU_RED_DC1, 0);
	m_pSGu->add_matt(SGU_KAL_KAN, SGU_TYP_REA, SGU_FOR_ABS, SGU_RED_TUS, 0);
	m_pSGu->add_matt(SGU_KAL_KAN, SGU_TYP_TID, SGU_FOR_ABS, SGU_RED_DC1, 0);
	m_pSGu->add_matt(SGU_KAL_KAN, SGU_TYP_SHR, SGU_FOR_REL, SGU_RED_DC1, 0);
	m_pSGu->add_matt(SGU_KAL_KAN, SGU_TYP_VRE, SGU_FOR_REL, SGU_RED_DC1, 0);

	std::vector<CSpadData::typ_SpadRapp>  rappvector;
	rappvector.clear();
	CSpadData::typ_SpadRapp sr;
	memset(&sr, 0, sizeof(CSpadData::typ_SpadRapp));
	sr.BestNummer = sb->BestNummer;
	sr.RappTyp = 3;

	_Messa("Körning startar, tar några minuter...", 0);
	retur = m_pSGu->sgu_run();
	temp.Format("Körning klar, %d anrop på basic() och werea()", retur);

	CSGupper::typ_cell cell;
	int vardind = 0;

	cell = m_pSGu->res_cell(&vardind);	// hämtning av första cellen
	if (cell.slotind < 0)
	{
		_Messa("Resultatet tomt", 0);
		return;
	}

	char wmstdnamn[20];
	memset(wmstdnamn, 0, sizeof(wmstdnamn));
	m_pSGu->HamtaMgrpNamn(wmstdnamn, cell.mstd_nr);

	CTime time;
	CString temp2;
	time = CTime::GetCurrentTime();
	temp2 = time.Format("%y-%m-%d  %H.%M.%S  ");

	temp.Format(" Årsrapport 2013: %06d och %d efterföljande, totalt %d dagar", cell.datum6, cell.antal - 1, cell.antal);
	sr.RadNummer++;
	strcpy_s(sr.Rad, temp);
	rappvector.push_back(sr);

	temp.Format(" Räckviddskriterium: sett minst %d sammanhängande minuter, vecka: minst 15 minuter, målgrupp: nr %03d %s, timeshift: %s",
		cell.remimi, cell.mstd_nr, wmstdnamn, cell.ts_text);
	sr.RadNummer++;
	strcpy_s(sr.Rad, temp);
	rappvector.push_back(sr);

	temp = " Framställd " + temp2.Trim();
	temp.AppendFormat(" (bnr %d)", sr.BestNummer);
	sr.RadNummer++;
	strcpy_s(sr.Rad, temp);
	rappvector.push_back(sr);

	sr.RadNummer++;
	strcpy_s(sr.Rad, " ");
	rappvector.push_back(sr);

	sr.RadNummer++;
	strcpy_s(sr.Rad, " kanal\t\t\treach %\t\treach (000)\t   ttid\t\t share \t\tvreach %");
	rappvector.push_back(sr);

	int rad = 0;
	while (cell.slotind >= 0) // snurra igenom från 0
	{
		int kol = 0;
		int kanal_nr = cell.kanal_nr;
		char wstr[5][20];
		while (kol < 5)	// 5 värden per kanalrad i rapporten
		{
			strcpy_s(wstr[kol], cell.vardred);
			vardind++;
			cell = m_pSGu->res_cell(&vardind);	// nästa cell
			kol++;
		}
		// testar högerställda strängar (%7s), nja verkar bli centrerat högerställda
		temp.Format(" %03d %s  \t\t%7s\t\t%7s\t\t%7s\t\t%7s\t\t%7s", kanal_nr, tkanal[rad], wstr[0], wstr[1], wstr[2], wstr[3], wstr[4]);
		sr.RadNummer++;
		strcpy_s(sr.Rad, temp);
		rappvector.push_back(sr);
		rad++;
	}

	sr.RadNummer++;
	strcpy_s(sr.Rad, " ");
	rappvector.push_back(sr);

	// uppdatera spaddata med sr-vektorn
	m_psd->InsertSpadRappAll(&rappvector);
}

int _callback(int *x)
{
//  callbackarrangemang, se ovan: m_pSGu->ange_refr_callback(_callback);
	CString temp;
	temp.Format("Nu behandlas kanal nummer %d", *x+1);
	AfxGetMainWnd()->SetWindowTextA(temp);
	g_pThis->_Messa(temp, 0);	// g_pThis pekare till programmets egen klass

	CSpadData::typ_SpadBest sb;	// gick bra
	memset(&sb, 0, sizeof(CSpadData::typ_SpadBest));
	sb.FunktionsNummer = 1004; 
	sb.BestNummer = g_bestnr;
	sb.ProgressNummer = *x + 1;
	sb.KorPostAntal = *x + 1;
	g_pThis->m_psd->HamtaDatumKlocka(&sb.ProgressDatum, &sb.ProgressKlock);	
	sb.SenastDatum = sb.ProgressDatum;
	sb.SenastKlock = sb.ProgressKlock;
	sb.Kommando = 3;
	g_pThis->m_psd->InsertSpadBest(&sb);
	return 0;
}

void CSpadFunkTVrapDlg::ReachutvRapport(CSpadData::typ_SpadBest *sb)
{
	// Rapport veckoräckviddsutveckling
	#define ANTVECK 15
	int retur;
	int veckonr, veckodag;
	CString temp;
	char wstr[13][ANTVECK][20];

	int datum, antveck, mstd, rad;
	int  kanaler[] = { 1,2,28,29,15,74,61,197,43,683,50,108,901, -1 };
	char tkanal[][13] = { "SVT1","SVT2","TV3","TV4","Kanal 5","TV6","Sjuan","TV8","Kanal 9","TV10","SVT barn","Nickelod","Totalt" };

	memset(wstr, 0, sizeof(wstr));
	datum = sb->ResultatBelopp1;
	antveck = sb->ResultatBelopp2;
	mstd = sb->ResultatBelopp3;

	CSGupper::typ_cell startcell;
	veckonr = hots_z_HamtaVeckonummer(datum, &veckodag);

	std::vector<CSpadData::typ_SpadRapp>  rappvector;
	rappvector.clear();
	CSpadData::typ_SpadRapp sr;
	memset(&sr, 0, sizeof(CSpadData::typ_SpadRapp));
	sr.BestNummer = sb->BestNummer;
	sr.RappTyp = 4;

	_Messa("Körningar startar", 0);
	for (int vnu = 0; vnu < antveck; vnu++)
	{
		// Separata körningar per vecka: en multislott per kanal

		sb->ProgressNummer = vnu + 1;
		sb->KorPostAntal = vnu + 1;
		m_psd->HamtaDatumKlocka(&sb->ProgressDatum, &sb->ProgressKlock);
		sb->SenastDatum = sb->ProgressDatum;
		sb->SenastKlock = sb->ProgressKlock;
		sb->Kommando = 3;
		m_psd->InsertSpadBest(sb);
		temp.Format("Nu behandlas vecka %d av %d", vnu + 1, antveck);
//		_Messa(temp, 0);

		m_pSGu->sgu_init();
		m_pSGu->add_slot(datum, 120, 1440, kanaler, 0, 5, ',', 'm', 1, 7);
		m_pSGu->add_mstd(mstd);
		m_pSGu->add_matt(SGU_KAL_KAN, SGU_TYP_REA, SGU_FOR_REL, SGU_RED_DC1, 0);
		retur = m_pSGu->sgu_run();

		CSGupper::typ_cell cell;
		int vardind = 0;

		// hämtning av första cellen
		cell = m_pSGu->res_cell(&vardind);
		if (cell.slotind < 0)
		{
			_Messa("Resultatet tomt", 0);
			return;
		}
		if (vnu == 0)
			startcell = cell;

		while (cell.slotind >= 0) // snurra igenom resultatet från 0
		{
			// lagra en resultatkolumn
			strcpy_s(wstr[vardind][vnu], cell.vardred);
			vardind++;
			cell = m_pSGu->res_cell(&vardind);	// hämtning av nästa cell
		}

		if (vnu + 1 < antveck)
		{
			for (int d = 1; d <= 7; d++)
				m_pSGu->zdat6addi(&datum);		// stega upp datum
		}
	}
	_Messa("Körningar klara", 0);

	CTime time;
	CString temp2;
	time = CTime::GetCurrentTime();
	temp2 = time.Format("%y-%m-%d  %H.%M.%S  ");

	// "Utskrift" av rapporten

	char wmstdnamn[20];
	memset(wmstdnamn, 0, sizeof(wmstdnamn));
	m_pSGu->HamtaMgrpNamn(wmstdnamn, startcell.mstd_nr);

	temp.Format(" Räckviddsutveckling: %06d och %d veckor", startcell.datum6, antveck);
	sr.RadNummer++;
	strcpy_s(sr.Rad, temp);
	rappvector.push_back(sr);

	temp.Format(" Genomsnittlig daglig räckvidd, räckviddskriterium: sett minst %d sammanhängande minuter, målgrupp: nr %03d %s, timeshift: %s",
		startcell.remimi, startcell.mstd_nr, wmstdnamn, startcell.ts_text);
	sr.RadNummer++;
	strcpy_s(sr.Rad, temp);
	rappvector.push_back(sr);

	temp = " Framställd " + temp2.Trim();
	temp.AppendFormat(" (bnr %d)", sr.BestNummer);
	sr.RadNummer++;
	strcpy_s(sr.Rad, temp);
	rappvector.push_back(sr);

	sr.RadNummer++;
	strcpy_s(sr.Rad, " ");
	rappvector.push_back(sr);

	temp.Format(" kanal\t");
	int wvenr;
	for (wvenr = veckonr; wvenr <= veckonr+antveck-1; wvenr++)
		temp.AppendFormat("\t    v%02d", wvenr);

	sr.RadNummer++;
	strcpy_s(sr.Rad, temp);
	rappvector.push_back(sr);

	for (rad = 0; rad < 13; rad++)
	{
		temp.Format(" %03d %s  \t%7s\t%7s\t%7s\t%7s\t%7s\t%7s\t%7s\t%7s\t%7s\t%7s\t%7s\t%7s\t%7s\t%7s\t%7s", kanaler[rad], tkanal[rad],
			wstr[rad][0], wstr[rad][1], wstr[rad][2], wstr[rad][3], wstr[rad][4],
			wstr[rad][5], wstr[rad][6], wstr[rad][7], wstr[rad][8], wstr[rad][9],
			wstr[rad][10], wstr[rad][11], wstr[rad][12], wstr[rad][13], wstr[rad][14]);

		sr.RadNummer++;
		strcpy_s(sr.Rad, temp);
		rappvector.push_back(sr);

	}

	sr.RadNummer++;
	strcpy_s(sr.Rad, " ");
	rappvector.push_back(sr);

	// uppdatera spaddata med sr-vektorn
	m_psd->InsertSpadRappAll(&rappvector);
}

void CSpadFunkTVrapDlg::DagdelsRapport(CSpadData::typ_SpadBest *sb)
{
	// per kanal, per dagdel (12 st 2-timmars) + hel dag
	// startdag antal dagar enl best, likaså målgrupp
	// genomsnittlig relativ rating
	// Rapport nr 5

	int retur;
	CString temp;

	int startdatum = sb->ResultatBelopp1;
	int dagantal = sb->ResultatBelopp2;
	int mstd = sb->ResultatBelopp3;

	#define ANTKANAL 13
	int  kanaler[ANTKANAL+1] = { 1,2,28,29,15,74,61,197,43,683,50,108,901, -1 };
	char tkanal[15][ANTKANAL] = { "SVT1","SVT2","TV3","TV4","Kanal 5","TV6","Sjuan","TV8","Kanal 9","TV10","SVT barn","Nickelod","Totalt" };

	m_pSGu->sgu_init();

	// en slot för hel dag
	m_pSGu->add_slot(startdatum, 120, 1440, kanaler, 0, 5, ',', 'x', 1, dagantal);

	// 12 st dagdelar per kanal
	for (int x = 1; x <= 12; x++)
		m_pSGu->add_slot(startdatum, x*120, 120, kanaler, 0, 5, ',', 'x', 1, dagantal);

	m_pSGu->add_mstd(mstd);
	m_pSGu->add_matt(SGU_KAL_KAN, SGU_TYP_RAT, SGU_FOR_REL, SGU_RED_DC3, 0);

	_Messa("Körning startar", 0);
	retur = m_pSGu->sgu_run();
	temp.Format("Körning klar, %d anrop på basic() och werea()", retur);
	_Messa(temp, 0);

	std::vector<CSpadData::typ_SpadRapp>  rappvector;
	rappvector.clear();
	CSpadData::typ_SpadRapp sr;
	memset(&sr, 0, sizeof(CSpadData::typ_SpadRapp));
	sr.BestNummer = sb->BestNummer;
	sr.RappTyp = 5;

	CTime time;
	CString temp2;
	time = CTime::GetCurrentTime();
	temp2 = time.Format("%y-%m-%d  %H.%M.%S  ");

	// "Utskrift" av rapporten

	CSGupper::typ_cell cell;
	int vardind = 0;
	cell = m_pSGu->res_cell(&vardind);	// hämtning av första cellen, ska finnas 36 st
	if (cell.slotind < 0)
	{
		_Messa("Resultatet tomt", 0);
		return;
	}

	char wmstdnamn[20];
	memset(wmstdnamn, 0, sizeof(wmstdnamn));
	m_pSGu->HamtaMgrpNamn(wmstdnamn, cell.mstd_nr);

	temp.Format(" Dagdelsrapport: %06d och %d efterföljande dagar, totalt %d dagar", cell.datum6, dagantal-1, dagantal);
	sr.RadNummer++;
	strcpy_s(sr.Rad, temp);
	rappvector.push_back(sr);

	temp.Format(" Genomsnittlig publikstorlek per dagdel, målgrupp: nr %03d %s, timeshift: %s",
		cell.mstd_nr, wmstdnamn, cell.ts_text);
	sr.RadNummer++;
	strcpy_s(sr.Rad, temp);
	rappvector.push_back(sr);

	temp = " Framställd " + temp2.Trim();
	temp.AppendFormat(" (bnr %d)", sr.BestNummer);
	sr.RadNummer++;
	strcpy_s(sr.Rad, temp);
	rappvector.push_back(sr);

	sr.RadNummer++;
	strcpy_s(sr.Rad, " ");
	rappvector.push_back(sr);

	sr.RadNummer++;
	strcpy_s(sr.Rad, " kanal\t\t\t dygn \t 02-04\t 04-06\t 06-08\t 08-10\t 10-12\t 12-14\t 14-16\t 16-18\t 18-20\t 20-22\t 22-24\t 24-26");
	rappvector.push_back(sr);

	#define ANTVARDKOL 13
	char wstr[ANTKANAL][ANTVARDKOL][20];

	int rad = 0;
	int kol = 0;
	memset(wstr, 0, sizeof(wstr));

	// Detta är inte helt lätt; ordningen totalt blir först värde (kol), sedan kanal (rad)
	// Vi måste fylla en tabell med värden uppifrån och ned, per kolumn alla rader (kanaler)

	// kolumn 1 - 13 kanal för kanal (rad)  
	kol = 0;
	while (kol < ANTVARDKOL)
	{
		rad = 0;
		while (rad < ANTKANAL)	// värden per kanalrad
		{
			strcpy_s(wstr[rad][kol], cell.vardred);
			vardind++;
			cell = m_pSGu->res_cell(&vardind);	// nästa cell
			rad++;
		}
		kol++;
	}

	// töm tabellenens värden, formatterade, ut i resultatvektorn
	rad = 0;
	while (rad < ANTKANAL)
	{
		temp.Format(" %03d %s  \t\t%7s\t%7s\t%7s\t%7s\t%7s\t%7s\t%7s\t%7s\t%7s\t%7s\t%7s\t%7s\t%7s", kanaler[rad], tkanal[rad],
			wstr[rad][0], wstr[rad][1], wstr[rad][2], wstr[rad][3], wstr[rad][4], wstr[rad][5], wstr[rad][6],
			wstr[rad][7], wstr[rad][8], wstr[rad][9], wstr[rad][10], wstr[rad][11], wstr[rad][12]);
		sr.RadNummer++;
		strcpy_s(sr.Rad, temp);
		rappvector.push_back(sr);
		rad++;
	}

	sr.RadNummer++;
	strcpy_s(sr.Rad, " ");
	rappvector.push_back(sr);

	// uppdatera spaddata med sr-vektorn
	m_psd->InsertSpadRappAll(&rappvector);

}

// Get veckonummer och veckodag för ett datum 6/8-ställigt
int hots_z_HamtaVeckonummer(int iDatum, int *iVeckodag)
{
	if (iDatum == 0) return 0;
	void    _get_week_number(const struct tm *, int *, int *);
	int     iVecka;
	int     ar = hots_z_Datum68(iDatum) / 10000;
	int     manad = (iDatum % 10000) / 100;
	int     dag = iDatum % 100;
	int     iVdag;
	CTime   t = CTime(ar, manad, dag, 0, 0, 0);

	struct tm osTime;
	t.GetLocalTm(&osTime);
	_get_week_number(&osTime, &iVecka, &iVdag);
	*iVeckodag = iVdag;

	return iVecka;
}

// Intern hjälpfunktion för hots_z_HamtaVeckonummer(...)
void _get_week_number(const struct tm *t, int *week, int *dayofweek)
{
	int tmp1, tmp2, fourthdaynum, year, isleap;

	tmp1 = 1 - t->tm_wday;
	tmp2 = t->tm_yday + ((tmp1 > 0) ? 3 : 10) + tmp1;
	fourthdaynum = tmp2 % 7;

	*week = tmp2 / 7;

	if (*week == 0)
	{
		year = t->tm_year + 1900 - 1;
		isleap = !(year % 4) && ((year % 100) || !(year % 400));
		*week = (fourthdaynum + isleap >= 6) ? 53 : 52;
	}
	else
		if (*week == 53)
		{
			year = t->tm_year + 1900;
			isleap = !(year % 4) && ((year % 100) || !(year % 400));
			if (fourthdaynum > isleap)
				*week = 1;
		}

	if (t->tm_wday == 0)
		*dayofweek = 7;
	else
		*dayofweek = t->tm_wday;

	return;
}

// Konverterar ett 6-ställigt datum till 8-ställigt, 8-ställiga förblir oförändrade
int hots_z_Datum68(int iDatum)
{
	if (iDatum / 10000 > 1900) return iDatum;
	if (iDatum / 10000 >50) return 19 * 1000000 + iDatum;
	if (iDatum / 10000 <= 50) return 20 * 1000000 + iDatum;

	return iDatum;  // Borde aldrig komma hit, men i alla fall...
}

// Konverterar ett 8-ställigt datum till 6-ställigt, 6-ställiga förblir oförändrade
int hots_z_Datum86(int iDatum)
{
	return iDatum % 1000000;
}
