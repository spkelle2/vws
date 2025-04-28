*NAME:         diamond
*ROWS:         4
*COLUMNS:      2
*INTEGER:      2
*NONZERO:      8
*BEST SOLN:    integer infeasible
*LP SOLN:      -1.0
*SOURCE:       John W. Gregory (Cray Research)          
*              John W. Gregory (Cray Research)
*              E. Andrew Boyd (Rice University)
*APPLICATION:  this problem created solely for debugging 
*COMMENTS:     LP feasible
*              pure 0/1 IP                     
*      
NAME          DIAMOND
ROWS
 N  OBJ     
 G  ROW1    
 L  ROW2    
 L  ROW3    
 L  ROW4    
COLUMNS
    MARK0000  'MARKER'                 'INTORG'
    X         OBJ                 -1   ROW1                 1
    X         ROW2                 1   ROW3                 1
    X         ROW4                -1
    Y         ROW1                 1   ROW2                 1
    Y         ROW3                -1   ROW4                 1
    MARK0001  'MARKER'                 'INTEND'
RHS
    RHS1      ROW1               0.5   ROW2               1.5
    RHS1      ROW3               0.5   ROW4               0.5
BOUNDS
 UP BD        X                    1
 UP BD        Y                    1
ENDATA
