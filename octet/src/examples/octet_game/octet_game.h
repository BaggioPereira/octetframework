namespace octet
{
	class octet_game : public app
	{	
		ref<visual_scene> app_scene;
		collada_builder loader;

	public:
		octet_game(int argc, char**argv) :app(argc, argv)
		{			

		}

		~octet_game()
		{
			
		}

		void app_init()
		{
			app_scene = new visual_scene();
			app_scene->create_default_camera_and_lights();
			app_scene->get_camera_instance(0)->set_near_plane(1);
			app_scene->get_camera_instance(0)->set_far_plane(2000);
			scene_node *cam = app_scene->get_camera_instance(0)->get_node();
			cam->loadIdentity();
			cam->translate(vec3(0, 1000, 0));
			cam->rotate(-90, vec3(1, 0, 0));
			
			if (!loader.load_xml("src/examples/octet_game/pool.dae"))
			{
				return;
			}
			resource_dict pool;
			loader.get_resources(pool);

			pool.dump_assets(log(""));

			material *poolMat = new material(new image("src/examples/octet_game/poolMat.gif"));

			scene_node *pooltableNode = pool.get_scene_node("pooltable");
			scene_node *cueballNode = pool.get_scene_node("cueball");
			scene_node *blackballNode = pool.get_scene_node("blackball");
			scene_node *yellowball1Node = pool.get_scene_node("yellowball1");
			scene_node *yellowball2Node = pool.get_scene_node("yellowball2");
			scene_node *yellowball3Node = pool.get_scene_node("yellowball3");
			scene_node *yellowball4Node = pool.get_scene_node("yellowball4");
			scene_node *yellowball5Node = pool.get_scene_node("yellowball5");
			scene_node *yellowball6Node = pool.get_scene_node("yellowball6");
			scene_node *yellowball7Node = pool.get_scene_node("yellowball7");
			scene_node *redball1Node = pool.get_scene_node("redball1");
			scene_node *redball2Node = pool.get_scene_node("redball2");
			scene_node *redball3Node = pool.get_scene_node("redball3");
			scene_node *redball4Node = pool.get_scene_node("redball4");
			scene_node *redball5Node = pool.get_scene_node("redball5");
			scene_node *redball6Node = pool.get_scene_node("redball6");
			scene_node *redball7Node = pool.get_scene_node("redball7");
			mesh *pooltableMesh = pool.get_mesh("pooltable-mesh+lambert3-material");
			mesh *cueballMesh = pool.get_mesh("cueball-mesh+lambert2-material");
			mesh *blackballMesh = pool.get_mesh("blackball-mesh+lambert2-material");
			mesh *yellowball1Mesh = pool.get_mesh("yellowball1-mesh+lambert2-material");
			mesh *yellowball2Mesh = pool.get_mesh("yellowball2-mesh+lambert2-material");
			mesh *yellowball3Mesh = pool.get_mesh("yellowball3-mesh+lambert2-material");
			mesh *yellowball4Mesh = pool.get_mesh("yellowball4-mesh+lambert2-material");
			mesh *yellowball5Mesh = pool.get_mesh("yellowball5-mesh+lambert2-material");
			mesh *yellowball6Mesh = pool.get_mesh("yellowball6-mesh+lambert2-material");
			mesh *yellowball7Mesh = pool.get_mesh("yellowball7-mesh+lambert2-material");
			mesh *redball1Mesh = pool.get_mesh("redball1-mesh+lambert2-material");
			mesh *redball2Mesh = pool.get_mesh("redball2-mesh+lambert2-material");
			mesh *redball3Mesh = pool.get_mesh("redball3-mesh+lambert2-material");
			mesh *redball4Mesh = pool.get_mesh("redball4-mesh+lambert2-material");
			mesh *redball5Mesh = pool.get_mesh("redball5-mesh+lambert2-material");
			mesh *redball6Mesh = pool.get_mesh("redball6-mesh+lambert2-material");
			mesh *redball7Mesh = pool.get_mesh("redball7-mesh+lambert2-material");
			visual_scene *Scene = pool.get_visual_scene("Scene");

			mat4t poolSetup;
			poolSetup.rotateY(-90);
			app_scene->add_shape(poolSetup, pooltableMesh, poolMat, false);
			app_scene->add_shape(poolSetup, cueballMesh, poolMat, true);
			app_scene->add_shape(poolSetup, blackballMesh, poolMat, true);
			app_scene->add_shape(poolSetup, yellowball1Mesh, poolMat, true);
			app_scene->add_shape(poolSetup, yellowball2Mesh, poolMat, true);
			app_scene->add_shape(poolSetup, yellowball3Mesh, poolMat, true);
			app_scene->add_shape(poolSetup, yellowball4Mesh, poolMat, true);
			app_scene->add_shape(poolSetup, yellowball5Mesh, poolMat, true);
			app_scene->add_shape(poolSetup, yellowball6Mesh, poolMat, true);
			app_scene->add_shape(poolSetup, yellowball7Mesh, poolMat, true);
			app_scene->add_shape(poolSetup, redball1Mesh, poolMat, true);
			app_scene->add_shape(poolSetup, redball2Mesh, poolMat, true);
			app_scene->add_shape(poolSetup, redball3Mesh, poolMat, true);
			app_scene->add_shape(poolSetup, redball4Mesh, poolMat, true);
			app_scene->add_shape(poolSetup, redball5Mesh, poolMat, true);
			app_scene->add_shape(poolSetup, redball6Mesh, poolMat, true);
			app_scene->add_shape(poolSetup, redball7Mesh, poolMat, true);
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