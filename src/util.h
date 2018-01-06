#ifndef SKUNKS_UTIL_H
#define SKUNKS_UTIL_H

// directions of local axes in global coordinates - v[1]=i, v[2]=j, v[3]=k
// and origin of local system, in global coord. - v[0];
// in local coord. vx[1]=1, vy[1]=0, vz[1]=0; vx[2]=0, vy[2]=1, vz[2]=0 etc.
// rotation and translation of object only affect vx, vy, vz

typedef struct _matrix
{
  float vx[4];
  float vy[4];
  float vz[4];
} matrix;

#endif // #ifndef SKUNKS_UTIL_H
