struct _camera;
struct _matrix;

struct _camera* create_camera();

struct _matrix* camera_get_view_transform(struct _camera* cam);

struct _matrix* camera_get_focus(struct _camera* cam);

void camera_set_focus(struct _camera* cam, struct _matrix* object);

void camera_update(struct _camera* cam);
