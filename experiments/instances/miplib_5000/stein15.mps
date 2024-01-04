*NAME:         stein15
*ROWS:         36
*COLUMNS:      15
*INTEGER:      15
*NONZERO:      120
*BEST SOLN:    9 (opt)
*LP SOLN:      7.0
*SOURCE:       George L. Nemhauser (Georgia Institute of Technology)
*              John W. Gregory (Cray Research)
*              E. Andrew Boyd (Rice University)                        
*APPLICATION:  unknown
*COMMENTS:     pure 0/1 IP
*      
*      
NAME          STEIN15
ROWS
 N  OBJ     
 G  A1      
 G  A2      
 G  A3      
 G  A4      
 G  A5      
 G  A6      
 G  A7      
 G  A8      
 G  A9      
 G  A10     
 G  A11     
 G  A12     
 G  A13     
 G  A14     
 G  A15     
 G  A16     
 G  A17     
 G  A18     
 G  A19     
 G  A20     
 G  A21     
 G  A22     
 G  A23     
 G  A24     
 G  A25     
 G  A26     
 G  A27     
 G  A28     
 G  A29     
 G  A30     
 G  A31     
 G  A32     
 G  A33     
 G  A34     
 G  A35     
 G  OB2     
COLUMNS
    MARK0000  'MARKER'                 'INTORG'
    0001      OBJ                  1   A3                   1
    0001      A4                   1   A7                   1
    0001      A10                  1   A21                  1
    0001      A26                  1   A31                  1
    0001      OB2                  1
    0002      OBJ                  1   A4                   1
    0002      A5                   1   A6                   1
    0002      A8                   1   A22                  1
    0002      A27                  1   A32                  1
    0002      OB2                  1
    0003      OBJ                  1   A1                   1
    0003      A5                   1   A7                   1
    0003      A9                   1   A23                  1
    0003      A28                  1   A33                  1
    0003      OB2                  1
    0004      OBJ                  1   A1                   1
    0004      A2                   1   A8                   1
    0004      A10                  1   A24                  1
    0004      A29                  1   A34                  1
    0004      OB2                  1
    0005      OBJ                  1   A2                   1
    0005      A3                   1   A6                   1
    0005      A9                   1   A25                  1
    0005      A30                  1   A35                  1
    0005      OB2                  1
    0006      OBJ                  1   A1                   1
    0006      A6                   1   A13                  1
    0006      A14                  1   A17                  1
    0006      A20                  1   A31                  1
    0006      OB2                  1
    0007      OBJ                  1   A2                   1
    0007      A7                   1   A14                  1
    0007      A15                  1   A16                  1
    0007      A18                  1   A32                  1
    0007      OB2                  1
    0008      OBJ                  1   A3                   1
    0008      A8                   1   A11                  1
    0008      A15                  1   A17                  1
    0008      A19                  1   A33                  1
    0008      OB2                  1
    0009      OBJ                  1   A4                   1
    0009      A9                   1   A11                  1
    0009      A12                  1   A18                  1
    0009      A20                  1   A34                  1
    0009      OB2                  1
    0010      OBJ                  1   A5                   1
    0010      A10                  1   A12                  1
    0010      A13                  1   A16                  1
    0010      A19                  1   A35                  1
    0010      OB2                  1
    0011      OBJ                  1   A11                  1
    0011      A16                  1   A23                  1
    0011      A24                  1   A27                  1
    0011      A30                  1   A31                  1
    0011      OB2                  1
    0012      OBJ                  1   A12                  1
    0012      A17                  1   A24                  1
    0012      A25                  1   A26                  1
    0012      A28                  1   A32                  1
    0012      OB2                  1
    0013      OBJ                  1   A13                  1
    0013      A18                  1   A21                  1
    0013      A25                  1   A27                  1
    0013      A29                  1   A33                  1
    0013      OB2                  1
    0014      OBJ                  1   A14                  1
    0014      A19                  1   A21                  1
    0014      A22                  1   A28                  1
    0014      A30                  1   A34                  1
    0014      OB2                  1
    0015      OBJ                  1   A15                  1
    0015      A20                  1   A22                  1
    0015      A23                  1   A26                  1
    0015      A29                  1   A35                  1
    0015      OB2                  1
    MARK0001  'MARKER'                 'INTEND'
RHS
    RHS       A1                   1   A2                   1
    RHS       A3                   1   A4                   1
    RHS       A5                   1   A6                   1
    RHS       A7                   1   A8                   1
    RHS       A9                   1   A10                  1
    RHS       A11                  1   A12                  1
    RHS       A13                  1   A14                  1
    RHS       A15                  1   A16                  1
    RHS       A17                  1   A18                  1
    RHS       A19                  1   A20                  1
    RHS       A21                  1   A22                  1
    RHS       A23                  1   A24                  1
    RHS       A25                  1   A26                  1
    RHS       A27                  1   A28                  1
    RHS       A29                  1   A30                  1
    RHS       A31                  1   A32                  1
    RHS       A33                  1   A34                  1
    RHS       A35                  1   OB2                  7
BOUNDS
 UP bnd       0001                 1
 UP bnd       0002                 1
 UP bnd       0003                 1
 UP bnd       0004                 1
 UP bnd       0005                 1
 UP bnd       0006                 1
 UP bnd       0007                 1
 UP bnd       0008                 1
 UP bnd       0009                 1
 UP bnd       0010                 1
 UP bnd       0011                 1
 UP bnd       0012                 1
 UP bnd       0013                 1
 UP bnd       0014                 1
 UP bnd       0015                 1
ENDATA
