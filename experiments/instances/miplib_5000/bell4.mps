*NAME:         bell4
*ROWS:         105
*COLUMNS:      117
*INTEGER:      64
*NONZERO:      302
*BEST SOLN:    18541484.20 (opt)
*LP SOLN:      17984775.91
*SOURCE:       William Cook (Bellcore)
*              William Cook (Bellcore)
*              William Cook (Bellcore)
*APPLICATION:  fiber optic network design
*COMMENTS:     34 of the integer variables are binary
*              hard problem
*              solved with new code based on Lovasz-Scarf basis reduction      
NAME          BELL4
ROWS
 N  OBJ     
 L  A1      
 L  A2      
 L  A3      
 L  A4      
 L  A5      
 L  A6      
 L  A7      
 L  A8      
 L  A9      
 L  A10     
 L  A11     
 L  A12     
 L  A13     
 L  A14     
 L  A15     
 L  A16     
 L  A17     
 L  A18     
 L  B1      
 L  B2      
 L  B3      
 L  B4      
 L  B5      
 L  B6      
 L  B7      
 L  B8      
 L  B9      
 L  B10     
 L  B11     
 L  B12     
 L  B13     
 L  B15     
 L  B19     
 L  C1      
 L  C2      
 L  C3      
 L  C4      
 L  C5      
 L  C6      
 L  C7      
 L  C8      
 L  C9      
 L  C10     
 L  C11     
 L  C12     
 L  C13     
 L  C14     
 L  C15     
 L  C16     
 L  C17     
 L  C18     
 L  C19     
 L  D1      
 L  D2      
 L  D3      
 L  D4      
 L  D5      
 L  D6      
 L  D7      
 L  D8      
 L  D9      
 L  D10     
 L  D11     
 L  D12     
 L  D13     
 L  D15     
 L  D19     
 L  E1      
 L  E2      
 L  E3      
 L  E4      
 L  E5      
 L  E6      
 L  E7      
 L  E8      
 L  E9      
 L  E10     
 L  E11     
 L  E12     
 L  E13     
 L  E14     
 L  E15     
 L  E16     
 L  E17     
 L  E18     
 L  E19     
 L  F0      
 L  F1      
 L  F2      
 L  F3      
 L  F4      
 L  F5      
 L  F6      
 L  F7      
 L  F8      
 L  F9      
 L  F10     
 L  F11     
 L  F12     
 L  F13     
 L  F14     
 L  F15     
 L  F16     
 L  F17     
 L  F18     
COLUMNS
    MARK0000  'MARKER'                 'INTORG'
    c1        A1                  -1   B1                 -20
    c1        F0                   1
    c2        A1                   1   A2                  -1
    c2        B2                 -20   F1                   1
    c3        A2                   1   A3                  -1
    c3        B3                 -20   F2                   1
    c4        A3                   1   A4                  -1
    c4        B4                 -20   F3                   1
    c5        A4                   1   A5                  -1
    c5        A10                 -1   B5                 -20
    c5        F4                   1
    c6        A5                   1   A6                  -1
    c6        A9                  -1   B6                 -20
    c6        F5                   1
    c7        A6                   1   A7                  -1
    c7        B7                 -20   F6                   1
    c8        A7                   1   A8                  -1
    c8        B8                 -20   F7                   1
    c9        A8                   1   B9                 -20
    c9        F8                   1
    c10       A9                   1   B10                -20
    c10       F9                   1
    c11       A10                  1   A11                 -1
    c11       A16                 -1   B11                -20
    c11       F10                  1
    c12       A11                  1   A12                 -1
    c12       A13                 -1   B12                -20
    c12       F11                  1
    c13       A12                  1   B13                -20
    c13       F12                  1
    c14       A13                  1   A14                 -1
    c14       F13                  1
    c15       A14                  1   A15                 -1
    c15       B15                -20   F14                  1
    c16       A15                  1   F15                  1
    c17       A16                  1   A17                 -1
    c17       F16                  1
    c18       A17                  1   A18                 -1
    c18       F17                  1
    c19       A18                  1   B19                -20
    c19       F18                  1
    d1        OBJ              43000   B1                   1
    d1        C1                -672
    d2        OBJ              43000   B2                   1
    d2        C2                -672
    d3        OBJ              43000   B3                   1
    d3        C3                -672
    d4        OBJ              43000   B4                   1
    d4        C4                -672
    d5        OBJ              43000   B5                   1
    d5        C5                -672
    d6        OBJ              43000   B6                   1
    d6        C6                -672
    d7        OBJ              43000   B7                   1
    d7        C7                -672
    d8        OBJ              43000   B8                   1
    d8        C8                -672
    d9        OBJ              43000   B9                   1
    d9        C9                -672
    d10       OBJ              43000   B10                  1
    d10       C10               -672
    d11       OBJ              43000   B11                  1
    d11       C11               -672
    d12       OBJ              43000   B12                  1
    d12       C12               -672
    d13       OBJ              43000   B13                  1
    d13       C13               -672
    d15       OBJ              43000   B15                  1
    d15       C15               -672
    d19       OBJ              43000   B19                  1
    d19       C19               -672
    h1        OBJ              58000   B1                   1
    h1        C1               -1344
    h2        OBJ              58000   B2                   1
    h2        C2               -1344
    h3        OBJ              58000   B3                   1
    h3        C3               -1344
    h4        OBJ              58000   B4                   1
    h4        C4               -1344
    h5        OBJ              58000   B5                   1
    h5        C5               -1344
    h6        OBJ              58000   B6                   1
    h6        C6               -1344
    h7        OBJ              59000   B7                   1
    h7        C7               -1344
    h8        OBJ              59000   B8                   1
    h8        C8               -1344
    h9        OBJ              59000   B9                   1
    h9        C9               -1344
    h10       OBJ              59000   B10                  1
    h10       C10              -1344
    h11       OBJ              58000   B11                  1
    h11       C11              -1344
    h12       OBJ              59000   B12                  1
    h12       C12              -1344
    h13       OBJ              59000   B13                  1
    h13       C13              -1344
    h15       OBJ              59000   B15                  1
    h15       C15              -1344
    h19       OBJ              59000   B19                  1
    h19       C19              -1344
    g1        OBJ              10000   D1                 -24
    g2        OBJ              10000   D2                 -24
    g3        OBJ              10000   D3                 -24
    g4        OBJ              10000   D4                 -24
    g5        OBJ              10000   D5                 -24
    g6        OBJ              10000   D6                 -24
    g7        OBJ              10000   D7                 -24
    g8        OBJ              10000   D8                 -24
    g9        OBJ              10000   D9                 -24
    g10       OBJ              10000   D10                -24
    g11       OBJ              10000   D11                -24
    g12       OBJ              10000   D12                -24
    g13       OBJ              10000   D13                -24
    g15       OBJ              10000   D15                -24
    g19       OBJ              10000   D19                -24
    MARK0001  'MARKER'                 'INTEND'
    a1        OBJ                7.3   E1                  -1
    a1        F0             8.33E-4
    a2        OBJ               14.6   E1                   1
    a2        E2                  -1   F1             8.33E-4
    a3        OBJ               14.6   E2                   1
    a3        E3                  -1   F2             8.33E-4
    a4        OBJ              18.25   E3                   1
    a4        E4                  -1   F3             8.33E-4
    a5        OBJ              18.25   E4                   1
    a5        E5                  -1   F4             8.33E-4
    a6        OBJ              18.25   E5                   1
    a6        E6                  -1   F5             8.33E-4
    a7        OBJ              18.25   E6                   1
    a7        E7                  -1   F6             8.33E-4
    a8        OBJ               21.9   E7                   1
    a8        E8                  -1   F7             8.33E-4
    a9        OBJ               51.1   E8                   1
    a9        E9                  -1   F8             8.33E-4
    a10       OBJ             58.035   E6                   1
    a10       E10                 -1   F9             8.33E-4
    a11       OBJ               14.6   E5                   1
    a11       E11                 -1   F10            8.33E-4
    a12       OBJ               36.5   E11                  1
    a12       E12                 -1   F11            8.33E-4
    a13       OBJ                 73   E12                  1
    a13       E13                 -1   F12            8.33E-4
    a14       OBJ                 73   E12                  1
    a14       E14                 -1   F13            8.33E-4
    a15       OBJ                 73   E14                  1
    a15       E15                 -1   F14            8.33E-4
    a16       OBJ                7.3   E15                  1
    a16       E16                 -1   F15            8.33E-4
    a17       OBJ              0.073   E11                  1
    a17       E17                 -1   F16            8.33E-4
    a18       OBJ               14.6   E17                  1
    a18       E18                 -1   F17            8.33E-4
    a19       OBJ               43.8   E18                  1
    a19       E19                 -1   F18            8.33E-4
    b1        OBJ               0.73   C1                  -1
    b1        F0              8.3E-5
    b2        OBJ               1.46   C1                   1
    b2        C2                  -1   F1              8.3E-5
    b3        OBJ               1.46   C2                   1
    b3        C3                  -1   F2              8.3E-5
    b4        OBJ              1.825   C3                   1
    b4        C4                  -1   F3              8.3E-5
    b5        OBJ              1.825   C4                   1
    b5        C5                  -1   F4              8.3E-5
    b6        OBJ              1.825   C5                   1
    b6        C6                  -1   F5              8.3E-5
    b7        OBJ              1.825   C6                   1
    b7        C7                  -1   F6              8.3E-5
    b8        OBJ               2.19   C7                   1
    b8        C8                  -1   F7              8.3E-5
    b9        OBJ               5.11   C8                   1
    b9        C9                  -1   F8              8.3E-5
    b10       OBJ             5.8035   C6                   1
    b10       C10                 -1   F9              8.3E-5
    b11       OBJ               1.46   C5                   1
    b11       C11                 -1   F10             8.3E-5
    b12       OBJ               3.65   C11                  1
    b12       C12                 -1   F11             8.3E-5
    b13       OBJ                7.3   C12                  1
    b13       C13                 -1   F12             8.3E-5
    b14       OBJ                7.3   C12                  1
    b14       C14                 -1   F13             8.3E-5
    b15       OBJ                7.3   C14                  1
    b15       C15                 -1   F14             8.3E-5
    b16       OBJ               0.73   C15                  1
    b16       C16                 -1   F15             8.3E-5
    b17       OBJ             0.0073   C11                  1
    b17       C17                 -1   F16             8.3E-5
    b18       OBJ               1.46   C17                  1
    b18       C18                 -1   F17             8.3E-5
    b19       OBJ               4.38   C18                  1
    b19       C19                 -1   F18             8.3E-5
    f1        C1                   1   D1                   1
    f1        E1                  -1
    f2        C2                   1   D2                   1
    f2        E2                  -1
    f3        C3                   1   D3                   1
    f3        E3                  -1
    f4        C4                   1   D4                   1
    f4        E4                  -1
    f5        C5                   1   D5                   1
    f5        E5                  -1
    f6        C6                   1   D6                   1
    f6        E6                  -1
    f7        C7                   1   D7                   1
    f7        E7                  -1
    f8        C8                   1   D8                   1
    f8        E8                  -1
    f9        C9                   1   D9                   1
    f9        E9                  -1
    f10       C10                  1   D10                  1
    f10       E10                 -1
    f11       C11                  1   D11                  1
    f11       E11                 -1
    f12       C12                  1   D12                  1
    f12       E12                 -1
    f13       C13                  1   D13                  1
    f13       E13                 -1
    f15       C15                  1   D15                  1
    f15       E15                 -1
    f19       C19                  1   D19                  1
    f19       E19                 -1
RHS
    RHS       E1               -1000   E2              -10000
    RHS       E3               -5000   E4                -400
    RHS       E5               -2000   E6                -100
    RHS       E7                -700   E8               -2000
    RHS       E9               -1400   E10               -900
    RHS       E11                -40   E12               -700
    RHS       E13               -120   E14              -1400
    RHS       E15              -9000   E16                -80
    RHS       E17               -200   E18               -750
    RHS       E19              -5000   F0                   2
    RHS       F1                   3   F2                   3
    RHS       F3                   3   F4                   3
    RHS       F5                   2   F6                   1
    RHS       F7                   3   F8                   2
    RHS       F9                   3   F10                  2
    RHS       F11                  3   F12                  1
    RHS       F13                  2   F14                  2
    RHS       F15                  1   F16                 99
    RHS       F17                  2   F18                  1
BOUNDS
 UP ONE       c1                   1
 UP ONE       c2                   1
 UP ONE       c3                   1
 UP ONE       c4                   1
 UP ONE       c5                   1
 UP ONE       c6                   1
 UP ONE       c7                   1
 UP ONE       c8                   1
 UP ONE       c9                   1
 UP ONE       c10                  1
 UP ONE       c11                  1
 UP ONE       c12                  1
 UP ONE       c13                  1
 UP ONE       c14                  1
 UP ONE       c15                  1
 UP ONE       c16                  1
 UP ONE       c17                  1
 UP ONE       c18                  1
 UP ONE       c19                  1
 UP ONE       d1                   1
 UP ONE       d2                   1
 UP ONE       d3                   1
 UP ONE       d4                   1
 UP ONE       d5                   1
 UP ONE       d6                   1
 UP ONE       d7                   1
 UP ONE       d8                   1
 UP ONE       d9                   1
 UP ONE       d10                  1
 UP ONE       d11                  1
 UP ONE       d12                  1
 UP ONE       d13                  1
 UP ONE       d15                  1
 UP ONE       d19                  1
 UP ONE       h1               10000
 UP ONE       h2               10000
 UP ONE       h3               10000
 UP ONE       h4               10000
 UP ONE       h5               10000
 UP ONE       h6               10000
 UP ONE       h7               10000
 UP ONE       h8               10000
 UP ONE       h9               10000
 UP ONE       h10               1000
 UP ONE       h11               1000
 UP ONE       h12               1000
 UP ONE       h13               1000
 UP ONE       h15               1000
 UP ONE       h19               1000
 UP ONE       g1               10000
 UP ONE       g2               10000
 UP ONE       g3               10000
 UP ONE       g4               10000
 UP ONE       g5               10000
 UP ONE       g6               10000
 UP ONE       g7               10000
 UP ONE       g8               10000
 UP ONE       g9               10000
 UP ONE       g10               1000
 UP ONE       g11               1000
 UP ONE       g12               1000
 UP ONE       g13               1000
 UP ONE       g15               1000
 UP ONE       g19               1000
ENDATA
