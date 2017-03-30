       identification division.
       program-id. SpadDataCbl.

       Environment Division.
       Input-Output Section.
       File-Control.
           Select inifil  assign "c:\spadfiler\spad.ini"
             ORGANIZATION IS LINE SEQUENTIAL.
       
       Data Division.
       File Section.
       fd  inifil.
       01  infile-record     pic x(80).

       working-storage section.
       
       01  work.
           05  wantal        pic 9(9).
           05  wkommando     pic 9(9).
           05  wdatamode     pic 9 value 1.
           05  wpath         pic x(80).
           05  winirad.
               10  wir-parm  pic x(9).
               10  wir-vard  pic 9.
               10  filler    pic x(75).
           05  winirad-re redefines winirad.
               10  wir-parm2 pic x(5).
               10  wir-vard2 pic x(80).
           05  weofinifil    pic x.
         
       linkage section.
       
       01  wfunktionsnummer  pic 9(9).
       01  wreturkod         pic 9(9).
       01  wbestid           pic 9(9).
       01  wfinnsJN          pic x.
       01  ldatamode         pic 9.
       01  lpath             pic x(80).
       01  sa. copy "admi-bin.cpy" replacing leading ==xxxx== by ==sa==.
       01  sb. copy "best-bin.cpy" replacing leading ==xxxx== by ==sb==.
       01  vsb.
           10  vsb-antal  pic 9(9).
           10  vsb-sb pic x(240) occurs 10.
       
       procedure division.
       a-huvud section.
       a010.
       a999.
           exit program.
       
       b-inifil section.
       b010.
           entry 'sd-hantera-inifil-datamode-path' 
             using ldatamode, lpath.
      * i inifilen står vilket datamode som gäller just nu
      * 1 för databas, 2 för enkla filer
           move space to wpath.
           move 0 to wdatamode.
           open input inifil.
           move 'N' to weofinifil.
           read inifil into winirad at end move 'J' to weofinifil.
       b100.
           if weofinifil = 'J'
               go to b200
           end-if.
           if wir-parm = 'DataMode=' 
               move wir-vard to wdatamode.
           if wir-parm2 = 'Path='
               move wir-vard2 to wpath.
           read inifil into winirad at end move 'J' to weofinifil.
           go to b100.
           
       b200.    
           move wpath to lpath.
           move wdatamode to ldatamode.
       b999.
           exit program.
       
       c-lagradatamode section.
       c010.
           entry 'sd-lagradatamode' using ldatamode.
           move ldatamode to wdatamode.
       c999.
           exit program.
       
       k-selectspadadmifunk section.
       kk010.
           entry "sd-selectspadadmifunk" using wfunktionsnummer, sa.
           if wdatamode = 1
             call  "db-selectspadadmifunk" using wfunktionsnummer, sa.
           if wdatamode = 2
             call  "sf-selectspadadmifunk" using wfunktionsnummer, sa.
       kk999.
           exit program.
           
       l-Selectspadbestny section.
       l010.
           entry "sd-SelectSpadBestNya" using wfunktionsnummer, vsb.
           if wdatamode = 1
             call  "db-SelectSpadBestNya" using wfunktionsnummer, vsb.
           if wdatamode = 2
             call  "sf-SelectSpadBestNya" using wfunktionsnummer, vsb.  
       l999.
           exit program.
           
       m-InsertSpadBest section.
       m010.
           entry "sd-InsertSpadBest" using wfunktionsnummer, sb         .
           if wdatamode = 1
             call  "db-InsertSpadBest" using wfunktionsnummer, sb.
           if wdatamode = 2  
             call  "sf-InsertSpadBest" using wfunktionsnummer, sb.
       m999.
           exit program.
       
       n-UppdateraSpadBestBestIDReturkod section.
       n010.
           entry 'sd-UppdateraSpadBestBestIDReturkod'
             using wbestid, wreturkod.
           if wdatamode = 1
             call  'db-UppdateraSpadBestBestIDReturkod'
             using wbestid, wreturkod.
           if wdatamode = 2
             call 'sf-UppdateraSpadBestBestIDReturkod'
             using wbestid, wreturkod.
       n999.
           exit program.
           
       p-KollaBegaranAvbrytExekveringFinns section.
       p010.  
           entry 'sd-KollaBegaranAvbrytExekveringFinns' 
             using wfunktionsnummer, wfinnsJN. 
           move  0 to wantal.
           move 99 to wkommando.
           if wdatamode = 1
             call 'db-RaknaAdmiFunkKommando' 
             using wfunktionsnummer, wkommando, wantal.  
           if wdatamode = 2
             call 'sf-RaknaAdmiFunkKommando' 
             using wfunktionsnummer, wkommando, wantal.      
           move 'N' to wfinnsJN.
           if wantal > 0
               move 'J' to wfinnsJN.  
       p999.
           exit program.
       
       q-NollstallBegaran section.
       q010.
           entry 'sd-NollstallBegaran' using wfunktionsnummer.
           move 0 to wkommando.
           if wdatamode = 1    
             call 'db-UpdateAdmiFunkKommando' using 
               wfunktionsnummer, wkommando.
           if wdatamode = 2    
             call 'sf-UpdateAdmiFunkKommando' using 
               wfunktionsnummer, wkommando.        
       q999.
           exit program.
           
       r-KollaBegaranPausaKorningFinns section.
       r010.    
           entry 'sd-KollaBegaranPausaKorningFinns' 
             using wfunktionsnummer, wfinnsJN.
           move  0 to wantal.
           move 16 to wkommando.
           if wdatamode = 1
             call 'db-RaknaAdmiFunkKommando' 
               using wfunktionsnummer, wkommando, wantal.  
           if wdatamode = 2
             call 'sf-RaknaAdmiFunkKommando' 
               using wfunktionsnummer, wkommando, wantal.          
           move 'N' to wfinnsJN.
           if wantal > 0
               move 'J' to wfinnsJN.        
       r999.
           exit program.
           
       s-KollaBegaranAvbrytKorningFinns section.
       s010.    
           entry 'sd-KollaBegaranAvbrytKorningFinns' 
             using wfunktionsnummer, wfinnsJN.
           move  0 to wantal.
           move 6 to wkommando.
           if wdatamode = 1
             call 'db-RaknaAdmiFunkKommando' 
               using wfunktionsnummer, wkommando, wantal.  
           if wdatamode = 2
             call 'sf-RaknaAdmiFunkKommando' 
               using wfunktionsnummer, wkommando, wantal.   
           move 'N' to wfinnsJN.
           if wantal > 0
               move 'J' to wfinnsJN.
       s999.
           exit.                                                                                                                   