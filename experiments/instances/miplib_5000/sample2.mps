*NAME:         sample2
*ROWS:         45
*COLUMNS:      67
*INTEGER:      21
*NONZERO:      146
*BEST SOLN:    375 (opt)
*LP SOLN:      247.0
*SOURCE:        
*              Laurence A. Wolsey (University of Louvain)
*              Martin W. P. Savelsbergh (Eindhoven Univ. of Technology)
*APPLICATION:  unknown
*COMMENTS:     all integer variables are binary
*       
*       
NAME          SAMPLE2
ROWS
 N  COST    
 E  ...     
 E  F01     
 E  F02     
 E  F03     
 E  W01I    
 E  W01O    
 E  W02I    
 E  W02O    
 E  W03I    
 E  W03O    
 E  D01I    
 E  D01O    
 E  D02I    
 E  D02O    
 E  D03I    
 E  D03O    
 E  D04I    
 E  D04O    
 E  C01     
 E  C02     
 E  C03     
 L  SIC01   
 L  SIC02   
 L  SIC03   
 L  U....F01
 L  U....F02
 L  U....F03
 L  U.W01W01
 L  U.W02W02
 L  U.W03W03
 L  U.D01D01
 L  U.D02D02
 L  U.D03D03
 L  U.D01C01
 L  U.D01C02
 L  U.D01C03
 L  U.D02C01
 L  U.D02C02
 L  U.D02C03
 L  U.D03C01
 L  U.D03C02
 L  U.D03C03
 L  U.D04C01
 L  U.D04C02
 L  U.D04C03
COLUMNS
    MARK0000  'MARKER'                 'INTORG'
    I....F01  COST               100   U....F01           -45
    I....F02  COST               100   U....F02           -45
    I....F03  COST               100   U....F03           -45
    I.W01W01  COST                60   U.W01W01           -45
    I.W02W02  COST                60   U.W02W02           -45
    I.W03W03  COST                60   U.W03W03           -45
    I.D01D01  COST                40   U.D01D01           -25
    I.D02D02  COST                40   U.D02D02           -25
    I.D03D03  COST                40   U.D03D03           -25
    I.D01C01  SIC01                1   U.D01C01           -45
    I.D01C02  SIC02                1   U.D01C02           -45
    I.D01C03  SIC03                1   U.D01C03           -45
    I.D02C01  SIC01                1   U.D02C01           -45
    I.D02C02  SIC02                1   U.D02C02           -45
    I.D02C03  SIC03                1   U.D02C03           -45
    I.D03C01  SIC01                1   U.D03C01           -45
    I.D03C02  SIC02                1   U.D03C02           -45
    I.D03C03  SIC03                1   U.D03C03           -45
    I.D04C01  SIC01                1   U.D04C01           -45
    I.D04C02  SIC02                1   U.D04C02           -45
    I.D04C03  SIC03                1   U.D04C03           -45
    MARK0001  'MARKER'                 'INTEND'
    F....F01  ...                  1   F01                 -1
    F....F01  U....F01             1
    F....F02  ...                  1   F02                 -1
    F....F02  U....F02             1
    F....F03  ...                  1   F03                 -1
    F....F03  U....F03             1
    F.F01W01  COST                 1   F01                  1
    F.F01W01  W01I                -1
    F.F01W02  COST                 2   F01                  1
    F.F01W02  W02I                -1
    F.F01W03  F01                  1   W03I                -1
    F.F02W01  F02                  1   W01I                -1
    F.F02W02  COST                 1   F02                  1
    F.F02W02  W02I                -1
    F.F02W03  COST                 2   F02                  1
    F.F02W03  W03I                -1
    F.F03W01  COST                 2   F03                  1
    F.F03W01  W01I                -1
    F.F03W02  F03                  1   W02I                -1
    F.F03W03  COST                 1   F03                  1
    F.F03W03  W03I                -1
    F.W01W01  W01I                 1   W01O                -1
    F.W01W01  U.W01W01             1
    F.W02W02  W02I                 1   W02O                -1
    F.W02W02  U.W02W02             1
    F.W03W03  W03I                 1   W03O                -1
    F.W03W03  U.W03W03             1
    F.W01D01  W01O                 1   D01I                -1
    F.W01D02  COST                 2   W01O                 1
    F.W01D02  D02I                -1
    F.W01D03  COST                 4   W01O                 1
    F.W01D03  D03I                -1
    F.W01D04  COST                 6   W01O                 1
    F.W01D04  D04I                -1
    F.W02D01  COST                 6   W02O                 1
    F.W02D01  D01I                -1
    F.W02D02  W02O                 1   D02I                -1
    F.W02D03  COST                 2   W02O                 1
    F.W02D03  D03I                -1
    F.W02D04  COST                 4   W02O                 1
    F.W02D04  D04I                -1
    F.W03D01  COST                 4   W03O                 1
    F.W03D01  D01I                -1
    F.W03D02  COST                 6   W03O                 1
    F.W03D02  D02I                -1
    F.W03D03  W03O                 1   D03I                -1
    F.W03D04  COST                 2   W03O                 1
    F.W03D04  D04I                -1
    F.D01D01  D01I                 1   D01O                -1
    F.D01D01  U.D01D01             1
    F.D02D02  D02I                 1   D02O                -1
    F.D02D02  U.D02D02             1
    F.D03D03  D03I                 1   D03O                -1
    F.D03D03  U.D03D03             1
    F.D04D04  D04I                 1   D04O                -1
    F.D01C01  COST                 1   D01O                 1
    F.D01C01  C01                 -1   U.D01C01             1
    F.D01C02  D01O                 1   C02                 -1
    F.D01C02  U.D01C02             1
    F.D01C03  COST                 3   D01O                 1
    F.D01C03  C03                 -1   U.D01C03             1
    F.D02C01  COST                 1   D02O                 1
    F.D02C01  C01                 -1   U.D02C01             1
    F.D02C02  COST                 3   D02O                 1
    F.D02C02  C02                 -1   U.D02C02             1
    F.D02C03  D02O                 1   C03                 -1
    F.D02C03  U.D02C03             1
    F.D03C01  COST                 3   D03O                 1
    F.D03C01  C01                 -1   U.D03C01             1
    F.D03C02  D03O                 1   C02                 -1
    F.D03C02  U.D03C02             1
    F.D03C03  COST                 1   D03O                 1
    F.D03C03  C03                 -1   U.D03C03             1
    F.D04C01  COST                 3   D04O                 1
    F.D04C01  C01                 -1   U.D04C01             1
    F.D04C02  COST                 3   D04O                 1
    F.D04C02  C02                 -1   U.D04C02             1
    F.D04C03  COST                 3   D04O                 1
    F.D04C03  C03                 -1   U.D04C03             1
    F.C01...  ...                 -1   C01                  1
    F.C02...  ...                 -1   C02                  1
    F.C03...  ...                 -1   C03                  1
RHS
    RHS       SIC01                1   SIC02                1
    RHS       SIC03                1
BOUNDS
 UP BOUNDS    I....F01             1
 UP BOUNDS    I....F02             1
 UP BOUNDS    I....F03             1
 UP BOUNDS    I.W01W01             1
 UP BOUNDS    I.W02W02             1
 UP BOUNDS    I.W03W03             1
 UP BOUNDS    I.D01D01             1
 UP BOUNDS    I.D02D02             1
 UP BOUNDS    I.D03D03             1
 UP BOUNDS    I.D01C01             1
 UP BOUNDS    I.D01C02             1
 UP BOUNDS    I.D01C03             1
 UP BOUNDS    I.D02C01             1
 UP BOUNDS    I.D02C02             1
 UP BOUNDS    I.D02C03             1
 UP BOUNDS    I.D03C01             1
 UP BOUNDS    I.D03C02             1
 UP BOUNDS    I.D03C03             1
 UP BOUNDS    I.D04C01             1
 UP BOUNDS    I.D04C02             1
 UP BOUNDS    I.D04C03             1
 UP BOUNDS    F.D04D04            25
 FX BOUNDS    F.C01...            15
 FX BOUNDS    F.C02...            15
 FX BOUNDS    F.C03...            15
ENDATA
