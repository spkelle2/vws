NAME graphdraw-gemcutter_copy_pre
ROWS
 N  OBJ
 L  centerDistAxis1[r,Subscription]
 L  centerDistAxis1[r,Linkset]
 L  centerDistAxis1[c,Rubygem]
 L  centerDistAxis2[r,Subscription]
 L  centerDistAxis2[r,Linkset]
 L  centerDistAxis2[c,Rubygem]
 L  axisVdifLB[r,WebHook,Subscription]
 L  axisVdifLB[r,Subscription,Dependency]
 L  axisVdifLB[c,Rubygem,Subscription]
 L  axisVdifLB[c,Ownership,Linkset]
 L  axisVdifRT[r,Rubygem,WebHook]
 L  axisVdifRT[r,Rubygem,Subscription]
 L  axisVdifRT[r,Rubygem,Linkset]
 L  axisVdifRT[r,Rubygem,Dependency]
 L  axisVdifRT[r,User,WebHook]
 L  axisVdifRT[r,User,Linkset]
 L  axisVdifRT[r,Subscription,Ownership]
 L  axisVdifRT[r,Ownership,Version]
 L  axisVdifRT[r,Ownership,Dependency]
 L  axisVdifRT[r,Linkset,Version]
 L  axisVdifRT[c,Rubygem,Ownership]
 L  axisVdifRT[c,Rubygem,Linkset]
 L  axisVdifRT[c,WebHook,Linkset]
 L  axisVdifRT[c,WebHook,Version]
 L  axisVdifRT[c,Subscription,Version]
 L  axisVdifRT[c,Linkset,Version]
 L  compDistAxis1[r,User,Subscription]
 L  compDistAxis1[r,Rubygem,Linkset]
 L  compDistAxis1[c,Rubygem,Linkset]
 L  compDistAxis1[c,Rubygem,Ownership]
 L  compDistAxis1[c,Rubygem,Version]
 L  compDistAxis1[c,Rubygem,Dependency]
 L  compDistAxis2[r,User,Subscription]
 L  compDistAxis2[r,Rubygem,Dependency]
 L  compDistAxis2[r,Version,Dependency]
 L  compDistAxis2[c,Rubygem,User]
 L  compDistAxis2[c,Rubygem,Subscription]
 L  compDistAxis2[c,Rubygem,WebHook]
 L  DistSumLB[Rubygem,Dependency]
 L  DistAxisLB1[r,Rubygem,Dependency]
 L  DistAxisLB1[c,Rubygem,User]
 L  DistAxisLB1[c,Rubygem,Linkset]
 L  DistAxisLB1[c,Rubygem,Dependency]
 L  triangle2[Rubygem,User,Dependency,c]
 L  triangle2[Rubygem,Subscription,Linkset,c]
 L  triangle2[Rubygem,Subscription,Dependency,c]
COLUMNS
    MARKER    'MARKER'                 'INTORG'
    x[r,Rubygem]  OBJ       384.5
    x[r,Rubygem]  axisVdifRT[r,Rubygem,WebHook]  -1
    x[r,Rubygem]  axisVdifRT[r,Rubygem,Subscription]  -1
    x[r,Rubygem]  axisVdifRT[r,Rubygem,Linkset]  -1
    x[r,Rubygem]  axisVdifRT[r,Rubygem,Dependency]  -1
    x[r,Rubygem]  compDistAxis1[r,Rubygem,Linkset]  2
    x[r,Rubygem]  compDistAxis2[r,Rubygem,Dependency]  -1
    x[r,WebHook]  OBJ       -97
    x[r,WebHook]  axisVdifLB[r,WebHook,Subscription]  1
    x[r,WebHook]  axisVdifRT[r,Rubygem,WebHook]  2
    x[r,WebHook]  axisVdifRT[r,User,WebHook]  1
    x[r,Subscription]  OBJ       -64
    x[r,Subscription]  centerDistAxis1[r,Subscription]  -2
    x[r,Subscription]  centerDistAxis2[r,Subscription]  2
    x[r,Subscription]  axisVdifLB[r,WebHook,Subscription]  -1
    x[r,Subscription]  axisVdifLB[r,Subscription,Dependency]  1
    x[r,Subscription]  axisVdifRT[r,Rubygem,Subscription]  2
    x[r,Subscription]  axisVdifRT[r,Subscription,Ownership]  -1
    x[r,Subscription]  compDistAxis1[r,User,Subscription]  -4
    x[r,Subscription]  compDistAxis2[r,User,Subscription]  8
    x[r,Ownership]  OBJ       68
    x[r,Ownership]  compDistAxis2[r,User,Subscription]  -8
    x[r,Ownership]  compDistAxis1[r,User,Subscription]  4
    x[r,Ownership]  axisVdifRT[r,User,Linkset]  -2
    x[r,Ownership]  axisVdifRT[r,User,WebHook]  -1
    x[r,Ownership]  axisVdifRT[r,Subscription,Ownership]  1
    x[r,Ownership]  axisVdifRT[r,Ownership,Version]  -1
    x[r,Ownership]  axisVdifRT[r,Ownership,Dependency]  -1
    x[r,Linkset]  OBJ       -64
    x[r,Linkset]  centerDistAxis1[r,Linkset]  -1
    x[r,Linkset]  centerDistAxis2[r,Linkset]  2
    x[r,Linkset]  axisVdifRT[r,Rubygem,Linkset]  1
    x[r,Linkset]  axisVdifRT[r,User,Linkset]  1
    x[r,Linkset]  axisVdifRT[r,Linkset,Version]  -1
    x[r,Linkset]  compDistAxis1[r,Rubygem,Linkset]  -1
    x[r,Version]  OBJ       -193
    x[r,Version]  axisVdifRT[r,Ownership,Version]  1
    x[r,Version]  axisVdifRT[r,Linkset,Version]  1
    x[r,Version]  compDistAxis2[r,Version,Dependency]  -2
    x[r,Dependency]  OBJ       -1
    x[r,Dependency]  axisVdifLB[r,Subscription,Dependency]  -1
    x[r,Dependency]  axisVdifRT[r,Rubygem,Dependency]  1
    x[r,Dependency]  axisVdifRT[r,Ownership,Dependency]  1
    x[r,Dependency]  compDistAxis2[r,Rubygem,Dependency]  1
    x[r,Dependency]  compDistAxis2[r,Version,Dependency]  1
    x[c,Rubygem]  centerDistAxis1[c,Rubygem]  -1
    x[c,Rubygem]  centerDistAxis2[c,Rubygem]  1
    x[c,Rubygem]  axisVdifLB[c,Rubygem,Subscription]  1
    x[c,Rubygem]  axisVdifRT[c,Rubygem,Ownership]  -1
    x[c,Rubygem]  axisVdifRT[c,Rubygem,Linkset]  -1
    x[c,Rubygem]  compDistAxis1[c,Rubygem,Linkset]  1
    x[c,Rubygem]  compDistAxis1[c,Rubygem,Ownership]  1
    x[c,Rubygem]  compDistAxis1[c,Rubygem,Version]  1
    x[c,Rubygem]  compDistAxis1[c,Rubygem,Dependency]  1
    x[c,Rubygem]  compDistAxis2[c,Rubygem,User]  -2
    x[c,Rubygem]  compDistAxis2[c,Rubygem,Subscription]  -1
    x[c,Rubygem]  compDistAxis2[c,Rubygem,WebHook]  -1
    x[c,WebHook]  OBJ       192.5
    x[c,WebHook]  axisVdifRT[c,WebHook,Linkset]  -1
    x[c,WebHook]  axisVdifRT[c,WebHook,Version]  -1
    x[c,WebHook]  compDistAxis2[c,Rubygem,WebHook]  1
    x[c,Subscription]  OBJ       129.5
    x[c,Subscription]  axisVdifLB[c,Rubygem,Subscription]  -1
    x[c,Subscription]  axisVdifRT[c,Subscription,Version]  -1
    x[c,Subscription]  compDistAxis2[c,Rubygem,Subscription]  2
    x[c,Ownership]  OBJ       -3
    x[c,Ownership]  axisVdifLB[c,Ownership,Linkset]  1
    x[c,Ownership]  axisVdifRT[c,Rubygem,Ownership]  1
    x[c,Ownership]  compDistAxis1[c,Rubygem,Ownership]  1
    x[c,Linkset]  OBJ       2.5
    x[c,Linkset]  compDistAxis1[c,Rubygem,Dependency]  -1
    x[c,Linkset]  axisVdifLB[c,Ownership,Linkset]  -1
    x[c,Linkset]  axisVdifRT[c,Rubygem,Linkset]  1
    x[c,Linkset]  axisVdifRT[c,WebHook,Linkset]  1
    x[c,Linkset]  axisVdifRT[c,Linkset,Version]  -1
    x[c,Linkset]  compDistAxis1[c,Rubygem,Linkset]  -1
    x[c,Version]  OBJ       -65
    x[c,Version]  axisVdifRT[c,WebHook,Version]  1
    x[c,Version]  axisVdifRT[c,Subscription,Version]  1
    x[c,Version]  axisVdifRT[c,Linkset,Version]  1
    x[c,Version]  compDistAxis1[c,Rubygem,Version]  -1
    MARKER    'MARKER'                 'INTEND'
    center_dist[r,Subscription]  OBJ       0.5
    center_dist[r,Subscription]  centerDistAxis1[r,Subscription]  -1
    center_dist[r,Subscription]  centerDistAxis2[r,Subscription]  -1
    center_dist[r,Linkset]  OBJ       0.5
    center_dist[r,Linkset]  centerDistAxis1[r,Linkset]  -1
    center_dist[r,Linkset]  centerDistAxis2[r,Linkset]  -1
    center_dist[c,Rubygem]  OBJ       1
    center_dist[c,Rubygem]  centerDistAxis1[c,Rubygem]  -1
    center_dist[c,Rubygem]  centerDistAxis2[c,Rubygem]  -1
    dist[r,User,Subscription]  OBJ       32
    dist[r,User,Subscription]  compDistAxis1[r,User,Subscription]  -1
    dist[r,User,Subscription]  compDistAxis2[r,User,Subscription]  -1
    dist[r,Rubygem,Dependency]  OBJ       64
    dist[r,Rubygem,Dependency]  compDistAxis2[r,Rubygem,Dependency]  -1
    dist[r,Rubygem,Dependency]  DistSumLB[Rubygem,Dependency]  -2
    dist[r,Rubygem,Dependency]  DistAxisLB1[r,Rubygem,Dependency]  -1
    dist[r,Version,Dependency]  OBJ       32
    dist[r,Version,Dependency]  compDistAxis2[r,Version,Dependency]  -1
    dist[c,Rubygem,User]  OBJ       16
    dist[c,Rubygem,User]  compDistAxis2[c,Rubygem,User]  -1
    dist[c,Rubygem,User]  DistAxisLB1[c,Rubygem,User]  -1
    dist[c,Rubygem,WebHook]  OBJ       64
    dist[c,Rubygem,WebHook]  compDistAxis2[c,Rubygem,WebHook]  -1
    dist[c,Rubygem,Linkset]  OBJ       32
    dist[c,Rubygem,Linkset]  compDistAxis1[c,Rubygem,Linkset]  -1
    dist[c,Rubygem,Linkset]  DistAxisLB1[c,Rubygem,Linkset]  -1
    dist[c,Rubygem,Ownership]  OBJ       32
    dist[c,Rubygem,Ownership]  compDistAxis1[c,Rubygem,Ownership]  -1
    dist[c,Rubygem,Dependency]  OBJ       64
    dist[c,Rubygem,Dependency]  compDistAxis1[c,Rubygem,Dependency]  -2
    dist[c,Rubygem,Dependency]  DistSumLB[Rubygem,Dependency]  -1
    dist[c,Rubygem,Dependency]  DistAxisLB1[c,Rubygem,Dependency]  -2
    MARKER    'MARKER'                 'INTORG'
    zV[c,lb,Rubygem,User]  OBJ       -448
    zV[c,lb,Rubygem,User]  DistSumLB[Rubygem,Dependency]  7.75
    zV[c,lb,Rubygem,User]  compDistAxis2[c,Rubygem,User]  25
    zV[c,lb,Rubygem,User]  compDistAxis2[r,Rubygem,Dependency]  5
    zV[c,lb,Rubygem,User]  axisVdifRT[r,Ownership,Dependency]  1
    zV[c,lb,Rubygem,User]  DistAxisLB1[c,Rubygem,User]  43
    zV[c,lb,Rubygem,User]  triangle2[Rubygem,User,Dependency,c]  1
    zV[c,lb,Rubygem,Subscription]  compDistAxis1[c,Rubygem,Version]  2
    zV[c,lb,Rubygem,Subscription]  axisVdifRT[c,Subscription,Version]  3
    zV[c,lb,Rubygem,Subscription]  axisVdifRT[r,Rubygem,Subscription]  -15
    zV[c,lb,Rubygem,Subscription]  axisVdifLB[c,Rubygem,Subscription]  14
    zV[c,lb,Rubygem,Subscription]  triangle2[Rubygem,Subscription,Linkset,c]  1
    zV[c,lb,Rubygem,Subscription]  triangle2[Rubygem,Subscription,Dependency,c]  1
    zV[c,rt,Rubygem,Linkset]  DistAxisLB1[c,Rubygem,Dependency]  8
    zV[c,rt,Rubygem,Linkset]  compDistAxis2[c,Rubygem,Subscription]  3
    zV[c,rt,Rubygem,Linkset]  compDistAxis1[c,Rubygem,Linkset]  8
    zV[c,rt,Rubygem,Linkset]  axisVdifRT[c,WebHook,Linkset]  2
    zV[c,rt,Rubygem,Linkset]  axisVdifRT[c,Rubygem,Ownership]  7
    zV[c,rt,Rubygem,Linkset]  axisVdifLB[c,Rubygem,Subscription]  3
    zV[c,rt,Rubygem,Linkset]  centerDistAxis1[c,Rubygem]  14
    zV[c,rt,Rubygem,Linkset]  axisVdifRT[c,Linkset,Version]  -4
    zV[c,rt,Rubygem,Linkset]  axisVdifRT[r,Linkset,Version]  1
    zV[c,rt,Rubygem,Linkset]  DistAxisLB1[c,Rubygem,Linkset]  16
    zV[c,rt,Rubygem,Linkset]  axisVdifRT[r,Rubygem,Linkset]  -2
    zV[c,rt,Rubygem,Linkset]  axisVdifRT[c,Rubygem,Linkset]  12
    zV[c,rt,Rubygem,Linkset]  triangle2[Rubygem,Subscription,Linkset,c]  1
    zV[c,rt,Rubygem,Dependency]  axisVdifRT[r,User,WebHook]  1
    zV[c,rt,Rubygem,Dependency]  axisVdifRT[r,Rubygem,WebHook]  5
    zV[c,rt,Rubygem,Dependency]  centerDistAxis2[r,Subscription]  18
    zV[c,rt,Rubygem,Dependency]  DistAxisLB1[c,Rubygem,Dependency]  4
    zV[c,rt,Rubygem,Dependency]  axisVdifRT[r,Rubygem,Dependency]  -6
    zV[c,rt,Rubygem,Dependency]  DistAxisLB1[r,Rubygem,Dependency]  -5
    zV[c,rt,Rubygem,Dependency]  triangle2[Rubygem,User,Dependency,c]  1
    zV[c,rt,Rubygem,Dependency]  triangle2[Rubygem,Subscription,Dependency,c]  1
    MARKER    'MARKER'                 'INTEND'
RHS
    RHS1      OBJ       1129.5
    RHS1      centerDistAxis1[r,Subscription]  -29
    RHS1      centerDistAxis1[r,Linkset]  -13
    RHS1      centerDistAxis1[c,Rubygem]  -12
    RHS1      centerDistAxis2[r,Subscription]  29
    RHS1      centerDistAxis2[r,Linkset]  26
    RHS1      centerDistAxis2[c,Rubygem]  12
    RHS1      axisVdifLB[r,WebHook,Subscription]  -5
    RHS1      axisVdifLB[r,Subscription,Dependency]  -4
    RHS1      axisVdifLB[c,Rubygem,Subscription]  5
    RHS1      axisVdifLB[c,Ownership,Linkset]  -9
    RHS1      axisVdifRT[r,Rubygem,WebHook]  -5
    RHS1      axisVdifRT[r,Rubygem,Subscription]  -4
    RHS1      axisVdifRT[r,Rubygem,Linkset]  -7
    RHS1      axisVdifRT[r,Rubygem,Dependency]  -5
    RHS1      axisVdifRT[r,User,WebHook]  -4
    RHS1      axisVdifRT[r,User,Linkset]  -9
    RHS1      axisVdifRT[r,Subscription,Ownership]  3
    RHS1      axisVdifRT[r,Ownership,Version]  -9
    RHS1      axisVdifRT[r,Ownership,Dependency]  10
    RHS1      axisVdifRT[r,Linkset,Version]  -8
    RHS1      axisVdifRT[c,Rubygem,Ownership]  -9
    RHS1      axisVdifRT[c,Rubygem,Linkset]  5
    RHS1      axisVdifRT[c,WebHook,Linkset]  -6
    RHS1      axisVdifRT[c,WebHook,Version]  -16
    RHS1      axisVdifRT[c,Subscription,Version]  -13
    RHS1      axisVdifRT[c,Linkset,Version]  -12
    RHS1      compDistAxis1[r,User,Subscription]  -1
    RHS1      compDistAxis1[r,Rubygem,Linkset]  33
    RHS1      compDistAxis1[c,Rubygem,Linkset]  -1
    RHS1      compDistAxis1[c,Rubygem,Version]  15
    RHS1      compDistAxis1[c,Rubygem,Dependency]  -10.5
    RHS1      compDistAxis2[r,User,Subscription]  -3
    RHS1      compDistAxis2[r,Version,Dependency]  3
    RHS1      compDistAxis2[c,Rubygem,User]  -41
    RHS1      compDistAxis2[c,Rubygem,Subscription]  32
    RHS1      compDistAxis2[c,Rubygem,WebHook]  -1
    RHS1      DistSumLB[Rubygem,Dependency]  -5
    RHS1      DistAxisLB1[r,Rubygem,Dependency]  -5
    RHS1      DistAxisLB1[c,Rubygem,User]  -3
    RHS1      DistAxisLB1[c,Rubygem,Dependency]  -5.5
    RHS1      triangle2[Rubygem,User,Dependency,c]  1
    RHS1      triangle2[Rubygem,Subscription,Linkset,c]  1
    RHS1      triangle2[Rubygem,Subscription,Dependency,c]  1
BOUNDS
 LO BND1      x[r,Rubygem]  19
 UP BND1      x[r,Rubygem]  24
 LO BND1      x[r,WebHook]  3
 UP BND1      x[r,WebHook]  8
 LO BND1      x[r,Subscription]  8
 UP BND1      x[r,Subscription]  15
 LO BND1      x[r,Ownership]  10
 UP BND1      x[r,Ownership]  12
 LO BND1      x[r,Linkset]  11
 UP BND1      x[r,Linkset]  15
 LO BND1      x[r,Version]  1
 UP BND1      x[r,Version]  3
 LO BND1      x[r,Dependency]  14
 UP BND1      x[r,Dependency]  21
 LO BND1      x[c,Rubygem]  10
 UP BND1      x[c,Rubygem]  19
 LO BND1      x[c,WebHook]  19
 UP BND1      x[c,WebHook]  21
 LO BND1      x[c,Subscription]  14
 UP BND1      x[c,Subscription]  24
 LO BND1      x[c,Ownership]  1
 UP BND1      x[c,Ownership]  6
 LO BND1      x[c,Linkset]  12
 UP BND1      x[c,Linkset]  15
 LO BND1      x[c,Version]  1
 UP BND1      x[c,Version]  4
 LO BND1      center_dist[r,Subscription]  -1
 UP BND1      center_dist[r,Subscription]  13
 LO BND1      center_dist[r,Linkset]  -2
 UP BND1      center_dist[r,Linkset]  4
 LO BND1      center_dist[c,Rubygem]  -2
 UP BND1      center_dist[c,Rubygem]  7
 UP BND1      dist[r,User,Subscription]  43
 UP BND1      dist[r,Rubygem,Dependency]  5
 LO BND1      dist[r,Version,Dependency]  14
 UP BND1      dist[r,Version,Dependency]  16
 LO BND1      dist[c,Rubygem,User]  3
 UP BND1      dist[c,Rubygem,User]  46
 LO BND1      dist[c,Rubygem,WebHook]  2
 UP BND1      dist[c,Rubygem,WebHook]  12
 UP BND1      dist[c,Rubygem,Linkset]  16
 LO BND1      dist[c,Rubygem,Ownership]  18
 UP BND1      dist[c,Rubygem,Ownership]  25
 LO BND1      dist[c,Rubygem,Dependency]  2.75
 UP BND1      dist[c,Rubygem,Dependency]  8.75
 BV BND1      zV[c,lb,Rubygem,User]
 BV BND1      zV[c,lb,Rubygem,Subscription]
 BV BND1      zV[c,rt,Rubygem,Linkset]
 BV BND1      zV[c,rt,Rubygem,Dependency]
ENDATA
