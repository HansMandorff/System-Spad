#include "DBwco.h"
//#include "SpadData.h"
#include <vector>

DBwco::DBwco()
{
}

DBwco::DBwco(const CString& host, const CString& schema, const CString& username, const CString& password) 
    : _host(host), 
      _schema(schema), 
      _username(username), 
      _password(password),
      _connectionString(),
      _isLocalSqlServer(),
      _connection(new SAConnection())
{
    _buildConnectionString();
}

DBwco::~DBwco() 
{
}

void DBwco::SetConnectionInfo(const CString& host, const CString& schema, const CString& user, const CString& pass)
{
    _host = host;
    _schema = schema;
    _username = user;
    _password = pass;
    _buildConnectionString();
}

void DBwco::_buildConnectionString()
{
    ASSERT(!_host.IsEmpty());
    ASSERT(!_schema.IsEmpty());

    // specialhantering av lokala databasinstanser
    if (_host.CompareNoCase(_T("(local)")) == 0)
    {
        TCHAR szBuffer[MAX_COMPUTERNAME_LENGTH + 1];
        DWORD dwSize = MAX_COMPUTERNAME_LENGTH + 1;
        GetComputerName(szBuffer, &dwSize);
		if (memcmp(szBuffer, "DUST", 4) == 0)
			_host.Format(_T("%s"), szBuffer);	// bara datornamn här, för dustin-dator
		else
	        _host.Format(_T("%s\\SQLEXPRESS"), szBuffer);

        _connectionString.Format(_T("%s@%s"), _host, _schema);
        _isLocalSqlServer= true;
    }
    else
    {
        _connectionString.Format(_T("%s@%s"), _host, _schema);
        _isLocalSqlServer = false;
    }
}

void DBwco::Connect()
{
    ASSERT(!_connectionString.IsEmpty());

    if (_isLocalSqlServer)
    {
        _connection->Connect(
            static_cast<LPCSTR>(_connectionString), 
            _T(""), 
            _T(""), 
            SA_SQLServer_Client); // om användarnamnet är en tom sträng använder SQLAPI++ automatiskt Windows Authentication
    }
    else
    {
        _connection->Connect(
            static_cast<LPCSTR>(_connectionString), 
            static_cast<LPCSTR>(_username), 
            static_cast<LPCSTR>(_password), 
            SA_SQLServer_Client);
    }
}

void DBwco::Disconnect()
{
    _connection->Disconnect();
}

bool DBwco::IsConnected() const
{
    return _connection->isConnected();
}

bool DBwco::IsAlive() const
{
    return _connection->isAlive();
}

// här kommer nytt för spad, så småningom ta bort mycket av det befintligt

void DBwco::DeleteSpadBestAll(void)
{
    _connection->setAutoCommit(SA_AutoCommitOn);
    SACommand cmd;
    cmd.setConnection(_connection.get());

    cmd.setCommandText(_T(
        " DELETE   FROM [Best] "
		" WHERE NOT (Kommando = 1 AND Returkod = 0) "
    ));

    cmd.Execute();
    cmd.Close();
}

void DBwco::InsertSpadBest(CSpadData::typ_SpadBest *sb)
{
    _connection->setAutoCommit(SA_AutoCommitOn);
    SACommand cmd;

    cmd.setConnection(_connection.get());

    cmd.setCommandText(_T(
        " INSERT INTO [Best]   "
        " (                               "
        " FunktionsNummer,                " 
        " Kommando,                       "
        " ReturKod,                       "
        " SenasteDatum,                   "  
        " SenasteKlock,                   "  
        " BestNummer,                     "
		" BestDatum,                      "
		" BestKlock,                      "
		" BestTitel,                      "
		" BestAntal,                      "
		" KorstartDatum,                  "
		" KorstartKlock,                  "
        " ProgressNummer,                 "
		" ProgressDatum,                  "
		" ProgressKlock,                  "
		" KorslutDatum,                   "
		" KorslutKlock,                   "
		" KorPostAntal,                   "
		" ResultatBelopp1,                "
		" ResultatBelopp2,                "
		" ResultatBelopp3                 "
        " )                               "
		" VALUES ( :1,:2,:3,:4,:5,:6,:7,  "
		" :8,:9,:10,:11,:12,:13,:14,:15,  "
		" :16,:17,:18,:19,:20,:21      )  "
        ));

    cmd.Param(1).setAsLong()      = sb->FunktionsNummer;
    cmd.Param(2).setAsLong()      = sb->Kommando;
    cmd.Param(3).setAsLong()      = sb->ReturKod;
	cmd.Param(4).setAsLong()      = sb->SenastDatum;
	cmd.Param(5).setAsLong()      = sb->SenastKlock;

    cmd.Param(6).setAsLong()      = sb->BestNummer;
	cmd.Param(7).setAsLong()      = sb->BestDatum;
	cmd.Param(8).setAsLong()      = sb->BestKlock;
	cmd.Param(9).setAsString()    = sb->BestTitel;
	cmd.Param(10).setAsLong()     = sb->BestAntal;

	cmd.Param(11).setAsLong()     = sb->KorstartDatum; 
    cmd.Param(12).setAsLong()     = sb->KorstartKlock;
    cmd.Param(13).setAsLong()     = sb->ProgressNummer;
	cmd.Param(14).setAsLong()     = sb->ProgressDatum;
    cmd.Param(15).setAsLong()     = sb->ProgressKlock;

	cmd.Param(16).setAsLong()     = sb->KorslutDatum;
    cmd.Param(17).setAsLong()     = sb->KorslutKlock;
	cmd.Param(18).setAsLong()     = sb->KorPostAntal;
	cmd.Param(19).setAsLong()     = sb->ResultatBelopp1;
	cmd.Param(20).setAsLong()     = sb->ResultatBelopp2;
	cmd.Param(21).setAsLong()     = sb->ResultatBelopp3;
   
    cmd.Execute();

    cmd.Close();
    return;
}

void DBwco::SelectSpadBestAll(std::vector<CSpadData::typ_SpadBest> *bv)
{
	_connection->setAutoCommit(SA_AutoCommitOn);
    SACommand cmd;

    cmd.setConnection(_connection.get());

    cmd.setCommandText(_T(
        " SELECT                          "
		" BestID,                         "
        " FunktionsNummer,                " 
        " Kommando,                       "
        " ReturKod,                       "
        " SenasteDatum,                   "  
        " SenasteKlock,                   "  
        " BestNummer,                     "
		" BestDatum,                      "
		" BestKlock,                      "
		" BestTitel,                      "
		" BestAntal,                      "
		" KorstartDatum,                  "
		" KorstartKlock,                  "
        " ProgressNummer,                 "
		" ProgressDatum,                  "
		" ProgressKlock,                  "
		" KorslutDatum,                   "
		" KorslutKlock,                   "
		" KorPostAntal,                   "
		" ResultatBelopp1,                "
		" ResultatBelopp2,                "
		" ResultatBelopp3                 "
		" FROM Best                       "
		" ORDER BY BestID     ASC         "
        ));

	cmd.Execute();

	int wantal = 0;
	CSpadData::typ_SpadBest wsb;
	
    bool wexist = cmd.FetchNext();
    while(wexist)
    {
		wantal++;
		memset(&wsb, 0, sizeof(CSpadData::typ_SpadBest));

		wsb.BestID              = cmd.Field(1).asLong();
		wsb.FunktionsNummer     = cmd.Field(2).asLong();
        wsb.Kommando            = cmd.Field(3).asLong();
        wsb.ReturKod            = cmd.Field(4).asLong();
		wsb.SenastDatum         = cmd.Field(5).asLong();

		wsb.SenastKlock         = cmd.Field(6).asLong();
        wsb.BestNummer          = cmd.Field(7).asLong();
		wsb.BestDatum           = cmd.Field(8).asLong();
		wsb.BestKlock           = cmd.Field(9).asLong();
		strcpy_s(wsb.BestTitel,   cmd.Field(10).asString());

        wsb.BestAntal           = cmd.Field(11).asLong();
		wsb.KorstartDatum       = cmd.Field(12).asLong();
		wsb.KorstartKlock       = cmd.Field(13).asLong();
        wsb.ProgressNummer      = cmd.Field(14).asLong();
		wsb.ProgressDatum       = cmd.Field(15).asLong();

		wsb.ProgressKlock       = cmd.Field(16).asLong();
		wsb.KorslutDatum        = cmd.Field(17).asLong();
		wsb.KorslutKlock        = cmd.Field(18).asLong();
		wsb.KorPostAntal        = cmd.Field(19).asLong();
		wsb.ResultatBelopp1     = cmd.Field(20).asLong();

		wsb.ResultatBelopp2     = cmd.Field(21).asLong();
		wsb.ResultatBelopp3     = cmd.Field(22).asLong();

		bv->push_back(wsb);
		wexist = cmd.FetchNext();
    }
	
    cmd.Close();
    return;
}

void DBwco::SelectSpadBestNya(std::vector<CSpadData::typ_SpadBest> *bv, int FunktionsNummer)
{
	_connection->setAutoCommit(SA_AutoCommitOn);
    SACommand cmd;

    cmd.setConnection(_connection.get());

    cmd.setCommandText(_T(
        " SELECT                          "
		" BestID,                         "
        " FunktionsNummer,                " 
        " Kommando,                       "
        " ReturKod,                       "
        " SenasteDatum,                   "  
        " SenasteKlock,                   "  
        " BestNummer,                     "
		" BestDatum,                      "
		" BestKlock,                      "
		" BestTitel,                      "
		" BestAntal,                      "
		" KorstartDatum,                  "
		" KorstartKlock,                  "
        " ProgressNummer,                 "
		" ProgressDatum,                  "
		" ProgressKlock,                  "
		" KorslutDatum,                   "
		" KorslutKlock,                   "
		" KorPostAntal,                   "
		" ResultatBelopp1,                "
		" ResultatBelopp2,                "
		" ResultatBelopp3                 "
		" FROM Best                       "
		" WHERE FunktionsNummer = :23     "
		" AND   Kommando = 1              "
		" AND   Returkod = 0              "
		" ORDER BY BestID                 "
        ));

    cmd.Param(23).setAsLong() = FunktionsNummer;
//    cmd.Param(24).setAsLong() = Kommando;

	cmd.Execute();

	int wantal = 0;
	CSpadData::typ_SpadBest wsb;
	
    bool wexist = cmd.FetchNext();
    while(wexist)
    {
		wantal++;
		memset(&wsb, 0, sizeof(CSpadData::typ_SpadBest));

		wsb.BestID              = cmd.Field(1).asLong();
		wsb.FunktionsNummer     = cmd.Field(2).asLong();
        wsb.Kommando            = cmd.Field(3).asLong();
        wsb.ReturKod            = cmd.Field(4).asLong();
		wsb.SenastDatum         = cmd.Field(5).asLong();

		wsb.SenastKlock         = cmd.Field(6).asLong();
        wsb.BestNummer          = cmd.Field(7).asLong();
		wsb.BestDatum           = cmd.Field(8).asLong();
		wsb.BestKlock           = cmd.Field(9).asLong();
		strcpy_s(wsb.BestTitel,   cmd.Field(10).asString());

        wsb.BestAntal           = cmd.Field(11).asLong();
		wsb.KorstartDatum       = cmd.Field(12).asLong();
		wsb.KorstartKlock       = cmd.Field(13).asLong();
        wsb.ProgressNummer      = cmd.Field(14).asLong();
		wsb.ProgressDatum       = cmd.Field(15).asLong();

		wsb.ProgressKlock       = cmd.Field(16).asLong();
		wsb.KorslutDatum        = cmd.Field(17).asLong();
		wsb.KorslutKlock        = cmd.Field(18).asLong();
		wsb.KorPostAntal        = cmd.Field(19).asLong();
		wsb.ResultatBelopp1     = cmd.Field(20).asLong();

		wsb.ResultatBelopp2     = cmd.Field(21).asLong();
		wsb.ResultatBelopp3     = cmd.Field(22).asLong();

		bv->push_back(wsb);
		wexist = cmd.FetchNext();
    }
	
    cmd.Close();
    return;
}

void DBwco::UpdateraSpadBestBestIDReturkod(int BestID, int Returkod)
{
	_connection->setAutoCommit(SA_AutoCommitOn);
    SACommand cmd;

    cmd.setConnection(_connection.get());

    cmd.setCommandText(_T(
        " UPDATE Best                     "
		" SET Returkod = :2               "
		" WHERE BestID = :1               " 
		));

    cmd.Param(1).setAsLong() = BestID;
    cmd.Param(2).setAsLong() = Returkod;
	cmd.Execute();
	cmd.Close();
	return;
}

void DBwco::UpdateraSpadBestBestNummerKommando(int BestNummer, int Kommando)
{
	_connection->setAutoCommit(SA_AutoCommitOn);
	SACommand cmd;

	cmd.setConnection(_connection.get());

	cmd.setCommandText(_T(
		" UPDATE Best                     "
		" SET Kommando     = :2           "
		" WHERE BestNummer = :1           "
		));

	cmd.Param(1).setAsLong() = BestNummer;
	cmd.Param(2).setAsLong() = Kommando;
	cmd.Execute();
	cmd.Close();
	return;
}

int  DBwco::GeSpadBestReturkod(int BestNummer)
{
	_connection->setAutoCommit(SA_AutoCommitOn);
	SACommand cmd;
	cmd.setConnection(_connection.get());

	cmd.setCommandText(_T(
		" SELECT Returkod FROM [Best]    "
		" WHERE BestNummer = :1          "
		));

	cmd.Param(1).setAsLong() = BestNummer;
	cmd.Execute();
	int wreturkod = -1;
	if (cmd.FetchNext())
	{
		wreturkod = cmd.Field(1).asLong();
	}

	cmd.Close();
	return wreturkod;
}

void DBwco::UpdateAdmiFunkKommando(int FunktionsNummer, int Kommando)
{
	_connection->setAutoCommit(SA_AutoCommitOn);
    SACommand cmd;

    cmd.setConnection(_connection.get());

    cmd.setCommandText(_T(
        " UPDATE Admi                    "
		" SET    Kommando = :2           "
		" WHERE  FunktionsNummer = :1    "   
        ));

    cmd.Param(1).setAsLong() = FunktionsNummer;
    cmd.Param(2).setAsLong() = Kommando;

	cmd.Execute();
	cmd.Close();
	return;
}

void DBwco::UpdateAdmiAdmiIDSniffVarvRappParam(int AdmiID, int SniffIntervallSekunder, int KorVarvSekunder, int AliveRapp, int Param1, int Param2, int Param3)
{
	_connection->setAutoCommit(SA_AutoCommitOn);
    SACommand cmd;

    cmd.setConnection(_connection.get());

    cmd.setCommandText(_T(
        " UPDATE Admi                          "
		" SET    SniffIntervallSekunder = :2,  "
		"        KorVarvSekunder        = :3,  "
		"        AliveRapp              = :4,  "
		"        Param1                 = :5,  "
		"        Param2                 = :6,  "
		"        Param3                 = :7   "
		" WHERE  AdmiID                 = :1   "      
        ));

    cmd.Param(1).setAsLong() = AdmiID;
    cmd.Param(2).setAsLong() = SniffIntervallSekunder;
    cmd.Param(3).setAsLong() = KorVarvSekunder;
    cmd.Param(4).setAsLong() = AliveRapp;
    cmd.Param(5).setAsLong() = Param1;
    cmd.Param(6).setAsLong() = Param2;
    cmd.Param(7).setAsLong() = Param3;

	cmd.Execute();
	cmd.Close();
	return;
}


int DBwco::RaknaAdmiFunkKommando(int FunktionsNummer, int Kommando)
{
	_connection->setAutoCommit(SA_AutoCommitOn);
    SACommand cmd;

    cmd.setConnection(_connection.get());

    cmd.setCommandText(_T(
        " SELECT   COUNT(*)              "
        " FROM     Admi                  "
		" WHERE    FunktionsNummer = :1  "
		" AND      Kommando = :2         "
    ));
    cmd.Param(1).setAsLong() = FunktionsNummer;
    cmd.Param(2).setAsLong() = Kommando;
    cmd.Execute();
    cmd.FetchNext();
    int antal = cmd.Field(1).asLong();

	cmd.Close();
	return antal;
}

void DBwco::InitAdmi(void)
{
	_connection->setAutoCommit(SA_AutoCommitOn);
    SACommand cmd;

    cmd.setConnection(_connection.get());

    cmd.setCommandText(_T(
    " DELETE FROM Admi             "
    " INSERT INTO Admi ( FunktionsNummer,Kommando,SniffIntervallSekunder,KorVarvSekunder,AliveRapp,Param1,Param2,Param3)  VALUES (1000,0,2,1003,1,1,0,2100)   "
    " INSERT INTO Admi ( FunktionsNummer,Kommando,SniffIntervallSekunder,KorVarvSekunder,AliveRapp,Param1,Param2,Param3)  VALUES (1001,0,3,5,1,0,0,0)  "
    " INSERT INTO Admi ( FunktionsNummer,Kommando,SniffIntervallSekunder,KorVarvSekunder,AliveRapp,Param1,Param2,Param3)  VALUES (1002,0,3,5,1,0,0,0)  "
    " INSERT INTO Admi ( FunktionsNummer,Kommando,SniffIntervallSekunder,KorVarvSekunder,AliveRapp,Param1,Param2,Param3)  VALUES (1003,0,3,5,1,0,0,0)  "
    " INSERT INTO Admi ( FunktionsNummer,Kommando,SniffIntervallSekunder,KorVarvSekunder,AliveRapp,Param1,Param2,Param3)  VALUES (1004,0,3,5,1,0,0,0)  "
    " INSERT INTO Admi ( FunktionsNummer,Kommando,SniffIntervallSekunder,KorVarvSekunder,AliveRapp,Param1,Param2,Param3)  VALUES (1005,0,3,5,1,0,0,0)  "
	));

    cmd.Execute();

	cmd.Close();
}

void DBwco::SelectSpadAdmiAll(std::vector<CSpadData::typ_SpadAdmi> *av)
{
	_connection->setAutoCommit(SA_AutoCommitOn);
    SACommand cmd;

	CString stabell = " Select * from ADMI";

    cmd.setConnection(_connection.get());

    cmd.setCommandText(_T(
        " SELECT                          "
		" AdmiID,                         "
        " FunktionsNummer,                " 
        " Kommando,                       "
		" SniffIntervallSekunder,         "
		" KorVarvSekunder,                "
		" AliveRapp,                      "
		" Param1,                         "
		" Param2,                         "
		" Param3                          "
		" FROM Admi                       "
		" ORDER BY FunktionsNummer ASC    "
        ));

	cmd.Execute();

	int wantal = 0;
	CSpadData::typ_SpadAdmi wsa;
	
    bool wexist = cmd.FetchNext();
    while(wexist)
    {
		wantal++;
		memset(&wsa, 0, sizeof(CSpadData::typ_SpadAdmi));

		wsa.AdmiID                  = cmd.Field(1).asLong();
		wsa.FunktionsNummer         = cmd.Field(2).asLong();
        wsa.Kommando                = cmd.Field(3).asLong();
        wsa.SniffIntervallSekunder  = cmd.Field(4).asLong();
        wsa.KorVarvSekunder         = cmd.Field(5).asLong();
        wsa.AliveRapp               = cmd.Field(6).asLong();
        wsa.Param1                  = cmd.Field(7).asLong();
        wsa.Param2                  = cmd.Field(8).asLong();
        wsa.Param3                  = cmd.Field(9).asLong();

		av->push_back(wsa);
		wexist = cmd.FetchNext();
    }
	
    cmd.Close();
    return;
}

void DBwco::SelectSpadAdmiFunk(int FunktionsNummer, CSpadData::typ_SpadAdmi *sa)
{
	_connection->setAutoCommit(SA_AutoCommitOn);
    SACommand cmd;

    cmd.setConnection(_connection.get());

    cmd.setCommandText(_T(
        " SELECT                          "
		" AdmiID,                         "
        " FunktionsNummer,                " 
        " Kommando,                       "
		" SniffIntervallSekunder,         "
		" KorVarvSekunder,                "
		" AliveRapp,                      "
		" Param1,                         "
		" Param2,                         "
		" Param3                          "
		" FROM Admi                       "
		" WHERE FunktionsNummer = :1      "

        ));

    cmd.Param(1).setAsLong() = FunktionsNummer;
	cmd.Execute();

	CSpadData::typ_SpadAdmi wsa;
	memset(&wsa, 0, sizeof(wsa));
	
    bool wexist = cmd.FetchNext();
    if (wexist)
    {
		wsa.AdmiID                  = cmd.Field(1).asLong();
		wsa.FunktionsNummer         = cmd.Field(2).asLong();
        wsa.Kommando                = cmd.Field(3).asLong();
        wsa.SniffIntervallSekunder  = cmd.Field(4).asLong();
        wsa.KorVarvSekunder         = cmd.Field(5).asLong();
		wsa.AliveRapp               = cmd.Field(6).asLong();
        wsa.Param1                  = cmd.Field(7).asLong();
        wsa.Param2                  = cmd.Field(8).asLong();
        wsa.Param3                  = cmd.Field(9).asLong();

    }

	*sa = wsa;

    cmd.Close();
    return;
}

// satt till static så man kan använda den utan att skapa en instans av mmsDB
SADateTime DBwco::datum8_till_SADatetime(int datum8, int hhmmss)
{
    int y, m, d, hh, mm, ss;
    CString sDate, sDate2, sClock;
    sDate.Format("%d", datum8);
    y = atoi(sDate.Left(4));
    sDate2 = sDate.Right(4);
    m = atoi(sDate2.Left(2));
    d = atoi(sDate.Right(2));
	sClock.Format("%06d", hhmmss);
	hh = atoi(sClock.Left(2));
	mm = atoi(sClock.Mid(2,2));
	ss = atoi(sClock.Right(2));

    return SADateTime(y, m, d, hh, mm, ss);
}

// satt till static så man kan använda den utan att skapa en instans av mmsDB
int DBwco::SADatetime_till_datum8(SADateTime date)
{
    int y, m, d;
    
    y = date.GetYear();
    m = date.GetMonth();
    d = date.GetDay();
    CString temp;
    temp.Format("%04d%02d%02d", y, m, d);
    int datum8 = atoi(temp);
    return datum8;    
}

void DBwco::InsertSpadBestHist(CSpadData::typ_SpadBest *sb)
{
	_connection->setAutoCommit(SA_AutoCommitOn);
	SACommand cmd;

	cmd.setConnection(_connection.get());

	cmd.setCommandText(_T(
		" INSERT INTO [BestHist]   "
		" (                               "
		" FunktionsNummer,                "
		" Kommando,                       "
		" ReturKod,                       "
		" SenasteDatum,                   "
		" SenasteKlock,                   "
		" BestNummer,                     "
		" BestDatum,                      "
		" BestKlock,                      "
		" BestTitel,                      "
		" BestAntal,                      "
		" KorstartDatum,                  "
		" KorstartKlock,                  "
		" ProgressNummer,                 "
		" ProgressDatum,                  "
		" ProgressKlock,                  "
		" KorslutDatum,                   "
		" KorslutKlock,                   "
		" KorPostAntal,                   "
		" ResultatBelopp1,                "
		" ResultatBelopp2,                "
		" ResultatBelopp3                 "
		" )                               "
		" VALUES ( :1,:2,:3,:4,:5,:6,:7,  "
		" :8,:9,:10,:11,:12,:13,:14,:15,  "
		" :16,:17,:18,:19,:20,:21      )  "
		));

	cmd.Param(1).setAsLong() = sb->FunktionsNummer;
	cmd.Param(2).setAsLong() = sb->Kommando;
	cmd.Param(3).setAsLong() = sb->ReturKod;
	cmd.Param(4).setAsLong() = sb->SenastDatum;
	cmd.Param(5).setAsLong() = sb->SenastKlock;

	cmd.Param(6).setAsLong() = sb->BestNummer;
	cmd.Param(7).setAsLong() = sb->BestDatum;
	cmd.Param(8).setAsLong() = sb->BestKlock;
	cmd.Param(9).setAsString() = sb->BestTitel;
	cmd.Param(10).setAsLong() = sb->BestAntal;

	cmd.Param(11).setAsLong() = sb->KorstartDatum;
	cmd.Param(12).setAsLong() = sb->KorstartKlock;
	cmd.Param(13).setAsLong() = sb->ProgressNummer;
	cmd.Param(14).setAsLong() = sb->ProgressDatum;
	cmd.Param(15).setAsLong() = sb->ProgressKlock;

	cmd.Param(16).setAsLong() = sb->KorslutDatum;
	cmd.Param(17).setAsLong() = sb->KorslutKlock;
	cmd.Param(18).setAsLong() = sb->KorPostAntal;
	cmd.Param(19).setAsLong() = sb->ResultatBelopp1;
	cmd.Param(20).setAsLong() = sb->ResultatBelopp2;
	cmd.Param(21).setAsLong() = sb->ResultatBelopp3;

	cmd.Execute();

	cmd.Close();
	return;
}

void DBwco::SelectSpadBestHistAll(std::vector<CSpadData::typ_SpadBest> *bv)
{
	_connection->setAutoCommit(SA_AutoCommitOn);
	SACommand cmd;

	cmd.setConnection(_connection.get());

	cmd.setCommandText(_T(
		" SELECT                          "
		" BestHistID,                     "
		" FunktionsNummer,                "
		" Kommando,                       "
		" ReturKod,                       "
		" SenasteDatum,                   "
		" SenasteKlock,                   "
		" BestNummer,                     "
		" BestDatum,                      "
		" BestKlock,                      "
		" BestTitel,                      "
		" BestAntal,                      "
		" KorstartDatum,                  "
		" KorstartKlock,                  "
		" ProgressNummer,                 "
		" ProgressDatum,                  "
		" ProgressKlock,                  "
		" KorslutDatum,                   "
		" KorslutKlock,                   "
		" KorPostAntal,                   "
		" ResultatBelopp1,                "
		" ResultatBelopp2,                "
		" ResultatBelopp3                 "
		" FROM BestHist                   "
		" ORDER BY                        "
		"   FunktionsNummer,              "
		"   BestDatum,                    "
		"   BestKlock                     "
		));

	cmd.Execute();

	int wantal = 0;
	CSpadData::typ_SpadBest wsb;

	bool wexist = cmd.FetchNext();
	while (wexist)
	{
		wantal++;
		memset(&wsb, 0, sizeof(CSpadData::typ_SpadBest));

		wsb.BestID = cmd.Field(1).asLong();
		wsb.FunktionsNummer = cmd.Field(2).asLong();
		wsb.Kommando = cmd.Field(3).asLong();
		wsb.ReturKod = cmd.Field(4).asLong();
		wsb.SenastDatum = cmd.Field(5).asLong();

		wsb.SenastKlock = cmd.Field(6).asLong();
		wsb.BestNummer = cmd.Field(7).asLong();
		wsb.BestDatum = cmd.Field(8).asLong();
		wsb.BestKlock = cmd.Field(9).asLong();
		strcpy_s(wsb.BestTitel, cmd.Field(10).asString());

		wsb.BestAntal = cmd.Field(11).asLong();
		wsb.KorstartDatum = cmd.Field(12).asLong();
		wsb.KorstartKlock = cmd.Field(13).asLong();
		wsb.ProgressNummer = cmd.Field(14).asLong();
		wsb.ProgressDatum = cmd.Field(15).asLong();

		wsb.ProgressKlock = cmd.Field(16).asLong();
		wsb.KorslutDatum = cmd.Field(17).asLong();
		wsb.KorslutKlock = cmd.Field(18).asLong();
		wsb.KorPostAntal = cmd.Field(19).asLong();
		wsb.ResultatBelopp1 = cmd.Field(20).asLong();

		wsb.ResultatBelopp2 = cmd.Field(21).asLong();
		wsb.ResultatBelopp3 = cmd.Field(22).asLong();

		bv->push_back(wsb);
		wexist = cmd.FetchNext();
	}

	cmd.Close();
	return;
}

int DBwco::InsertMessArciFromBest()
{
	_connection->setAutoCommit(SA_AutoCommitOn);
	SACommand cmd;

	cmd.setConnection(_connection.get());

	cmd.setCommandText(_T(
		" INSERT INTO MessArci            "
		" (                               "
		" FunktionsNummer,                "
		" Kommando,                       "
		" ReturKod,                       "
		" SenasteDatum,                   "
		" SenasteKlock,                   "
		" BestNummer,                     "
		" BestDatum,                      "
		" BestKlock,                      "
		" BestTitel,                      "
		" BestAntal,                      "
		" KorstartDatum,                  "
		" KorstartKlock,                  "
		" ProgressNummer,                 "
		" ProgressDatum,                  "
		" ProgressKlock,                  "
		" KorslutDatum,                   "
		" KorslutKlock,                   "
		" KorPostAntal,                   "
		" ResultatBelopp1,                "
		" ResultatBelopp2,                "
		" ResultatBelopp3                 "
		" )                               "
		"                                 "
		" SELECT                          "
		" FunktionsNummer,                "
		" Kommando,                       "
		" ReturKod,                       "
		" SenasteDatum,                   "
		" SenasteKlock,                   "
		" BestNummer,                     "
		" BestDatum,                      "
		" BestKlock,                      "
		" BestTitel,                      "
		" BestAntal,                      "
		" KorstartDatum,                  "
		" KorstartKlock,                  "
		" ProgressNummer,                 "
		" ProgressDatum,                  "
		" ProgressKlock,                  "
		" KorslutDatum,                   "
		" KorslutKlock,                   "
		" KorPostAntal,                   "
		" ResultatBelopp1,                "
		" ResultatBelopp2,                "
		" ResultatBelopp3                 "
		" FROM Best                       "					// väntande körningar
		" WHERE NOT (Kommando = 1 AND Returkod = 0) "
		" SELECT COUNT (*) FROM MessArci            "
		));

	cmd.Execute();
	int antal = 0;
	bool wexist = cmd.FetchNext();
	if (wexist)
		antal = cmd.Field(1).asLong();

	cmd.Close();
	return antal;
}

void DBwco::SelectSpadMessArciAll(std::vector<CSpadData::typ_SpadBest> *bv)
{
	_connection->setAutoCommit(SA_AutoCommitOn);
	SACommand cmd;

	cmd.setConnection(_connection.get());

	cmd.setCommandText(_T(
		" SELECT                          "
		" MessArciID,                     "
		" FunktionsNummer,                "
		" Kommando,                       "
		" ReturKod,                       "
		" SenasteDatum,                   "
		" SenasteKlock,                   "
		" BestNummer,                     "
		" BestDatum,                      "
		" BestKlock,                      "
		" BestTitel,                      "
		" BestAntal,                      "
		" KorstartDatum,                  "
		" KorstartKlock,                  "
		" ProgressNummer,                 "
		" ProgressDatum,                  "
		" ProgressKlock,                  "
		" KorslutDatum,                   "
		" KorslutKlock,                   "
		" KorPostAntal,                   "
		" ResultatBelopp1,                "
		" ResultatBelopp2,                "
		" ResultatBelopp3                 "
		" FROM MessArci                   "
		" ORDER BY                        "
		"   MessArciID                    "

		));

	cmd.Execute();

	int wantal = 0;
	CSpadData::typ_SpadBest wsb;

	bool wexist = cmd.FetchNext();
	while (wexist)
	{
		wantal++;
		memset(&wsb, 0, sizeof(CSpadData::typ_SpadBest));

		wsb.BestID = cmd.Field(1).asLong();
		wsb.FunktionsNummer = cmd.Field(2).asLong();
		wsb.Kommando = cmd.Field(3).asLong();
		wsb.ReturKod = cmd.Field(4).asLong();
		wsb.SenastDatum = cmd.Field(5).asLong();

		wsb.SenastKlock = cmd.Field(6).asLong();
		wsb.BestNummer = cmd.Field(7).asLong();
		wsb.BestDatum = cmd.Field(8).asLong();
		wsb.BestKlock = cmd.Field(9).asLong();
		strcpy_s(wsb.BestTitel, cmd.Field(10).asString());

		wsb.BestAntal = cmd.Field(11).asLong();
		wsb.KorstartDatum = cmd.Field(12).asLong();
		wsb.KorstartKlock = cmd.Field(13).asLong();
		wsb.ProgressNummer = cmd.Field(14).asLong();
		wsb.ProgressDatum = cmd.Field(15).asLong();

		wsb.ProgressKlock = cmd.Field(16).asLong();
		wsb.KorslutDatum = cmd.Field(17).asLong();
		wsb.KorslutKlock = cmd.Field(18).asLong();
		wsb.KorPostAntal = cmd.Field(19).asLong();
		wsb.ResultatBelopp1 = cmd.Field(20).asLong();

		wsb.ResultatBelopp2 = cmd.Field(21).asLong();
		wsb.ResultatBelopp3 = cmd.Field(22).asLong();

		bv->push_back(wsb);
		wexist = cmd.FetchNext();
	}

	cmd.Close();
	return;
}

void DBwco::SelectSpadMessArciBest(std::vector<CSpadData::typ_SpadBest> *bv, int BestNummer)
{
	_connection->setAutoCommit(SA_AutoCommitOn);
	SACommand cmd;

	cmd.setConnection(_connection.get());

	cmd.setCommandText(_T(
		" SELECT                          "
		" MessArciID,                     "
		" FunktionsNummer,                "
		" Kommando,                       "
		" ReturKod,                       "
		" SenasteDatum,                   "
		" SenasteKlock,                   "
		" BestNummer,                     "
		" BestDatum,                      "
		" BestKlock,                      "
		" BestTitel,                      "
		" BestAntal,                      "
		" KorstartDatum,                  "
		" KorstartKlock,                  "
		" ProgressNummer,                 "
		" ProgressDatum,                  "
		" ProgressKlock,                  "
		" KorslutDatum,                   "
		" KorslutKlock,                   "
		" KorPostAntal,                   "
		" ResultatBelopp1,                "
		" ResultatBelopp2,                "
		" ResultatBelopp3                 "
		" FROM MessArci                   "
		" WHERE BestNummer = :1           "
		" ORDER BY                        "
		"   MessArciID                    "

		));

	cmd.Param(1).setAsLong() = BestNummer;
	cmd.Execute();

	int wantal = 0;
	CSpadData::typ_SpadBest wsb;

	bool wexist = cmd.FetchNext();
	while (wexist)
	{
		wantal++;
		memset(&wsb, 0, sizeof(CSpadData::typ_SpadBest));

		wsb.BestID = cmd.Field(1).asLong();
		wsb.FunktionsNummer = cmd.Field(2).asLong();
		wsb.Kommando = cmd.Field(3).asLong();
		wsb.ReturKod = cmd.Field(4).asLong();
		wsb.SenastDatum = cmd.Field(5).asLong();

		wsb.SenastKlock = cmd.Field(6).asLong();
		wsb.BestNummer = cmd.Field(7).asLong();
		wsb.BestDatum = cmd.Field(8).asLong();
		wsb.BestKlock = cmd.Field(9).asLong();
		strcpy_s(wsb.BestTitel, cmd.Field(10).asString());

		wsb.BestAntal = cmd.Field(11).asLong();
		wsb.KorstartDatum = cmd.Field(12).asLong();
		wsb.KorstartKlock = cmd.Field(13).asLong();
		wsb.ProgressNummer = cmd.Field(14).asLong();
		wsb.ProgressDatum = cmd.Field(15).asLong();

		wsb.ProgressKlock = cmd.Field(16).asLong();
		wsb.KorslutDatum = cmd.Field(17).asLong();
		wsb.KorslutKlock = cmd.Field(18).asLong();
		wsb.KorPostAntal = cmd.Field(19).asLong();
		wsb.ResultatBelopp1 = cmd.Field(20).asLong();

		wsb.ResultatBelopp2 = cmd.Field(21).asLong();
		wsb.ResultatBelopp3 = cmd.Field(22).asLong();

		bv->push_back(wsb);
		wexist = cmd.FetchNext();
	}

	cmd.Close();
	return;
}


int  DBwco::FixaNyttBestNummer(int FunktionsNummer)
{
	_connection->setAutoCommit(SA_AutoCommitOn);
	SACommand cmd;

	cmd.setConnection(_connection.get());

	cmd.setCommandText(_T(
		" SELECT                          "
		" Param3                          "
		" FROM Admi                       "
		" WHERE FunktionsNummer = :1      "

		));

	cmd.Param(1).setAsLong() = FunktionsNummer;
	cmd.Execute();

	int wbestnummer = 0;
	bool wexist = cmd.FetchNext();
	if (wexist)
	{
		wbestnummer = cmd.Field(1).asLong();
		wbestnummer++;
		cmd.setCommandText(_T(
			" UPDATE Admi                     "
			" SET Param3 = :2                 "
			" WHERE FunktionsNummer = :1      "
			));
		cmd.Param(1).setAsLong() = FunktionsNummer;
		cmd.Param(2).setAsLong() = wbestnummer;
		cmd.Execute();
	}

	cmd.Close();
	return wbestnummer;
}

void DBwco::DataStatus(int *BestAnt, int *BestHistAnt, int *MessArciAnt)
{
	_connection->setAutoCommit(SA_AutoCommitOn);
	SACommand cmd;

	cmd.setConnection(_connection.get());

	cmd.setCommandText(_T(
		" SELECT COUNT (*) FROM Best       "
		" SELECT COUNT (*) FROM BestHist   "
		" SELECT COUNT (*) FROM MessArci   "
		));

	cmd.Execute();

	*BestAnt     = 0;
	*BestHistAnt = 0;
	*MessArciAnt = 0;
	bool wexist;

	wexist = cmd.FetchNext();
	if (wexist)
		*BestAnt     = cmd.Field(1).asLong();

	wexist = cmd.FetchNext(); // varför? konsekvent?
	wexist = cmd.FetchNext();
	if (wexist)
		*BestHistAnt = cmd.Field(1).asLong();

	wexist = cmd.FetchNext();
	wexist = cmd.FetchNext();
	if (wexist)
		*MessArciAnt = cmd.Field(1).asLong();

	cmd.Close();
}

void DBwco::InsertSpadAdmi(CSpadData::typ_SpadAdmi *sa)
{
	_connection->setAutoCommit(SA_AutoCommitOn);
	SACommand cmd;

	cmd.setConnection(_connection.get());

	cmd.setCommandText(_T(
		" INSERT INTO [Admi]   "
		" (                               "
		" FunktionsNummer,                "
		" Kommando,                       "
		" SniffIntervallSekunder,         "
		" KorVarvSekunder,                "
		" AliveRapp,                      "
		" Param1,                         "
		" Param2,                         "
		" Param3                          "
		" )                               "
		" VALUES ( :1,:2,:3,:4,:5,:6,     "
		" :7, :8    )  "
		));

	cmd.Param(1).setAsLong() = sa->FunktionsNummer;
	cmd.Param(2).setAsLong() = sa->Kommando;
	cmd.Param(3).setAsLong() = sa->SniffIntervallSekunder;
	cmd.Param(4).setAsLong() = sa->KorVarvSekunder;

	cmd.Param(5).setAsLong() = sa->AliveRapp;
	cmd.Param(6).setAsLong() = sa->Param1;
	cmd.Param(7).setAsLong() = sa->Param2;
	cmd.Param(8).setAsLong() = sa->Param3;

	cmd.Execute();

	cmd.Close();

	return;
}

void DBwco::InsertSpadMessArci(CSpadData::typ_SpadBest *sb)
{
	_connection->setAutoCommit(SA_AutoCommitOn);
	SACommand cmd;

	cmd.setConnection(_connection.get());

	cmd.setCommandText(_T(
		" INSERT INTO [MessArci]          "
		" (                               "
		" FunktionsNummer,                "
		" Kommando,                       "
		" ReturKod,                       "
		" SenasteDatum,                   "
		" SenasteKlock,                   "
		" BestNummer,                     "
		" BestDatum,                      "
		" BestKlock,                      "
		" BestTitel,                      "
		" BestAntal,                      "
		" KorstartDatum,                  "
		" KorstartKlock,                  "
		" ProgressNummer,                 "
		" ProgressDatum,                  "
		" ProgressKlock,                  "
		" KorslutDatum,                   "
		" KorslutKlock,                   "
		" KorPostAntal,                   "
		" ResultatBelopp1,                "
		" ResultatBelopp2,                "
		" ResultatBelopp3                 "
		" )                               "
		" VALUES ( :1,:2,:3,:4,:5,:6,:7,  "
		" :8,:9,:10,:11,:12,:13,:14,:15,  "
		" :16,:17,:18,:19,:20,:21      )  "
		));

	cmd.Param(1).setAsLong() = sb->FunktionsNummer;
	cmd.Param(2).setAsLong() = sb->Kommando;
	cmd.Param(3).setAsLong() = sb->ReturKod;
	cmd.Param(4).setAsLong() = sb->SenastDatum;
	cmd.Param(5).setAsLong() = sb->SenastKlock;

	cmd.Param(6).setAsLong() = sb->BestNummer;
	cmd.Param(7).setAsLong() = sb->BestDatum;
	cmd.Param(8).setAsLong() = sb->BestKlock;
	cmd.Param(9).setAsString() = sb->BestTitel;
	cmd.Param(10).setAsLong() = sb->BestAntal;

	cmd.Param(11).setAsLong() = sb->KorstartDatum;
	cmd.Param(12).setAsLong() = sb->KorstartKlock;
	cmd.Param(13).setAsLong() = sb->ProgressNummer;
	cmd.Param(14).setAsLong() = sb->ProgressDatum;
	cmd.Param(15).setAsLong() = sb->ProgressKlock;

	cmd.Param(16).setAsLong() = sb->KorslutDatum;
	cmd.Param(17).setAsLong() = sb->KorslutKlock;
	cmd.Param(18).setAsLong() = sb->KorPostAntal;
	cmd.Param(19).setAsLong() = sb->ResultatBelopp1;
	cmd.Param(20).setAsLong() = sb->ResultatBelopp2;
	cmd.Param(21).setAsLong() = sb->ResultatBelopp3;

	cmd.Execute();

	cmd.Close();
	return;

}

void DBwco::DeleteSpadAll(void)
{
	_connection->setAutoCommit(SA_AutoCommitOn);
	SACommand cmd;
	cmd.setConnection(_connection.get());

	cmd.setCommandText(_T(
		" DELETE FROM Admi        "
		" DBCC CHECKIDENT ('Admi',     RESEED, 0) " // resettar autogenerering idnummer
		" DELETE FROM Best        "
		" DBCC CHECKIDENT ('Best',     RESEED, 0) "
		" DELETE FROM Besthist    "
		" DBCC CHECKIDENT ('Besthist', RESEED, 0) "
		" DELETE FROM MessArci    "
		" DBCC CHECKIDENT ('MessArci', RESEED, 0) "
		));

	cmd.Execute();
	cmd.Close();
}

void DBwco::SelectSpadRappAll(std::vector<CSpadData::typ_SpadRapp> *rv)
{
	_connection->setAutoCommit(SA_AutoCommitOn);
	SACommand cmd;

	cmd.setConnection(_connection.get());

	cmd.setCommandText(_T(
		" SELECT                          "
		" RappID,                         "
		" BestNummer,                     "
		" RappTyp,                        "
		" RadNummer,                      "
		" Rad                             "
		" FROM Rapp                       "
		" ORDER BY RappID     ASC         "
		));

	cmd.Execute();

	int wantal = 0;
	CSpadData::typ_SpadRapp wsr;

	bool wexist = cmd.FetchNext();
	while (wexist)
	{
		wantal++;
		memset(&wsr, 0, sizeof(CSpadData::typ_SpadRapp));

		wsr.RappID     = cmd.Field(1).asLong();
		wsr.BestNummer = cmd.Field(2).asLong();
		wsr.RappTyp    = cmd.Field(3).asLong();
		wsr.RadNummer  = cmd.Field(4).asLong();
		strcpy_s(wsr.Rad, cmd.Field(5).asString());

		rv->push_back(wsr);
		wexist = cmd.FetchNext();
	}

	cmd.Close();
	return;
}

void DBwco::InsertSpadRappAll(std::vector<CSpadData::typ_SpadRapp> *rv)
{
	_connection->setAutoCommit(SA_AutoCommitOn);
	SACommand cmd;

	cmd.setConnection(_connection.get());

	cmd.setCommandText(_T(
		" INSERT into [Rapp]              "
		" (                               "
		" BestNummer,                     "
		" RappTyp,                        "
		" RadNummer,                      "
		" Rad                             "
		" )                               "
		" VALUES                          "
		" ( :1, :2, :3, :4 )              "
		));

	CSpadData::typ_SpadRapp wsr;
// sök på iterator och pointer i spadfile mfl för att fräscha upp vektor-kunskapen
	std::vector<CSpadData::typ_SpadRapp>::iterator irv;
	for (irv = rv->begin(); irv != rv->end(); irv++)
	{
		wsr = *irv;
		cmd.Param(1).setAsLong() = irv->BestNummer;	// testar
		cmd.Param(1).setAsLong() = wsr.BestNummer;
		cmd.Param(2).setAsLong() = wsr.RappTyp;
		cmd.Param(3).setAsLong() = wsr.RadNummer;
		cmd.Param(4).setAsString() = wsr.Rad;

		cmd.Execute();
	}

	cmd.Close();
	return;
}

void DBwco::DeleteRappAll()
{
	_connection->setAutoCommit(SA_AutoCommitOn);
	SACommand cmd;
	cmd.setConnection(_connection.get());

	cmd.setCommandText(_T(
		" DELETE FROM Rapp        "
		));

	cmd.Execute();
	cmd.Close();

}
