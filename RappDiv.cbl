       identification division.
       program-id. RappDiv.
      * Diverse
      * Skapa textfiler för notepad/excel att ladda in
      *
       environment division.
       Input-Output Section.
       File-Control.
             Select Print assign wprintfilnamn
             file status is wfs    
             ORGANIZATION IS LINE SEQUENTIAL.
             
       data division.
       File Section.
       fd  Print.
       01  Print-rad     pic x(210).
       
       working-storage section.
       01  wra. copy "rapp.cpy" replacing leading ==xxxx== by wra.
       01  work.
           05  wfs.
               10  wfskey1     pic x.
               10  wfskey2     pic x.
           05  wprintfilnamn   pic x(60).
           05  x               pic s9(4) comp.
           
       linkage section. 
       copy "vra.cpy" replacing leading ==xxxx== by vec.
       
       01  filtyp              pic 9. *> txt = 1, xls = 2
       
       procedure division.
       a-huvud section.
       a010.
       a999.
           exit.
       
       b-entry-RappDivSkapaRappfiler section.
       b010.
           entry "RappDivSkapaRappfiler" using vec, filtyp.
           if vec-antal = 0
               go to b999.
           if filtyp = 1
               move "\spadfiler\print.txt" to wprintfilnamn.
           if filtyp = 2
               move "\spadfiler\print.xls" to wprintfilnamn.
           open output Print.
           move 1 to x.
           perform varying x from 1 by 1 until x > vec-antal
               move vec-ra(x) to wra
               write Print-rad from wra-Rad
           end-perform.
           close Print.  
     
       b999.
           exit program.
           