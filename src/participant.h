struct participant;
struct _sgob;

void participant_initialize();
void participant_process();
void participant_release();

struct participant* participant_create();
void participant_destroy(struct participant* pParticipant);
void participant_assign_entity(struct participant* pParticipant, struct _sgob* pEntity);
