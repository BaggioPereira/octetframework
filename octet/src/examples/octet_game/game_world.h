#include "../../octet.h"
namespace octet  {

	class game_world {
		ref<visual_scene> app_scene;

	public:
		static material *ground;
		static mesh *box;
		static scene_node *node;

		void app_init()
		{
			ground = new material(vec4(0, 1, 0, 1));
			box = new mesh_box(vec3(1000, 20, 1000));
			node = new scene_node();
			app_scene->add_child(node);
			app_scene->add_mesh_instance(new mesh_instance(node, box, ground));
		}

		void draw_world()
		{
			int vx = 0, vy = 0;
			//get_viewport_size(vx, vy);
			app_scene->begin_render(vx, vy);
			app_scene->update(1.0f / 30);
			app_scene->render((float)vx / vy);
		}
	};
}