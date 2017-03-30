
// SpadDlg.cpp : implementation file
//
// 2015-12-16 hm	övningsuppgift påbörjad
//					databasprojekt måste här vara vs 2010, extra lib i länkning
//					se properties vc++sökvägar, SQLAPI include och lib + mera
//	2015-12-28		lagt in data-mellanhand: SpadData, i form av en egen klass
//  2016-01-12		Nu finns det mesta på plats; två funktionsmoduler, starta
//					funktions-exe från menyn spad, aktuell funktion för exe bestäms via 
//					argument vid create_process, parametrar till alla moduler via "Admi"
//	2016-01-14		Putsat vidare, övningsuppgiften nu avslutad
//  2016-01-17		Fortsätter ändå med "autostart" och med "historik"-popupdialog.
//					Historiktabell innehåller färdiga körningar (kommando 4)
//  2016-01-22		Lagt till arkivtabell med påfyllning allt utom väntande best vid tömning Best
//					Högsta bestnummer ligger nu i admi:param4
//	2016-01-25		Mera småfunktioner som listning vänt best, databasantal i fönsterhuvudet
//	2016-01-28		Lagt till historik-funkrapport
//	2016-01-29		Nu borttag av väntande beställning via select i lb, tabkänslig historik-listbox
//  2016-02-03		Totalnivå i historikens funkrapport
//	2016-02-05		Provar ide' med filer som alternativ till databas, se spaddata.cpp och ge_DataMode och spadfile.cpp
//					Fullföljer idee'n. Sakta forskat fram vad som ska användas för filhanteringen, fgetpos(), fseek() mfl
//  2016-02-07		lärt mer, ::pointer, om att jobba mot vektorpekares data med pekare, och med iterator och (*vec)[varv]
//					Se experiment i CSpadFile::SkapaAdmiMass(), nästan klar, återstår tester och tillbakaladdning filer->bas
//  2016-02-08		Funderar på ev ini-fil...lägger till
//  2016-02-12		Testat att läsa spadadmi.bin från cobol-program, binär lagring inte samma, cobol kör med "från
//					höger till vänster" för de fyra bajtsen inom ett ord, funkar om man i cobolprogrammet först kastar om dem
//	2016-02-14		Nu kan datalagring växlas fram och tillbaka flygande med konvertering av befintligt innehåll
//  2016-02-17		Fått igång databasen mot cobol, odbc, 64bit, fortsätter...
//  2016-02-21		Spadfunk i cobol databasklar och fullt fungerande enl protokoll, återstår ev att lägga in även fil-metoden?
//  2016-02-22		Nu två inifiler, varav en gemensam spad.ini i c:\spadfiles för funkmoduler att titta i efter initialt datamode
//					Två titt-knappar gi och ei
//  2016-02-23		Justerat spadfile tekniken med fseek, fel vid tom fil
//  2016-02-24		Gått över till binära versioner av copyposter (struktar) i cobol, dock icke enskilda parameterfält.
//  2016-02-26      ID i filer nu inte längre i stigande ordning, bara unika inom filen/tabellen, konsekvens av hur db jobbar,
//					minns även om bastabell tom, så icke med flata världen, nu skapas flata id från ett.
//					Kompletterar coboldelen med flata filer samt bett om förlängd testlicens, färdig, återstår mer testning
//					Cobol låser filer vid open i-o för rewrite eller open extend. Kanske lägga in upprepade öppningsförsök i c-delen.
//	2016-02-27		Rättat buggar: fel termordning i cobol-best, init titel low-value annars knas vid tillägg bas, behövs gås till botten,
//					hur man bäst gör, hur det funkar, med textfält/strängar och databas. 
//  2016-02-28		Vid "läsning" här av titel blir resultatet i mottagande fält en sträng 50 lång + en bnolla i pos 51.
//					Vid "skrivning" här förväntas en bnolla någonstans i indatafältet i de 50, kanske i 51.
//					Lagt in DBCC CHECKIDENT ('Besthist', RESEED, 0) för alla tabellerna i DeleteSpadAll(void) i samband med återskapning
//					Lagt in arkivrader i historiken för viss beställning, lagt in markering i histlista och knapp för detaljer
//	2016-02-29		Lagt till dubbelklick i historiklistan, fixat känslighet för markering, nu lärt mig lite mera om "messages"
//  2016-03-01		upptäckt bug, i filläge så blir inte selecthistall "sorterad", sort nu på funknr
//					Lagt in upprepade försök att i filläge öppna admi vid felsignal, ett sätt att klara kollisitioner, 
//					kanske göra fullt ut om det verkar hjälpa...verkar hjälpa, får den inte att krascha längre...
//					Lägger in motsvarande i cobol-delen, lärt mig file status, fortsätter en bit, skulle troligen behövas en
//					gemensam funktion för hantering och köbildning av requests för att få en hållbar lösning med kvalitet
//  2016-03-02		Just nu snurrigt med historik, dubbelklick, förväxling bestid-bestnr, sorteringar, 
//					jag börjar styra upp det hela, samordna utprodukterna...
//					Utprodukter i historik: funkrapport, enradig listning best - flera sammanhang, detaljer viss best.
//					Funkrapport: gruppering av indata per funktion krävs, görs via sort och för db "order by" OK
//					Enradig bestlistning: samma layout alla sammanhang nja inte, bestnummer längst till vänster 4 tecken, sedan funknummer
//					Hittat fel, histvector måste ligga orörd om selindex ska användas igen. 
//					Detaljer viss best: inledande arkiverade rader samma layout som i arkivlistning, möjligen klartext till kommando och returkod
//	2016-03-05		Möblerat om i historik-bilden och i hoppen mellan boxarna. Nu får det vara bra.
//	2016-03-07		Funkmodul kunna lägga beställning?. Nu i admi1000: korvarvsek=funknr som cobol kan hämta, p1=datamode, p3=högstbestnr
//					Lägger denna ide på is
//	2016-03-10		Lagt in bestnummerfilter vid läsning av messarci i historikens detaljbild, lasbestbest() för file
//			
//  2016-08-05		Ny funktion 1004, TVrapporter, funktionsmodul spadfunktvrap, använder nya SGupper, fasta rapporter
//					Här i spad behövs ett sätt att betrakta rapporterna. Nu alla fyra rapporttyperna. OK
//					  

#include "stdafx.h"
#include "Spad.h"
#include "SpadDlg.h"
#include "SpadData.h"
#include "Historik.h"
#include "afxdialogex.h"
#include <algorithm>    // std::sort

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// globaler, löses vid länkning för användare i andra cpp:er genom extern...
// värden uppdateras nu från ini-fil
int     ge_DataMode = 1; 
CString ge_Path     = "c:\\SpadFiler\\";

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


// CSpadDlg dialog


// Konstruktor
CSpadDlg::CSpadDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSpadDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_psd = NULL;
	m_psd = new CSpadData;
	m_phi = NULL;
}

// Destruktor
CSpadDlg::~CSpadDlg()
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	if (m_psd != NULL)
		delete m_psd;
	if (m_phi != NULL)
		delete m_phi;
}

void CSpadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSpadDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_FUNKSTART1, &CSpadDlg::OnBnClickedFunkstart1)
	ON_BN_CLICKED(IDC_FUNKAVBRYT1, &CSpadDlg::OnBnClickedFunkavbryt1)
	ON_BN_CLICKED(IDC_FUNKBEST1, &CSpadDlg::OnBnClickedFunkbest1)
	ON_BN_CLICKED(IDC_FUNKANGRA1, &CSpadDlg::OnBnClickedFunkangra1)
	ON_BN_CLICKED(IDC_BESTLIST, &CSpadDlg::OnBnClickedBestlist)
	ON_BN_CLICKED(IDC_TIMER, &CSpadDlg::OnBnClickedTimer)
	ON_BN_CLICKED(IDC_CLEAR, &CSpadDlg::OnBnClickedClear)
	ON_BN_CLICKED(IDOK, &CSpadDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSpadDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CLEARDB, &CSpadDlg::OnBnClickedCleardb)
	ON_BN_CLICKED(IDC_INITADMI, &CSpadDlg::OnBnClickedInitadmi)
	ON_BN_CLICKED(IDC_PAUSA, &CSpadDlg::OnBnClickedPausa)
	ON_BN_CLICKED(IDC_FORTSATT, &CSpadDlg::OnBnClickedFortsatt)
	ON_BN_CLICKED(IDC_VISAADMI, &CSpadDlg::OnBnClickedVisaadmi)
	ON_BN_CLICKED(IDC_ALLABEST, &CSpadDlg::OnBnClickedAllabest)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CSpadDlg::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_UPPDATADMI, &CSpadDlg::OnBnClickedUppdatadmi)
	ON_BN_CLICKED(IDC_SAVEADMI, &CSpadDlg::OnBnClickedSaveadmi)
	ON_BN_CLICKED(IDC_AVBRYTADMI, &CSpadDlg::OnBnClickedAvbrytadmi)
	ON_BN_CLICKED(IDC_START1001, &CSpadDlg::OnBnClickedStart1001)
	ON_BN_CLICKED(IDC_START1002, &CSpadDlg::OnBnClickedStart1002)
	ON_BN_CLICKED(IDC_FUNKSTART2, &CSpadDlg::OnBnClickedFunkstart2)
	ON_BN_CLICKED(IDC_FUNKBEST2, &CSpadDlg::OnBnClickedFunkbest2)
	ON_BN_CLICKED(IDC_FUNKANGRA2, &CSpadDlg::OnBnClickedFunkangra2)
	ON_BN_CLICKED(IDC_PAUSA2, &CSpadDlg::OnBnClickedPausa2)
	ON_BN_CLICKED(IDC_FORTSATT2, &CSpadDlg::OnBnClickedFortsatt2)
	ON_BN_CLICKED(IDC_FUNKAVBRYT2, &CSpadDlg::OnBnClickedFunkavbryt2)
	ON_BN_CLICKED(IDC_AVBRYTEXE1, &CSpadDlg::OnBnClickedAvbrytexe1)
	ON_BN_CLICKED(IDC_AVBRYTEXE2, &CSpadDlg::OnBnClickedAvbrytexe2)
	ON_BN_CLICKED(IDC_CHECK1, &CSpadDlg::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_HISTORIK, &CSpadDlg::OnBnClickedHistorik)
	ON_BN_CLICKED(IDC_ARKIVMESS, &CSpadDlg::OnBnClickedArkivmess)
	ON_BN_CLICKED(IDC_CLEARBEST, &CSpadDlg::OnBnClickedClearbest)
	ON_BN_CLICKED(IDC_DATASTATUS, &CSpadDlg::OnBnClickedDatastatus)
	ON_BN_CLICKED(IDC_LBSEL, &CSpadDlg::OnBnClickedLbsel)
	ON_BN_CLICKED(IDC_LBSEL2, &CSpadDlg::OnBnClickedLbsel2)

	ON_BN_CLICKED(IDC_VISA, &CSpadDlg::OnBnClickedVisa)
	ON_BN_CLICKED(IDC_DATAKALLA, &CSpadDlg::OnBnClickedDatakalla)
	ON_BN_CLICKED(IDC_KALLAAVBRYT, &CSpadDlg::OnBnClickedKallaavbryt)
	ON_BN_CLICKED(IDC_FUNKSTART3, &CSpadDlg::OnBnClickedFunkstart3)
	ON_BN_CLICKED(IDC_FUNKBEST3, &CSpadDlg::OnBnClickedFunkbest3)
	ON_BN_CLICKED(IDC_FUNKANGRA3, &CSpadDlg::OnBnClickedFunkangra3)
	ON_BN_CLICKED(IDC_PAUSA3, &CSpadDlg::OnBnClickedPausa3)
	ON_BN_CLICKED(IDC_FORTSATT3, &CSpadDlg::OnBnClickedFortsatt3)
	ON_BN_CLICKED(IDC_FUNKAVBRYT3, &CSpadDlg::OnBnClickedFunkavbryt3)
	ON_BN_CLICKED(IDC_START1003, &CSpadDlg::OnBnClickedStart1003)
	ON_BN_CLICKED(IDC_AVBRYTEXE3, &CSpadDlg::OnBnClickedAvbrytexe3)
	ON_BN_CLICKED(IDC_CLEAR2, &CSpadDlg::OnBnClickedClear2)
	ON_BN_CLICKED(IDC_VISAGINIT, &CSpadDlg::OnBnClickedVisaginit)
	ON_BN_CLICKED(IDC_VISAEINIT, &CSpadDlg::OnBnClickedVisaeinit)
	ON_BN_CLICKED(IDC_CLEARALLATAB, &CSpadDlg::OnBnClickedClearallatab)
	ON_BN_CLICKED(IDC_FUNKSTART4, &CSpadDlg::OnBnClickedFunkstart4)
	ON_BN_CLICKED(IDC_FUNKBEST4, &CSpadDlg::OnBnClickedFunkbest4)
	ON_BN_CLICKED(IDC_FUNKANGRA4, &CSpadDlg::OnBnClickedFunkangra4)
	ON_BN_CLICKED(IDC_START1004, &CSpadDlg::OnBnClickedStart1004)
	ON_BN_CLICKED(IDC_AVBRYTEXE4, &CSpadDlg::OnBnClickedAvbrytexe4)

	ON_BN_CLICKED(IDC_PRINT, &CSpadDlg::OnBnClickedPrint)
	ON_BN_CLICKED(IDC_PRINT2, &CSpadDlg::OnBnClickedPrint2)
	ON_BN_CLICKED(IDC_PRINTALLT, &CSpadDlg::OnBnClickedPrintallt)
	ON_CBN_SELCHANGE(IDC_COMBO3, &CSpadDlg::OnCbnSelchangeCombo3)
	ON_BN_CLICKED(ID_PLUS7, &CSpadDlg::OnBnClickedPlus7)
	ON_EN_CHANGE(IDC_MSTD4, &CSpadDlg::OnEnChangeMstd4)
	ON_BN_CLICKED(IDC_RAPP, &CSpadDlg::OnBnClickedRapp)
	ON_BN_CLICKED(IDC_BUTTON5, &CSpadDlg::OnBnClickedButton5)
END_MESSAGE_MAP()


// CSpadDlg message handlers

BOOL CSpadDlg::OnInitDialog()
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

	Las_inifil();
	if (ge_DataMode == 1) SetDlgItemText(IDC_DATAKALLATEXT, "Databas");
	if (ge_DataMode == 2) SetDlgItemText(IDC_DATAKALLATEXT, "Filer");

	CSpadData::typ_SpadAdmi sa;
	m_psd->SelectSpadAdmiFunk(1000, &sa); // uppdatera param1 i enlighet med datamode
	m_psd->UpdateAdmiAdmiIDSniffVarvRappParam(sa.AdmiID, sa.SniffIntervallSekunder, 
		sa.KorVarvSekunder, sa.AliveRapp, ge_DataMode, sa.Param2, sa.Param3);
	m_sniffintervall = sa.SniffIntervallSekunder;

	GetDlgItem( IDC_COMBO1       )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_TIMER    )->SetWindowText("Timer off");	
	GetDlgItem(IDC_AVBRYTEXE1 )->EnableWindow(FALSE);
	GetDlgItem(IDC_AVBRYTEXE2 )->EnableWindow(FALSE);
	GetDlgItem(IDC_AVBRYTEXE3)->EnableWindow(FALSE);
	GetDlgItem(IDC_AVBRYTEXE4)->EnableWindow(FALSE);

	_ResettaFunk1();
	_ResettaFunk2();
	_ResettaFunk3();
	_ResettaFunk4();
	_SlackAlltAdmi();
	m_iTimerId = 1;

	m_aktiv_1001    = false;
	m_aktiv_1002    = false;
	m_aktiv_1003    = false;	
	m_aktiv_1004    = false;
	m_autostart     = false;
	m_lbselbekrafta = false;
	m_datakallabekrafta = false;
	m_printallt = false;

	m_lbseltext     = "Tag bort markerad beställning";
	SetDlgItemText(IDC_LBSEL, m_lbseltext);
	SetDlgItemText(IDC_DATAKALLA, "Byt datakälla");
	CheckDlgButton(IDC_VISA, 1);
	OnBnClickedVisa();
	CheckDlgButton(IDC_CHECK1, 1);
	OnBnClickedCheck1();


	m_iOntimerAnt = 0;
	m_bestidhighsenast = 0;
	TimerOn();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSpadDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSpadDlg::OnPaint()
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
HCURSOR CSpadDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//------------------------------------------------------------------------------

void CSpadDlg::Las_inifil()
{
	CString m_myini = ".\\SpadDlg.ini";
	CString wini;
	char wstr[100];
	char csDataMode[2] = {'1'};

	GetPrivateProfileString("X", "DataMode", "", wstr, 5, m_myini);
	if (wstr[0] == '1' || wstr[0] == '2')
	{
		ge_DataMode = atoi(wstr);
		csDataMode[0] = wstr[0];
	}

	GetPrivateProfileString("X", "Path", "", wstr, 90, m_myini);
	ge_Path = wstr;

// Uppdatera gemensam ini-fil så funkmoduler kan hämta datamode
// utan att använda databasen
	wini = ge_Path + "Spad.ini";
	WritePrivateProfileString("Y", "DataMode", csDataMode, wini);
	WritePrivateProfileString("Y", "Path",     ge_Path,    wini);
}

void CSpadDlg::OnTimer(UINT nIDEvent) 
{
	TimerOff();

	m_iOntimerAnt++;
	CString temp, wtitel;
	int kom1ret0 = 0;

// hämta senaste admiinfo för 1000
	CSpadData::typ_SpadAdmi sa;
	m_psd->SelectSpadAdmiFunk(1000, &sa);
	m_sniffintervall = sa.SniffIntervallSekunder;

// uppdatera bildens timerinfo
	temp.Format("sniff %d  (%d sek) ", m_iOntimerAnt, m_sniffintervall);
	SetDlgItemText(IDC_ONTIMERANT, temp);
	GetDlgItem(IDC_ONTIMERANT)->UpdateWindow();

// hämta hem och lägg ut senaste databasantal
	OnBnClickedDatastatus2();

// Vid varje sniffning så läser vi in hela beställningstabellen till vektorn
	std::vector<CSpadData::typ_SpadBest>  bestvector;
	bestvector.clear();
	m_psd->SelectSpadBestAll(&bestvector);
	CSpadData::typ_SpadBest sb;

// Här kommer stora genomsnurrningen av beställningarna
	for (unsigned int varv = 0; varv < bestvector.size(); varv++)
	{

		sb = bestvector[varv];

		// räkna antalet väntande beställningar
		if (sb.Kommando == 1 && sb.ReturKod == 0)
			kom1ret0++;

		// lite småknepigt med auto, tror detta är rätt och bra
		if (m_autostart && !m_aktiv_1001 
		&& sb.Kommando == 1 
		&& sb.ReturKod == 0 
		&& sb.FunktionsNummer == 1001) 
			OnBnClickedStart1001(); // starta

		if (m_autostart && !m_aktiv_1002
		&& sb.Kommando == 1
		&& sb.ReturKod == 0
		&& sb.FunktionsNummer == 1002)
			OnBnClickedStart1002(); // starta

		if (m_autostart && !m_aktiv_1003
		&& sb.Kommando == 1
		&& sb.ReturKod == 0
		&& sb.FunktionsNummer == 1003)
			OnBnClickedStart1003(); // starta

		if (m_autostart && !m_aktiv_1004
			&& sb.Kommando == 1
			&& sb.ReturKod == 0
			&& sb.FunktionsNummer == 1004)
			OnBnClickedStart1004(); // starta

		if (sb.BestID <= m_bestidhighsenast) continue; // visa bara nytillkommet
		m_bestidhighsenast = sb.BestID;

//		if (varv+1 <= m_bestidhighsenast) continue; // bytt till varv, bestid i oordning men unikt inom fil
//		m_bestidhighsenast = varv+1;

		switch (sb.Kommando)
		{
			case 1:
				wtitel = sb.BestTitel;
				wtitel.Trim();
				temp.Format("Best nr %d (%d) '%s' VÄNTAR på körning sedan %d %06d", 
					sb.BestNummer, 
					sb.FunktionsNummer,					
					wtitel,
					sb.BestDatum,
					sb.BestKlock);
				_Messa(temp, 0);
				break;

			case 2:
				wtitel = sb.BestTitel;
				wtitel.Trim();
				temp.Format("Best nr %d (%d) '%s': körning STARTAD %d %06d", 
					sb.BestNummer, 
					sb.FunktionsNummer, 
					wtitel,
					sb.KorstartDatum,
					sb.KorstartKlock );
				_Messa(temp, 0);

				if (sb.FunktionsNummer == 1001)
				{
					m_aktiv_1001 = true;
					GetDlgItem( IDC_FUNKAVBRYT1    )->ShowWindow(SW_SHOW);
					GetDlgItem( IDC_PAUSA          )->ShowWindow(SW_SHOW);
					GetDlgItem( IDC_FORTSATT       )->ShowWindow(SW_SHOW);
					GetDlgItem( IDC_FUNKAVBRYT1    )->EnableWindow(TRUE);
					GetDlgItem( IDC_PAUSA          )->EnableWindow(TRUE);
					GetDlgItem( IDC_FORTSATT       )->EnableWindow(FALSE);
				}

				if (sb.FunktionsNummer == 1002)
				{
					m_aktiv_1002 = true;
					GetDlgItem( IDC_FUNKAVBRYT2    )->ShowWindow(SW_SHOW);
					GetDlgItem( IDC_PAUSA2         )->ShowWindow(SW_SHOW);
					GetDlgItem( IDC_FORTSATT2      )->ShowWindow(SW_SHOW);
					GetDlgItem( IDC_FUNKAVBRYT2    )->EnableWindow(TRUE);
					GetDlgItem( IDC_PAUSA2         )->EnableWindow(TRUE);
					GetDlgItem( IDC_FORTSATT2      )->EnableWindow(FALSE);
				}

				if (sb.FunktionsNummer == 1003)
				{
					m_aktiv_1003 = true;
					GetDlgItem(IDC_FUNKAVBRYT3)->ShowWindow(SW_SHOW);
					GetDlgItem(IDC_PAUSA3)->ShowWindow(SW_SHOW);
					GetDlgItem(IDC_FORTSATT3)->ShowWindow(SW_SHOW);
					GetDlgItem(IDC_FUNKAVBRYT3)->EnableWindow(TRUE);
					GetDlgItem(IDC_PAUSA3)->EnableWindow(TRUE);
					GetDlgItem(IDC_FORTSATT3)->EnableWindow(FALSE);
				}

				if (sb.FunktionsNummer == 1004)
				{
					m_aktiv_1003 = true;
					GetDlgItem(IDC_FUNKAVBRYT4)->ShowWindow(SW_SHOW);
					GetDlgItem(IDC_PAUSA4)->ShowWindow(SW_SHOW);
					GetDlgItem(IDC_FORTSATT4)->ShowWindow(SW_SHOW);
					GetDlgItem(IDC_FUNKAVBRYT4)->EnableWindow(TRUE);
					GetDlgItem(IDC_PAUSA4)->EnableWindow(TRUE);
					GetDlgItem(IDC_FORTSATT4)->EnableWindow(FALSE);
				}

				break;

			case 3:
				wtitel = sb.BestTitel;
				wtitel.Trim();
				temp.Format("Best nr %d (%d) '%s': körning PÅGÅR (%d) hittills '%d':%d,%d,%d", 
					sb.BestNummer, 
					sb.FunktionsNummer, 
					wtitel,
					sb.ProgressNummer,
					sb.KorPostAntal,
					sb.ResultatBelopp1,
					sb.ResultatBelopp2,
					sb.ResultatBelopp3
					);
				_Messa(temp, 0);
				break;

			case 4:
				wtitel = sb.BestTitel;
				wtitel.Trim();
				temp.Format("Best nr %d (%d) '%s': körning KLAR %d %06d, resultat '%d':%d,%d,%d", 
					sb.BestNummer, 
					sb.FunktionsNummer, 
					wtitel,
					sb.KorslutDatum,
					sb.KorslutKlock,
					sb.KorPostAntal,
					sb.ResultatBelopp1,
					sb.ResultatBelopp2,
					sb.ResultatBelopp3
					);
				_Messa(temp, 0);

				if (sb.FunktionsNummer == 1001)
					_ResettaFunk1();
				if (sb.FunktionsNummer == 1002)
					_ResettaFunk2();
				if (sb.FunktionsNummer == 1003)
					_ResettaFunk3();
				if (sb.FunktionsNummer == 1004)
					_ResettaFunk4();

				if (sb.ReturKod == 0) // tecken på att 4:an ännu inte omhändertagits
				{
					// ev stänga ned funkmodul
					if (m_autostart &&  m_aktiv_1001 && sb.FunktionsNummer == 1001) OnBnClickedAvbrytexe1(); // sluta
					if (m_autostart &&  m_aktiv_1002 && sb.FunktionsNummer == 1002) OnBnClickedAvbrytexe2(); // sluta
					if (m_autostart &&  m_aktiv_1003 && sb.FunktionsNummer == 1003) OnBnClickedAvbrytexe3(); // sluta
					if (m_autostart &&  m_aktiv_1004 && sb.FunktionsNummer == 1004) OnBnClickedAvbrytexe4(); // sluta

					// I verkligheten kanske aktiviteter med resultatet

					// lägg in 4:an i historiktabellen
					m_psd->InsertSpadBestHist(&sb);
					// markera i returkoden
					m_psd->UpdateraSpadBestBestIDReturkod(sb.BestID, 184);
					// skapa en femma
					sb.Kommando = 5;
					m_psd->InsertSpadBest(&sb);
				}

				break;

			case 5:
				wtitel = sb.BestTitel;
				wtitel.Trim();
				temp.Format("Best nr %d (%d) '%s': klar %d %06d, RESULTAT hanterat,%d:%d,%d,%d", 
					sb.BestNummer, 
					sb.FunktionsNummer, 
					wtitel,
					sb.KorslutDatum,
					sb.KorslutKlock,
					sb.KorPostAntal,
					sb.ResultatBelopp1,
					sb.ResultatBelopp2,
					sb.ResultatBelopp3
					);
				_Messa(temp, 0);
				break;

			case 7:
				wtitel = sb.BestTitel;
				wtitel.Trim();
				temp.Format("Best nr %d (%d) '%s': körning AVBRUTEN %d %06d", 
					sb.BestNummer, 
					sb.FunktionsNummer,
					wtitel,
					sb.KorslutDatum,
					sb.KorslutKlock
     				);
				_Messa(temp, 0);
				if (sb.FunktionsNummer == 1001)
					_ResettaFunk1();
				if (sb.FunktionsNummer == 1002)
					_ResettaFunk2();
				if (sb.FunktionsNummer == 1003)
					_ResettaFunk3();
				if (sb.FunktionsNummer == 1004)
					_ResettaFunk4();

				break;

			case 13:
				wtitel = sb.BestTitel;
				wtitel.Trim();
				temp.Format("Best nr %d (%d) '%s' BORTTAGEN innan körning, beställd %d %06d",
					sb.BestNummer,
					sb.FunktionsNummer,
					wtitel,
					sb.BestDatum,
					sb.BestKlock);
				_Messa(temp, 0);
				break;


			case 16:
				wtitel = sb.BestTitel;
				wtitel.Trim();
				temp.Format("Best nr %d (%d) '%s': körning PAUSAD (%d) hittills '%d':%d,%d,%d", 
					sb.BestNummer, 
					sb.FunktionsNummer,
					wtitel,
					sb.ProgressNummer,
					sb.KorPostAntal,
					sb.ResultatBelopp1,
					sb.ResultatBelopp2,
					sb.ResultatBelopp3
					);
				_Messa(temp, 0);
				break;

			case 17:
				wtitel = sb.BestTitel;
				wtitel.Trim();
				temp.Format("Best nr %d (%d) '%s': körning FORTSÄTTER (%d) hittills '%d':%d,%d,%d", 
					sb.BestNummer, 
					sb.FunktionsNummer, 
					wtitel,
					sb.ProgressNummer,
					sb.KorPostAntal,
					sb.ResultatBelopp1,
					sb.ResultatBelopp2,
					sb.ResultatBelopp3
					);
				_Messa(temp, 0);
				break;

			case 77:
				temp.Format("Modul %d alive and IDLE %d %06d", 
					sb.FunktionsNummer,					
					sb.SenastDatum,
					sb.SenastKlock);
				_Messa(temp, 0);
				break;

			case 526:
				temp.Format("Funktionsmodul %d har STARTAT %d %06d", 
					sb.FunktionsNummer,
					sb.SenastDatum,
					sb.SenastKlock
     				);
				_Messa(temp, 0);
				if (sb.FunktionsNummer == 1001)
				{
					GetDlgItem( IDC_START1001      )->SetWindowText("Term funk 1");
					GetDlgItem( IDC_AVBRYTEXE1     )->EnableWindow(TRUE);
				}
				if (sb.FunktionsNummer == 1002)
				{
					GetDlgItem( IDC_START1002      )->SetWindowText("Term funk 2");
					GetDlgItem( IDC_AVBRYTEXE2     )->EnableWindow(TRUE);
				}
				if (sb.FunktionsNummer == 1003)
				{
					GetDlgItem(IDC_START1003)->SetWindowText("Term funk 3");
					GetDlgItem(IDC_AVBRYTEXE3)->EnableWindow(TRUE);
				}
				if (sb.FunktionsNummer == 1004)
				{
					GetDlgItem(IDC_START1004)->SetWindowText("Term funk 4");
					GetDlgItem(IDC_AVBRYTEXE4)->EnableWindow(TRUE);
				}

				break;

			case 527:
				temp.Format("Funktionsmodul %d har ABRUPT TERMINERATS %d %06d", 
					sb.FunktionsNummer,
					sb.SenastDatum,
					sb.SenastKlock
     				);
				_Messa(temp, 0);
				break;

			case 528:
				temp.Format("Funktionsmodul %d har SLUTAT %d %06d", 
					sb.FunktionsNummer,
					sb.SenastDatum,
					sb.SenastKlock
     				);
				_Messa(temp, 0);
				if (sb.FunktionsNummer == 1001)
				{
					m_aktiv_1001 = false;
					GetDlgItem( IDC_START1001 )->SetWindowText("Starta funk 1");
					GetDlgItem(IDC_AVBRYTEXE1 )->EnableWindow(FALSE);
				}
				if (sb.FunktionsNummer == 1002)
				{
					m_aktiv_1002 = false;
					GetDlgItem( IDC_START1002 )->SetWindowText("Starta funk 2");
					GetDlgItem(IDC_AVBRYTEXE2 )->EnableWindow(FALSE);
				}
				if (sb.FunktionsNummer == 1003)
				{
					m_aktiv_1003 = false;
					GetDlgItem(IDC_START1003)->SetWindowText("Starta funk 3");
					GetDlgItem(IDC_AVBRYTEXE3)->EnableWindow(FALSE);
				}
				if (sb.FunktionsNummer == 1004)
				{
					m_aktiv_1004 = false;
					GetDlgItem(IDC_START1004)->SetWindowText("Starta funk 4");
					GetDlgItem(IDC_AVBRYTEXE4)->EnableWindow(FALSE);
				}

				break;

			default:
				break;
		}

	}

	// redovisa antalet väntande beställningar
	if (kom1ret0 > 0)
	{
		m_wintex.AppendFormat(",  Väntande beställningar---%d", kom1ret0);
		SetWindowTextA(m_wintex);
	}

	TimerOn();
}

void CSpadDlg::TimerOn()
{
	// enligt manualen: wintimerid kan skilja sig från itimerid
	m_WinTimerId = SetTimer(m_iTimerId, m_sniffintervall*1000, NULL);
	SetDlgItemText(IDC_ONOFF, "Timer on");
	GetDlgItem(IDC_ONOFF)->UpdateWindow();
	GetDlgItem( IDC_TIMER    )->SetWindowText("Timer");
	m_timeron = true;
}

void CSpadDlg::TimerOff()
{
    KillTimer(m_WinTimerId);
	SetDlgItemText(IDC_ONOFF, "Timer off");
	GetDlgItem(IDC_ONOFF)->UpdateWindow();
	GetDlgItem( IDC_TIMER    )->SetWindowText("Timer");
	m_timeron = false;
}

int CSpadDlg::_Messa(CString csmess, char how)
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

int CSpadDlg::_PumpMessages()
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

void CSpadDlg::OnBnClickedFunkstart1()
{
	// TODO: Add your control notification handler code here
	TimerOff();
	GetDlgItem( IDC_FUNKBEST1      )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_BESTTITEL1     )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_BESTANTAL1     )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_FUNKANGRA1     )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_STATIC1        )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_STATIC2        )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_BESTTITEL1     )->SetFocus();
	SetDlgItemInt( IDC_BESTANTAL1, 3);

	int datum, klock;
	CString temp;
	m_psd->HamtaDatumKlocka(&datum, &klock);
	temp.Format("Långbro-%d-%06d", 1001, klock);
	SetDlgItemText( IDC_BESTTITEL1, temp);
}

void CSpadDlg::OnBnClickedFunkavbryt1()
{
	// TODO: Add your control notification handler code here
	GetDlgItem( IDC_FUNKSTART1    )->EnableWindow(TRUE);
	GetDlgItem( IDC_FUNKAVBRYT1   )->EnableWindow(FALSE);
	_Messa("Pågående körning begärd avbrytas", 0);
	m_psd->BegaranAvbrytKorning(1001);
	TimerOn();
}

void CSpadDlg::OnBnClickedFunkbest1()
{
	// TODO: Add your control notification handler code here

	CString wBestTitel, temp;
	int wBestAntal;
	int wBestNummer;

//	ge_DataMode = 2;
	wBestNummer = m_psd->FixaNyttBestNummer();

	wBestAntal = GetDlgItemInt(IDC_BESTANTAL1);
	GetDlgItemText(IDC_BESTTITEL1, wBestTitel);

	GetDlgItem( IDC_FUNKBEST1     )->ShowWindow(SW_HIDE);
//	GetDlgItem( IDC_FUNKSTART1    )->EnableWindow(FALSE);
	GetDlgItem( IDC_BESTTITEL1     )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_BESTANTAL1     )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_FUNKANGRA1     )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_STATIC1        )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_STATIC2        )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_FUNKAVBRYT1    )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_PAUSA          )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_FORTSATT       )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_FUNKAVBRYT1    )->EnableWindow(FALSE);
	GetDlgItem( IDC_PAUSA          )->EnableWindow(FALSE);
	GetDlgItem( IDC_FORTSATT       )->EnableWindow(FALSE);

	m_psd->NollstallBegaran(1001);

	CSpadData::typ_SpadBest sb;
	memset(&sb, 0, sizeof(sb));
	sb.FunktionsNummer = 1001;
	sb.BestNummer = wBestNummer;
	sb.BestAntal = wBestAntal;
	strncpy_s(sb.BestTitel, wBestTitel, 45); // glömt hur bäst göra med textfält och bnoll i db
	sb.Kommando = 1;
	m_psd->HamtaDatumKlocka(&sb.BestDatum, &sb.BestKlock);
	sb.SenastDatum = sb.BestDatum;
	sb.SenastKlock = sb.BestKlock;

	m_psd->InsertSpadBest(&sb);

	temp.Format("Beställning %d lagd för funktion %d; titel:%s, antal:%d", 
		sb.BestNummer,sb.FunktionsNummer,sb.BestTitel, wBestAntal);
	_Messa(temp, 0);

	TimerOn();
}

void CSpadDlg::OnBnClickedFunkangra1()
{
	// TODO: Add your control notification handler code here
	_ResettaFunk1();
	_Messa("Beställning funktion 1 avbruten", 0);

	TimerOn();
}

void CSpadDlg::OnBnClickedTimer()
{
	// TODO: Add your control notification handler code here
	if  (m_timeron)
	{
		TimerOff();
	}
	else
	{
		TimerOn();
	}

}

void CSpadDlg::_ResettaFunk1()
{
	GetDlgItem( IDC_FUNKBEST1     )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_FUNKSTART1    )->EnableWindow(TRUE);
	GetDlgItem( IDC_BESTTITEL1     )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_BESTANTAL1     )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_FUNKANGRA1     )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_STATIC1        )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_STATIC2        )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_FUNKAVBRYT1    )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_PAUSA          )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_FORTSATT       )->ShowWindow(SW_HIDE);
}

void CSpadDlg::_ResettaFunk2()
{
	GetDlgItem( IDC_FUNKBEST2     )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_FUNKSTART2    )->EnableWindow(TRUE);
	GetDlgItem( IDC_BESTTITEL2     )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_BESTANTAL2     )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_FUNKANGRA2     )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_STATIC6        )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_STATIC7        )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_FUNKAVBRYT2    )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_PAUSA2         )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_FORTSATT2      )->ShowWindow(SW_HIDE);
}

void CSpadDlg::_ResettaFunk3()
{
	GetDlgItem(IDC_FUNKBEST3)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_FUNKSTART3)->EnableWindow(TRUE);
	GetDlgItem(IDC_BESTTITEL3)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BESTANTAL3)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_FUNKANGRA3)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC12)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC13)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_FUNKAVBRYT3)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_PAUSA3)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_FORTSATT3)->ShowWindow(SW_HIDE);
}

void CSpadDlg::OnBnClickedBestlist()
{
//	TimerOff();

	std::vector<CSpadData::typ_SpadBest>  bestvector;
	CSpadData::typ_SpadBest sb;
	bestvector.clear();

	m_psd->SelectSpadBestAll(&bestvector);

	CString temp, wtitel;
	unsigned int varv;

	temp.Format("Radnr Best: Nr Titel Funk Kommando Returkod Datum Klockslag");
	_Messa(temp, 14);

	for (varv = 0; varv < bestvector.size(); varv++)
	{
		sb = bestvector[varv];
		wtitel = sb.BestTitel;
		wtitel.Trim();
		if (wtitel.GetLength() == 0)
			wtitel.Format("Intert meddelande %03d", sb.Kommando);
		temp.Format("%03d %d '%s' %d %03d %03d %06d %06d",
		varv+1,
		sb.BestNummer, wtitel, sb.FunktionsNummer, 
		sb.Kommando, 
		sb.ReturKod,
		sb.SenastDatum,  sb.SenastKlock);
		_Messa(temp, 14);
	}

//	TimerOn();

}

void CSpadDlg::OnBnClickedClear()
{
	SendDlgItemMessage(IDC_LIST1, LB_RESETCONTENT, 0, 0); 
}

void CSpadDlg::OnBnClickedAllabest()
{
	SendDlgItemMessage(IDC_LIST1, LB_RESETCONTENT, 0, 0);
	m_bestidhighsenast = 0;
}

void CSpadDlg::OnBnClickedOk()
{
	CDialogEx::OnOK();
}

void CSpadDlg::OnBnClickedCancel()
{
//	CDialogEx::OnCancel();
	CDialogEx::OnOK();
}

void CSpadDlg::OnBnClickedCleardb()
{
	// Fyll på i MessArci, töm best exkluxive väntande beställningar
	TimerOff();
	OnBnClickedAllabest();
	int antal = m_psd->InsertMessArciFromBest();
	m_psd->DeleteSpadBestAll();	 // int kom=1, retur=0
	CString temp;
	temp.Format("Antal (överförda) rader nu i Arkivet: %d", antal);
	_Messa(temp, 0);
	SetWindowTextA(temp);
	TimerOn();
}

void CSpadDlg::OnBnClickedInitadmi()
{
	// TODO: Add your control notification handler code here
	m_psd->InitAdmi();
}


void CSpadDlg::OnBnClickedPausa()
{
	// TODO: Add your control notification handler code here
	GetDlgItem( IDC_PAUSA       )->EnableWindow(FALSE);
	GetDlgItem( IDC_FORTSATT    )->EnableWindow(TRUE);
	_Messa("Pågående körning begärd pausas", 0);
	m_psd->BegaranPausaKorning(1001);
}


void CSpadDlg::OnBnClickedFortsatt()
{
	// TODO: Add your control notification handler code here
	GetDlgItem( IDC_FORTSATT       )->EnableWindow(FALSE);
	GetDlgItem( IDC_PAUSA          )->EnableWindow(TRUE);
	_Messa("Pågående körning begärd fortsättas", 0);
	m_psd->BegaranFortsattKorning(1001);
}


void CSpadDlg::OnBnClickedVisaadmi()
{
	// TODO: Add your control notification handler code here
	std::vector<CSpadData::typ_SpadAdmi>  admivector;
	admivector.clear();
	m_psd->SelectSpadAdmiAll(&admivector);

	CString temp;
	unsigned int varv;
	CSpadData::typ_SpadAdmi sa;

	temp.Format("Admi: ID Funk Kommando Sniff Varv Rapp P1 P2 P3");
	_Messa(temp, 0);

	for (varv = 0; varv < admivector.size(); varv++)
	{
		sa = admivector[varv];
		temp.Format("%d %d %d %d %d %d %d %d %d",
		sa.AdmiID, 
		sa.FunktionsNummer, 
		sa.Kommando,
		sa.SniffIntervallSekunder,
		sa.KorVarvSekunder,
		sa.AliveRapp,
		sa.Param1,
		sa.Param2,
		sa.Param3);
		_Messa(temp, 0);
	}

}

void CSpadDlg::OnBnClickedUppdatadmi()
{
	// TODO: Add your control notification handler code here
	m_admivector.clear();
	m_psd->SelectSpadAdmiAll(&m_admivector);

	GetDlgItem( IDC_COMBO1       )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_STATIC5      )->ShowWindow(SW_SHOW);

	SendDlgItemMessage(IDC_COMBO1, CB_RESETCONTENT, 0, 0);

	unsigned int varv, retur;
	CString temp;
	for (varv = 0; varv < m_admivector.size(); varv++)
	{
		temp.Format("Funktion %04d", m_admivector[varv].FunktionsNummer);
		retur = SendDlgItemMessage(IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)(LPCSTR)temp);
	}
	SendDlgItemMessage(IDC_COMBO1,CB_SETCURSEL,0,0);	// variant pang på
	OnCbnSelchangeCombo1();
	
}

void CSpadDlg::OnCbnSelchangeCombo1()
{
	// TODO: Add your control notification handler code here
	GetDlgItem( IDC_STATIC3      )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_STATIC4      )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_STATIC8      )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_ADMISNIFF    )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_ADMIVARV     )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_SAVEADMI     )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_AVBRYTADMI   )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_ALIVERAPP    )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_P1           )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_P2           )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_P3           )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_STATIC9      )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_STATIC10     )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_STATIC11     )->ShowWindow(SW_SHOW);

	int retur = SendDlgItemMessage(IDC_COMBO1, CB_GETCURSEL, 0, 0);

	SetDlgItemInt( IDC_ADMISNIFF, m_admivector[retur].SniffIntervallSekunder);
	SetDlgItemInt( IDC_ADMIVARV,  m_admivector[retur].KorVarvSekunder);
	SetDlgItemInt( IDC_ALIVERAPP, m_admivector[retur].AliveRapp);
	SetDlgItemInt( IDC_P1,        m_admivector[retur].Param1);
	SetDlgItemInt( IDC_P2,        m_admivector[retur].Param2);
	SetDlgItemInt( IDC_P3,        m_admivector[retur].Param3);

	m_combonummer = retur;
}

void CSpadDlg::OnBnClickedSaveadmi()
{
	// TODO: Add your control notification handler code here
	int sniff, varv, admiid, rapp, p1, p2, p3;
	sniff = GetDlgItemInt( IDC_ADMISNIFF );
	varv  = GetDlgItemInt( IDC_ADMIVARV  );
	rapp  = GetDlgItemInt( IDC_ALIVERAPP );
	p1    = GetDlgItemInt( IDC_P1        );
	p2    = GetDlgItemInt( IDC_P2        );
	p3    = GetDlgItemInt( IDC_P3        );
	m_admivector[m_combonummer].SniffIntervallSekunder = sniff;
	m_admivector[m_combonummer].KorVarvSekunder        = varv;
	m_admivector[m_combonummer].AliveRapp              = rapp;
	m_admivector[m_combonummer].Param1                 = p1;
	m_admivector[m_combonummer].Param2                 = p2;
	m_admivector[m_combonummer].Param3                 = p3;

	admiid = m_admivector[m_combonummer].AdmiID;
	m_psd->UpdateAdmiAdmiIDSniffVarvRappParam(admiid, sniff, varv, rapp, p1, p2, p3);

	CSpadData::typ_SpadAdmi sa;
	m_psd->SelectSpadAdmiFunk(1000, &sa);
	m_sniffintervall = sa.SniffIntervallSekunder;

	OnBnClickedVisaadmi();
}

void CSpadDlg::_SlackAlltAdmi()
{
	GetDlgItem( IDC_COMBO1       )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_STATIC5      )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_STATIC3      )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_STATIC4      )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_STATIC8      )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_ADMISNIFF    )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_ADMIVARV     )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_SAVEADMI     )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_AVBRYTADMI   )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_ALIVERAPP    )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_P1           )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_P2           )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_P3           )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_STATIC9      )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_STATIC10     )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_STATIC11     )->ShowWindow(SW_HIDE);
}

void CSpadDlg::OnBnClickedAvbrytadmi()
{
	// TODO: Add your control notification handler code here
	_SlackAlltAdmi();
}

void CSpadDlg::OnBnClickedStart1001()
{
	if (m_aktiv_1001 == false)
	{
		m_aktiv_1001 = true;
		StartaProcess("c:\\dev\\spadfunk\\release\\spadfunk.exe 1001", CREATE_NEW_CONSOLE, "c:\\dev\\spad\\", 1001);
		GetDlgItem( IDC_START1001 )->SetWindowText("Term funk 1");
		GetDlgItem(IDC_AVBRYTEXE1 )->EnableWindow(TRUE);
	}
	else
	{
		m_aktiv_1001 = false;

		int retur = TerminateProcess(m_pi[0].hProcess, 0);
		if (retur == 0)
			_Messa("FELFELFEL vid terminering funk 1",0);

		CSpadData::typ_SpadBest sb;
		memset(&sb,0,sizeof(sb));
		sb.Kommando = 527;
		m_psd->HamtaDatumKlocka(&sb.KorslutDatum, &sb.KorslutKlock);
		sb.SenastDatum = sb.KorslutDatum;
		sb.SenastKlock = sb.KorslutKlock;
		sb.FunktionsNummer = 1001;
		m_psd->InsertSpadBest(&sb);
//		_Messa("Funktionsmodul 1001 har abrupt TERMINERATS", 0);
		GetDlgItem( IDC_START1001 )->SetWindowText("Starta funk 1");
		GetDlgItem(IDC_AVBRYTEXE1 )->EnableWindow(FALSE);
	}
}

void CSpadDlg::OnBnClickedStart1002()
{
	if (m_aktiv_1002 == false)
	{
		m_aktiv_1002 = true;
		StartaProcess("c:\\dev\\spadfunk\\release\\spadfunk.exe 1002", CREATE_NEW_CONSOLE, "c:\\dev\\spad\\", 1002);
		GetDlgItem( IDC_START1002 )->SetWindowText("Term funk 2");
		GetDlgItem(IDC_AVBRYTEXE2 )->EnableWindow(TRUE);
	}
	else
	{
		m_aktiv_1002 = false;

		int retur = TerminateProcess(m_pi[1].hProcess, 0);
		if (retur == 0)
			_Messa("FELFELFEL vid terminering funk 2",0);

		CSpadData::typ_SpadBest sb;
		memset(&sb,0,sizeof(sb));
		sb.Kommando = 527;
		m_psd->HamtaDatumKlocka(&sb.KorslutDatum, &sb.KorslutKlock);
		sb.SenastDatum = sb.KorslutDatum;
		sb.SenastKlock = sb.KorslutKlock;
		sb.FunktionsNummer = 1002;
		m_psd->InsertSpadBest(&sb);
		GetDlgItem( IDC_START1002 )->SetWindowText("Starta funk 2");
		GetDlgItem(IDC_AVBRYTEXE2 )->EnableWindow(FALSE);
	}
}


int CSpadDlg::StartaProcess(CString sprocess, int creaflag, CString sstartdir, int funktionsnummer)
{

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    char cprocess[200];
    char cstartdir[200];
//    DWORD exitkod;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );
    strcpy_s(cprocess, sprocess);
    strcpy_s(cstartdir, sstartdir);

    if( !CreateProcess( NULL,   
        cprocess,       // modul
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        creaflag,
//        0,              // No creation flags
//        CREATE_NO_WINDOW, CREATE_NEW_CONSOLE // googla och lär
        NULL,           // Use parent's environment block
        cstartdir,      // Starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure 
        )

    {
        CString temp;
        temp.Format("CreateProcess failed (%d)", GetLastError() );        
        AfxMessageBox(temp);        
    }
    else
    {
        // Wait until child process exits.
//        WaitForSingleObject( pi.hProcess, INFINITE );

        // Hämta exitkod från processen
//        CString temp;       
//        GetExitCodeProcess(pi.hProcess, &exitkod);
 //       temp.Format("Fortsätter, exitcode (%d)", exitkod );        
 //       AfxMessageBox(temp);  

		// spara undan processinfo till ev terminate
		if (funktionsnummer == 1001)
			m_pi[0] = pi;
		else
			m_pi[1] = pi;

        // Close process and thread handles. 
//        CloseHandle( pi.hProcess );
        CloseHandle( pi.hThread );

    }

//    return exitkod;
	return 0;
}


void CSpadDlg::OnBnClickedFunkstart2()
{
	// TODO: Add your control notification handler code here

	TimerOff();
	GetDlgItem( IDC_FUNKBEST2      )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_BESTTITEL2     )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_BESTANTAL2     )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_FUNKANGRA2     )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_STATIC6        )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_STATIC7        )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_BESTTITEL2     )->SetFocus();
	SetDlgItemInt( IDC_BESTANTAL2, 3);

	int datum, klock;
	CString temp;
	m_psd->HamtaDatumKlocka(&datum, &klock);
	temp.Format("Långbro-%d-%06d", 1002, klock);
	SetDlgItemText( IDC_BESTTITEL2, temp);
}


void CSpadDlg::OnBnClickedFunkbest2()
{
	// TODO: Add your control notification handler code here

	CString wBestTitel, temp;
	int wBestAntal;
	int wBestNummer;
	
	wBestNummer = m_psd->FixaNyttBestNummer();

	wBestAntal = GetDlgItemInt(IDC_BESTANTAL2);
	GetDlgItemText(IDC_BESTTITEL2, wBestTitel);

	GetDlgItem( IDC_FUNKBEST2      )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_BESTTITEL2     )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_BESTANTAL2     )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_FUNKANGRA2     )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_STATIC6        )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_STATIC7        )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_FUNKAVBRYT2    )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_PAUSA2         )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_FORTSATT2      )->ShowWindow(SW_SHOW);
	GetDlgItem( IDC_FUNKAVBRYT2    )->EnableWindow(FALSE);
	GetDlgItem( IDC_PAUSA2         )->EnableWindow(FALSE);
	GetDlgItem( IDC_FORTSATT2      )->EnableWindow(FALSE);

	m_psd->NollstallBegaran(1002);

	CSpadData::typ_SpadBest sb;
	memset(&sb, 0, sizeof(sb));
	sb.FunktionsNummer = 1002;
	sb.BestNummer = wBestNummer;
	sb.BestAntal = wBestAntal;
	strncpy_s(sb.BestTitel, wBestTitel, 45); // glömt hur bäst göra med textfält och bnoll i db
	sb.Kommando = 1;
	m_psd->HamtaDatumKlocka(&sb.BestDatum, &sb.BestKlock);
	sb.SenastDatum = sb.BestDatum;
	sb.SenastKlock = sb.BestKlock;

	m_psd->InsertSpadBest(&sb);

	temp.Format("Beställning %d lagd för funktion %d; titel:%s, antal:%d", 
		sb.BestNummer,sb.FunktionsNummer,sb.BestTitel, wBestAntal);
	_Messa(temp, 0);

	TimerOn();
}


void CSpadDlg::OnBnClickedFunkangra2()
{
	_ResettaFunk2();
	_Messa("Beställning funktion 2 avbruten", 0);

	TimerOn();
}


void CSpadDlg::OnBnClickedPausa2()
{
	// TODO: Add your control notification handler code here
	GetDlgItem( IDC_PAUSA2      )->EnableWindow(FALSE);
	GetDlgItem( IDC_FORTSATT2   )->EnableWindow(TRUE);
	_Messa("Pågående körning begärd pausas", 0);
	m_psd->BegaranPausaKorning(1002);
}


void CSpadDlg::OnBnClickedFortsatt2()
{
	// TODO: Add your control notification handler code here
	GetDlgItem( IDC_FORTSATT2      )->EnableWindow(FALSE);
	GetDlgItem( IDC_PAUSA2         )->EnableWindow(TRUE);
	_Messa("Pågående körning begärd fortsättas", 0);
	m_psd->BegaranFortsattKorning(1002);
}


void CSpadDlg::OnBnClickedFunkavbryt2()
{
	// TODO: Add your control notification handler code here
	GetDlgItem( IDC_FUNKSTART2    )->EnableWindow(TRUE);
	GetDlgItem( IDC_FUNKAVBRYT2   )->EnableWindow(FALSE);
	_Messa("Pågående körning begärd avbrytas", 0);
	m_psd->BegaranAvbrytKorning(1002);
	TimerOn();
}


void CSpadDlg::OnBnClickedAvbrytexe1()
{
	_Messa("Funk 1 begärd tas bort", 0);
	m_psd->BegaranAvbrytExekvering(1001);
}


void CSpadDlg::OnBnClickedAvbrytexe2()
{
	_Messa("Funk 2 begärd tas bort", 0);
	m_psd->BegaranAvbrytExekvering(1002);
}


void CSpadDlg::OnBnClickedCheck1()
{
	// TODO: Add your control notification handler code here
	if (IsDlgButtonChecked(IDC_CHECK1) == 1)
		m_autostart = true;
	else
		m_autostart = false;
}


void CSpadDlg::OnBnClickedHistorik()
{
	if (m_phi == NULL)
	{
		m_phi = new CHistorik;
		m_phi->Create(CHistorik::IDDH, this);  // bara en gång om inte DestroyWindow() emellan       
	}

	//	m_phi->PekareCSpadData(m_psd);  // nu har historik egen instans av spaddataklassen
	m_phi->ShowWindow(SW_SHOW);
}


void CSpadDlg::OnBnClickedArkivmess()
{
	// Enkel listning
	std::vector<CSpadData::typ_SpadBest>  messarcivector;

	messarcivector.clear();
	m_psd->SelectSpadMessArciAll(&messarcivector);

	SendDlgItemMessage(IDC_LIST1, LB_RESETCONTENT, 0, 0);

	unsigned int varv;
	CString temp, titel;
	CSpadData::typ_SpadBest sb;

	for (varv = 0; varv < messarcivector.size(); varv++)
	{
		sb = messarcivector[varv];
		titel = sb.BestTitel;
		titel.Trim();
		temp.Format("rad %04d: %04d %04d %04d %03d %d %s %d %06d %d %d %d %d %d", varv + 1, sb.BestID, sb.BestNummer, sb.FunktionsNummer,
			sb.Kommando, sb.ReturKod, titel, sb.SenastDatum, sb.SenastKlock, sb.ProgressNummer ,sb.KorPostAntal, 
			sb.ResultatBelopp1, sb.ResultatBelopp2, sb.ResultatBelopp3);
		_Messa(temp, 14);
	}
}


void CSpadDlg::OnBnClickedClearbest()
{
	// listar väntande beställningar (kommando=1, returkod=0) och ingenting annat
	CString temp, wtitel;
	std::vector<CSpadData::typ_SpadBest>  bestvector;
	CSpadData::typ_SpadBest sb;
	unsigned int varv;

	SendDlgItemMessage(IDC_LIST1, LB_RESETCONTENT, 0, 0);

	bestvector.clear();
	m_psd->SelectSpadBestNya(&bestvector, 1001);	// befinlig funktion utnyttjas
	m_psd->SelectSpadBestNya(&bestvector, 1002);
	m_psd->SelectSpadBestNya(&bestvector, 1003);
	m_psd->SelectSpadBestNya(&bestvector, 1004);

	for (varv = 0; varv < bestvector.size(); varv++)
	{
		sb = bestvector[varv];
		wtitel = sb.BestTitel;
		wtitel.Trim();
		temp.Format("Best nr %d (%d) '%s' VÄNTAR på körning sedan %d %06d",
			sb.BestNummer,
			sb.FunktionsNummer,
			wtitel,
			sb.BestDatum,
			sb.BestKlock);
		_Messa(temp, 14); 
	}
}


void CSpadDlg::OnBnClickedDatastatus()
{
	OnBnClickedDatastatus2();
	_Messa(m_wintex, 0);
}

void CSpadDlg::OnBnClickedDatastatus2()
{
	int BestAnt, BestHistAnt, MessArci;
	m_psd->DataStatus(&BestAnt, &BestHistAnt, &MessArci);
	if (ge_DataMode == 1)
		m_wintex.Format("Antal i databasen just nu;   Best---%d,  Historik---%d,  Arkiv---%d", BestAnt, BestHistAnt, MessArci);
	else
		m_wintex.Format("Antal i filerna just nu;   Best---%d,  Historik---%d,  Arkiv---%d", BestAnt, BestHistAnt, MessArci);
	SetWindowTextA(m_wintex);
}

void CSpadDlg::OnBnClickedLbsel()
{
	if (!m_lbselbekrafta)
	{

		int langd, result;
		CString temp, temp2, temp3;
		char tempc[500];

		GetDlgItem(IDC_LBSEL2)->EnableWindow(TRUE);

		m_lbselselind = SendDlgItemMessage(IDC_LIST1, LB_GETCURSEL, 0, 0);
		if (m_lbselselind < 0) return;

		langd = SendDlgItemMessage(IDC_LIST1, LB_GETTEXT, m_lbselselind, (LPARAM)(LPCTSTR)tempc);  // skrivit av från stackoverflow.com
		temp.AppendFormat(tempc);
		result = temp.Find("VÄNTAR", 0);
		if (result < 0) 				
			goto ogiltigt;

		result = temp.Find("Best nr ", 0);
		if (result < 0)
			goto ogiltigt;

		temp2.Format(temp.Mid(result + 7, 6));
		m_lbselbestnr = atoi(temp2);

		// kolla i basen att den har returkod noll
		int wreturkod = m_psd->GeSpadBestReturkod(m_lbselbestnr);
		if (wreturkod != 0)
			goto ogiltigt;

		temp3.Format("Bekräfta borttag av beställning %d", m_lbselbestnr);
		SetDlgItemText(IDC_LBSEL, temp3);
		GetDlgItem(IDC_LBSEL)->UpdateWindow();
		m_lbselbekrafta = true;
	}
	else
	{
		SendDlgItemMessage(IDC_LIST1, LB_DELETESTRING, m_lbselselind, 0);
		m_lbselbekrafta = false;
		SetDlgItemText(IDC_LBSEL, m_lbseltext);
		GetDlgItem(IDC_LBSEL)->UpdateWindow();

		// ändra kommando till 13 för beställningen; "Borttagen beställning"	
		m_psd->UpdateraSpadBestBestNummerKommando(m_lbselbestnr, 13);
		
	}

	return;

ogiltigt:	

	_Messa("OGILTIG MARKERING", 14);
	SetDlgItemText(IDC_LBSEL, "OGILTIG MARKERING");
	Sleep(1500);
	SetDlgItemText(IDC_LBSEL, m_lbseltext);
	return;
}

void CSpadDlg::OnBnClickedLbsel2()
{
	// återställ efter ångra borttagsmarkering av väntande beställning
	m_lbselbekrafta = false;
	m_lbselbestnr = 0;
	SendDlgItemMessage(IDC_LIST1, LB_SETCURSEL, -1, 0);
	SetDlgItemText(IDC_LBSEL, m_lbseltext);
	GetDlgItem(IDC_LBSEL)->UpdateWindow();
//	GetDlgItem(IDC_LBSEL2)->EnableWindow(FALSE);
}

void CSpadDlg::OnBnClickedVisa()
{
	if (IsDlgButtonChecked(IDC_VISA) == 1)
	{
		GetDlgItem(IDC_UPPDATADMI)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_VISAADMI  )->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_INITADMI  )->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_ALLABEST  )->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BESTLIST  )->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CLEARDB   )->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CLEARBEST )->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_ARKIVMESS )->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_HISTORIK  )->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHECK1    )->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_START1001 )->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_START1002 )->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_START1003)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_START1004)->ShowWindow(SW_SHOW);
		//		GetDlgItem(IDC_DATASTATUS)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_AVBRYTEXE1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_AVBRYTEXE2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_AVBRYTEXE3)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_AVBRYTEXE4)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_UPPDATADMI)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_VISAADMI  )->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_INITADMI  )->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ALLABEST  )->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BESTLIST  )->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CLEARDB   )->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CLEARBEST )->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ARKIVMESS )->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_HISTORIK  )->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK1    )->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_START1001 )->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_START1002 )->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_START1003)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_START1004)->ShowWindow(SW_HIDE);
//		GetDlgItem(IDC_DATASTATUS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_AVBRYTEXE1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_AVBRYTEXE2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_AVBRYTEXE3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_AVBRYTEXE4)->ShowWindow(SW_HIDE);
	}
}

void CSpadDlg::OnBnClickedDatakalla()
{
	if (m_datakallabekrafta == false)
	{
		SetDlgItemText(IDC_DATAKALLA, "Bekräfta");
		m_datakallabekrafta = true;
		return;
	}

	TimerOff();

	if (ge_DataMode == 1)
	{
		CSpadData::typ_SpadAdmi sa;	// ordningen här är viktig, uppdat först db
		m_psd->SelectSpadAdmiFunk(1000, &sa); // uppdatera param1 till en tvåa, signalen till spadfunk, orkar inte göra direkt funktion
		m_psd->UpdateAdmiAdmiIDSniffVarvRappParam(sa.AdmiID, sa.SniffIntervallSekunder, sa.KorVarvSekunder, sa.AliveRapp, 2, sa.Param2, sa.Param3);
		m_psd->_FixBestAdmiDbFiMass();
		SetDlgItemText(IDC_DATAKALLATEXT, "Filer");
		ge_DataMode = 2;
		OnBnClickedAllabest();
		_Messa("Enkla filer laddade från databas, filerna används nu som datakälla", 0);
	}
	else
	{
		CSpadData::typ_SpadAdmi sa;
		SetDlgItemText(IDC_DATAKALLA, "Laddar bas...");
		m_psd->SelectSpadAdmiFunk(1000, &sa); // uppdatera param1 till en etta, signalen till spadfunk
		m_psd->UpdateAdmiAdmiIDSniffVarvRappParam(sa.AdmiID, sa.SniffIntervallSekunder, sa.KorVarvSekunder, sa.AliveRapp, 1, sa.Param2, sa.Param3);
		m_psd->_FixBestAdmiFiDbMass();
		SetDlgItemText(IDC_DATAKALLATEXT, "Databas");
		ge_DataMode = 1;
		OnBnClickedAllabest();
		_Messa("Databasen laddad från enkla filerna, databasen används nu som datakälla", 0);
	}

	// uppdatera egen + gemensam inifil
	char wstr[5];
	CString cstr;
	cstr = ge_Path + "Spad.ini";
	_itoa_s(ge_DataMode, wstr, 10);
	WritePrivateProfileString("X", "DataMode", wstr, ".\\SpadDlg.ini"); // egen
	WritePrivateProfileString("Y", "DataMode", wstr, cstr); // gemensam

	SetDlgItemText(IDC_DATAKALLA, "Byt datakälla");
	m_datakallabekrafta = false;
	TimerOn();
	return;
}

void CSpadDlg::OnBnClickedKallaavbryt()
{
	SetDlgItemText(IDC_DATAKALLA, "Byt datakälla");
	m_datakallabekrafta = false;
	_Messa("Växling av datakälla avbruten", 14);
}


void CSpadDlg::OnBnClickedFunkstart3()
{
	// TODO: Add your control notification handler code here

	TimerOff();
	GetDlgItem(IDC_FUNKBEST3)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_BESTTITEL3)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_BESTANTAL3)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_FUNKANGRA3)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC12)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC13)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_BESTTITEL3)->SetFocus();
	SetDlgItemInt(IDC_BESTANTAL3, 3);

	int datum, klock;
	CString temp;
	m_psd->HamtaDatumKlocka(&datum, &klock);
	temp.Format("Långbro-%d-%06d", 1003, klock);
	SetDlgItemText(IDC_BESTTITEL3, temp);

}


void CSpadDlg::OnBnClickedFunkbest3()
{
	// TODO: Add your control notification handler code here
	CString wBestTitel, temp;
	int wBestAntal;
	int wBestNummer;

	wBestNummer = m_psd->FixaNyttBestNummer();

	wBestAntal = GetDlgItemInt(IDC_BESTANTAL3);
	GetDlgItemText(IDC_BESTTITEL3, wBestTitel);

	GetDlgItem(IDC_FUNKBEST3)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BESTTITEL3)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BESTANTAL3)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_FUNKANGRA3)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC12)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC13)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_FUNKAVBRYT3)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_PAUSA3)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_FORTSATT3)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_FUNKAVBRYT3)->EnableWindow(FALSE);
	GetDlgItem(IDC_PAUSA3)->EnableWindow(FALSE);
	GetDlgItem(IDC_FORTSATT3)->EnableWindow(FALSE);

	m_psd->NollstallBegaran(1003);

	CSpadData::typ_SpadBest sb;
	memset(&sb, 0, sizeof(sb));
	sb.FunktionsNummer = 1003;
	sb.BestNummer = wBestNummer;
	sb.BestAntal = wBestAntal;
	strncpy_s(sb.BestTitel, wBestTitel, 45); // glömt hur bäst göra med textfält och bnoll i db
	sb.Kommando = 1;
	m_psd->HamtaDatumKlocka(&sb.BestDatum, &sb.BestKlock);
	sb.SenastDatum = sb.BestDatum;
	sb.SenastKlock = sb.BestKlock;

	m_psd->InsertSpadBest(&sb);

	temp.Format("Beställning %d lagd för funktion %d; titel:%s, antal:%d",
		sb.BestNummer, sb.FunktionsNummer, sb.BestTitel, wBestAntal);
	_Messa(temp, 0);

	TimerOn();
}


void CSpadDlg::OnBnClickedFunkangra3()
{
	_ResettaFunk3();
	_Messa("Beställning funktion 3 avbruten", 0);

	TimerOn();
}


void CSpadDlg::OnBnClickedPausa3()
{
	GetDlgItem(IDC_PAUSA3)->EnableWindow(FALSE);
	GetDlgItem(IDC_FORTSATT3)->EnableWindow(TRUE);
	_Messa("Pågående körning begärd pausas", 0);
	m_psd->BegaranPausaKorning(1003);
}


void CSpadDlg::OnBnClickedFortsatt3()
{
	GetDlgItem(IDC_FORTSATT3)->EnableWindow(FALSE);
	GetDlgItem(IDC_PAUSA3)->EnableWindow(TRUE);
	_Messa("Pågående körning begärd fortsättas", 0);
	m_psd->BegaranFortsattKorning(1003);
}


void CSpadDlg::OnBnClickedFunkavbryt3()
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_FUNKSTART3)->EnableWindow(TRUE);
	GetDlgItem(IDC_FUNKAVBRYT3)->EnableWindow(FALSE);
	_Messa("Pågående körning begärd avbrytas", 0);
	m_psd->BegaranAvbrytKorning(1003);
	TimerOn();
}


void CSpadDlg::OnBnClickedStart1003()
{
	if (m_aktiv_1003 == false)
	{
		m_aktiv_1003 = true;
		//		StartaProcess("c:\\dev\\spadfunk\\release\\spadfunk.exe 1002", CREATE_NEW_CONSOLE, "c:\\dev\\spad\\", 1002);
		StartaProcess("c:\\dev\\spadcobol\\spadfunkcbl\\bin\\x64\\release\\spadfunkcbl.exe", CREATE_NEW_CONSOLE, "c:\\dev\\spad\\", 1003);
		GetDlgItem(IDC_START1003)->SetWindowText("Term funk 3");
		GetDlgItem(IDC_AVBRYTEXE3)->EnableWindow(TRUE);
	}
	else
	{
		m_aktiv_1003 = false;

		int retur = TerminateProcess(m_pi[1].hProcess, 0);
		if (retur == 0)
			_Messa("FELFELFEL vid terminering funk 3", 0);

		CSpadData::typ_SpadBest sb;
		memset(&sb, 0, sizeof(sb));
		sb.Kommando = 527;
		m_psd->HamtaDatumKlocka(&sb.KorslutDatum, &sb.KorslutKlock);
		sb.SenastDatum = sb.KorslutDatum;
		sb.SenastKlock = sb.KorslutKlock;
		sb.FunktionsNummer = 1003;
		m_psd->InsertSpadBest(&sb);
		GetDlgItem(IDC_START1003)->SetWindowText("Starta funk 3");
		GetDlgItem(IDC_AVBRYTEXE3)->EnableWindow(FALSE);
	}
}

void CSpadDlg::OnBnClickedAvbrytexe3()
{
	_Messa("Funk 3 begärd tas bort", 0);
	m_psd->BegaranAvbrytExekvering(1003);
}


void CSpadDlg::OnBnClickedClear2()
{
	m_psd->DeleteSpadBestAll();	 // int kom=1, retur=0
	OnBnClickedClear(); // clear bild
	_Messa("Best clear", 14);
}


void CSpadDlg::OnBnClickedVisaginit()
{
	// visa gemensan initfil
	WinExec("notepad.exe "+ ge_Path + "spad.ini", SW_SHOW);
}


void CSpadDlg::OnBnClickedVisaeinit()
{
	// visa egen ursprunglig inifil
	WinExec("notepad.exe .\\spaddlg.ini", SW_SHOW);
}

void CSpadDlg::OnBnClickedClearallatab()
{
	m_psd->DeleteSpadAll();
	m_psd->InitAdmi();
}

void CSpadDlg::OnBnClickedFunkstart4()
{
	TimerOff();
	int retur;
	char rapptext[5][20] = { "Veckorapport", "Månadsrapport", "Årsrapport", "Räckviddsutv", "Dagdelsrapport"};
	retur = SendDlgItemMessage(IDC_COMBO3, CB_RESETCONTENT, 0, 0);
	for (int x = 0; x <= 4; x++)
	{
		retur = SendDlgItemMessage(IDC_COMBO3, CB_ADDSTRING, 0, (LPARAM)(LPCSTR)rapptext[x]);
	}
	SendDlgItemMessage(IDC_COMBO3, CB_SETCURSEL, 0, 0);	// variant pang på
	OnCbnSelchangeCombo3();

	GetDlgItem(IDC_FUNKBEST4)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_BESTTITEL4)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_BESTANTAL4)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_FUNKANGRA4)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC14)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_COMBO3)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_MSTD4)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_MSTDTEXT)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT3)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT1)->ShowWindow(SW_SHOW);
	GetDlgItem(ID_PLUS7)->ShowWindow(SW_SHOW);
//	HamtaMgrpNamn(char *snamn, int iMgrp)

}

void CSpadDlg::OnBnClickedFunkbest4()
{
	CString wBestTitel, temp;
	int wBestAntal;
	int wBestNummer;

	//	ge_DataMode = 2;
	wBestNummer = m_psd->FixaNyttBestNummer();

	wBestAntal = GetDlgItemInt(IDC_BESTANTAL4);
	GetDlgItemText(IDC_BESTTITEL4, wBestTitel);

	GetDlgItem(IDC_FUNKBEST4)->ShowWindow(SW_HIDE);
	//	GetDlgItem( IDC_FUNKSTART1    )->EnableWindow(FALSE);
	GetDlgItem(IDC_BESTTITEL4)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BESTANTAL4)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_FUNKANGRA4)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC14)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_MSTD4)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_MSTDTEXT)->ShowWindow(SW_HIDE);

	GetDlgItem(IDC_EDIT1)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT3)->ShowWindow(SW_HIDE);
	GetDlgItem(ID_PLUS7)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_COMBO3)->ShowWindow(SW_HIDE);

	GetDlgItem(IDC_FUNKAVBRYT4)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_PAUSA4)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_FORTSATT4)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_FUNKAVBRYT4)->EnableWindow(FALSE);
	GetDlgItem(IDC_PAUSA4)->EnableWindow(FALSE);
	GetDlgItem(IDC_FORTSATT4)->EnableWindow(FALSE);

	m_psd->NollstallBegaran(1004);

	CSpadData::typ_SpadBest sb;
	memset(&sb, 0, sizeof(sb));
	sb.FunktionsNummer = 1004;
	sb.BestNummer = wBestNummer;
	sb.BestAntal = wBestAntal;
	strncpy_s(sb.BestTitel, wBestTitel, 45); // glömt hur bäst göra med textfält och bnoll i db
	sb.Kommando = 1;
	m_psd->HamtaDatumKlocka(&sb.BestDatum, &sb.BestKlock);
	sb.SenastDatum = sb.BestDatum;
	sb.SenastKlock = sb.BestKlock;
	sb.ResultatBelopp1 = GetDlgItemInt(IDC_EDIT3);
	sb.ResultatBelopp2 = GetDlgItemInt(IDC_EDIT1);
	sb.ResultatBelopp3 = GetDlgItemInt(IDC_MSTD4);

	m_psd->InsertSpadBest(&sb);

	temp.Format("Beställning %d lagd för funktion %d; titel:%s, antal:%d",
		sb.BestNummer, sb.FunktionsNummer, sb.BestTitel, wBestAntal);
	_Messa(temp, 0);

	TimerOn();
}

void CSpadDlg::OnBnClickedFunkangra4()
{
	_ResettaFunk4();
	_Messa("Beställning funktion 4 avbruten", 0);

	TimerOn();
}

void CSpadDlg::_ResettaFunk4()
{
	GetDlgItem(IDC_FUNKBEST4)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_FUNKSTART4)->EnableWindow(TRUE);
	GetDlgItem(IDC_BESTTITEL4)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BESTANTAL4)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_FUNKANGRA4)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC14)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_FUNKAVBRYT4)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_PAUSA4)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_FORTSATT4)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_COMBO3)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_MSTD4)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_MSTDTEXT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT1)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT3)->ShowWindow(SW_HIDE);
	GetDlgItem(ID_PLUS7)->ShowWindow(SW_HIDE);


}

void CSpadDlg::OnBnClickedStart1004()
{
	if (m_aktiv_1004 == false)
	{
		m_aktiv_1004 = true;
		StartaProcess("c:\\dev\\spadfunkTVrap\\release\\spadfunktvrap.exe 1004", CREATE_NEW_CONSOLE, "c:\\dev\\spadfunkTVrap\\", 1004);
		GetDlgItem(IDC_START1004)->SetWindowText("Term funk 4");
		GetDlgItem(IDC_AVBRYTEXE4)->EnableWindow(TRUE);
	}
	else
	{
		m_aktiv_1004 = false;

		int retur = TerminateProcess(m_pi[1].hProcess, 0);
		if (retur == 0)
			_Messa("FELFELFEL vid terminering funk 4", 0);

		CSpadData::typ_SpadBest sb;
		memset(&sb, 0, sizeof(sb));
		sb.Kommando = 527;
		m_psd->HamtaDatumKlocka(&sb.KorslutDatum, &sb.KorslutKlock);
		sb.SenastDatum = sb.KorslutDatum;
		sb.SenastKlock = sb.KorslutKlock;
		sb.FunktionsNummer = 1004;
		m_psd->InsertSpadBest(&sb);
		GetDlgItem(IDC_START1004)->SetWindowText("Starta funk 4");
		GetDlgItem(IDC_AVBRYTEXE4)->EnableWindow(FALSE);
	}
}

void CSpadDlg::OnBnClickedAvbrytexe4()
{
	_Messa("Funk 4 begärd tas bort", 0);
	m_psd->BegaranAvbrytExekvering(1004);
}

void CSpadDlg::OnBnClickedPrint()
{
	if (m_printallt)
		Printa(1, 1);
	else
		Printa(1, 2);
}


void CSpadDlg::OnBnClickedPrint2()
{
	if (m_printallt)
		Printa(2, 1);
	else
		Printa(2, 2);
}

void CSpadDlg::Printa(int filtyp, int select)
{
	char tempc[500];
	CString temps;
	int startradnr = 1;
	unsigned int radnr;
	FILE *printfil;

	std::vector<CSpadData::typ_SpadRapp>  rappvector;
	rappvector.clear();
	m_psd->SelectSpadRappAll(&rappvector);

	if (select == 2) // bara senaste
	{
		int x;
		if (rappvector.size() > 0)
		{
			int wbestnr = rappvector[rappvector.size() - 1].BestNummer;
			for (x = 0; rappvector[x].BestNummer != wbestnr; x++);
			startradnr = x + 1;
		}
	}

	if (filtyp == 1)
		fopen_s(&printfil, "print.txt", "wt");
	else
		fopen_s(&printfil, "print.xls", "wt");

	for (radnr = startradnr; radnr <= rappvector.size(); radnr++)
	{
		temps = rappvector[radnr - 1].Rad;
		temps.TrimRight(); // avslutande \n åker också bort
		temps += "\n";
		fputs(temps, printfil);
	}
	fclose(printfil);

	if (filtyp == 1)
		sprintf_s(tempc, "%s%s", "notepad.exe ", "print.txt");
	else
		sprintf_s(tempc, "%s%s", "c:\\program files\\microsoft office 15\\root\\office15\\excel.exe ", "print.xls");
	WinExec(tempc, SW_SHOW);
}

void CSpadDlg::OnBnClickedPrintallt()
{
	if (IsDlgButtonChecked(IDC_PRINTALLT) == 1)
		m_printallt = true;
	else
		m_printallt = false;
}


void CSpadDlg::OnLbnSelchangeList2()
{
	// TODO: Add your control notification handler code here
}


void CSpadDlg::OnCbnSelchangeCombo3()
{
	int langd;
	char tempchar[200];
	CString temp;

	int rad = SendDlgItemMessage(IDC_COMBO3, CB_GETCURSEL, 0, 0);
	SetDlgItemInt(IDC_BESTANTAL4, rad+1);
	SetDlgItemInt(IDC_EDIT3, 131230);
	SetDlgItemInt(IDC_EDIT1, 1);
	if (rad == 0)
	{
		SetDlgItemInt(IDC_EDIT1, 7);
		SetDlgItemInt(IDC_EDIT3, 131230);
		SetDlgItemInt(IDC_MSTD4, 223);
	}
	if (rad == 1)
	{
		SetDlgItemInt(IDC_EDIT1, 35);
		SetDlgItemInt(IDC_EDIT3, 131230);
		SetDlgItemInt(IDC_MSTD4, 223);
	}
	if (rad == 2)
	{
		SetDlgItemInt(IDC_EDIT1, 364);
		SetDlgItemInt(IDC_EDIT3, 121231);
		SetDlgItemInt(IDC_MSTD4, 223);
	}
	if (rad == 3)
	{
		SetDlgItemInt(IDC_EDIT1, 15);
		SetDlgItemInt(IDC_EDIT3, 121231);
		SetDlgItemInt(IDC_MSTD4, 223);
	}
	if (rad == 4)
	{
		SetDlgItemInt(IDC_EDIT1, 7);
		SetDlgItemInt(IDC_EDIT3, 131230);
		SetDlgItemInt(IDC_MSTD4, 223);
	}

	langd = SendDlgItemMessage(IDC_COMBO3, CB_GETLBTEXT, rad, (LPARAM)(LPCTSTR)tempchar);  // skrivit av från stackoverflow.com
	temp = "TV ";
	temp.Append(tempchar);
	temp.AppendFormat(" %d %d", GetDlgItemInt(IDC_EDIT3), GetDlgItemInt(IDC_EDIT1));
	int datum, klock;
	m_psd->HamtaDatumKlocka(&datum, &klock);
	temp.AppendFormat(" best kl %06d", klock);

	SetDlgItemText(IDC_BESTTITEL4, temp);
}


void CSpadDlg::OnBnClickedPlus7()
{
	// TODO: Add your control notification handler code here
	int datum = GetDlgItemInt(IDC_EDIT3);
	for (int x = 1; x <= 7; x++)
		zdat6addi(&datum);
	SetDlgItemInt(IDC_EDIT3, datum);
}

void CSpadDlg::zdat6addi(int *datum6i)
{
	// gammal z för uppstegning datum ett snäpp
	int aar, man, dag;

	aar = *datum6i / 10000;
	man = ((*datum6i - (aar * 10000)) / 100);
	dag = (*datum6i - (aar * 10000) - (man * 100));

	dag++;

	if (dag == 29 && man == 2 && aar != 96 && aar != 0 && aar != 4 && aar != 8 && aar != 12 && aar != 16 && aar != 20) // inte skottår
	{
		man++;
		dag = 1;
	};

	if (dag == 30 && man == 2 && (aar == 96 || aar == 0 || aar == 4 || aar == 8 || aar == 12 || aar == 16 || aar == 20)) // skottår
	{
		man++;
		dag = 1;
	};

	if (dag == 31 && (man == 4 || man == 6 || man == 9 || man == 11))
	{
		man++;
		dag = 1;
	};

	if (dag == 32)
	{
		man++;
		dag = 1;
	};

	if (man == 13)
	{
		aar++;
		if (aar == 100)
			aar = 0;
		man = 1;
	};

	*datum6i = aar * 10000 + man * 100 + dag;

	return;
};

int CSpadDlg::HamtaMgrpNamn(char *snamn, int iMgrp)
{
	//  bd9510_HamtaMgrpNamn
	//  Fyller anroparens snamn med namnet på målgruppen som har nummer iMgrp
	//  enligt 409-målgruppsmodellen

	const int iGTAntal = 8;
	int     iGTNr[iGTAntal] = { 521,522,523,524,525,526,527,528 };
	char    sGTNamn[iGTAntal][30] = { "Tittande, alla 3-99", "Singeltittande", "Grupptittande 2+", "Grupptittande 3+",
		"Ung+äldre", "Ung+vuxen", "Enbart småbarn", "Enbart skolbarn" };

	int     aldPM2[15] = { 3,7,12,15,20,25,30,35,40,45,50,55,60,70,100 };
	//  char    namn[19][8]=    {   "Famliv0","Famliv1","Famliv2","Famliv3","Famliv4",
	//                              "Inkans0","Inkans1","Inkans2","Inkans3",
	//                              "Indink0","Indink1","Indink2","Indink3","Indink4",
	//                              "Urban1" ,"Urban2" ,"Urban3" ,"Urban4" ,"Urban5"
	//                          };

	char    namn[107][12] = {
		"Famliv0",
		"Famliv1","Famliv01",
		"Famliv2","Famliv02","Famliv12","Famliv012",
		"Famliv3","Famliv03","Famliv13","Famliv013",
		"Famliv23","Famliv023","Famliv123","Famliv0123",
		"Famliv4","Famliv04","Famliv14","Famliv014",
		"Famliv24","Famliv024","Famliv124","Famliv0124",
		"Famliv34","Famliv034","Famliv134","Famliv0134",
		"Famliv234","Famliv0234","Famliv1234","Famliv01234",

		"Inkans0",
		"Inkans1","Inkans01",
		"Inkans2","Inkans02","Inkans12","Inkans012",
		"Inkans3","Inkans03","Inkans13","Inkans013",
		"Inkans23","Inkans023","Inkans123","Inkans0123",

		// tv46064
		"M15-64",
		"K15-64","A15-64",
		"M60-64","K60-64","A60-64","M65-69",
		"K65-69","A65-69","Indink24","Indink124",
		"Indink34","Indink134","Indink234","Indink1234",

		"Urban1",
		"Urban2","Urban12",
		"Urban3","Urban13","Urban23","Urban123",
		"Urban4","Urban14","Urban24","Urban124",
		"Urban34","Urban134","Urban234","Urban1234",
		"Urban5","Urban15","Urban25","Urban125",
		"Urban35","Urban135","Urban235","Urban1235",
		"Urban45","Urban145","Urban245","Urban1245",
		"Urban345","Urban1345","Urban2345","Urban12345",

		// tv46064ny
		"3-64","7-64","12-64","15-64","20-64","25-64","30-64","35-64",
		"40-64","45-64","50-64","55-64","60-64","65-69","65-99"

	};

	// -----  Tillsammanstittande  -------------------------------
	if (iMgrp >= 521 && iMgrp<521 + iGTAntal)
	{
		for (int i = 0; i<iGTAntal; i++)
		{
			if (iMgrp == iGTNr[i])
			{
				lstrcpy(snamn, sGTNamn[i]);
				return 0;
			}
		}
	}
	// -----------------------------------------------------------


	if (iMgrp>316 && iMgrp <= 408)
	{
		strcpy_s(snamn, 15, namn[iMgrp - 317]);
		return 0;
	}

	int     iKon, iFran, iTill;
	int     iFack;

	// Hantera de två lacade specialmålgrupperna A7-10 och A11-14
	if (iMgrp == 315)
	{
		sprintf_s(snamn, 15, "A7-10");
		return 0;
	}
	if (iMgrp == 316)
	{
		sprintf_s(snamn, 15, "A11-14");
		return 0;
	}

	//  tv46064ny se även bd9510_HamtaMgrpNamnKort

	//	Målgruppsummer (= nollbaserat elementnummer) 409-453, 45 st nya pga 60-64 och 65-69
	//  15 st män, 15 st kvinnor, 15 st alla
	//  M 409-423, K 424-438, A 439-453

	if (iMgrp >= 409 && iMgrp <= 453)
	{
		char kon[3] = { 'M','K','A' };
		int i, iKon;
		if (iMgrp >= 409 && iMgrp <= 423)
		{
			iKon = 0;
			i = iMgrp - 317;
		}
		if (iMgrp >= 424 && iMgrp <= 438)
		{
			iKon = 1,
				i = iMgrp - 317 - 15;
		}
		if (iMgrp >= 439 && iMgrp <= 453)
		{
			iKon = 2;
			i = iMgrp - 317 - 30;
		}
		sprintf_s(snamn, 15, "%c%s", kon[iKon], namn[i]);
		return 0;
	}


	iFack = 0;
	for (iKon = 0; iKon<3; iKon++)
	{
		for (iFran = 0; iFran<14; iFran++)
		{
			for (iTill = iFran + 1; iTill<15; iTill++)
			{
				if (iFack == iMgrp)
				{
					// Hittad
					char kon[3] = { 'M','K','A' };
					sprintf_s(snamn, 15, "%c%d-%d",
						kon[iKon],
						aldPM2[iFran],
						aldPM2[iTill] - 1);
					return 0;
				}
				++iFack;
			}
		}
	}

	// Hittar ingen
	return -1;
}




void CSpadDlg::OnEnChangeMstd4()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	int mstdnr;
	char mstdtext[120];
	mstdnr = GetDlgItemInt(IDC_MSTD4);
	HamtaMgrpNamn(mstdtext, mstdnr);
	SetDlgItemText(IDC_MSTDTEXT, mstdtext);
}


void CSpadDlg::OnBnClickedRapp()
{
	m_psd->DeleteRappAll();
	_Messa("Rapp clear", 14);

}

void CSpadDlg::OnBnClickedButton5()
{
	// Starta rapporthaneraren Cobol:RappHant
	StartaProcess("c:\\dev\\rapphant\\rapphant\\bin\\debug\\rapphant.exe ", CREATE_NEW_CONSOLE, "c:\\dev\\rapphant\\", 1004);

}
