typedef struct _vhc
{
  float action_accelerate;
  float action_brake;
  float action_left;
  float action_right;
  float action_reverse;

  float vrx; // steering speed
  float af;  // acceleration factor
  float bf;  // brake factor

  float accel;
  float brake; // acceleration and brake torques/wheel

  int turn;  // -1: left; 0: no turn; 1: right
  int dmode; // 1 forward, -1 reverse
} vhc;

struct _vhc* vehicle_create();
void vehicle_process();
