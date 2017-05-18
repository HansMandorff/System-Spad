//
//		2016-02-06	Lagt upp denna klass och testkopplat in i spaddata	
//

#include "stdafx.h"
#include "SpadFile.h"
#include "share.h"

extern int     ge_DataMode; // finns i spaddlg.cpp
extern CString ge_Path;

CSpadFile::CSpadFile() // konstruktor
{
	m_safullname = ge_Path + "SpadAdmi.bin";
	m_sbfullname = ge_Path + "SpadBest.bin";
	m_shfullname = ge_Path + "SpadBestHist.bin";
	m_smfullname = ge_Path + "SpadMessArci.bin";
	m_stfullname = ge_Path + "SpadTemp.bin";
}

CSpadFile::~CSpadFile() // destruktor
{
}

void CSpadFile::Felsignalera(int varnagonstans)
{
	CString temp;
	temp.Format("Fel i filhanteringen i SpadFile: situation %d", varnagonstans);
	AfxMessageBox(temp);
	return;
}

void CSpadFile::LasAdmiFunk(int FunktionsNummer, CSpadData::typ_SpadAdmi *sa)
{
	int retur;
	int var = 0;
	CSpadData::typ_SpadAdmi wsa;
	memset(&wsa, 0, sizeof(wsa));
	wsa.Kommando = 999; // fel
	wsa.SniffIntervallSekunder = 5;

	m_spadadmi = _fsopen(m_safullname, "rb", _SH_DENYNO);
	for (int wfelant = 1; m_spadadmi == NULL && wfelant <= 9; wfelant++)
	{
		Sleep(300);
		m_spadadmi = _fsopen(m_safullname, "rb", _SH_DENYNO);
	}

	if (m_spadadmi == NULL)
	{
		var = 101;
		Felsignalera(var);
		*sa = wsa;
		return;
	}

	retur = fread(&wsa, sizeof(wsa), 1, m_spadadmi);
	while (retur == 1 && wsa.FunktionsNummer != FunktionsNummer)
		retur = fread(&wsa, sizeof(wsa), 1, m_spadadmi);
	if (retur != 1)		
	{
		var = 102;
		Felsignalera(var);
		*sa = wsa;
		return;
	}

	*sa = wsa;
	retur = fclose(m_spadadmi);

	return;
}

void CSpadFile::SkrivNyBest(CSpadData::typ_SpadBest *sb, char filecode)
{
	// späder på i slutet
	int var = 0;
	int retur;
	CString filefullname;

	if (filecode == 'b') filefullname = m_sbfullname;
	if (filecode == 'h') filefullname = m_shfullname;
	if (filecode == 'm') filefullname = m_smfullname;

	m_spadbest = _fsopen(filefullname, "r+b", _SH_DENYNO);
	// kanske lägga in upprepade försök med liten sleep emellan
	if (m_spadbest == NULL)
	{
		var = 105;
		Felsignalera(var);
	}

	// 2016-02-23 En bättre lösning än nedan är nog att helt enkelt läsa igenom hela filen först 
	// och räkna antal poster och hela tiden spara bestid
	CSpadData::typ_SpadBest tempsb;
	int sparbestid = sb->BestID;								// spara undan befintligt bestid, finns ju tre filtyper b h m
	retur = fseek(m_spadbest, -(int)sizeof(tempsb), SEEK_END);	// vi ställer oss på sista postens början
	if (retur == 0)
	{
		retur = fread(&tempsb, sizeof(tempsb), 1, m_spadbest);	// läser sista posten
		sb->BestID = tempsb.BestID + 1;							// adderar upp automatiska unika id-numret för aktuell fil att skriva till
	}
	else // fseek gick inte bra, troligen är filen tom, kanske senare göra om detta, kansk genom en läsning se om filen är tom
	{
		sb->BestID = 1;
	}
	fseek(m_spadbest, 0, SEEK_END);							// redo för "append", krävs innan skrivning
	retur = fwrite(sb, sizeof(CSpadData::typ_SpadBest), 1, m_spadbest);
	if (retur != 1)
	{
		var = 106;
		Felsignalera(var);
	}

	if (sparbestid == 0 && filecode == 'b') // var det ett nyupplägg av en beställning så låt stå bestid, annars lägg tillbaka orginalet in
	{ }
	else
		sb->BestID = sparbestid;								// återställ bestid för beställningsfilen
	retur = fclose(m_spadbest);
}

void CSpadFile::SkapaBestMass(std::vector<CSpadData::typ_SpadBest> *bv, char filecode)
{
	// slår ut ev befintlig fil
	// används till att skapa de tre filerna av best-typ initialt från db
	int var = 0;
	int retur;
	unsigned int varv;
	CString filefullname = "Elvis";
	std::vector<CSpadData::typ_SpadBest>::pointer pbv; // enligt ms nät

	if (filecode == 'b') filefullname = m_sbfullname;
	if (filecode == 'h') filefullname = m_shfullname;
	if (filecode == 'm') filefullname = m_smfullname;

	m_spadbest = _fsopen(filefullname, "wb", _SH_DENYRW);
	if (m_spadbest == NULL)
	{
		var = 107;
		Felsignalera(var);
	}

	varv = 0;
	pbv = bv->data(); // enligt ms nät, INTE bra stega upp själva parameter-bv
	while (varv < bv->size())
	{
		(pbv + varv)->BestID = varv + 1; // börja från början för flat fil
		retur = fwrite( (pbv+varv), sizeof(CSpadData::typ_SpadBest), 1, m_spadbest); 
		if (retur != 1)
		{
			var = 108;
			Felsignalera(var);
		}
		varv++;
	}

	retur = fclose(m_spadbest);
}

void CSpadFile::LasBestAlla(std::vector<CSpadData::typ_SpadBest> *bv, char filecode)
{
	int var = 0;
	int retur;
	CSpadData::typ_SpadBest sb;

	CString filefullname = "Elvis";

	if (filecode == 'b') filefullname = m_sbfullname;
	if (filecode == 'h') filefullname = m_shfullname;
	if (filecode == 'm') filefullname = m_smfullname;

	m_spadbest = _fsopen(filefullname, "rb", _SH_DENYNO);
	if (m_spadbest == NULL)
	{
		var = 109;
		Felsignalera(var);
	}

	retur = fread(&sb, sizeof(sb), 1, m_spadbest);
	while (retur == 1)
	{
		bv->push_back(sb);
		retur = fread(&sb, sizeof(sb), 1, m_spadbest);
	}

	retur = fclose(m_spadbest);

}

void CSpadFile::LasBestBest(std::vector<CSpadData::typ_SpadBest> *bv, char filecode, int BestNummer)
{
	int var = 0;
	int retur;
	CSpadData::typ_SpadBest sb;

	CString filefullname = "Elvis";

	if (filecode == 'b') filefullname = m_sbfullname;
	if (filecode == 'h') filefullname = m_shfullname;
	if (filecode == 'm') filefullname = m_smfullname;

	m_spadbest = _fsopen(filefullname, "rb", _SH_DENYNO);
	if (m_spadbest == NULL)
	{
		var = 140;
		Felsignalera(var);
	}

	retur = fread(&sb, sizeof(sb), 1, m_spadbest);
	while (retur == 1)
	{
		if (sb.BestNummer == BestNummer)
			bv->push_back(sb);
		retur = fread(&sb, sizeof(sb), 1, m_spadbest);
	}

	retur = fclose(m_spadbest);

}

void CSpadFile::UpdateraSpadBestBestIDReturkod(int BestID, int Returkod)
{
	int var = 0;
	int retur;
	CSpadData::typ_SpadBest sb;
	fpos_t pos;

	m_spadbest = _fsopen(m_sbfullname, "r+b", _SH_DENYNO);
	if (m_spadbest == NULL)
	{
		var = 112;
		Felsignalera(var);
	}

	// Först läsa från början och söka efter bestid 
	fgetpos(m_spadbest, &pos);
	retur = fread(&sb, sizeof(sb), 1, m_spadbest);
	while (retur == 1 && BestID != sb.BestID)
	{
		fgetpos(m_spadbest, &pos); // verkar som att fread stegar fram pos till nästa läsning
		retur = fread(&sb, sizeof(sb), 1, m_spadbest);
	}

	if (retur != 1)
	{
		var = 113;
		Felsignalera(var);
		return;
	}

	// pos innehåller positionen för senast inläst post, den med sökt BestID, INTE nästa pos i tur
	sb.ReturKod = Returkod;
	fsetpos(m_spadbest, &pos);
	retur = fwrite((&sb), sizeof(CSpadData::typ_SpadBest), 1, m_spadbest);

	fclose(m_spadbest);

}

void CSpadFile::SkapaAdmiMass(std::vector<CSpadData::typ_SpadAdmi> *av)
{
	// slår ut ev befintlig fil
	// används till att skapa admi initialt från db
	int var = 0;
	int retur;
	int widnr;

	m_spadadmi = _fsopen(m_safullname, "wb", _SH_DENYRW);
	if (m_spadadmi == NULL)
	{
		var = 110;
		Felsignalera(var);
	}

	std::vector<CSpadData::typ_SpadAdmi>::iterator iav; // se msdn nät för att fatta iterator
	CSpadData::typ_SpadAdmi wsa;
	widnr = 0;
	for (iav = av->begin(); iav != av->end(); iav++) // elegant lösning, min favorit just nu
	{
		wsa = *iav; // tydligare så här att mellanlagra i wsa
		widnr++;
		wsa.AdmiID = widnr; // börja från början i filerna
		retur = fwrite(&wsa, sizeof(CSpadData::typ_SpadAdmi), 1, m_spadadmi);
		if (retur != 1)
		{
			var = 111;
			Felsignalera(var);
			return;
		}
	}

	retur = fclose(m_spadadmi);

	std::vector<CSpadData::typ_SpadAdmi> wav;
	wav.clear(); // kolla att resultatet ser bra ut nu så här i test
	LasAdmiAlla(&wav);
	return;

	/*
	// experiment
	int x;
	std::vector<CSpadData::typ_SpadAdmi>::iterator iav; // enligt msdn nät
	iav = av->begin();
	x = iav->FunktionsNummer; // iteratorn verkar vara en pekare
	iav = av->end();
	iav--; // verkar stå för värdet efter sista, värde för brytning av iterators uppstegning i loop utan att använda size()
	x = iav->FunktionsNummer; // iteratorn verkar vara en pekare
	CSpadData::typ_SpadAdmi sa;
	sa = (*av)[0]; // aha, så parantesar man
	std::vector<CSpadData::typ_SpadAdmi> wav;
	wav.push_back(*iav);
	iav = wav.begin();
	x = iav->FunktionsNummer; // pekare här också när vi har vektor "direkt"
	x = wav[0].FunktionsNummer;
	pav = wav.data();
	x = pav->FunktionsNummer;
	wav.push_back((*av)[0]);
	pav = wav.data();
	x = (pav + 0)->FunktionsNummer;
	x = (pav + 1)->FunktionsNummer;
	// slut experiment
	*/
// rester från experiment
//		retur = fwrite(&*(pav + varv), sizeof(CSpadData::typ_SpadAdmi), 1, m_spadadmi); // pav är en "riktig" pekare
//		retur = fwrite(pav + varv, sizeof(CSpadData::typ_SpadAdmi), 1, m_spadadmi); // samma som ovan
//		retur = fwrite( &((*av)[varv]), sizeof(CSpadData::typ_SpadAdmi), 1, m_spadadmi); // svårt att begripa
//		retur = fwrite( &*iav, sizeof(CSpadData::typ_SpadAdmi), 1, m_spadadmi); // svårt att begripa nu också, 
//		först pekare iav, sen data *iav, sen adress till data &*iav, bättre med &(*iav)
//		retur = fwrite(iav, sizeof(CSpadData::typ_SpadAdmi), 1, m_spadadmi); // går unte med iterator
//		varv++;
//		iav++;
//	varv = 0;
//	pav = av->data(); // enligt ms nät, INTE bra stega upp själva parameter-av, joho (*av)[varv]
//	iav = av->begin();
//	while (iav != av->end())
//	while (varv < av->size())
//	unsigned int varv;
//	std::vector<CSpadData::typ_SpadAdmi>::pointer pav; // enligt msdn nät

}

int CSpadFile::FixaNyttBestNummer()
{
	// hittills högsta bestnummer ligger i param3 i admiposten för funk 1000

	int var = 0;
	int retur;
	CSpadData::typ_SpadAdmi sa;
	fpos_t pos;

	// läs in posten för 1000

	m_spadadmi = _fsopen(m_safullname, "r+b", _SH_DENYNO);
	if (m_spadadmi == NULL)
	{
		var = 114;
		Felsignalera(var);
	}

	// Först läsa från början och söka efter funktion 1000 
	fgetpos(m_spadadmi, &pos);
	retur = fread(&sa, sizeof(sa), 1, m_spadadmi);
	while (retur == 1 && sa.FunktionsNummer != 1000)
	{
		fgetpos(m_spadadmi, &pos); // verkar som att fread stegar fram pos till nästa läsning
		retur = fread(&sa, sizeof(sa), 1, m_spadadmi);
	}

	if (retur != 1)
	{
		var = 115;
		Felsignalera(var);
		return -1;
	}

	// pos innehåller positionen för senast inläst post, tagen innan läsning, INTE nästa pos i tur
	// addera upp param3 ett steg
	sa.Param3++;

	// skriv tillbaka 1000-post
	fsetpos(m_spadadmi, &pos);
	retur = fwrite((&sa), sizeof(CSpadData::typ_SpadAdmi), 1, m_spadadmi);

	fclose(m_spadadmi);

	// reurnera nytt högsta nummer
	return sa.Param3;
}

void CSpadFile::DeleteSpadBestNearlyAll()
{
	// görs genom att kopiera delar av inläst vektor till ny vektor och skapa best på nytt
	int retur;
	int var = 0;

	std::vector<CSpadData::typ_SpadBest> bvold;
	std::vector<CSpadData::typ_SpadBest>::iterator ibvold;
	std::vector<CSpadData::typ_SpadBest> bvnew;

	bvold.clear();
	LasBestAlla(&bvold, 'b');

	// tanken är att om nått går snett så får man återställa med hjälp av tempfilen
	retur = remove(m_stfullname);	// gammal temp
	Sleep(200); // ge remove lite tid? Har fått fel 117 en gång
	retur = rename(m_sbfullname, m_stfullname);	// temp
	if (retur != 0)
	{
		var = 117;
		Felsignalera(var);
		return;
	}

	bvnew.clear();
	ibvold = bvold.begin();
	while ( ibvold != bvold.end() )	// iterator-varianten
	{
		if (ibvold->Kommando == 1 && ibvold->ReturKod == 0) // behåll väntande beställningar
			bvnew.push_back(*ibvold);
		ibvold++;
	}

	SkapaBestMass(&bvnew, 'b');

	return;
}

void CSpadFile::LasAdmiAlla(std::vector<CSpadData::typ_SpadAdmi> *av)
{
	int var = 0;
	int retur;
	CSpadData::typ_SpadAdmi sa;

	m_spadadmi = _fsopen(m_safullname, "rb", _SH_DENYNO);
	if (m_spadadmi == NULL)
	{
		var = 116;
		Felsignalera(var);
	}

	retur = fread(&sa, sizeof(sa), 1, m_spadadmi);
	while (retur == 1)
	{
		av->push_back(sa);
		retur = fread(&sa, sizeof(sa), 1, m_spadadmi);
	}

	retur = fclose(m_spadadmi);

}

void CSpadFile::LasBestNya(std::vector<CSpadData::typ_SpadBest> *bv, int FunktionsNummer)
{
	// träff på funknummer, kommando=1, returkod=0

	int var = 0;
	int retur;
	CSpadData::typ_SpadBest sb;

	m_spadbest = _fsopen(m_sbfullname, "rb", _SH_DENYNO);
	if (m_spadbest == NULL)
	{
		var = 117;
		Felsignalera(var);
	}

	retur = fread(&sb, sizeof(sb), 1, m_spadbest);
	while (retur == 1)
	{
		if (sb.FunktionsNummer == FunktionsNummer &&
			sb.Kommando == 1 && sb.ReturKod == 0)
			bv->push_back(sb);
		retur = fread(&sb, sizeof(sb), 1, m_spadbest);
	}

	retur = fclose(m_spadbest);

}

void CSpadFile::UpdateraSpadBestBestNummerKommando(int BestNummer, int Kommando)
{
	int var = 0;
	int retur;
	CSpadData::typ_SpadBest sb;
	fpos_t pos;

	m_spadbest = _fsopen(m_sbfullname, "r+b", _SH_DENYNO);
	if (m_spadbest == NULL)
	{
		var = 118;
		Felsignalera(var);
	}

	// Först läsa från början och söka efter bestnummer 
	// Vi tar den först påträffade, om fler så är det buggit
	// Det borde jobbas enbart med unika BestID, inte bestnummer
	fgetpos(m_spadbest, &pos);
	retur = fread(&sb, sizeof(sb), 1, m_spadbest);
	while (retur == 1 && BestNummer != sb.BestNummer)
	{
		fgetpos(m_spadbest, &pos); // verkar som att fread stegar fram pos till nästa läsning
		retur = fread(&sb, sizeof(sb), 1, m_spadbest);
	}

	if (retur != 1)
	{
		var = 119;
		Felsignalera(var);
		return;
	}

	if (sb.Kommando == 1 && sb.ReturKod == 0) // har vi fåt rätt beställningspost? Annars bug
		{ }
	else
		{
			var = 120;
			Felsignalera(var);
			return;
		}

	// pos innehåller positionen för senast inläst post, den med sökt BestID, INTE nästa pos i tur
	sb.Kommando = Kommando;
	fsetpos(m_spadbest, &pos);
	retur = fwrite((&sb), sizeof(CSpadData::typ_SpadBest), 1, m_spadbest);

	fclose(m_spadbest);

}

int  CSpadFile::GeSpadBestReturkod(int BestNummer)
{
	int var = 0;
	int retur;
	CSpadData::typ_SpadBest sb;

	m_spadbest = _fsopen(m_sbfullname, "rb", _SH_DENYNO);
	if (m_spadbest == NULL)
	{
		var = 121;
		Felsignalera(var);
		return -1;
	}

	// Först läsa från början och söka efter bestnummer 
	// Vi tar den först påträffade, om fler så är det buggit
	// Det borde jobbas enbart med unika BestID, inte bestnummer

	retur = fread(&sb, sizeof(sb), 1, m_spadbest);
	while (retur == 1 && BestNummer != sb.BestNummer)
	{
		retur = fread(&sb, sizeof(sb), 1, m_spadbest);
	}

	if (retur != 1)
	{
		var = 122;
		Felsignalera(var);
		return -1;
	}

	if (sb.Kommando == 1) // har vi fåt rätt beställningspost? Annars bug
	{
	}
	else
	{
		var = 123;
		Felsignalera(var);
		return -1;
	}
	return sb.ReturKod;
}

void CSpadFile::UpdateraAdmiFunktionsnummerKommando(int FunktionsNummer, int Kommando)
{
	int var = 0;
	int retur;
	CSpadData::typ_SpadAdmi sa;
	fpos_t pos;

	m_spadadmi = _fsopen(m_safullname, "r+b", _SH_DENYNO);
	if (m_spadadmi == NULL)
	{
		var = 124;
		Felsignalera(var);
		return;
	}

	// Först läsa från början och söka efter funktionsnummer
	// Vi tar den först påträffade, om fler så är det buggit
	// Ska finnas en och endast en post per funktion

	fgetpos(m_spadadmi, &pos);
	retur = fread(&sa, sizeof(sa), 1, m_spadadmi);
	while (retur == 1 && FunktionsNummer != sa.FunktionsNummer)
	{
		fgetpos(m_spadadmi, &pos); // verkar som att fread stegar fram pos till nästa läsning
		retur = fread(&sa, sizeof(sa), 1, m_spadadmi);
	}

	if (retur != 1)
	{
		var = 125;
		Felsignalera(var);
		return;
	}

	// Då måtte vi ha fått tag i posten med sökt funktionsnummer
	// pos innehåller positionen för senast inläst post, den med sökt BestID, INTE nästa pos i tur

	sa.Kommando = Kommando;
	fsetpos(m_spadadmi, &pos);
	retur = fwrite((&sa), sizeof(CSpadData::typ_SpadAdmi), 1, m_spadadmi);

	fclose(m_spadadmi);
}

int CSpadFile::RaknaAdmiFunktionsNummerKommando(int FunktionsNummer, int Kommando)
{
	std::vector<CSpadData::typ_SpadAdmi> av;
	std::vector<CSpadData::typ_SpadAdmi>::iterator iav;
	int wantal = 0;

	LasAdmiAlla(&av);

	for (iav = av.begin(); iav != av.end(); iav++)
		if (iav->FunktionsNummer == FunktionsNummer && iav->Kommando == Kommando)
			wantal++;
	
	return wantal;
}

void CSpadFile::UpdateAdmiAdmiIDSniffVarvRappParam(int AdmiID, int SniffIntervallSekunder, int KorVarvSekunder,
	int AliveRapp, int Param1, int Param2, int Param3)
{
	int var = 0;
	int retur;
	CSpadData::typ_SpadAdmi sa;
	fpos_t pos;

	m_spadadmi = _fsopen(m_safullname, "r+b", _SH_DENYNO);
	if (m_spadadmi == NULL)
	{
		var = 126;
		Felsignalera(var);
		return;
	}

	// Först läsa från början och söka efter funktionsnummer
	// Vi tar den först påträffade, om fler så är det buggit
	// Ska finnas en och endast en post per funktion

	fgetpos(m_spadadmi, &pos);
	retur = fread(&sa, sizeof(sa), 1, m_spadadmi);
	while (retur == 1 && AdmiID != sa.AdmiID)
	{
		fgetpos(m_spadadmi, &pos); // verkar som att fread efteråt stegar fram pos till nästa läsning
		retur = fread(&sa, sizeof(sa), 1, m_spadadmi);
	}

	if (retur != 1)
	{
		var = 127;
		Felsignalera(var);
		return;
	}

	// Då måtte vi ha fått tag i posten med sökt funktionsnummer
	// Bäst att vi kollar

	if (sa.AdmiID != AdmiID)
	{
		var = 128;
		Felsignalera(var);
		return;
	}

	// Vi uppdaterar

	sa.SniffIntervallSekunder	= SniffIntervallSekunder;
	sa.KorVarvSekunder			= KorVarvSekunder;
	sa.AliveRapp				= AliveRapp;
	sa.Param1					= Param1;
	sa.Param2					= Param2;
	sa.Param3					= Param3;

	// pos innehåller positionen för senast inläst post, den med sökt BestID, INTE nästa pos i tur

	fsetpos(m_spadadmi, &pos);
	retur = fwrite((&sa), sizeof(CSpadData::typ_SpadAdmi), 1, m_spadadmi);
	if (retur != 1)
	{
		var = 129;
		Felsignalera(var);
		return;
	}

	fclose(m_spadadmi);

}

int  CSpadFile::FyllpaMessArciFranBest()
{
	// För över nästan allt från beställninarna till messarkivet
	int var;
	int retur;
	int antal = 0;
	int wbestidsenast;

	m_spadbest = _fsopen(m_sbfullname, "rb", _SH_DENYNO);
	if (m_spadbest == NULL)
	{
		var = 130;
		Felsignalera(var);
		return -1;
	}
	m_spadmessarci = _fsopen(m_smfullname, "r+b", _SH_DENYNO);
	if (m_spadmessarci == NULL)
	{
		var = 131;
		Felsignalera(var);
		return -1;
	}

	CSpadData::typ_SpadBest sm;

	retur = fseek(m_spadmessarci, -(int)sizeof(sm), SEEK_END);		// vi ställer oss på sista postens början
	if (retur != 0) // trolien tom fil
		wbestidsenast = 0;
	else
	{
		retur = fread(&sm, sizeof(sm), 1, m_spadmessarci);		// läser sista posten
		wbestidsenast = sm.BestID;								// sparar hittills högsta BestID i messarci-filen
	}
	
	fseek(m_spadmessarci, 0, SEEK_END);						// redo för "append", krävs innan skrivning

	retur = fread(&sm, sizeof(sm), 1, m_spadbest);
	while (retur == 1)
	{
		if (sm.Kommando == 1 && sm.ReturKod == 0)
		{ }
		else
		{
			wbestidsenast++;
			sm.BestID = wbestidsenast;
			retur = fwrite(&sm, sizeof(sm), 1, m_spadmessarci);
			antal++;
			if (retur != 1)
			{
				var = 132;
				Felsignalera(var);
			}
		}
		retur = fread(&sm, sizeof(sm), 1, m_spadbest);
	}

	fclose(m_spadbest);
	fclose(m_spadmessarci);

	return antal;
}

void CSpadFile::Datastatus(int *BestAnt, int *BestHistAnt, int *MessArciAnt)
{
	int var = 0;
	int retur;

	*BestAnt = 0;
	*BestHistAnt = 0;
	*MessArciAnt = 0;

	CSpadData::typ_SpadBest sb;

	// best
	m_spadbest = _fsopen(m_sbfullname, "rb", _SH_DENYNO);
	if (m_spadbest == NULL)
	{
		var = 133;
		Felsignalera(var);
		return;
	}
	retur = fread(&sb, sizeof(sb), 1, m_spadbest);
	while (retur == 1)
	{
		(*BestAnt)++;
		retur = fread(&sb, sizeof(sb), 1, m_spadbest);
	}
	fclose(m_spadbest);

	// hist
	m_spadbest = _fsopen(m_shfullname, "rb", _SH_DENYNO);
	if (m_spadbest == NULL)
	{
		var = 134;
		Felsignalera(var);
		return;
	}
	retur = fread(&sb, sizeof(sb), 1, m_spadbest);
	while (retur == 1)
	{
		(*BestHistAnt)++;
		retur = fread(&sb, sizeof(sb), 1, m_spadbest);
	}
	fclose(m_spadbest);

	// mess
	m_spadbest = _fsopen(m_smfullname, "rb", _SH_DENYNO);
	if (m_spadbest == NULL)
	{
		var = 135;
		Felsignalera(var);
		return;
	}
	retur = fread(&sb, sizeof(sb), 1, m_spadbest);
	while (retur == 1)
	{
		(*MessArciAnt)++;
		retur = fread(&sb, sizeof(sb), 1, m_spadbest);
	}
	fclose(m_spadbest);
}

void CSpadFile::InitAdmi()
{
	int var = 0;
	int retur;
	CSpadData::typ_SpadAdmi sa;

// den bästa versionen finns för databasen i dbwco.cpp

	m_spadadmi = _fsopen(m_safullname, "wb", _SH_DENYRW);
	if (m_spadadmi == NULL)
	{
		var = 136;
		Felsignalera(var);
		return;
	}

	ZeroMemory(&sa, sizeof(sa));
	int varv = 0;
	sa.FunktionsNummer = 1000;
	sa.AliveRapp = 1;
	sa.KorVarvSekunder = 10;
	sa.SniffIntervallSekunder = 5;
	
	retur = fwrite(&sa, sizeof(sa), 1, m_spadadmi);
	while (retur == 1 && varv < 5)
	{
		sa.FunktionsNummer++;
		varv++;
		retur = fwrite(&sa, sizeof(sa), 1, m_spadadmi);
	}
	if (retur != 1)
	{
		var = 137;
		Felsignalera(var);
		return;
	}

	fclose(m_spadadmi);
}