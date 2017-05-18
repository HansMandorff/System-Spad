       identification division.
       program-id. SpadDbCbl2.
      * Själva databasanropen
      * Haft problem vid insert, löste genom AUTOCOMMT i prop 
      * för spaddbcbl.cbl
      * Läst om begin trans --- commit, ska testa, funkar!
      * 2016-10-23 Lagt in db2 parallellt med sqlserver för tre funktioner
      * 
       
       environment division.
       configuration section.

       data division.
       working-storage section.
       EXEC SQL INCLUDE SQLCA END-EXEC. 
       01 MFSQLMESSAGETEXT  PIC X(250).  
       
      *EXEC SQL BEGIN DECLARE SECTION END-EXEC.
      * behovet av declare oklart här, enl nätet:db2 så omgärda "host variables", dom man refererar till med :
          EXEC SQL 
          DECLARE Rapp TABLE *> funkar utan spad.dbo.
          ( RappID                 int
          , BestNummer             int
          , RappTyp                int
          , RadNummer              int        
          , Rad                    char(200)
          ) 
          
          END-EXEC.
       
          EXEC SQL 
      *   DECLARE spad.dbo.Text TABLE
          DECLARE Text TABLE
          
          ( TextID                 int
          , Typ                    int
          , Kod                    int
          , Ktext                  char(30)        
          ) 
          
          END-EXEC.
       
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
       
       exec sql declare BestHist table 
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
      
       EXEC SQL 
       DECLARE SelectRappTypLista CURSOR FOR
      * exempel på inner join   
               select 
               r.Rapptyp, 
               t.Ktext, 
               COUNT(*) Antal
               
               from spad.dbo.Rapp r
      *        from Rapp r
               inner join spad.dbo.Text t on t.Kod = r.Rapptyp and t.Typ = 1

               where r.RadNummer = 1
               
               group by r.RappTyp, t.Ktext
               order by r.Rapptyp
       END-EXEC
       
       EXEC SQL 
       DECLARE SelectRappTypListaDb2 CURSOR FOR
      * exempel på inner join   
               select 
               r.Rapptyp, 
               t.Ktext, 
               COUNT(*) Antal
               
               from Rapp r
               inner join Text t on t.Kod = r.Rapptyp and t.Typ = 1

               where r.RadNummer = 1
               
               group by r.RappTyp, t.Ktext
               order by r.Rapptyp
       END-EXEC
       
       
      *exec sql declare MessArci table end-exec.
       
       EXEC SQL 
       
           DECLARE SelectRapp CURSOR FOR 
          
               select 
               RappID,
               BestNummer,
               RappTyp,
               RadNummer,
               Rad
               FROM spad.dbo.Rapp
               WHERE (BestNummer = :wbestnummer)
               order by RadNummer
       
       END-EXEC        
       
       EXEC SQL 
       
           DECLARE SelectRappDb2 CURSOR FOR 
          
               select 
               RappID,
               BestNummer,
               RappTyp,
               RadNummer,
               Rad
               FROM Rapp
               WHERE (BestNummer = :wbestnummer)
               order by RadNummer
       
       END-EXEC        
       
       
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
       
      *EXEC SQL BEGIN DECLARE SECTION END-EXEC. 
      *EXEC SQL END DECLARE SECTION END-EXEC.
      * provar att lägga work-areor utanför declare section, nu helt kopplat bort begin/end declare
      * kanske är ett måste i andra miljöer, tex db2
        
       01  wrt. copy "rapptyp.cpy"
                replacing leading ==xxxx== by ==wrt==.
       
       01  wra. copy "rapp.cpy"
                replacing leading ==xxxx== by ==wra==.
       
       01  wa1. copy "admi-bin.cpy" 
                replacing leading ==xxxx== by ==wa1==.
       
       01  wsb. copy "best-bin.cpy" 
                replacing leading ==xxxx== by ==wsb==.
       
       01  work.
           05  vx pic s9(9) comp.
           05  wKtext pic x(30).
           05  wKod   pic 9.
           05  wTyp   pic 9.
       
       linkage section.
       01  wfunktionsnummer    pic 9(9).
       01  wbestnummer         pic 9(9).
       01  wrapptyp            pic 9(9).
       01  wreturkod           pic 9(9).
       01  wbestid             pic 9(9).
       01  wkommando           pic 9(9).
       01  wantal              pic 9(9).
       01  lkomu. copy "komu.cpy" replacing leading ==xxxx== by ==lkomu==.
       01  sa. copy "admi-bin.cpy" 
               replacing leading ==xxxx== by ==sa==.
       01  sb. copy "best-bin.cpy" 
               replacing leading ==xxxx== by ==sb==.
       copy "vsb-bin.cpy".
       copy "vrt.cpy".
       copy "vra.cpy" replacing leading ==xxxx== by ==vra==.
       
       procedure division.
       a-huvud section.
       a010.
       a999.
       
           exit.
       
       d-entry-db-select-rapptyp-lista section.
       d010.
           entry "db-select-rapptyp-lista" using lkomu, vrt.
           move 0 to lkomu-RETURKOD.
           move 0 to lkomu-SQLCODE.
           move space to lkomu-MFSQLMESSAGETEXT.
           move space to lkomu-SQLSTATE.
           EXEC SQL WHENEVER SQLERROR perform OpenESQL-Error END-EXEC.
           if lkomu-KOMMANDO1 = 1
               EXEC SQL CONNECT TO 'Spad' end-exec
               EXEC SQL OPEN SelectRappTypLista end-exec
               move 0 to vx
               PERFORM UNTIL (SQLSTATE >= "02000" or vx >= 99)
                   EXEC SQL FETCH SelectRappTypLista INTO
                   :wrt-RappTyp,
                   :wrt-RappNamn,
                   :wrt-Antal
                   end-exec
                   IF SQLSTATE < "02000" 
                       add 1 to vx
                       move wrt to vrt-rt(vx)  
                       move vx to vrt-antal
                   END-IF
               end-perform
               exec sql close SelectRappTypLista end-exec
           else
               EXEC SQL CONNECT TO 'SpadDb2' end-exec
               EXEC SQL OPEN SelectRappTypListaDb2 end-exec
               move 0 to vx
               PERFORM UNTIL (SQLSTATE >= "02000" or vx >= 99)
                   EXEC SQL FETCH SelectRappTypListaDb2 INTO
                   :wrt-RappTyp,
                   :wrt-RappNamn,
                   :wrt-Antal
                   end-exec
                   IF SQLSTATE < "02000" 
                       add 1 to vx
                       move wrt to vrt-rt(vx)  
                       move vx to vrt-antal
                   END-IF
               end-perform
               exec sql close SelectRappTypListaDb2 end-exec
           end-if.
           
           EXEC SQL DISCONNECT CURRENT END-EXEC.
        d999.
           exit program.
            
       e-entry-db-select-rapp-lista section.
       e010.
           entry "db-select-rapp-lista" using lkomu, wrapptyp, vra.
           move 0 to lkomu-RETURKOD.
           move 0 to lkomu-SQLCODE.
           move space to lkomu-MFSQLMESSAGETEXT.
           move space to lkomu-SQLSTATE.
           EXEC SQL WHENEVER SQLERROR perform OpenESQL-Error END-EXEC.
           if lkomu-KOMMANDO1 = 1
               EXEC SQL CONNECT TO 'Spad' end-exec
      * declare cursor kan ligga här
               EXEC SQL 
               DECLARE SelectRappLista CURSOR FOR
                   select 
                   r.Bestnummer, 
                   b.SenasteDatum,
                   b.SenasteKlock,
                   r.Rad 
                   from spad.dbo.Rapp r
                   inner join spad.dbo.besthist b on r.BestNummer = b.BestNummer 
                   where r.RappTyp = :wrapptyp
                   and r.RadNummer = 1
                   order by r.BestNummer desc
               END-EXEC
       
               EXEC SQL OPEN SelectRappLista end-exec
               move 0 to vx
               PERFORM UNTIL (SQLSTATE >= "02000" or vx >= 99)
                   EXEC SQL FETCH SelectRappLista INTO
                   :wra-BestNummer,
                   :wra-RadNummer,
                   :wra-RappTyp,
                   :wra-Rad 
                   end-exec
                   IF SQLSTATE < "02000" 
                       add 1 to vx
                       move 0 to wra-RappID
                       move wra to vra-ra(vx)  
                       move vx to vra-antal
                   END-IF
           
               end-perform
               exec sql close SelectRappLista end-exec
           else
                EXEC SQL CONNECT TO 'SpadDb2' end-exec
      * declare cursor kan ligga här
               EXEC SQL 
               DECLARE SelectRappListaDb2 CURSOR FOR
                   select 
                   r.Bestnummer, 
                   b.SenasteDatum,
                   b.SenasteKlock,
                   r.Rad 
                   from Rapp r
                   inner join besthist b on r.BestNummer = b.BestNummer 
                   where r.RappTyp = :wrapptyp
                   and r.RadNummer = 1
                   order by r.BestNummer desc
               END-EXEC
       
               EXEC SQL OPEN SelectRappListaDb2 end-exec
               move 0 to vx
               PERFORM UNTIL (SQLSTATE >= "02000" or vx >= 99)
                   EXEC SQL FETCH SelectRappListaDb2 INTO
                   :wra-BestNummer,
                   :wra-RadNummer,
                   :wra-RappTyp,
                   :wra-Rad 
                   end-exec
                   IF SQLSTATE < "02000" 
                       add 1 to vx
                       move 0 to wra-RappID
                       move wra to vra-ra(vx)  
                       move vx to vra-antal
                   END-IF
               end-perform
               exec sql close SelectRappListaDb2 end-exec
          end-if.
           
           EXEC SQL DISCONNECT CURRENT END-EXEC.
           
        e999.
           exit program.
       
       f-entry-db-select-rapp section.
       f010.
           entry "db-select-rapp" using lkomu, wbestnummer, vra.
           move 0 to lkomu-RETURKOD.
           move 0 to lkomu-SQLCODE.
           move space to lkomu-MFSQLMESSAGETEXT.
           move space to lkomu-SQLSTATE.
           move spaces to wra.
           EXEC SQL WHENEVER SQLERROR perform OpenESQL-Error END-EXEC.
           if lkomu-KOMMANDO1 = 1
               EXEC SQL CONNECT TO 'Spad' end-exec
               EXEC SQL OPEN SelectRapp end-exec
               move 0 to vx
               PERFORM UNTIL (SQLSTATE >= "02000" or vx >= 99)
                   EXEC SQL FETCH SelectRapp INTO
               
                   :wra-RappID,
                   :wra-BestNummer,
                   :wra-RappTyp,
                   :wra-RadNummer,
                   :wra-Rad 
               
                   end-exec
               
                   IF SQLSTATE < "02000" 
                       add 1 to vx
                       move wra to vra-ra(vx)  
                       move vx to vra-antal
                   END-IF
           
               end-perform
               exec sql close SelectRapp end-exec
           else   
               EXEC SQL CONNECT TO 'SpadDb2' end-exec
               EXEC SQL OPEN SelectRappDb2 end-exec
               move 0 to vx
               PERFORM UNTIL (SQLSTATE >= "02000" or vx >= 99)
                   EXEC SQL FETCH SelectRappDb2 INTO
               
                   :wra-RappID,
                   :wra-BestNummer,
                   :wra-RappTyp,
                   :wra-RadNummer,
                   :wra-Rad 
               
                   end-exec
               
                   IF SQLSTATE < "02000" 
                       add 1 to vx
                       move wra to vra-ra(vx)  
                       move vx to vra-antal
                   END-IF
           
               end-perform
               exec sql close SelectRappDb2 end-exec
           end-if.    
           
           EXEC SQL DISCONNECT CURRENT END-EXEC.
           
        f999.
           exit program.
       
       
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
           entry 'db-RaknaAdmiFunkKommando' using wfunktionsnummer, wkommando, wantal.
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
      * Gör om på samma sätt som i Db2Test, OK
           move 9 to lkomu-RETURKOD.
           move MFSQLMESSAGETEXT to lkomu-MFSQLMESSAGETEXT.
           move SQLSTATE to lkomu-SQLSTATE.
           move SQLCODE  to lkomu-SQLCODE.
           exit program.
       