NAME graphdraw-gemcutter_copy_pre
ROWS
 N  OBJ
 L  axisVdifLB[r,Rubygem,User]
 L  axisVdifRT[r,Rubygem,Dependency]
 L  axisVdifRT[r,User,WebHook]
 L  compDistAxis1[r,Rubygem,Dependency]
 L  compDistAxis1[r,Version,Dependency]
 L  compDistAxis2[r,Rubygem,User]
COLUMNS
    MARKER    'MARKER'                 'INTORG'
    x[r,Rubygem]  OBJ       129
    x[r,Rubygem]  axisVdifLB[r,Rubygem,User]  2
    x[r,Rubygem]  axisVdifRT[r,Rubygem,Dependency]  -1
    x[r,Rubygem]  compDistAxis1[r,Rubygem,Dependency]  2
    x[r,Rubygem]  compDistAxis2[r,Rubygem,User]  -1
    x[r,User]  OBJ       320.5
    x[r,User]  axisVdifLB[r,Rubygem,User]  -1
    x[r,User]  axisVdifRT[r,User,WebHook]  -1
    x[r,User]  compDistAxis2[r,Rubygem,User]  1
    x[r,Version]  OBJ       -2
    x[r,Version]  axisVdifRT[r,User,WebHook]  1
    x[r,Version]  compDistAxis1[r,Version,Dependency]  1
    x[r,Dependency]  OBJ       1
    x[r,Dependency]  axisVdifRT[r,Rubygem,Dependency]  1
    x[r,Dependency]  compDistAxis1[r,Rubygem,Dependency]  -1
    x[r,Dependency]  compDistAxis1[r,Version,Dependency]  -2
    MARKER    'MARKER'                 'INTEND'
    dist[r,Rubygem,Dependency]  OBJ       64
    dist[r,Rubygem,Dependency]  compDistAxis1[r,Rubygem,Dependency]  -1
    dist[r,Version,Dependency]  OBJ       64
    dist[r,Version,Dependency]  compDistAxis1[r,Version,Dependency]  -1
    MARKER    'MARKER'                 'INTORG'
    zV[c,lb,Rubygem,User]  OBJ       4.1733333333333326e+02
    zV[c,lb,Rubygem,User]  axisVdifLB[r,Rubygem,User]  -3
    MARKER    'MARKER'                 'INTEND'
RHS
    RHS1      OBJ       -8.6219166666666679e+03
    RHS1      axisVdifLB[r,Rubygem,User]  -5
    RHS1      axisVdifRT[r,Rubygem,Dependency]  -5
    RHS1      axisVdifRT[r,User,WebHook]  -10
    RHS1      compDistAxis1[r,Version,Dependency]  -2
    RHS1      compDistAxis2[r,Rubygem,User]  12
BOUNDS
 LO BND1      x[r,Rubygem]  6
 UP BND1      x[r,Rubygem]  8
 LO BND1      x[r,User]  18
 UP BND1      x[r,User]  19
 LO BND1      x[r,Version]  8
 UP BND1      x[r,Version]  9
 LO BND1      x[r,Dependency]  1
 UP BND1      x[r,Dependency]  3
 LO BND1      dist[r,Rubygem,Dependency]  11
 UP BND1      dist[r,Rubygem,Dependency]  15
 LO BND1      dist[r,Version,Dependency]  7
 UP BND1      dist[r,Version,Dependency]  9
 BV BND1      zV[c,lb,Rubygem,User]
ENDATA
