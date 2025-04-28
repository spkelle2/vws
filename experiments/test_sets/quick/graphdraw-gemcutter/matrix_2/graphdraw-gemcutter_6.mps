NAME graphdraw-gemcutter_copy_pre
ROWS
 N  OBJ
 L  centerDistAxis1[r,Subscription]
 L  centerDistAxis2[r,Subscription]
 L  axisVdifRT[r,Rubygem,Dependency]
 L  axisVdifRT[r,User,Subscription]
 L  axisVdifRT[r,User,Version]
 L  axisVdifRT[r,WebHook,Version]
 L  compDistAxis1[r,User,WebHook]
 L  compDistAxis1[r,Rubygem,Subscription]
 E  R8      
COLUMNS
    MARKER    'MARKER'                 'INTORG'
    x[r,Rubygem]  OBJ       -4
    x[r,Rubygem]  axisVdifRT[r,Rubygem,Dependency]  1
    x[r,Rubygem]  compDistAxis1[r,Rubygem,Subscription]  1
    x[r,User]  OBJ       129
    x[r,User]  axisVdifRT[r,User,Subscription]  -1
    x[r,User]  axisVdifRT[r,User,Version]  -1
    x[r,User]  compDistAxis1[r,User,WebHook]  1
    x[r,WebHook]  OBJ       64.5
    x[r,WebHook]  axisVdifRT[r,WebHook,Version]  -1
    x[r,WebHook]  compDistAxis1[r,User,WebHook]  1
    x[r,Subscription]  centerDistAxis1[r,Subscription]  -2
    x[r,Subscription]  centerDistAxis2[r,Subscription]  4
    x[r,Subscription]  axisVdifRT[r,User,Subscription]  1
    x[r,Subscription]  compDistAxis1[r,Rubygem,Subscription]  -1
    x[r,Version]  OBJ       -66
    x[r,Version]  R8        1
    x[r,Version]  axisVdifRT[r,User,Version]  1
    x[r,Version]  axisVdifRT[r,WebHook,Version]  1
    x[r,Dependency]  OBJ       -65
    x[r,Dependency]  R8        -1
    x[r,Dependency]  axisVdifRT[r,Rubygem,Dependency]  1
    MARKER    'MARKER'                 'INTEND'
    center_dist[r,Subscription]  OBJ       0.5
    center_dist[r,Subscription]  centerDistAxis1[r,Subscription]  -1
    center_dist[r,Subscription]  centerDistAxis2[r,Subscription]  -1
    dist[r,Rubygem,Subscription]  OBJ       32
    dist[r,Rubygem,Subscription]  compDistAxis1[r,Rubygem,Subscription]  -1
    C8        OBJ       64
    C8        R8        -1
    C9        R8        -1
RHS
    RHS1      OBJ       -12007.5
    RHS1      centerDistAxis1[r,Subscription]  -29
    RHS1      centerDistAxis2[r,Subscription]  29
    RHS1      axisVdifRT[r,Rubygem,Dependency]  15
    RHS1      axisVdifRT[r,User,Subscription]  -4
    RHS1      axisVdifRT[r,User,Version]  -8
    RHS1      axisVdifRT[r,WebHook,Version]  -8
    RHS1      compDistAxis1[r,User,WebHook]  31
    RHS1      compDistAxis1[r,Rubygem,Subscription]  6
    RHS1      R8        2
BOUNDS
 LO BND1      x[r,Rubygem]  13
 UP BND1      x[r,Rubygem]  14
 LO BND1      x[r,User]  11
 UP BND1      x[r,User]  16
 LO BND1      x[r,WebHook]  14
 UP BND1      x[r,WebHook]  16
 LO BND1      x[r,Subscription]  7
 UP BND1      x[r,Subscription]  12
 LO BND1      x[r,Version]  3
 UP BND1      x[r,Version]  8
 LO BND1      x[r,Dependency]  1
 UP BND1      x[r,Dependency]  2
 LO BND1      center_dist[r,Subscription]  5
 UP BND1      center_dist[r,Subscription]  19
 UP BND1      dist[r,Rubygem,Subscription]  1
 UP BND1      C8        2
 UP BND1      C9        3
ENDATA
