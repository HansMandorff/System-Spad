       identification division.
       program-id. SpadDbCbl.
      * Själva databasanropen
      * Haft problem vid insert, löste genom AUTOCOMMT i prop 
      * för spaddbcbl.cbl
      * Läst om begin trans --- commit, ska testa, funkar!
      *
       
       environment division.
       configuration section.

       data division.
       working-storage section.
       EXEC SQL INCLUDE SQLCA END-EXEC. 
       01 MFSQLMESSAGETEXT  PIC X(250).  
       
       EXEC SQL BEGIN DECLARE SECTION END-EXEC.
       
          EXEC SQL 
          DECLARE spad.dbo.Admi TABLE
          
          ( AdmiID                 int
          , FunktionsNummer        int
          , Kommando               int
          , SniffintervallSekunder int        
          , KorVarvSekunder        int
          , AliveRapp              int
          , Param1                 int
          , Param2                 int
          , Param3                 int
          ) 
          
          END-EXEC.
       
       exec sql declare Best table 
       (
       	    BestID int,
	        FunktionsNummer int  ,
	        Kommando int  ,
	        ReturKod int  ,
	        SenasteDatum int  ,
	        SenasteKlock int  ,
	        BestNummer int  ,
	        BestDatum int  ,
	        BestKlock int  ,
	        BestTitel char(50)  ,
	        BestAntal int  ,
	        KorstartDatum int  ,
	        KorstartKlock int  ,
	        ProgressNummer int  ,
	        ProgressDatum int  ,
	        ProgressKlock int  ,
	        KorslutDatum int  ,
	        KorslutKlock int  ,
	        KorPostAntal int  ,
	        ResultatBelopp1 int  ,
	        ResultatBelopp2 int  ,
	        ResultatBelopp3 int  
           )
       end-exec.
       
      *exec sql declare BestHist table end-exec.
      *exec sql declare MessArci table end-exec.
       
       01  wa1. copy "admi-bin.cpy" 
                replacing leading ==xxxx== by ==wa1==.
       
       01  wsb. copy "best-bin.cpy" 
                replacing leading ==xxxx== by ==wsb==.
       
       01  work.
           05  vx pic s9(9) comp.
           
       EXEC SQL
       
           DECLARE SelectSpadAdmiFunk CURSOR FOR SELECT 
                 AdmiID,     
                 FunktionsNummer,  
                 Kommando,
                 SniffintervallSekunder,      
                 KorVarvSekunder,
                 AliveRapp,
                 Param1,
                 Param2,
                 Param3      
           FROM spad.dbo.Admi
           WHERE (FunktionsNummer = :wfunktionsnummer)
        end-exec
        
        exec sql   
           DECLARE SelectSpadAdmiAll CURSOR FOR SELECT 
                 AdmiID,     
                 FunktionsNummer,  
                 Kommando,
                 SniffintervallSekunder,      
                 KorVarvSekunder,
                 AliveRapp,
                 Param1,
                 Param2,
                 Param3      
           FROM spad.dbo.Admi
       END-EXEC. 
       
        exec sql   
           DECLARE SelectSpadBestNya CURSOR FOR SELECT 
           
            BestID,  
            FunktionsNummer,
	        Kommando,
	        ReturKod,
	        SenasteDatum,
	        SenasteKlock,
	        BestNummer,
	        BestDatum,
	        BestKlock,
	        BestTitel,
	        BestAntal,
	        KorstartDatum,
	        KorstartKlock,
	        ProgressNummer,
	        ProgressDatum,
	        ProgressKlock,
	        KorslutDatum,
	        KorslutKlock,
	        KorPostAntal,
	        ResultatBelopp1,
	        ResultatBelopp2,
	        ResultatBelopp3             
                 
           FROM spad.dbo.Best
           where (FunktionsNummer = :wfunktionsnummer and
           Kommando = 1 and Returkod = 0)
           order by BestID
           
       end-exec.
        
       exec sql
           DECLARE RaknaAdmiFunkKommando CURSOR FOR 
           SELECT COUNT(*)              
           FROM     spad.dbo.Admi  
		   WHERE    FunktionsNummer = :wfunktionsnummer
		   AND      Kommando        = :wkommando
       end-exec.
        
       EXEC SQL END DECLARE SECTION END-EXEC.
        
       linkage section.
       01  wfunktionsnummer    pic 9(9).
       01  wreturkod           pic 9(9).
       01  wbestid             pic 9(9).
       01  wkommando           pic 9(9).
       01  wantal              pic 9(9).
       01  sa. copy "admi-bin.cpy" 
               replacing leading ==xxxx== by ==sa==.
       01  sb. copy "best-bin.cpy" 
               replacing leading ==xxxx== by ==sb==.
       copy "vsb-bin.cpy".
       
       procedure division.
       a-huvud section.
       a010.
       a999.
       
           exit.
           
       k-entry-db-selectspadadmifunk section.
       k010.    
           entry "db-selectspadadmifunk" using wfunktionsnummer, sa.
           EXEC SQL WHENEVER SQLERROR perform OpenESQL-Error END-EXEC.
           EXEC SQL CONNECT TO 'Spad' end-exec.
           
           EXEC SQL OPEN SelectSpadAdmiFunk end-exec.
           EXEC SQL FETCH SelectSpadAdmiFunk  INTO
           
            :sa-AdmiID,
            :sa-FunktionsNummer,
            :sa-Kommando,
            :sa-SniffSekunder,
            :sa-KorvarvSekunder,
            :sa-AliveRapp,
            :sa-Param1,
            :sa-Param2,
            :sa-Param3
        
           end-exec.
           
               if SQLSTATE = "02000"
               move 999999 to sa-AdmiID.
               
           exec sql close selectspadadmifunk end-exec.
           EXEC SQL DISCONNECT CURRENT END-EXEC.
       
       k999.
           exit program.
       
       l-entry-db-selectspadbestnya section.
       l010.
           entry "db-SelectSpadBestNya" using wfunktionsnummer, vsb.
           EXEC SQL WHENEVER SQLERROR perform OpenESQL-Error END-EXEC.
           EXEC SQL CONNECT TO 'Spad' end-exec.
           
           EXEC SQL OPEN SelectSpadBestNya end-exec.
           move 0 to vx.
           PERFORM UNTIL (SQLSTATE >= "02000" or vx >= 10)
               EXEC SQL FETCH SelectSpadBestNya  INTO
           
            :wsb-BestID,
            :wsb-FunktionsNummer,
	        :wsb-Kommando,
	        :wsb-ReturKod,
	        :wsb-SenasteDatum,
	        :wsb-SenasteKlock,
	        :wsb-BestNummer,
	        :wsb-BestDatum,
	        :wsb-BestKlock,
	        :wsb-BestTitel,
	        :wsb-BestAntal,
	        :wsb-KorstartDatum,
	        :wsb-KorstartKlock,
	        :wsb-ProgressNummer,
	        :wsb-ProgressDatum,
	        :wsb-ProgressKlock,
	        :wsb-KorslutDatum,
	        :wsb-KorslutKlock,
	        :wsb-KorPostAntal,
	        :wsb-ResultatBelopp1,
	        :wsb-ResultatBelopp2,
	        :wsb-ResultatBelopp3    
            
            end-exec
            
               IF SQLSTATE < "02000" 
                   add 1 to vx
                   move wsb to vsb-sb(vx)  
                   move vx to vsb-antal
               END-IF
           
           end-perform
           
           exec sql close selectspadbestnya end-exec
           EXEC SQL DISCONNECT CURRENT END-EXEC.
           
        l999.
           exit program.
           
       m-entry-db-InsertSpadBest section.
       m010.
           entry "db-InsertSpadBest" using sb.
           EXEC SQL WHENEVER SQLERROR perform OpenESQL-Error END-EXEC.
           EXEC SQL CONNECT TO 'Spad' end-exec.
           
           exec sql begin tran end-exec
           EXEC SQL
           
           INSERT into spad.dbo.Best (
      *     BestID,  
            FunktionsNummer,
	        Kommando,
	        ReturKod,
	        SenasteDatum,
	        SenasteKlock,
	        BestNummer,
	        BestDatum,
	        BestKlock,
	        BestTitel,
	        BestAntal,
	        KorstartDatum,
	        KorstartKlock,
	        ProgressNummer,
	        ProgressDatum,
	        ProgressKlock,
	        KorslutDatum,
	        KorslutKlock,
	        KorPostAntal,
	        ResultatBelopp1,
	        ResultatBelopp2,
	        ResultatBelopp3   )
            values (
      *     :sb-BestID,
            :sb-FunktionsNummer,
	        :sb-Kommando,
	        :sb-ReturKod,
	        :sb-SenasteDatum,
	        :sb-SenasteKlock,
	        :sb-BestNummer,
	        :sb-BestDatum,
	        :sb-BestKlock,
	        :sb-BestTitel,
	        :sb-BestAntal,
	        :sb-KorstartDatum,
	        :sb-KorstartKlock,
	        :sb-ProgressNummer,
	        :sb-ProgressDatum,
	        :sb-ProgressKlock,
	        :sb-KorslutDatum,
	        :sb-KorslutKlock,
	        :sb-KorPostAntal,
	        :sb-ResultatBelopp1,
	        :sb-ResultatBelopp2,
	        :sb-ResultatBelopp3    )
           
           end-exec 
           
           exec sql commit end-exec
           
          EXEC SQL DISCONNECT CURRENT END-EXEC.
       m999.
           exit program.
        
       n-entry-sd-UppdateraSpadBestBestIDReturkod section.
       n010.
           entry 'db-UppdateraSpadBestBestIDReturkod'
             using wbestid, wreturkod.
           EXEC SQL WHENEVER SQLERROR perform OpenESQL-Error END-EXEC.
           EXEC SQL CONNECT TO 'Spad' end-exec. 
           
      *  test med begin, oklart när måste klämma i med exec sql
      *  kanske bara klarar ett "statement" i taget?  
           exec sql begin tran end-exec
           
           exec sql
       
           UPDATE spad.dbo.best
           set ReturKod = :wreturkod
           where BestID = :wbestid
           
           end-exec
           
           exec sql commit end-exec.
           
           EXEC SQL DISCONNECT CURRENT END-EXEC.
        n999.
            exit program.
       
       p-RaknaAdmiFunkKommando section.
       p010.
           entry 'db-RaknaAdmiFunkKommando' 
             using wfunktionsnummer, wkommando, wantal.  
           EXEC SQL WHENEVER SQLERROR perform OpenESQL-Error END-EXEC.
           EXEC SQL CONNECT TO 'Spad' end-exec. 
           EXEC SQL OPEN RaknaAdmiFunkKommando end-exec.
           EXEC SQL FETCH RaknaAdmiFunkKommando INTO :wantal
           end-exec.
           if SQLSTATE >= "02000"
               perform OpenESQL-Error.
           EXEC SQL DISCONNECT CURRENT END-EXEC.
       p999.
           exit program.
           
       q-UpdateAdmiFunkKommando section.
       q010.
           entry 'db-UpdateAdmiFunkKommando' using
             wfunktionsnummer, wkommando.
           EXEC SQL WHENEVER SQLERROR perform OpenESQL-Error end-exec.
           EXEC SQL CONNECT TO 'Spad' end-exec.
           exec sql begin tran end-exec.
           exec sql
             UPDATE spad.dbo.admi 
             SET Kommando = :wkommando 
             WHERE FunktionsNummer = :wfunktionsnummer
           end-exec
           exec sql commit end-exec.
           EXEC SQL DISCONNECT CURRENT END-EXEC.
           
       q999.
           exit program.
            
        OpenESQL-Error Section.
       
           display "SQL Error = " sqlstate " " sqlcode 
           display MFSQLMESSAGETEXT.
           stop ' '.
           stop run.