//#include "game_world.h"
namespace octet {
	class octet_game : public app {
		ref<visual_scene> app_scene;

	public:
		octet_game(int argc, char **argv) : app(argc, argv) 
		{
		}

		void app_init()
		{
			app_scene = new visual_scene();
			app_scene->create_default_camera_and_lights();
			//app_scene->add_child(game_world::node);
			//app_scene->add_mesh_instance(new mesh_instance(game_world::node, game_world::box, game_world::ground));

		}

		void draw_world(int x, int y, int w, int h)
		{
			int vx = 0, vy = 0;
			get_viewport_size(vx, vy);
			app_scene->begin_render(vx, vy);
			app_scene->update(1.0f / 30);
			app_scene->render((float)vx / vy);
		}
	};
}