       identification division.
      *
      *2016-02-24 G�tt �ver till bin i copyposter
      *2016-02-25 haft problem med vem som �r huvudprogram
      *           vid till�gg av spadzfilecbl
      *           
       program-id. SpadFunkCblMain.
       environment division.
       configuration section.
       data division.
       working-storage section.
       01  work.
           05  wfunknrvi          pic 9(9) value 1003.
           05  wdatamode          pic 9.
           05  wpath              pic x(80).
           05  b-sniffsekunder    pic 9(9).
           05  b-returkod         pic 9.
           05  wkorvarvsekunder   pic 9(9).
           05  waliverapp         pic 9(9).
           
           05  wontimerant        pic 9(9) comp.
           05  wfalt              pic 9(8).
           05  wdate              pic 9(6).
           05  wtime              pic 9(6).
           
           05  wfunktionsnummer   pic 9(9).
           05  wreturkod          pic 9(9).
           05  wbestid            pic 9(9).
           05  wfinnsJN           pic x.
           05  wreturkodkorning   pic 9(9).
           
           05  varv               pic s9(4) comp.
           
           05  wdisprad.
               10  filler pic x(19) value 'sniff-sniff-sniff '.
               10  wd-ontimerant pic zz9.
               10  filler pic x(7) value ', funk '.
               10  wd-funknrvi   pic zzz9.
               10  filler pic x(8) value ', sniff '.
               10  wd-sniffsekunder pic zz9.
               10  filler pic x(7) value ', varv '.      
               10  wd-korvarvsekunder pic zz9.
               10  filler pic x(7) value ', rapp '.      
               10  wd-aliverapp pic 9.
               10  filler pic x(11) value ', datamode '.
               10  wd-datamode pic 9.
          
           05  wdisprad2.
               10  filler pic x(15) value 'Best�llning nr '.
               10  wd2-bestnummer pic zzz9.
               10  filler pic xx value '  '.
               10  wd2-besttitel   pic x(20).
               10  wd2-bestantal pic zzz.
               10  wd2-text pic x(18) value ' varv k�rs ig�ng '.      
               10  wd2-korstartdatum pic zzzzz9.
               10  wd2-korstartklock pic z9(6).
                  
           05  sa.
           copy "admi-bin.cpy" replacing leading ==xxxx== by ==sa==.
           
           05  sb.
           copy "best-bin.cpy" replacing leading ==xxxx== by ==sb==.
           
           copy "vsb-bin.cpy".    
      
       procedure division.
       main section.
       main010.
           perform x-initiera.
           perform b-OnTimer.
           perform until b-returkod = 9
               call 'c$sleep' using b-sniffsekunder
               perform b-OnTimer
           end-perform.    
           stop run.
       main999.
           exit.
       
       b-OnTimer section. *> namnet ett arv fr�n c-l�sningen
       b010.
           move 0 to b-returkod.
           add 1 to wontimerant.
           
      * h�mta f�rsk admi 1000, ta hand om param1 = datametod
      * och korvarvsekunder = v�rt funknummer 
           initialize sa.
           move 1000 to wfunktionsnummer.
           call 'sd-selectspadadmifunk' using wfunktionsnummer, sa.
           move sa-Param1 to wdatamode.
           move sa-KorVarvSekunder to wfunknrvi.
           
      * vi kan ha f�tt ett skifte i datamode, delge sd-datacbl   
           call 'sd-lagradatamode' using wdatamode.
      
      * h�mta hem f�rsk info f�r v�rt funknummer fr�n spad   
           initialize sa.
           move wfunknrvi to wfunktionsnummer.
           call 'sd-selectspadadmifunk' using wfunktionsnummer, sa.
           move sa-SniffSekunder   to b-sniffsekunder.
           move sa-KorVarvSekunder to wkorvarvsekunder.
           move sa-AliveRapp       to waliverapp.
           
      * kolla om avbryt-beg�ran finns
           call 'sd-KollaBegaranAvbrytExekveringFinns'
             using wfunknrvi, wfinnsJN.
           if wfinnsJN = 'J'
               call 'sd-NollstallBegaran' using wfunknrvi
      * meddela i best med 528
               move low-value to sb
               move wfunknrvi to sb-FunktionsNummer
               perform za-hamta-dag-klocka
               move wdate to sb-SenasteDatum
               move wtime to sb-SenasteKlock
               move 528 to sb-Kommando
               call 'sd-InsertSpadBest' using sb
      * visa att avbrott har beg�rts
               move 9 to b-returkod
               go to b999
           end-if.
           
           move wontimerant      to wd-ontimerant.
           move wfunknrvi        to wd-funknrvi.
           move b-sniffsekunder   to wd-sniffsekunder.
           move wkorvarvsekunder to wd-korvarvsekunder.
           move waliverapp       to wd-aliverapp.
           move wdatamode        to wd-datamode.
      * sniff-sniff-display    
           display wdisprad.
           
      * kolla efter om det finns best�llning(ar) till oss
      * kunde r�ckt med bara en best i taget, g�r �nd� som i C 
           move wfunknrvi to wfunktionsnummer. 
           initialize vsb.
           call 'sd-SelectSpadBestNya' using wfunktionsnummer, vsb.
           if vsb-antal = 0
      * nix, inget jobb 
      * eventuellt skicka en 77:a 
               if waliverapp = 1
                   move low-value to sb
                   move wfunknrvi to sb-FunktionsNummer
                   perform za-hamta-dag-klocka
                   move wdate to sb-SenasteDatum
                   move wtime to sb-SenasteKlock
                   move 77 to sb-Kommando
                   call 'sd-InsertSpadBest' using sb
               end-if
      * visa att inget fanns att g�ra        
               move 1 to b-returkod
               go to b999
           end-if.
               
      * Jobb finns att g�ra        
      * Markera att best�llningen, kommando 1, �r omh�n-
      * dertagen av k�raren
      * genom att m�rka den i returkod
          
           initialize sb.
           move vsb-sb(1) to sb.
           move 181 to wreturkod.
           move sb-BestID to wbestid.
           call 'sd-UppdateraSpadBestBestIDReturkod'
               using wbestid, wreturkod.
               
           perform za-hamta-dag-klocka.
           move wdate to sb-KorstartDatum, sb-SenasteDatum.
           move wtime to sb-KorstartKlock, sb-SenasteKlock.
           move 2 to sb-Kommando.
           call 'sd-InsertSpadBest' using sb.
           
           move sb-BestNummer    to wd2-bestnummer.
           move sb-BestTitel     to wd2-besttitel.
           move sb-BestAntal     to wd2-bestantal.
           move sb-KorstartDatum to wd2-korstartdatum.
           move sb-KorstartKlock to wd2-korstartklock.
           move ' varv K�RS IG�NG ' to wd2-text.
      * best-startar-display
           display wdisprad2.
        
      * k�rning dras ig�ng    
           perform c-korning-0000.
           
      *  skapa en kommando 4 bestpost; k�rning klar alt
      *  skapa en 7:a, k�rning avbruten
           if wreturkodkorning = 0
               move 4 to sb-Kommando
               move 'KLAR' to wd2-text
           else
               move 7 to sb-Kommando
               move 'AVBRUTEN' to wd2-text
           end-if.
           
           move 999999 to sb-ProgressNummer.
           perform za-hamta-dag-klocka.
           move wdate to sb-KorslutDatum, sb-SenasteDatum.
           move wtime to sb-KorslutKlock, sb-SenasteKlock. 
           call 'sd-InsertSpadBest' using sb.
           
      * messa i rutan    
           move 0 to wd2-bestantal.
           move sb-KorslutDatum to wd2-korstartdatum.
           move sb-KorslutKlock to wd2-korstartklock.
           display wdisprad2.
           
      * visa att k�rning genomf�rts    
           move 2 to b-returkod.  
 
       b999.
           exit.
       
       c-korning-0000 section.
       c010.
       
      * st�d saknas �nnu f�r paus-forts�tt och avbrott
        
           move 1 to varv.
           move 0 to wreturkodkorning.
           
      * g�r en loop med goto-konstruktion   
       c020.
           if varv > sb-BestAntal
               go to c100.
           call 'c$sleep' using wkorvarvsekunder.
           
      * skapa en kommando 3 bestpost; progress
           move 3 to sb-Kommando.
           move varv to sb-ProgressNummer.
           perform za-hamta-dag-klocka.
           move wdate to sb-ProgressDatum, sb-SenasteDatum.
           move wtime to sb-ProgressKlock, sb-SenasteKlock. 
           multiply varv by 1200 giving sb-ResultatBelopp1.
           multiply varv by 1500 giving sb-ResultatBelopp2.
           multiply varv by 1800 giving sb-ResultatBelopp3.
           multiply varv by 900  giving sb-KorPostAntal.
       
           call 'sd-InsertSpadBest' using sb.
           
      * kolla upp om beg�ran om paus finns
       c030.
           call 'sd-KollaBegaranPausaKorningFinns' 
             using wfunknrvi, wfinnsJN.
           if wfinnsJN = 'J'
      * skicka en 16 om att vi har paus    
               move wfunknrvi to sb-FunktionsNummer
               perform za-hamta-dag-klocka
               move wdate to sb-SenasteDatum
               move wtime to sb-SenasteKlock
               move 16 to sb-Kommando
               call 'sd-InsertSpadBest' using sb
      * sov en stund, hoppa tillbaka f�r f�rnyad test        
               call 'c$sleep' using wkorvarvsekunder
               go to c030
           end-if.
           
      * ber�tta med en 17 att vi �r ig�ng igen om vi haft en paus    
           if sb-Kommando = 16
               move wfunknrvi to sb-FunktionsNummer
               perform za-hamta-dag-klocka
               move wdate to sb-SenasteDatum
               move wtime to sb-SenasteKlock
               move 17 to sb-Kommando
               call 'sd-InsertSpadBest' using sb
           end-if.
      
	  * kolla efter om avbrott av k�rningen beg�rts  
           call 'sd-KollaBegaranAvbrytKorningFinns' 
             using wfunknrvi, wfinnsJN.
           if wfinnsJN = 'J'
               call 'sd-NollstallBegaran' using wfunknrvi
               move 7 to wreturkodkorning
               go to c999
           end-if.  
           
      * upp igen, n�sta varv eller slut       
           add 1 to varv.
           go to c020.
           
       c100.
      * slut p� loopen
       c999.
           exit.
       
       x-initiera section.
       x010.
           display 'SpadFunkCbl ver 2.0'.
      * l�t datamodulen h�mta gemensam init-fil
      * och lagra datamode f�r fortsatt bruk  
      * Lite soppigt men tanken �r att inte blanda in db alls
      * om vi k�r mode 2, filer, fr�n start
      * V�xling kan ske flygande i i b�rjan av ontimer 
        
           call 'sd-hantera-inifil-datamode-path' using 
             wdatamode, wpath.
           call 'sf-taemot-datamode-path' using wdatamode, wpath.
           
      * h�mta admi 1000, ta hand om korvarvsek: v�rt funknummer 
           initialize sa.
           move 1000 to wfunktionsnummer.
           call 'sd-selectspadadmifunk' using wfunktionsnummer, sa.
           move sa-KorVarvSekunder to wfunknrvi.
       
      * meddela via nytt bestmess med kommando 526 att vi �r ig�ng
           move low-value to sb. *> f�r titelns skull, utredning p�g�r
           move wfunknrvi to sb-FunktionsNummer.
           move 526 to sb-Kommando.
           perform za-hamta-dag-klocka.
           move wdate to sb-SenasteDatum.
           move wtime to sb-SenasteKlock.
           call 'sd-InsertSpadBest' using sb.    
           
      * h�mta hem info f�r v�rt funknummer fr�n spad 
           initialize sa.
           move wfunknrvi to wfunktionsnummer.
           call 'sd-selectspadadmifunk' using wfunktionsnummer, sa.
           move sa-SniffSekunder   to b-sniffsekunder.
           move sa-KorVarvSekunder to wkorvarvsekunder.
           move sa-AliveRapp       to waliverapp.
           
      * diverse 
           move 0 to wontimerant.
       x999.
           exit.
           
       za-hamta-dag-klocka section.
       za010.
          accept wfalt from date.
          move wfalt to wdate.
          accept wfalt from time.
          divide wfalt by 100 giving wtime.
       za999.
           exit.