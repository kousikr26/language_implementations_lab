0     COPY      START     0                   
0000            EXTDEF    BUFFER,BUFEND,LENGTH
0000            EXTREF    RDREC,WRREC         
0000  FIRST     STL       RETADR              
0003  CLOOP     +JSUB     RDREC               
0007            LDA       LENGTH              
000A            COMP      #0                  
000D            JEQ       ENDFIL              
0010            +JSUB     WRREC               
0014            J         CLOOP               
0017  ENDFIL    LDA       =C'EOF'             
001A            STA       BUFFER              
001D            LDA       #3                  
0020            STA       LENGTH              
0023            +JSUB     WRREC               
0027            J         @RETADR             
002A  RETADR    RESW      1                   
002D  LENGTH    RESW      1                   
0030            *         C'EOF'              
0033  BUFFER    RESB      4096                
0000  RDREC     CSECT                         
0000            EXTREF    BUFFER,LENGTH,BUFEND
0000            CLEAR     X                   
0002            CLEAR     A                   
0004            CLEAR     S                   
0006            LDT       MAXLEN              
0009  RLOOP     TD        INPUT               
000C            JEQ       RLOOP               
000F            RD        INPUT               
0012            COMPR     A,S                 
0014            JEQ       EXIT                
0017            +STCH     BUFFER,X            
001B            TIXR      T                   
001D            JLT       RLOOP               
0020  EXIT      +STX      LENGTH              
0024            RSUB                          
0027  INPUT     BYTE      X'F1'               
0028  MAXLEN    WORD      BUFEND-BUFFER       
0000  WRREC     CSECT                         
0000            EXTREF    LENGTH,BUFFER       
0000            CLEAR     X                   
0002            +LDT      LENGTH              
0006  WLOOP     TD        =X'05'              
0009            JEQ       WLOOP               
000C            +LDCH     BUFFER,X            
0010            WD        =X'05'              
0013            TIXR      T                   
0015            JLT       WLOOP               
0018            RSUB                          
001B            *         X'05'               
001C            END       FIRST               
