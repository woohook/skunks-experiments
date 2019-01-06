typedef struct _vhc
{
  float vrx; // steering speed
  float af;  // acceleration factor
  float bf;  // brake factor

  float accel;
  float brake; // acceleration and brake torques/wheel

  int turn;  // -1: left; 0: no turn; 1: right
  int dmode; // 1 forward, -1 reverse
} vhc;
