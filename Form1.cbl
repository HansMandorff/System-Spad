       class-id RappHant.Form1 is partial
                 inherits type System.Windows.Forms.Form.
      *          
      * Printfunktionen inte färdigutredd, det blir fult i textfilen.
      * Jag ser bnollor, mycket jobb att följa från databas till notepad.
      * Har tagit bort centrering, 7:an, i c-formatteringen, nu bättre
      * 2016-10-23  Nu går det att toggla mellan sql server och ibm db2
      *             Lagt in komu, vidareutveclinh av err
      * 
       working-storage section.
       01  wkomu. copy "komu.cpy" replacing leading ==xxxx== by ==wkomu==. 
           copy "vrt".
       01  wrt. copy "rapptyp.cpy" replacing leading ==xxxx== by ==wrt==.
           copy "vra" replacing leading ==xxxx== by ==vralb2==.
           copy "vra" replacing leading ==xxxx== by ==vralb3==.
       01  wra. copy "rapp.cpy "replacing leading ==xxxx== by ==wra==.
       01  wlb1-rapptyp-grupp.
           05  wlb1-rapptyp occurs 10 pic s9(9) comp-5.  *> mest binära/effektiva nu för tiden?
       01  wtest pic x(100).   
       
       method-id NEW.
       procedure division.
           invoke self::InitializeComponent
           goback.
       end method.
       
       method-id Form1_Load final private.
       procedure division using by value sender as object e as type System.EventArgs.
            set textBox1::Visible to true
            set textBox1::Visible = true
            set textBox1::Text to "Plats för felmeddelande från databashanteringen"
            set wtest to textBox1::Text *> testing
            move textBox1::Text to wtest *> testing
            set radioButton1::Checked = true
            set radioButton2::Checked to false
      *     invoke Ladda::PerformClick. *> där satt den! klickning görs nu pga check button true
       end method.
       
       method-id Laddaxxx. *> nu gick det med subrutin utan att trixa med en knapp
       working-storage section.
       01  wrapptyp pic 9(9).
       01  wbestnummer pic 9(9).
       01  work.
            05  x pic s9(9) comp-5.
            05  redrad.
                10  redant  pic z9.
                10  stycken pic xxxx.
                10  rednamn pic x(30).
        copy "vra.cpy".
        01  wra. copy "rapp.cpy"
            replacing leading ==xxxx== by ==wra==.
                   
       procedure division. *> using by value sender as object e as type System.EventArgs. BORT MED DETTA
           call "db-select-rapptyp-lista" using wkomu, vrt.
           invoke listBox1::Items::Clear.
           invoke listBox2::Items::Clear.
           invoke listBox3::Items::Clear.
           move low-values to wlb1-rapptyp-grupp.
           perform varying x from 1 by 1 until x > vrt-antal
               move vrt-rt(x) to wrt
               move spaces to redrad
               move " st" to stycken
               move wrt-RappNamn to rednamn
               move wrt-Antal to redant
               invoke listBox1::Items::Add(redrad)
               move wrt-RappTyp to wlb1-rapptyp(x)
           end-perform.
               
       end method.

       
       method-id listBox1_SelectedIndexChanged final private.
       working-storage section.
       01  work.
           05  wrapptyp pic 9(9).
           05  selind   pic 9(9).
           05  x        pic s9(4) comp.
        01  redrad.         
           10  BestNummer       pic zzz9.  
	       10  Datum            pic z999999.
           10  Klock            pic z999999.
           10  filler           pic xx.
	       10  Rad              pic x(200).  
	   
       procedure division using by value sender as object e as type System.EventArgs.
           move listBox1::SelectedIndex to selind.
           add 1 to selind.
           move wlb1-rapptyp(selind) to wrapptyp.
           call "db-select-rapp-lista" using wkomu, wrapptyp, vralb2.
           invoke listBox2::Items::Clear
           perform varying x from 1 by 1 until x > vralb2-antal
               move vralb2-ra(x) to wra
               move spaces to redrad
               move wra-BestNummer to BestNummer
               move wra-RadNummer to Datum
               move wra-RappTyp to Klock
               move wra-Rad to Rad
               invoke listBox2::Items::Add(redrad)
           end-perform.
       
       end method.
      
       method-id listBox2_SelectedIndexChanged final private.
       working-storage section.
       01  work.
           05  wbestnr pic 9(9).
           05  selind pic s9(4) comp.
           05  x pic s9(4) comp.
       01  redrad.
           05  Rad              pic x(200).  
	    
       procedure division using by value sender as object e as type System.EventArgs.
           move listBox2::SelectedIndex to selind.
           add 1 to selind.
           move vralb2-ra(selind) to wra.
           move wra-BestNummer to wbestnr.
           call "db-select-rapp" using wkomu, wbestnr, vralb3.
           invoke listBox3::Items::Clear.
           perform varying x from 1 by 1 until x > vralb3-antal
               move vralb3-ra(x) to wra
               move spaces to redrad
               move wra-Rad to Rad
               invoke listBox3::Items::Add(redrad)
           end-perform.
           
       end method.

       method-id listBox3_SelectedIndexChanged final private.
       procedure division using by value sender as object e as type System.EventArgs.
       end method.

       method-id Ladda_Click final. *> denna kod och knapp kan rensas bort, ersatt av metod utan knapp
       
       working-storage section.
       01  wrapptyp pic 9(9).
       01  wbestnummer pic 9(9).
       01  work.
            05  x pic s9(9) comp-5.
            05  redrad.
                10  redant  pic z9.
                10  stycken pic xxxx.
                10  rednamn pic x(30).
        copy "vra.cpy".
        01  wra. copy "rapp.cpy"
            replacing leading ==xxxx== by ==wra==.
                   
       procedure division using by value sender as object e as type System.EventArgs.
           call "db-select-rapptyp-lista" using wkomu, vrt.
           invoke listBox1::Items::Clear.
           invoke listBox2::Items::Clear.
           invoke listBox3::Items::Clear.
           move low-values to wlb1-rapptyp-grupp.
           perform varying x from 1 by 1 until x > vrt-antal
               move vrt-rt(x) to wrt
               move spaces to redrad
               move " st" to stycken
               move wrt-RappNamn to rednamn
               move wrt-Antal to redant
               invoke listBox1::Items::Add(redrad)
               move wrt-RappTyp to wlb1-rapptyp(x)
           end-perform.
               
       end method.

       method-id Printtxt_Click final private.
       working-storage section.
       01  command-line  pic x(40) value "notepad \spadfiler\print.txt".
       01  status-val    pic s9.
       01  filtyp       pic 9.
       
       procedure division using by value sender as object e as type System.EventArgs.
           move 1 to filtyp.
           call "RappDivSkapaRappfiler" using vralb3, filtyp. 
           call 'c$run' using command-line giving status-val.
      * Titta vad jag hittat! i dok för ims dl1 cobol    
      *    evaluate filtyp
      *    when 1 display "1"
      *    when 2 display "2"
      *    when other display "9"
      *    end-evaluate.
           
           
       end method.

       method-id Printxls_Click final private.
      *
      * plats för dok om vad och varför Spadstarter2
      * 
       working-storage section.
       01  command-line  pic x(100) value "\dev\spadstarter2\debug\Spadstarter2.exe".
       01  status-val    pic s9.
       01  filtyp        pic 9.
       
       procedure division using by value sender as object e as type System.EventArgs.
           move 2 to filtyp.
           call "RappDivSkapaRappfiler" using vralb3, filtyp. 
           call 'c$run' using command-line giving status-val.
           move 0 to filtyp.
       end method.

       method-id Avsluta_Click final private.
       procedure division using by value sender as object e as type System.EventArgs.
       stop run.
       end method.

       method-id radioButton1_CheckedChanged final private. *> sql server mss
       procedure division using by value sender as object e as type System.EventArgs.
           if radioButton1::Checked = true
               move 1 to wkomu-KOMMANDO1
               set wkomu-KOMMANDO1 to 1 *> test
               set textBox1::Text to "Aktuell databas är MS SQL SERVER (1)"
               set radioButton2::Checked = false
      *        invoke Ladda::PerformClick *> där satt den!
               invoke self::Laddaxxx *>går bra utan setf::
           end-if.
       end method.

       method-id radioButton2_CheckedChanged final private. *> ibm db2
       procedure division using by value sender as object e as type System.EventArgs.
           if radioButton2::Checked = true
               move 2 to wkomu-KOMMANDO1
               set textBox1::Text to "Aktuell databas är IBM DB2 (2)"
               set radioButton1::Checked = false
               invoke self::Laddaxxx *>::PerformClick *> där satt den!
           end-if.
       end method.
       
       end class.
