//#include "game_world.h"
namespace octet {
	class octet_game : public app {
		ref<visual_scene> app_scene;

		collada_builder loader;

	public:
		octet_game(int argc, char **argv) : app(argc, argv) 
		{
		}

		void app_init()
		{
			
			app_scene = new visual_scene();
			app_scene->create_default_camera_and_lights();
			resource_dict dict;
			if (!loader.load_xml("assets/box.dae"))
			{
				return;
			}
			loader.get_resources(dict);

			dynarray<resource*> meshes;
			dict.find_all(meshes, atom_mesh);

			

			if (meshes.size())
			{
				material *sky = new material(new image("assets/outUV.jpg"));
				material *blue = new material(vec4(0, 0, 1, 1));
				mesh *skyBox = meshes[0]->get_mesh();
				mesh *skyBox2 = new mesh_box(vec3(100, 100, 100));
				scene_node *node = new scene_node();
				//node->scale(100);
				app_scene->add_child(node);
				app_scene->add_mesh_instance(new mesh_instance(node, skyBox, sky));
			}
			app_scene->get_camera_instance(0)->set_far_plane(20000);
			app_scene->get_camera_instance(0)->set_near_plane(1);

			


		}

		void draw_world(int x, int y, int w, int h)
		{
			int vx = 0, vy = 0;
			get_viewport_size(vx, vy);
			app_scene->begin_render(vx, vy);
			app_scene->update(1.0f / 30);
			app_scene->render((float)vx / vy);

			mesh_instance *mi = app_scene->get_mesh_instance(0);
			scene_node *cam = app_scene->get_camera_instance(0)->get_node();
			//if (mi)
			//{
			//	scene_node *node = mi->get_node();
			//	//node->rotate(1, vec3(1, 1, 0));
			//}
			if (is_key_down(key_right))
			{
				cam->rotate(-1, vec3(0, 1, 0));
			}
			else if (is_key_down(key_left))
			{
				cam->rotate(1, vec3(0, 1, 0));
			}
		}
	};
}