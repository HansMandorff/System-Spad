       identification division.
       program-id. SpadzFileCbl.
      *
      * Hanterar datamode 2 - enkla filer
      * Under en tid trodde kompilatorn att detta var huvudprogrammet 
      * Därför z i programnamnet.
      * 
       environment division.
       Input-Output Section.
       File-Control.
           Select Admi  assign wadmiselnamn
             file status is wadmifs
             ORGANIZATION IS sequential.
           Select Best  assign wbestselnamn
             file status is wbestfs    
             ORGANIZATION IS sequential.
             
      * m_safullname = ge_Path + "SpadAdmi.bin";
      * m_sbfullname = ge_Path + "SpadBest.bin";
	  * m_shfullname = ge_Path + "SpadBestHist.bin";
	  * m_smfullname = ge_Path + "SpadMessArci.bin";
	                
       data division.
       File Section.
       fd  Admi.
       01  admi-record     pic x(36).
       fd  Best.
       01  best-record     pic x(136).
       
       working-storage section.
       01  wsa. copy "admi-bin.cpy" 
                replacing leading ==xxxx== by ==wsa==.
       01  wsa-redi redefines wsa.
               10  wsa-ord         pic x(4) occurs 9.
               
       01  wsb. copy "best-bin.cpy" 
                replacing leading ==xxxx== by ==wsb==.
       01  wsb-redi redefines wsb.
           05  wsb-ord1            pic x(4) occurs 9.
           05  filler              pic x(50).
           05  wsb-filler          pic xx.
           05  wsb-ord2            pic x(4) occurs 12.
           
       01  work.
           05  wadmifs.
               10  wadmifskey1     pic x.
               10  wadmifskey2     pic x.
           05  wbestfs.
               10  wbestfskey1     pic x.
               10  wbestfskey2     pic x.
               
           05  wfelant             pic s9(9) comp.
           05  vx                  pic s9(9) comp.
           05  varv                pic 9(4) comp.    
           05  w-ord-old.
               10  w-byte-old      pic x occurs 4.
           05  w-ord-new.
               10  w-byte-new      pic x occurs 4. 
           05  w-eof-admi          pic x value 'N'.
           05  w-eof-best          pic x value 'N'.
           05  wrak                pic s9(9) comp.
           05  wvx                 pic s9(9) comp.
           05  wid                 pic s9(9) comp.
           05  wdatamode           pic 9.
           05  wpath               pic x(50).
           05  wadmiselnamn        pic x(80).
           05  wbestselnamn        pic x(80).
           05  wzz-felnr           pic 999.
           05  wdispfelrad.
               10  filler pic x(18) value 'FEL vid situation '.
               10  wdfr-felnr pic zz9.
               10  filler pic x(16)  value '. Tryck <enter>.'.
           
       linkage section.
       01  lsa. copy "admi-bin.cpy" 
               replacing leading ==xxxx== by ==lsa==.
       01  lsb. copy "best-bin.cpy" 
               replacing leading ==xxxx== by ==lsb==.
       copy "vsb-bin.cpy" 
         replacing leading ==vsb== by ==lbv==.
         
       01  lfunktionsnummer pic 9(9).
       01  lantal           pic 9(9).
       01  lkommando        pic 9(9).
       01  lbestid          pic 9(9).
       01  lreturkod        pic 9(9).
       01  ldatamode        pic 9.
       01  lpath            pic x(80).
       
       procedure division.
       a-mode-path section.
       a010.
           entry 'sf-taemot-datamode-path' using ldatamode, lpath       .
           move ldatamode to wdatamode.
           move lpath     to wpath.
           string wpath "spadadmi.bin" delimited by space
             into wadmiselnamn.
           string wpath "spadbest.bin" delimited by space
             into wbestselnamn.      
       a999.
           exit program.
           
       k-entry-sf-selectspadadmifunk section.
       k010.    
           entry "sf-selectspadadmifunk" using lfunktionsnummer, lsa.
           move 0 to wfelant.
           open input sharing with all other Admi.
       k015.
           if wadmifskey1 not = '0'
               add 1 to wfelant
               if wfelant < 10
                   call 'c$sleep' using 1
                   open input sharing with all other Admi
                   go to k015
           end-if.
                   
           move 'N' to w-eof-admi.
           read Admi into wsa at end move 'J' to w-eof-admi.
           
       k020.
           if w-eof-admi = 'J'
               move 501 to wzz-felnr
               perform zz-felsignalera
           end-if.
           perform za-fixa-admi.
           if wsa-FunktionsNummer = lfunktionsnummer
               move wsa to lsa
               go to k999
           end-if.
           read Admi into wsa at end move 'J' to w-eof-admi.
           go to k020.   
               
       k999.
           close Admi.
           exit program.
       
       l-entry-sf-selectspadbestnya section.
       l010.
           entry "sf-SelectSpadBestNya" using lfunktionsnummer, lbv.
           move 'N' to w-eof-best.
           move 0 to wvx.
           open input sharing with all other Best.
           read Best into wsb at end move 'J' to w-eof-best.
       l020.    
           if w-eof-best = 'J'
               go to l999
           end-if.
           perform zb-fixa-best.   
           if (wsb-FunktionsNummer = lfunktionsnummer
           and wsb-Kommando = 1
           and wsb-ReturKod = 0)
               add 1 to wvx
               move wsb to lbv-sb(wvx)
           end-if.
           read Best into wsb at end move 'J' to w-eof-best.
           go to l020.
       l999.
           close Best.
           move wvx to lbv-antal.
           exit program.
           
       m-entry-sf-InsertSpadBest section.
       m010.
           entry "sf-InsertSpadBest" using lsb.
      * test med sharing 
           open input sharing with all other Best.
      *    open input Best.
           move 0 to wrak.
           move 0 to wid.
           move 'N' to w-eof-best.
           read Best into wsb at end move 'J' to w-eof-best.
       m020.    
           if w-eof-best = 'J'
               go to m100
           end-if.
           perform zb-fixa-best.
           if wsb-BestID > wid
               move wsb-BestID to wid
           end-if.
           add 1 to wrak.
           read Best into wsb at end move 'J' to w-eof-best.  
           go to m020.
           
       m100.  
           close Best.
           move 0 to wfelant.
           open extend Best.
       m115.    
           if wbestfskey1 not = '0'
               add 1 to wfelant
               if wfelant < 10
                   call 'c$sleep' using 1
                   open extend Best
                   go to m115
           end-if.    
           add 1 to wid.
           move wid to lsb-BestID.
           move lsb to wsb.
           perform zb-fixa-best.
           write best-record from wsb.
           close Best.
       m999.
           exit program.
        
       n-entry-sf-UppdateraSpadBestBestIDReturkod section.
       n010.
          entry 'sf-UppdateraSpadBestBestIDReturkod'
            using lbestid, lreturkod.
           move 'N' to w-eof-best.
           open i-o Best.
           read Best into wsb
             at end move 'J' to w-eof-best.
       n020.
           if w-eof-best = 'J' *> posten måste finnas
               move 503 to wzz-felnr
               perform zz-felsignalera
           end-if.
           perform zb-fixa-best.
           if lbestid = wsb-BestID
               move lreturkod to wsb-ReturKod
               perform zb-fixa-best
               rewrite best-record from wsb
               go to n999
           end-if.
           read Best into wsb
             at end move 'J' to w-eof-best.
           go to n020.
       n999.
           close Best.
           exit program.
       
       p-sf-RaknaAdmiFunkKommando section.
       p010.
           entry 'sf-RaknaAdmiFunkKommando' 
             using lfunktionsnummer, lkommando, lantal. 
           move 0 to lantal.
           open input Admi.
           read Admi into wsa at end move 'J' to w-eof-admi.
           
       p020.
           if w-eof-admi = 'J'
               go to p999
           end-if.
           perform za-fixa-admi.
           if wsa-FunktionsNummer = lfunktionsnummer
           and wsa-Kommando = lkommando
               add 1 to lantal
           end-if.
           read Admi into wsa at end move 'J' to w-eof-admi.
           go to p020.       
           
       p999.
           close Admi.
           exit program.
           
       q-sf-UpdateAdmiFunkKommando section.
       q010.
           entry 'sf-UpdateAdmiFunkKommando' using
             lfunktionsnummer, lkommando.
           move 'N' to w-eof-admi.
           open i-o Admi.
           read Admi into wsa
             at end move 'J' to w-eof-admi.
       q020.
           if w-eof-admi = 'J' *> posten måste finnas
               move 502 to wzz-felnr
               perform zz-felsignalera
           end-if.
           perform za-fixa-admi.
           if lfunktionsnummer = wsa-FunktionsNummer
               move lkommando to wsa-Kommando
               perform za-fixa-admi
               rewrite admi-record from wsa
               go to q030
           end-if.
           read Admi into wsa
             at end move 'J' to w-eof-admi.
           go to q020.
       q030.
           close Admi.
       q999.
           exit program.
            
       za-fixa-admi Section.
       za010.
           move 1 to varv.
           perform za100 until varv > 9.
           go to za999.
           
       za100.
           move wsa-ord(varv) to w-ord-old.
           move w-byte-old(4) to w-byte-new(1).
           move w-byte-old(3) to w-byte-new(2).    
           move w-byte-old(2) to w-byte-new(3).   
           move w-byte-old(1) to w-byte-new(4).  
           move w-ord-new to wsa-ord(varv).
           add 1 to varv.
           
       za999.
           exit.
           
       zb-fixa-best Section.
       zb010.
           move zero to wsb-filler.
           move 1 to varv.
           perform zb100 until varv > 9.
           move 1 to varv.
           perform zb200 until varv > 12.
           go to zb999.
           
       zb100.
           move wsb-ord1(varv) to w-ord-old.
           move w-byte-old(4) to w-byte-new(1).
           move w-byte-old(3) to w-byte-new(2).    
           move w-byte-old(2) to w-byte-new(3).   
           move w-byte-old(1) to w-byte-new(4).  
           move w-ord-new to wsb-ord1(varv).
           add 1 to varv.
           
       zb200.
           move wsb-ord2(varv) to w-ord-old.
           move w-byte-old(4) to w-byte-new(1).
           move w-byte-old(3) to w-byte-new(2).    
           move w-byte-old(2) to w-byte-new(3).   
           move w-byte-old(1) to w-byte-new(4).  
           move w-ord-new to wsb-ord2(varv).
           add 1 to varv.
           
       zb999.
           exit.
           
       zz-felsignalera section.
       zz010.
           move wzz-felnr to wdfr-felnr.
           stop wdispfelrad.
           stop run.
           
       zz999.
           exit.
       