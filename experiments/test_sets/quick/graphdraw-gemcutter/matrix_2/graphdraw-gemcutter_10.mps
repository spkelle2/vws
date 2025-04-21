NAME graphdraw-gemcutter_copy_pre
ROWS
 N  OBJ
 L  axisVdifLB[r,Rubygem,User]
 L  axisVdifLB[r,Ownership,Linkset]
 L  axisVdifLB[r,Ownership,Version]
 L  axisVdifLB[r,Linkset,Version]
 L  axisVdifRT[r,Rubygem,Ownership]
 L  axisVdifRT[r,User,WebHook]
 L  axisVdifRT[r,WebHook,Dependency]
 L  axisVdifRT[r,Ownership,Dependency]
 L  compDistAxis1[r,Rubygem,WebHook]
 L  compDistAxis1[r,Rubygem,Linkset]
 L  compDistAxis2[r,Rubygem,Linkset]
 L  compDistAxis2[r,Rubygem,Version]
COLUMNS
    MARKER    'MARKER'                 'INTORG'
    x[r,Rubygem]  OBJ       255.5
    x[r,Rubygem]  axisVdifLB[r,Rubygem,User]  1
    x[r,Rubygem]  axisVdifRT[r,Rubygem,Ownership]  -1
    x[r,Rubygem]  compDistAxis1[r,Rubygem,WebHook]  1
    x[r,Rubygem]  compDistAxis1[r,Rubygem,Linkset]  1
    x[r,Rubygem]  compDistAxis2[r,Rubygem,Linkset]  -1
    x[r,Rubygem]  compDistAxis2[r,Rubygem,Version]  -2
    x[r,User]  OBJ       127
    x[r,User]  axisVdifLB[r,Rubygem,User]  -2
    x[r,User]  axisVdifRT[r,User,WebHook]  -2
    x[r,WebHook]  OBJ       63
    x[r,WebHook]  axisVdifRT[r,User,WebHook]  1
    x[r,WebHook]  axisVdifRT[r,WebHook,Dependency]  -1
    x[r,WebHook]  compDistAxis1[r,Rubygem,WebHook]  -1
    x[r,Ownership]  OBJ       -66
    x[r,Ownership]  axisVdifLB[r,Ownership,Linkset]  1
    x[r,Ownership]  axisVdifLB[r,Ownership,Version]  2
    x[r,Ownership]  axisVdifRT[r,Rubygem,Ownership]  1
    x[r,Ownership]  axisVdifRT[r,Ownership,Dependency]  -1
    x[r,Linkset]  OBJ       2
    x[r,Linkset]  axisVdifLB[r,Ownership,Linkset]  -1
    x[r,Linkset]  axisVdifLB[r,Linkset,Version]  1
    x[r,Linkset]  compDistAxis1[r,Rubygem,Linkset]  -1
    x[r,Linkset]  compDistAxis2[r,Rubygem,Linkset]  1
    x[r,Version]  OBJ       65
    x[r,Version]  axisVdifLB[r,Ownership,Version]  -1
    x[r,Version]  axisVdifLB[r,Linkset,Version]  -1
    x[r,Version]  compDistAxis2[r,Rubygem,Version]  1
    x[r,Dependency]  OBJ       -130
    x[r,Dependency]  axisVdifRT[r,WebHook,Dependency]  1
    x[r,Dependency]  axisVdifRT[r,Ownership,Dependency]  1
    MARKER    'MARKER'                 'INTEND'
    dist[r,Rubygem,WebHook]  OBJ       64
    dist[r,Rubygem,WebHook]  compDistAxis1[r,Rubygem,WebHook]  -1
    dist[r,Rubygem,Linkset]  OBJ       64
    dist[r,Rubygem,Linkset]  compDistAxis1[r,Rubygem,Linkset]  -1
    dist[r,Rubygem,Linkset]  compDistAxis2[r,Rubygem,Linkset]  -1
    dist[r,Rubygem,Version]  OBJ       64
    dist[r,Rubygem,Version]  compDistAxis2[r,Rubygem,Version]  -1
RHS
    RHS1      OBJ       -5137.75
    RHS1      axisVdifLB[r,Rubygem,User]  -5
    RHS1      axisVdifLB[r,Ownership,Linkset]  -6
    RHS1      axisVdifLB[r,Ownership,Version]  -5
    RHS1      axisVdifLB[r,Linkset,Version]  -7
    RHS1      axisVdifRT[r,Rubygem,Ownership]  -5
    RHS1      axisVdifRT[r,User,WebHook]  -7
    RHS1      axisVdifRT[r,WebHook,Dependency]  -6
    RHS1      axisVdifRT[r,Ownership,Dependency]  -3
    RHS1      compDistAxis1[r,Rubygem,Linkset]  1
    RHS1      compDistAxis2[r,Rubygem,Linkset]  -1
    RHS1      compDistAxis2[r,Rubygem,Version]  -2
BOUNDS
 LO BND1      x[r,Rubygem]  9
 UP BND1      x[r,Rubygem]  13
 LO BND1      x[r,User]  7
 UP BND1      x[r,User]  9
 LO BND1      x[r,WebHook]  7
 UP BND1      x[r,WebHook]  10
 LO BND1      x[r,Ownership]  4
 UP BND1      x[r,Ownership]  7
 LO BND1      x[r,Linkset]  10
 UP BND1      x[r,Linkset]  13
 LO BND1      x[r,Version]  17
 UP BND1      x[r,Version]  20
 LO BND1      x[r,Dependency]  1
 UP BND1      x[r,Dependency]  4
 LO BND1      dist[r,Rubygem,WebHook]  5
 UP BND1      dist[r,Rubygem,WebHook]  6
 UP BND1      dist[r,Rubygem,Linkset]  5
 UP BND1      dist[r,Rubygem,Version]  4
ENDATA
