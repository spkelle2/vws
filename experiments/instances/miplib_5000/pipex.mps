*NAME:         pipex
*ROWS:         25
*COLUMNS:      48
*INTEGER:      48
*NONZERO:      192
*BEST SOLN:    788.263 (opt)
*LP SOLN:      773.751
*SOURCE:        
*              Laurence A. Wolsey (University of Louvain)
*              Martin W. P. Savelsbergh (Eindhoven Univ. of Technology)
*APPLICATION:  unknown
*COMMENTS:     pure 0/1 IP
*       
*       
NAME          PIPEX
ROWS
 N  TCOST...
 L  CAPL01..
 L  CAPL02..
 L  CAPL03..
 L  BAL101..
 L  BAL102..
 L  BAL103..
 G  BAL201..
 G  BAL202..
 G  BAL203..
 E  PROD01..
 E  PROD02..
 E  PROD03..
 E  PROD04..
 E  PROD05..
 E  PROD06..
 E  PROD07..
 E  PROD08..
 E  PROD09..
 E  PROD10..
 E  PROD11..
 E  PROD12..
 E  PROD13..
 E  PROD14..
 E  PROD15..
 E  PROD16..
COLUMNS
    MARK0000  'MARKER'                 'INTORG'
    QUAN0101  TCOST...          18.7   CAPL01..            11
    QUAN0101  BAL101..            -1   BAL201..            -1
    QUAN0101  PROD01..             1
    QUAN0102  TCOST...         22.04   CAPL01..          11.6
    QUAN0102  BAL101..          -1.4   BAL201..          -1.4
    QUAN0102  PROD02..             1
    QUAN0103  TCOST...         29.28   CAPL01..          12.2
    QUAN0103  BAL101..          -1.8   BAL201..          -1.8
    QUAN0103  PROD03..             1
    QUAN0104  TCOST...         28.28   CAPL01..          12.9
    QUAN0104  BAL101..          -2.3   BAL201..          -2.3
    QUAN0104  PROD04..             1
    QUAN0105  TCOST...         23.58   CAPL01..          13.1
    QUAN0105  BAL101..          -3.1   BAL201..          -3.1
    QUAN0105  PROD05..             1
    QUAN0106  TCOST...         23.46   CAPL01..          13.8
    QUAN0106  BAL101..            -3   BAL201..            -3
    QUAN0106  PROD06..             1
    QUAN0107  TCOST...            36   CAPL01..          14.4
    QUAN0107  BAL101..            -4   BAL201..            -4
    QUAN0107  PROD07..             1
    QUAN0108  TCOST...         39.52   CAPL01..          15.2
    QUAN0108  BAL101..            -5   BAL201..            -5
    QUAN0108  PROD08..             1
    QUAN0109  TCOST...         69.24   CAPL01..            12
    QUAN0109  BAL101..             4   BAL201..             4
    QUAN0109  PROD09..             1
    QUAN0110  TCOST...        84.459   CAPL01..          14.1
    QUAN0110  BAL101..           5.9   BAL201..           5.9
    QUAN0110  PROD10..             1
    QUAN0111  TCOST...        79.134   CAPL01..          12.1
    QUAN0111  BAL101..           4.1   BAL201..           4.1
    QUAN0111  PROD11..             1
    QUAN0112  TCOST...        71.416   CAPL01..          11.3
    QUAN0112  BAL101..           2.7   BAL201..           2.7
    QUAN0112  PROD12..             1
    QUAN0113  TCOST...        59.976   CAPL01..          10.2
    QUAN0113  BAL101..           2.4   BAL201..           2.4
    QUAN0113  PROD13..             1
    QUAN0114  TCOST...        57.123   CAPL01..           9.9
    QUAN0114  BAL101..           1.9   BAL201..           1.9
    QUAN0114  PROD14..             1
    QUAN0115  TCOST...        60.515   CAPL01..           9.1
    QUAN0115  BAL101..           1.1   BAL201..           1.1
    QUAN0115  PROD15..             1
    QUAN0116  TCOST...         60.84   CAPL01..             9
    QUAN0116  BAL101..           0.8   BAL201..           0.8
    QUAN0116  PROD16..             1
    QUAN0201  TCOST...          25.3   CAPL02..            11
    QUAN0201  BAL102..            -1   BAL202..            -1
    QUAN0201  PROD01..             1
    QUAN0202  TCOST...         32.38   CAPL02..          11.6
    QUAN0202  BAL102..          -1.4   BAL202..          -1.4
    QUAN0202  PROD02..             1
    QUAN0203  TCOST...         35.38   CAPL02..          12.2
    QUAN0203  BAL102..          -1.8   BAL202..          -1.8
    QUAN0203  PROD03..             1
    QUAN0204  TCOST...         33.54   CAPL02..          12.9
    QUAN0204  BAL102..          -2.3   BAL202..          -2.3
    QUAN0204  PROD04..             1
    QUAN0205  TCOST...         35.27   CAPL02..          13.1
    QUAN0205  BAL102..          -3.1   BAL202..          -3.1
    QUAN0205  PROD05..             1
    QUAN0206  TCOST...         33.12   CAPL02..          13.8
    QUAN0206  BAL102..            -3   BAL202..            -3
    QUAN0206  PROD06..             1
    QUAN0207  TCOST...         25.92   CAPL02..          14.4
    QUAN0207  BAL102..            -4   BAL202..            -4
    QUAN0207  PROD07..             1
    QUAN0208  TCOST...         25.84   CAPL02..          15.2
    QUAN0208  BAL102..            -5   BAL202..            -5
    QUAN0208  PROD08..             1
    QUAN0209  TCOST...          76.2   CAPL02..            12
    QUAN0209  BAL102..             4   BAL202..             4
    QUAN0209  PROD09..             1
    QUAN0210  TCOST...         97.29   CAPL02..          14.1
    QUAN0210  BAL102..           5.9   BAL202..           5.9
    QUAN0210  PROD10..             1
    QUAN0211  TCOST...        84.821   CAPL02..          12.1
    QUAN0211  BAL102..           4.1   BAL202..           4.1
    QUAN0211  PROD11..             1
    QUAN0212  TCOST...        75.484   CAPL02..          11.3
    QUAN0212  BAL102..           2.7   BAL202..           2.7
    QUAN0212  PROD12..             1
    QUAN0213  TCOST...        69.258   CAPL02..          10.2
    QUAN0213  BAL102..           2.4   BAL202..           2.4
    QUAN0213  PROD13..             1
    QUAN0214  TCOST...        63.954   CAPL02..           9.9
    QUAN0214  BAL102..           1.9   BAL202..           1.9
    QUAN0214  PROD14..             1
    QUAN0215  TCOST...         52.78   CAPL02..           9.1
    QUAN0215  BAL102..           1.1   BAL202..           1.1
    QUAN0215  PROD15..             1
    QUAN0216  TCOST...         51.21   CAPL02..             9
    QUAN0216  BAL102..           0.8   BAL202..           0.8
    QUAN0216  PROD16..             1
    QUAN0301  TCOST...            22   CAPL03..            11
    QUAN0301  BAL103..            -1   BAL203..            -1
    QUAN0301  PROD01..             1
    QUAN0302  TCOST...         30.16   CAPL03..          11.6
    QUAN0302  BAL103..          -1.4   BAL203..          -1.4
    QUAN0302  PROD02..             1
    QUAN0303  TCOST...         32.94   CAPL03..          12.2
    QUAN0303  BAL103..          -1.8   BAL203..          -1.8
    QUAN0303  PROD03..             1
    QUAN0304  TCOST...         36.12   CAPL03..          12.9
    QUAN0304  BAL103..          -2.3   BAL203..          -2.3
    QUAN0304  PROD04..             1
    QUAN0305  TCOST...          26.2   CAPL03..          13.1
    QUAN0305  BAL103..          -3.1   BAL203..          -3.1
    QUAN0305  PROD05..             1
    QUAN0306  TCOST...         26.22   CAPL03..          13.8
    QUAN0306  BAL103..            -3   BAL203..            -3
    QUAN0306  PROD06..             1
    QUAN0307  TCOST...         46.08   CAPL03..          14.4
    QUAN0307  BAL103..            -4   BAL203..            -4
    QUAN0307  PROD07..             1
    QUAN0308  TCOST...         48.64   CAPL03..          15.2
    QUAN0308  BAL103..            -5   BAL203..            -5
    QUAN0308  PROD08..             1
    QUAN0309  TCOST...         83.88   CAPL03..            12
    QUAN0309  BAL103..             4   BAL203..             4
    QUAN0309  PROD09..             1
    QUAN0310  TCOST...       107.865   CAPL03..          14.1
    QUAN0310  BAL103..           5.9   BAL203..           5.9
    QUAN0310  PROD10..             1
    QUAN0311  TCOST...        93.896   CAPL03..          12.1
    QUAN0311  BAL103..           4.1   BAL203..           4.1
    QUAN0311  PROD11..             1
    QUAN0312  TCOST...        88.931   CAPL03..          11.3
    QUAN0312  BAL103..           2.7   BAL203..           2.7
    QUAN0312  PROD12..             1
    QUAN0313  TCOST...        71.298   CAPL03..          10.2
    QUAN0313  BAL103..           2.4   BAL203..           2.4
    QUAN0313  PROD13..             1
    QUAN0314  TCOST...        68.112   CAPL03..           9.9
    QUAN0314  BAL103..           1.9   BAL203..           1.9
    QUAN0314  PROD14..             1
    QUAN0315  TCOST...         75.64   CAPL03..           9.1
    QUAN0315  BAL103..           1.1   BAL203..           1.1
    QUAN0315  PROD15..             1
    QUAN0316  TCOST...         74.79   CAPL03..             9
    QUAN0316  BAL103..           0.8   BAL203..           0.8
    QUAN0316  PROD16..             1
    MARK0001  'MARKER'                 'INTEND'
RHS
    RHSSET01  CAPL01..            45   CAPL02..            70
    RHSSET01  CAPL03..            80   BAL101..           4.5
    RHSSET01  BAL102..             7   BAL103..             8
    RHSSET01  BAL201..          -4.5   BAL202..            -7
    RHSSET01  BAL203..            -8   PROD01..             1
    RHSSET01  PROD02..             1   PROD03..             1
    RHSSET01  PROD04..             1   PROD05..             1
    RHSSET01  PROD06..             1   PROD07..             1
    RHSSET01  PROD08..             1   PROD09..             1
    RHSSET01  PROD10..             1   PROD11..             1
    RHSSET01  PROD12..             1   PROD13..             1
    RHSSET01  PROD14..             1   PROD15..             1
    RHSSET01  PROD16..             1
BOUNDS
 UP BOUND01   QUAN0101             1
 UP BOUND01   QUAN0102             1
 UP BOUND01   QUAN0103             1
 UP BOUND01   QUAN0104             1
 UP BOUND01   QUAN0105             1
 UP BOUND01   QUAN0106             1
 UP BOUND01   QUAN0107             1
 UP BOUND01   QUAN0108             1
 UP BOUND01   QUAN0109             1
 UP BOUND01   QUAN0110             1
 UP BOUND01   QUAN0111             1
 UP BOUND01   QUAN0112             1
 UP BOUND01   QUAN0113             1
 UP BOUND01   QUAN0114             1
 UP BOUND01   QUAN0115             1
 UP BOUND01   QUAN0116             1
 UP BOUND01   QUAN0201             1
 UP BOUND01   QUAN0202             1
 UP BOUND01   QUAN0203             1
 UP BOUND01   QUAN0204             1
 UP BOUND01   QUAN0205             1
 UP BOUND01   QUAN0206             1
 UP BOUND01   QUAN0207             1
 UP BOUND01   QUAN0208             1
 UP BOUND01   QUAN0209             1
 UP BOUND01   QUAN0210             1
 UP BOUND01   QUAN0211             1
 UP BOUND01   QUAN0212             1
 UP BOUND01   QUAN0213             1
 UP BOUND01   QUAN0214             1
 UP BOUND01   QUAN0215             1
 UP BOUND01   QUAN0216             1
 UP BOUND01   QUAN0301             1
 UP BOUND01   QUAN0302             1
 UP BOUND01   QUAN0303             1
 UP BOUND01   QUAN0304             1
 UP BOUND01   QUAN0305             1
 UP BOUND01   QUAN0306             1
 UP BOUND01   QUAN0307             1
 UP BOUND01   QUAN0308             1
 UP BOUND01   QUAN0309             1
 UP BOUND01   QUAN0310             1
 UP BOUND01   QUAN0311             1
 UP BOUND01   QUAN0312             1
 UP BOUND01   QUAN0313             1
 UP BOUND01   QUAN0314             1
 UP BOUND01   QUAN0315             1
 UP BOUND01   QUAN0316             1
ENDATA
