//
//	2016-02-05		Kanske alternativt använda simpla filer.
//					fwrite, fread, fseek
//					Använd befintliga typer, hantera själv idnummer typ BestID
//					remove, rename för rensning, helt eller delvis
//					Kanske extern global från spaddlg.cpp som visar läge; bas eller filer
//					Kanske göra en ny klass/modul SpadFile analogt med DBwco.
//					Eller göra en alternativ DBwco med samma funktionsnamn, alltså inga förändringar i spaddata.cpp
//					Nja, fastnar för att först testa CSpadFile med alternativa anrop i spaddata, behåller db i spaddata
//					Möjlighet att växla mellan db och file i demosyfte.
//	2016-02-08		_FixBestAdmiDbFiMass() laddar upp filer från db
//
//  2016-08-06		Lagt till SpadRapp åt SpadFunkTVrap (1004)
//

#include "StdAfx.h"
#include "SpadData.h"
#include "DBwco.h"
#include "SpadFile.h"
#include <vector>

extern int     ge_DataMode; // finns i spaddlg.cpp

// konstruktor
CSpadData::CSpadData(void)
{
	m_pdb = NULL;
	m_pdb = new DBwco("(local)", "Spad", "", ""); // nu funkar det med pdb som medlemsvariabel
	m_psf = NULL;
	m_psf = new CSpadFile;
}

// destruktor
CSpadData::~CSpadData(void)
{
	if (m_pdb != NULL)
		delete m_pdb;
	if (m_psf != NULL)
		delete m_psf;
}

int CSpadData::_FixBestDbFi()
{
	std::vector<CSpadData::typ_SpadBest> bestvector;

	SelectSpadBestAll(&bestvector);
	unsigned int antal = bestvector.size();

	unsigned int varv = 0;
	while (varv < antal)
	{ 
		m_psf->SkrivNyBest(&bestvector[varv], 'b');
		varv++;
	}

	return antal;
}

int CSpadData::_FixBestAdmiDbFiMass()
{
	// här skapas fil-versioner av databasens fyra tabeller

	std::vector<CSpadData::typ_SpadBest> bv;
	int totant = 0;

	bv.clear();
	SelectSpadBestAll(&bv);
	m_psf->SkapaBestMass(&bv, 'b');		// ev befintlig fil slås ut
	totant += bv.size();

	bv.clear();
	SelectSpadBestHistAll(&bv);
	m_psf->SkapaBestMass(&bv, 'h');		// ev befintlig fil slås ut	
	totant += bv.size();

	bv.clear();
	SelectSpadMessArciAll(&bv);
	m_psf->SkapaBestMass(&bv, 'm');		// ev befintlig fil slås ut	
	totant += bv.size();

	std::vector<CSpadData::typ_SpadAdmi> av;
	av.clear();
	SelectSpadAdmiAll(&av);
	m_psf->SkapaAdmiMass(&av);			// ev befintlig fil slås ut	
	totant += av.size();
/*
	bv.clear();
	m_psf->LasBestAlla(&bv);
	m_psf->UpdateraSpadBestBestIDReturkod(777, 888);
	bv.clear();
	m_psf->LasBestAlla(&bv);
	m_psf->DeleteSpadBestNearlyAll();
	m_psf->UpdateraSpadBestBestNummerKommando(58, 555);
	bv.clear();
	m_psf->LasBestNya(&bv, 1002);
	bv.clear();
	m_psf->LasBestAlla(&bv);
	int x = m_psf->GeSpadBestReturkod(58);
	m_psf->UpdateraAdmiFunktionsnummerKommando(1003, 789);
	av.clear();
	m_psf->LasAdmiAlla(&av);
	m_psf->UpdateraAdmiFunktionsnummerKommando(1005, 6);
	m_psf->UpdateAdmiAdmiIDSniffVarvRappParam(28, 101, 102,	103, 104, 105, 106);
	av.clear();
	m_psf->LasAdmiAlla(&av);
	m_psf->UpdateAdmiAdmiIDSniffVarvRappParam(28, 101, 102, 103, 104, 105, 106);
	av.clear();
	m_psf->LasAdmiAlla(&av);
	CSpadData::typ_SpadBest sb;
	memset(&sb, 0, sizeof(sb));
	sb.Kommando = 12345;
	m_psf->SkrivNyBest(&sb);
	bv.clear();
	m_psf->LasBestAlla(&bv, 'b');
	int antal = m_psf->FyllpaMessArciFranBest();
	bv.clear();
	m_psf->LasBestAlla(&bv, 'm');
	int bestant, histant, messant;
	m_psf->Datastatus(&bestant, &histant, &messant);
	m_psf->InitAdmi();
	av.clear();
	m_psf->LasAdmiAlla(&av);
*/
	return totant;
}

int CSpadData::_FixBestAdmiFiDbMass()
{
	// Här skapas databasens fyra tabeller från enkla filerna

	// vi börjar med att tömma alla fyra tabellerna

	ge_DataMode = 1; 
	DeleteSpadAll();

	// admi

	std::vector<CSpadData::typ_SpadAdmi> av;
	std::vector<CSpadData::typ_SpadAdmi>::iterator iav;

	av.clear();
	m_psf->LasAdmiAlla(&av);

	ge_DataMode = 1;
	for (iav = av.begin(); iav != av.end(); iav++)
		InsertSpadAdmi(&*iav);

	// best

	std::vector<CSpadData::typ_SpadBest> bv;
	std::vector<CSpadData::typ_SpadBest>::iterator ibv;

	bv.clear();
	m_psf->LasBestAlla(&bv, 'b');
	ge_DataMode = 1;
	for (ibv = bv.begin(); ibv != bv.end(); ibv++)
		InsertSpadBest(&*ibv);

	// best-hist

	bv.clear();
	m_psf->LasBestAlla(&bv, 'h');
	ge_DataMode = 1;
	for (ibv = bv.begin(); ibv != bv.end(); ibv++)
		InsertSpadBestHist(&*ibv);

	// messarci

	bv.clear();
	m_psf->LasBestAlla(&bv, 'm');
	ge_DataMode = 1;
	for (ibv = bv.begin(); ibv != bv.end(); ibv++)
		InsertSpadMessArci(&*ibv);
	{ }

	return 0;
}

void CSpadData::DeleteSpadBestAll(void)
{
	// dock icke deleta väntande beställningar

	if (ge_DataMode == 2)
	{
		m_psf->DeleteSpadBestNearlyAll();
		return;
	}

    try
    {
        m_pdb->Connect();
		m_pdb->DeleteSpadBestAll();
        m_pdb->Disconnect();

    }
    catch (SAException &x)
    {
        CString sText;
        sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
        AfxMessageBox(sText);
    }

	return;

}

void CSpadData::InsertSpadBest(typ_SpadBest *sb)
{
	if (ge_DataMode == 2)
	{
		m_psf->SkrivNyBest(sb, 'b');
		return;
	}

    try
    {
        m_pdb->Connect();
		m_pdb->InsertSpadBest(sb);
		m_pdb->Disconnect();

    }
    catch (SAException &x)
    {
        CString sText;
        sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
        AfxMessageBox(sText);
    }
	
}

void CSpadData::SelectSpadBestAll(std::vector<CSpadData::typ_SpadBest> *bv)
{
	if (ge_DataMode == 2)
	{
		m_psf->LasBestAlla(bv, 'b');
		return;
	}

    try
    {
        m_pdb->Connect();		
		m_pdb->SelectSpadBestAll(bv);
		m_pdb->Disconnect();

    }
    catch (SAException &x)
    {
        CString sText;
        sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
        AfxMessageBox(sText);
    }

}

void CSpadData::SelectSpadBestNya(std::vector<CSpadData::typ_SpadBest> *bv, int FunktionsNummer)
{
	if (ge_DataMode == 2)
	{
		m_psf->LasBestNya(bv, FunktionsNummer);
		return;
	}

    try
    {
        m_pdb->Connect();		
		m_pdb->SelectSpadBestNya(bv, FunktionsNummer);
		m_pdb->Disconnect();

    }
    catch (SAException &x)
    {
        CString sText;
        sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
        AfxMessageBox(sText);
    }

}

void CSpadData::UpdateraSpadBestBestIDReturkod(int BestID, int Returkod)
{
	if (ge_DataMode == 2)
	{
		m_psf->UpdateraSpadBestBestIDReturkod(BestID, Returkod);
		return;
	}

    try
    {
        m_pdb->Connect();		
		m_pdb->UpdateraSpadBestBestIDReturkod(BestID, Returkod);
		m_pdb->Disconnect();

    }
    catch (SAException &x)
    {
        CString sText;
        sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
        AfxMessageBox(sText);
    }

}

void CSpadData::UpdateraSpadBestBestNummerKommando(int BestNummer, int Kommando)
{
	if (ge_DataMode == 2)
	{
		m_psf->UpdateraSpadBestBestNummerKommando(BestNummer, Kommando);
		return;
	}

	try
	{
		m_pdb->Connect();
		m_pdb->UpdateraSpadBestBestNummerKommando(BestNummer, Kommando);
		m_pdb->Disconnect();

	}
	catch (SAException &x)
	{
		CString sText;
		sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
		AfxMessageBox(sText);
	}

}

int  CSpadData::GeSpadBestReturkod(int BestNummer)
{	
	int wreturkod = 999;

	if (ge_DataMode == 2)
	{
		wreturkod = m_psf->GeSpadBestReturkod(BestNummer);
		return wreturkod;
	}

	try
	{
		m_pdb->Connect();
		wreturkod = m_pdb->GeSpadBestReturkod(BestNummer);
		m_pdb->Disconnect();

	}
	catch (SAException &x)
	{
		CString sText;
		sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
		AfxMessageBox(sText);
	}

	return wreturkod;
}

void CSpadData::BegaranAvbrytKorning(int FunktionsNummer)
{
	if (ge_DataMode == 2)
	{
		m_psf->UpdateraAdmiFunktionsnummerKommando(FunktionsNummer, 6);
		return;
	}

    try
    {
        m_pdb->Connect();		
		m_pdb->UpdateAdmiFunkKommando(FunktionsNummer, 6);
		m_pdb->Disconnect();

    }
    catch (SAException &x)
    {
        CString sText;
        sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
        AfxMessageBox(sText);
    }

}

void CSpadData::BegaranAvbrytExekvering(int FunktionsNummer)
{
	if (ge_DataMode == 2)
	{
		m_psf->UpdateraAdmiFunktionsnummerKommando(FunktionsNummer, 99);
		return;
	}

    try
    {
        m_pdb->Connect();		
		m_pdb->UpdateAdmiFunkKommando(FunktionsNummer, 99);
		m_pdb->Disconnect();

    }
    catch (SAException &x)
    {
        CString sText;
        sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
        AfxMessageBox(sText);
    }

}

void CSpadData::BegaranPausaKorning(int FunktionsNummer)
{
	if (ge_DataMode == 2)
	{
		m_psf->UpdateraAdmiFunktionsnummerKommando(FunktionsNummer, 16);
		return;
	}

    try
    {
        m_pdb->Connect();		
		m_pdb->UpdateAdmiFunkKommando(FunktionsNummer, 16);
		m_pdb->Disconnect();

    }
    catch (SAException &x)
    {
        CString sText;
        sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
        AfxMessageBox(sText);
    }

}

void CSpadData::BegaranFortsattKorning(int FunktionsNummer)
{
	if (ge_DataMode == 2)
	{
		m_psf->UpdateraAdmiFunktionsnummerKommando(FunktionsNummer, 0);
		return;
	}

    try
    {
        m_pdb->Connect();		
		m_pdb->UpdateAdmiFunkKommando(FunktionsNummer, 0);
		m_pdb->Disconnect();
    }
    catch (SAException &x)
    {
        CString sText;
        sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
        AfxMessageBox(sText);
    }

}

bool CSpadData::KollaBegaranAvbrytKorningFinns(int FunktionsNummer)
{    
	int wantal;

	if (ge_DataMode == 2)
	{
		wantal = m_psf->RaknaAdmiFunktionsNummerKommando(FunktionsNummer, 6);
	}
	else
	{
		try
		{
			m_pdb->Connect();
			wantal = m_pdb->RaknaAdmiFunkKommando(FunktionsNummer, 6);
			m_pdb->Disconnect();
		}
		catch (SAException &x)
		{
			CString sText;
			sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
			AfxMessageBox(sText);
		}
	}

	if (wantal > 0)
		return true; 
	else 
		return false;
}

bool CSpadData::KollaBegaranAvbrytExekveringFinns(int FunktionsNummer)
{    
	int wantal;

	if (ge_DataMode == 2)
	{
		wantal = m_psf->RaknaAdmiFunktionsNummerKommando(FunktionsNummer, 99);
	}
	else
	{
		try
		{
			m_pdb->Connect();
			wantal = m_pdb->RaknaAdmiFunkKommando(FunktionsNummer, 99);
			m_pdb->Disconnect();


		}
		catch (SAException &x)
		{
			CString sText;
			sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
			AfxMessageBox(sText);
		}
	}

	if (wantal > 0)
		return true; 
	else 
		return false;
}

bool CSpadData::KollaBegaranPausaKorningFinns(int FunktionsNummer)
{    
	int wantal;

	if (ge_DataMode == 2)
	{
		wantal = m_psf->RaknaAdmiFunktionsNummerKommando(FunktionsNummer, 16);
	}
	else
	{
		try
		{
			m_pdb->Connect();
			wantal = m_pdb->RaknaAdmiFunkKommando(FunktionsNummer, 16);
			m_pdb->Disconnect();
		}
		catch (SAException &x)
		{
			CString sText;
			sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
			AfxMessageBox(sText);
		}
	}

	if (wantal > 0)
		return true; 
	else 
		return false;
}

void CSpadData::NollstallBegaran(int FunktionsNummer)
{

	if (ge_DataMode == 2)
	{
		m_psf->UpdateraAdmiFunktionsnummerKommando(FunktionsNummer, 0);
		return;
	}

    try
    {
        m_pdb->Connect();		
		m_pdb->UpdateAdmiFunkKommando(FunktionsNummer, 0);
		m_pdb->Disconnect();

    }
    catch (SAException &x)
    {
        CString sText;
        sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
        AfxMessageBox(sText);
    }

}

void CSpadData::UpdateAdmiAdmiIDSniffVarvRappParam(int AdmiID, int SniffIntervallSekunder, int KorVarvSekunder, int AliveRapp, int Param1, int Param2, int Param3)
{
	if (ge_DataMode == 2)
	{
		m_psf->UpdateAdmiAdmiIDSniffVarvRappParam(AdmiID, SniffIntervallSekunder, KorVarvSekunder, AliveRapp, Param1, Param2, Param3);
		return;
	}

    try
    {
        m_pdb->Connect();		
		m_pdb->UpdateAdmiAdmiIDSniffVarvRappParam(AdmiID, SniffIntervallSekunder, KorVarvSekunder, AliveRapp, Param1, Param2, Param3);
		m_pdb->Disconnect();

    }
    catch (SAException &x)
    {
        CString sText;
        sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
        AfxMessageBox(sText);
    }

}

void CSpadData::InitAdmi(void)
{
	if (ge_DataMode == 2)
	{
		// avvakta
		return;
	}
    try
    {
        m_pdb->Connect();		
		m_pdb->InitAdmi();
		m_pdb->Disconnect();

    }
    catch (SAException &x)
    {
        CString sText;
        sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
        AfxMessageBox(sText);
    }
}

void CSpadData::SelectSpadAdmiAll(std::vector<CSpadData::typ_SpadAdmi> *av)
{
	if (ge_DataMode == 2)
	{
		m_psf->LasAdmiAlla(av);
		return;
	}

    try
    {
        m_pdb->Connect();		
		m_pdb->SelectSpadAdmiAll(av);
		m_pdb->Disconnect();

    }
    catch (SAException &x)
    {
        CString sText;
        sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
        AfxMessageBox(sText);
    }

}

void CSpadData::SelectSpadAdmiFunk(int FunktionsNummer, CSpadData::typ_SpadAdmi *sa)
{
	if (ge_DataMode == 2)
	{
		m_psf->LasAdmiFunk(FunktionsNummer, sa);
		return;
	}

    try
    {
        m_pdb->Connect();		
		m_pdb->SelectSpadAdmiFunk(FunktionsNummer, sa);
		m_pdb->Disconnect();

    }
    catch (SAException &x)
    {
        CString sText;
        sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
        AfxMessageBox(sText);
    }

}

void CSpadData::HamtaDatumKlocka(int *Datum, int *Klock)
{
	CTime time;
	CString temp;
	time = CTime::GetCurrentTime();
	temp = time.Format("%y%m%d %H%M%S");
	*Datum = atoi(temp.Mid(0,6));
	*Klock = atoi(temp.Mid(7,6));
}

void CSpadData::InsertSpadBestHist(CSpadData::typ_SpadBest *sb)
{
	if (ge_DataMode == 2)
	{
		m_psf->SkrivNyBest(sb, 'h');
		return;
	}

	try
	{
		m_pdb->Connect();
		m_pdb->InsertSpadBestHist(sb);
		m_pdb->Disconnect();

	}
	catch (SAException &x)
	{
		CString sText;
		sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
		AfxMessageBox(sText);
	}

}

void CSpadData::SelectSpadBestHistAll(std::vector<CSpadData::typ_SpadBest> *bv)
{
	if (ge_DataMode == 2)
	{
		m_psf->LasBestAlla(bv, 'h');
		return;
	}
	try
	{
		m_pdb->Connect();
		m_pdb->SelectSpadBestHistAll(bv);
		m_pdb->Disconnect();

	}
	catch (SAException &x)
	{
		CString sText;
		sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
		AfxMessageBox(sText);
	}

}

int CSpadData::InsertMessArciFromBest()
{	
	int antal;

	if (ge_DataMode == 2)
	{
		antal = m_psf->FyllpaMessArciFranBest();
		return antal;
	}
	try
	{
		m_pdb->Connect();
		antal = m_pdb->InsertMessArciFromBest();
		m_pdb->Disconnect();

	}
	catch (SAException &x)
	{
		CString sText;
		sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
		AfxMessageBox(sText);
	}
	return antal;
}

void CSpadData::SelectSpadMessArciAll(std::vector<CSpadData::typ_SpadBest> *bv)
{
	if (ge_DataMode == 2)
	{
		m_psf->LasBestAlla(bv, 'm');
		return;
	}
	try
	{
		m_pdb->Connect();
		m_pdb->SelectSpadMessArciAll(bv);
		m_pdb->Disconnect();

	}
	catch (SAException &x)
	{
		CString sText;
		sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
		AfxMessageBox(sText);
	}

}

void CSpadData::SelectSpadMessArciBest(std::vector<CSpadData::typ_SpadBest> *bv, int BestNummer)
{
	if (ge_DataMode == 2)
	{
		m_psf->LasBestBest(bv, 'm', BestNummer);
		return;
	}
	try
	{
		m_pdb->Connect();
		m_pdb->SelectSpadMessArciBest(bv, BestNummer);
		m_pdb->Disconnect();

	}
	catch (SAException &x)
	{
		CString sText;
		sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
		AfxMessageBox(sText);
	}

}

int  CSpadData::FixaNyttBestNummer()
{
	int wbestnummer = 0;

	if (ge_DataMode == 2)
	{
		wbestnummer = m_psf->FixaNyttBestNummer();
		return wbestnummer;
	}

	try
	{
		m_pdb->Connect();
		wbestnummer = m_pdb->FixaNyttBestNummer(1000);
		m_pdb->Disconnect();

	}
	catch (SAException &x)
	{
		CString sText;
		sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
		AfxMessageBox(sText);
	}
	return wbestnummer;
}

void CSpadData::DataStatus(int *BestAnt, int *BestHistAnt, int *MessArciAnt)
{
	if (ge_DataMode == 2)
	{
		m_psf->Datastatus(BestAnt, BestHistAnt, MessArciAnt);
		return;
	}

	try
	{
		m_pdb->Connect();
		m_pdb->DataStatus(BestAnt, BestHistAnt, MessArciAnt);
		m_pdb->Disconnect();

	}
	catch (SAException &x)
	{
		CString sText;
		sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
		AfxMessageBox(sText);
	}
}

void CSpadData::InsertSpadMessArci(CSpadData::typ_SpadBest *sb)
{
	if (ge_DataMode == 2)
	{
		m_psf->SkrivNyBest(sb, 'm');
		return;
	}

	try
	{
		m_pdb->Connect();
		m_pdb->InsertSpadMessArci(sb);
		m_pdb->Disconnect();

	}
	catch (SAException &x)
	{
		CString sText;
		sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
		AfxMessageBox(sText);
	}

}

void CSpadData::InsertSpadAdmi(CSpadData::typ_SpadAdmi *sa)
{
	if (ge_DataMode == 2)
	{
//		m_psf->SkrivNyAmi(sa);	// finns inte
		return;
	}

	try
	{
		m_pdb->Connect();
		m_pdb->InsertSpadAdmi(sa);
		m_pdb->Disconnect();
	}
	catch (SAException &x)
	{
		CString sText;
		sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
		AfxMessageBox(sText);
	}

}

void CSpadData::DeleteSpadAll()
{
	if (ge_DataMode == 2)
	{
		// finns inte
		return;
	}

	try
	{
		m_pdb->Connect();
		m_pdb->DeleteSpadAll();
		m_pdb->Disconnect();
	}
	catch (SAException &x)
	{
		CString sText;
		sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
		AfxMessageBox(sText);
	}

}

void CSpadData::SelectSpadRappAll(std::vector<CSpadData::typ_SpadRapp> *rv)
{
	if (ge_DataMode == 2)
	{
//		m_psf->LasRappAlla(rv, 'b');
		return;
	}

	try
	{
		m_pdb->Connect();
		m_pdb->SelectSpadRappAll(rv);
		m_pdb->Disconnect();

	}
	catch (SAException &x)
	{
		CString sText;
		sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
		AfxMessageBox(sText);
	}

}

void CSpadData::InsertSpadRappAll(std::vector<CSpadData::typ_SpadRapp> *rv)
{
	if (ge_DataMode == 2)
	{
		//		m_psf->SkrivRappAlla(rv, 'b');
		return;
	}

	try
	{
		m_pdb->Connect();
		m_pdb->InsertSpadRappAll(rv);
		m_pdb->Disconnect();
	}
	catch (SAException &x)
	{
		CString sText;
		sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
		AfxMessageBox(sText);
	}

}

void CSpadData::DeleteRappAll()
{
	if (ge_DataMode == 2)
	{
		// finns inte
		return;
	}

	try
	{
		m_pdb->Connect();
		m_pdb->DeleteRappAll();
		m_pdb->Disconnect();
	}
	catch (SAException &x)
	{
		CString sText;
		sText.Format("Det inträffade ett fel med databasen %s:\n\n%s", __FUNCTION__, x.ErrText());
		AfxMessageBox(sText);
	}

}
