// Historik.cpp : implementation file
//

#include "stdafx.h"
#include "Spad.h"
#include "Historik.h"
#include "afxdialogex.h"
#include <algorithm>    // std::sort

// CHistorik dialog

IMPLEMENT_DYNAMIC(CHistorik, CDialogEx)

CHistorik::CHistorik(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{
	m_psd2 = NULL;
	m_psd2 = new CSpadData;
}

CHistorik::~CHistorik()
{
	if (m_psd2 != NULL)
		delete m_psd2; // ger krasch för delete i spaddlg
}

void CHistorik::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CHistorik, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CHistorik::OnBnClickedButton1)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CHistorik::OnCbnSelchangeCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO2, &CHistorik::OnCbnSelchangeCombo2)
	ON_BN_CLICKED(IDOK, &CHistorik::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON2, &CHistorik::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_FUNKRAPP, &CHistorik::OnBnClickedFunkrapp)
	ON_BN_CLICKED(IDC_BUTTON4, &CHistorik::OnBnClickedButton4)
	ON_LBN_DBLCLK(IDC_LIST1, &CHistorik::TestDC)
	ON_LBN_SELCHANGE(IDC_LIST1, &CHistorik::TestSC)
END_MESSAGE_MAP()

BOOL CHistorik::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	PostMessage(WM_COMMAND, IDC_BUTTON1, 0); // för att det hela ska starta på en gång
//	OnBnClickedButton1(); // en annan variant
	GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON4)->ShowWindow(SW_HIDE);
//	GetDlgItem(IDC_AVBRYTEXE3)->EnableWindow(FALSE);
	SetWindowTextA("Historik");
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHistorik::PekareCSpadData(CSpadData *psd)
{
// Vi gör så här s långe, kanske jag lära mig att ge parameter direkt till klasse, se DBwco
// Bekymmer om göra som vanligt, delete här påverkar delete i spaddlg
//	m_psd2 = psd;  // verkar som att klassinstansen från spaddlg används
}

int CHistorik::_Messa(CString csmess, char how)
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

int CHistorik::_PumpMessages()
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

static bool myfunc3(CSpadData::typ_SpadBest sb1, CSpadData::typ_SpadBest sb2)
{
	if (sb1.FunktionsNummer < sb2.FunktionsNummer)
		return true;
	else
		return false;
}

void CHistorik::OnBnClickedButton1()
{
	// Se upp med att listor och combo kan ha egenskapen sort påslagen
	m_besthistvector.clear();
	m_psd2->SelectSpadBestHistAll(&m_besthistvector); // m_psd2 funkar fast vi inte har satt något värde!!!, inte längre
	m_funktionsnummer.clear();

	std::sort(m_besthistvector.begin(), m_besthistvector.end(), myfunc3); // testar sort igen igen, egent bara i filläge

	SendDlgItemMessage(IDC_COMBO1, CB_RESETCONTENT, 0, 0);
	int funknu = 0;
	unsigned int varv, retur;
	CString temp;

	for (varv = 0; varv < m_besthistvector.size(); varv++)
	{
		if (m_besthistvector[varv].FunktionsNummer != funknu)
		{
			funknu = m_besthistvector[varv].FunktionsNummer;
			m_funktionsnummer.push_back(funknu);
			temp.Format("Funktion %04d", funknu);
			retur = SendDlgItemMessage(IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)(LPCSTR)temp);
		}
	}

	SendDlgItemMessage(IDC_LIST1, LB_RESETCONTENT, 0, 0);
	SendDlgItemMessage(IDC_COMBO1, CB_SETCURSEL, 0, 0);	// variant pang på
	OnCbnSelchangeCombo1();

}

void CHistorik::OnCbnSelchangeCombo1()
{
	// Se upp med att listor och combo kan ha egenskapen sort påslagen
	unsigned int retur, varv;
	CString temp, titel;
	CSpadData::typ_SpadBest sb;
	SendDlgItemMessage(IDC_LIST1, LB_RESETCONTENT, 0, 0);

	retur = SendDlgItemMessage(IDC_COMBO1, CB_GETCURSEL, 0, 0);
	m_selfunk = m_funktionsnummer[retur];
	m_bestvarv.clear();
	SendDlgItemMessage(IDC_COMBO2, CB_RESETCONTENT, 0, 0);

	for (varv = 0; varv < m_besthistvector.size(); varv++)
	{
		sb = m_besthistvector[varv];
		if (sb.FunktionsNummer == m_selfunk)
		{
			m_bestvarv.push_back(varv);
			titel = sb.BestTitel;
			titel.Trim();
			temp.Format("Bestnr %04d %s Körstart %d %06d", sb.BestNummer, titel, sb.KorstartDatum, sb.KorstartKlock);
			retur = SendDlgItemMessage(IDC_COMBO2, CB_ADDSTRING, 0, (LPARAM)(LPCSTR)temp);
		}
	}
//	SendDlgItemMessage(IDC_COMBO2, CB_SETCURSEL, 0, 0);	// variant pang på
//	OnCbnSelchangeCombo2();

}

void CHistorik::OnCbnSelchangeCombo2()
{
	int retur;

	CSpadData::typ_SpadBest sb;
	retur = SendDlgItemMessage(IDC_COMBO2, CB_GETCURSEL, 0, 0);
//	SendDlgItemMessage(IDC_COMBO2, CB_SETCURSEL, -1, 0);
	m_selvarv = m_bestvarv[retur];
	sb = m_besthistvector[m_selvarv];
	SelchangeCombo2Fortsatt(sb);
}

void CHistorik::OnBnClickedButton4()
{
	int lbselind;

	lbselind = SendDlgItemMessage(IDC_LIST1, LB_GETCURSEL, 0, 0);
	if (lbselind < 0
		|| lbselind >= (int)m_besthistvector.size())
		return;
	CSpadData::typ_SpadBest sb = m_besthistvector[lbselind];
	GetDlgItem(IDC_BUTTON4)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);
	SelchangeCombo2Fortsatt(sb);
}

void CHistorik::SelchangeCombo2Fortsatt(CSpadData::typ_SpadBest sb)
{
#define RADANT 15
	CString temp[RADANT], titel, tempm;
	std::vector<CSpadData::typ_SpadBest> mv;
	std::vector<CSpadData::typ_SpadBest>::iterator imv;
	CSpadData::typ_SpadBest sm;
	m_psd2->SelectSpadMessArciBest(&mv, sb.BestNummer);	// bra att ha inbyggt urval på bestnr

	SendDlgItemMessage(IDC_LIST1, LB_RESETCONTENT, 0, 0);
	tempm.Format("Arkiverade händelser för beställning %d, %d st:", sb.BestNummer, mv.size()); // antalet avser ju hela arkivet!!
	_Messa(tempm, 14);
	_Messa(" ", 14);

	for (imv = mv.begin(); imv != mv.end(); imv++)
	{
		sm = *imv;
		if (sm.BestNummer == sb.BestNummer)
		{
			titel = sm.BestTitel;
			titel.Trim();
			tempm.Format("%03d %d %s %d %06d %d %d %d %d", sm.Kommando, sm.ReturKod, titel, sm.SenastDatum, sm.SenastKlock, sm.KorPostAntal,
				sm.ResultatBelopp1, sm.ResultatBelopp2, sm.ResultatBelopp3);
			_Messa(tempm, 14);
		}
	}

	titel = sb.BestTitel;
	titel.Trim();
	_Messa(" ", 14);
	_Messa("Sammanfattning:", 14);
	_Messa(" ", 14);

	// listboxen prop tab

	temp[8].Format ("BestIDHist           \t\t%d",    sb.BestID);
	temp[1].Format ("Klardatum            \t\t%d",    sb.KorslutDatum);
	temp[2].Format ("Klarklock            \t\t%06d",  sb.KorslutKlock);
	temp[3].Format ("Titel                \t\t%s",    titel);
	temp[4].Format ("Resultatbelopp 1     \t%d",      sb.ResultatBelopp1);
	temp[5].Format ("Resultatbelopp 2     \t%d",      sb.ResultatBelopp2);
	temp[6].Format ("Resultatbelopp 3     \t%d",      sb.ResultatBelopp3);
	temp[7].Format ("Antal poster         \t\t%d",    sb.KorPostAntal);
	temp[0].Format ("Funktion             \t\t%d",    sb.FunktionsNummer);
	temp[9].Format ("Bestnummer           \t%d",      sb.BestNummer);
	temp[10].Format("Körningslängd        \t\t%d sek",    _BeraknaKorlangd(&sb));
	temp[11].Format("Bestdatum            \t\t%d",    sb.BestDatum);
	temp[12].Format("Bestklock            \t\t%06d",  sb.BestKlock);
	temp[13].Format("Körstartdatum        \t\t%d",    sb.KorstartDatum);
	temp[14].Format("Körstartklock        \t\t%06d",  sb.KorstartKlock);

	for (int x = 0; x < RADANT; x++)
	{
		_Messa(temp[x], 14);
	}


//	OnBnClickedButton1();
}

void CHistorik::OnBnClickedOk()
{
	CDialogEx::OnOK();
}

static bool myfunc1(CSpadData::typ_SpadBest sb1, CSpadData::typ_SpadBest sb2)
{
	if (sb1.BestNummer< sb2.BestNummer)
		return true;
	else
		return false;
}

void CHistorik::OnBnClickedButton2()
{
	// enradig listning alla historik-best
	m_besthistvector.clear();
	m_psd2->SelectSpadBestHistAll(&m_besthistvector); 

	std::sort(m_besthistvector.begin(), m_besthistvector.end(), myfunc1); // testar sort igen igen

	SendDlgItemMessage(IDC_COMBO1, CB_RESETCONTENT, 0, 0);
	SendDlgItemMessage(IDC_COMBO2, CB_RESETCONTENT, 0, 0);
	SendDlgItemMessage(IDC_LIST1,  LB_RESETCONTENT, 0, 0);

	unsigned int varv;
	CString temp, titel;
	CSpadData::typ_SpadBest sb;

	for (varv = 0; varv < m_besthistvector.size(); varv++)
	{
		sb = m_besthistvector[varv];
		titel = sb.BestTitel;
		titel.Trim();
		temp.Format("%04d %d %d %s %d %06d %d %d", sb.BestNummer, sb.FunktionsNummer, 
			sb.Kommando, titel, sb.KorstartDatum, sb.KorstartKlock, sb.ResultatBelopp1, sb.ResultatBelopp3);
		_Messa(temp, 14);	
	}

	temp.Format("Antal rader nu i historiken: %d", varv);
	SetWindowTextA(temp);
	AfxGetMainWnd()->SetWindowTextA(temp);
	GetDlgItem(IDC_BUTTON4)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);
//	OnBnClickedButton1();
}

static bool myfunc2(CSpadData::typ_SpadBest sb1, CSpadData::typ_SpadBest sb2)
{
	if (sb1.FunktionsNummer < sb2.FunktionsNummer)
		return true;
	else
		return false;
}

void CHistorik::OnBnClickedFunkrapp()
{
	// Sammanställning per funktion
	// - antal körningar, - genomsnittlig körtid, - genomsnittligt antal poster

	m_besthistvector.clear();
	m_psd2->SelectSpadBestHistAll(&m_besthistvector);

	std::sort(m_besthistvector.begin(), m_besthistvector.end(), myfunc2); // testar sort igen igen

	SendDlgItemMessage(IDC_COMBO1, CB_RESETCONTENT, 0, 0);
	SendDlgItemMessage(IDC_COMBO2, CB_RESETCONTENT, 0, 0);
	SendDlgItemMessage(IDC_LIST1, LB_RESETCONTENT, 0, 0);

	int korantal;
	int korpostantal;
	int korlangdsek;
	int korantaltot;
	int korpostantaltot;
	int korlangdsektot;

	int funktionsnummernu;
	int mindatum, maxdatum;

	unsigned int varv;
	CString temp;
	CSpadData::typ_SpadBest sb;

	mindatum = 99999999;
	maxdatum = 0;

	for (varv = 0; varv < m_besthistvector.size(); varv++)
	{
		sb = m_besthistvector[varv];
		if (sb.KorstartDatum < mindatum)
			mindatum = sb.KorstartDatum;
		if (sb.KorstartDatum > maxdatum)
			maxdatum = sb.KorstartDatum;
	}

	temp.Format("Period %06d - %06d", mindatum, maxdatum);
	_Messa(temp, 14);

	// Uppfräschning.
	// Klassisk konstruktion med read ahead och yttre och inre loop
	// Den inre innehåller den yttre + påbyggnad för brytning på delbegrepp (funktion)
	// Läsning (varv) och flytt sist i inre loopen samt initialt
	// INGET varvplussande i yttre loopen

	korantaltot     = 0;
	korpostantaltot = 0;
	korlangdsektot  = 0; 

	varv = 0;
	sb = m_besthistvector[varv];
	while (varv < m_besthistvector.size())
	{
		funktionsnummernu = sb.FunktionsNummer;
		korantal     = 0;
		korpostantal = 0;
		korlangdsek  = 0;

		while (varv <= m_besthistvector.size() && funktionsnummernu == sb.FunktionsNummer)
		{

			korantal++;
			korpostantal += sb.KorPostAntal;
			korlangdsek  += _BeraknaKorlangd(&sb); 

			varv++;
			if (varv < m_besthistvector.size())
				sb = m_besthistvector[varv];
			else
				sb.FunktionsNummer = 9999;
		}

		// hantera funkbrytning

		korantaltot     += korantal;
		korpostantaltot += korpostantal;
		korlangdsektot  += korlangdsek;

		_Messa(" ", 14);
		temp.Format("Funktion %04d: antal körningar...%d, antal enheter...%d", funktionsnummernu, korantal, korpostantal);
		_Messa(temp, 14);
		temp.Format("                       genomsnittlig körningslängd...%d sek", korlangdsek/korantal);
		_Messa(temp, 14);
		temp.Format("                       genomsnittligt antal enheter per körning...%d", korpostantal / korantal);
		_Messa(temp, 14);
	}

	if (m_besthistvector.size() > 0)
	{
		_Messa(" ", 14);
		temp.Format("Totalt:\t         antal körningar...%d, antal enheter...%d",korantaltot, korpostantaltot);
		_Messa(temp, 14);
		temp.Format("                       genomsnittlig körningslängd...%d sek", korlangdsektot / korantaltot);
		_Messa(temp, 14);
		temp.Format("                       genomsnittligt antal enheter per körning...%d", korpostantaltot / korantaltot);
		_Messa(temp, 14);
	}
	else
	{
		temp = "Historiken tom";
		_Messa(temp, 14);
	}

	temp.Format("Antal rader nu i historiken: %d", varv);
	SetWindowTextA(temp);
	AfxGetMainWnd()->SetWindowTextA(temp);
//	OnBnClickedButton1();

}

int CHistorik::_BeraknaKorlangd(CSpadData::typ_SpadBest *sb)
{
	int korsek, korsekstart, korsekslut;
	int korsekack;

	korsekack = sb->KorstartKlock / 10000 * 3600;
	korsek = (sb->KorstartKlock / 10000) * 10000;
	korsek = sb->KorstartKlock - korsek;
	korsekack += korsek / 100 * 60;
	korsek = (sb->KorstartKlock / 100) * 100;
	korsekack += sb->KorstartKlock - korsek;
	korsekstart = korsekack;

	korsekack = sb->KorslutKlock / 10000 * 3600;
	korsek = (sb->KorslutKlock / 10000) * 10000;
	korsek = sb->KorslutKlock - korsek;
	korsekack += korsek / 100 * 60;
	korsek = (sb->KorslutKlock / 100) * 100;
	korsekack += sb->KorslutKlock - korsek;
	korsekslut = korsekack;

	korsek = korsekslut - korsekstart;

	return korsek;
}

void CHistorik::TestDC()
{
	// test på dubbelklick
	//	ON_LBN_DBLCLK(IDC_LIST1, &CHistorik::TestDC)
	OnBnClickedButton4();
}

void CHistorik::TestSC()
{
	// test på select
	// ON_LBN_SELCHANGE(IDC_LIST1, &CHistorik::TestSC)
	GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE);

}