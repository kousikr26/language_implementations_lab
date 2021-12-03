1000  COPY      START     1000                
1000  FIRST     STL       RETADR              
1003  CLOOP     JSUB      RDREC               
1006            LDA       LENGTH              
1009            COMP      ZERO                
100C            JEQ       ENDFIL              
100F            JSUB      WRREC               
1012            J         CLOOP               
1015  ENDFIL    LDA       EOF                 
1018            STA       BUFFER              
101B            LDA       THREE               
101E            STA       LENGTH              
1021            JSUB      WRREC               
1024            LDL       RETADR              
1027            RSUB                          
102A  EOF       BYTE      C'EOF'              
102D  THREE     WORD      3                   
1030  ZERO      WORD      0                   
1033  RETADR    RESW      1                   
1036  LENGTH    RESW      1                   
1039  BUFFER    RESB      4096                
2039  RDREC     LDX       ZERO                
203C            LDA       ZERO                
203F  RLOOP     TD        INPUT               
2042            JEQ       RLOOP               
2045            RD        INPUT               
2048            COMP      ZERO                
204B            JEQ       EXIT                
204E            STCH      BUFFER,X            
2051            TIX       MAXLEN              
2054            JLT       RLOOP               
2057  EXIT      STX       LENGTH              
205A            RSUB                          
205D  INPUT     BYTE      X'F1'               
205E  MAXLEN    WORD      4096                
2061  WRREC     LDX       ZERO                
2064  WLOOP     TD        OUTPUT              
2067            JEQ       WLOOP               
206A            LDCH      BUFFER,X            
206D            WD        OUTPUT              
2070            TIX       LENGTH              
2073            JLT       WLOOP               
2076            RSUB                          
2079  OUTPUT    BYTE      X'05'               
207A            END       FIRST               
