namespace octet
{
	class player : public resource
	{

	};

	class octet_game : public app
	{
		ref <visual_scene> app_scene;
		btDefaultCollisionConfiguration config;
		btCollisionDispatcher *dispatcher;
		btDbvtBroadphase *broadphase;
		btSequentialImpulseConstraintSolver *solver;
		btDiscreteDynamicsWorld *world;

	public:
		octet_game(int argc, char**argv) :app(argc, argv)
		{
			dispatcher = new btCollisionDispatcher(&config);
			broadphase = new btDbvtBroadphase();
			solver = new btSequentialImpulseConstraintSolver();
			world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, &config);
		}
		
		~octet_game()
		{
			delete world;
			delete solver;
			delete broadphase;
			delete dispatcher;
		}

		void app_init()
		{
			app_scene = new visual_scene();
			app_scene->create_default_camera_and_lights();
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 5, 20));
			app_scene->get_camera_instance(0)->set_far_plane(200);
			app_scene->get_camera_instance(0)->set_near_plane(1);

			scene_node *cam = app_scene->get_camera_instance(0)->get_node();
			cam->translate(vec3(0, -5, -25));

			material *groundMat = new material(vec4(1, 1, 0, 1));
			material *wall = new material(vec4(1, 0, 0, 1));
			material *sky = new material(vec4(0, 0, 128, 1));
			material *playerMat = new material(vec4(1, 0, 1, 1));

			mesh *groundMesh = new mesh_box(vec3(10, 0, 10));
			mesh *skyMesh = new mesh_box(vec3(10, 0, 10));

			scene_node *floorNode = new scene_node();
			scene_node *leftWallNode = new scene_node();
			scene_node *rightWallNode = new scene_node();
			scene_node *skyNode = new scene_node();

			app_scene->add_child(floorNode);
			app_scene->add_child(leftWallNode);
			app_scene->add_child(rightWallNode);
			app_scene->add_child(skyNode);
			
			floorNode->translate(vec3(0, -10, 0));
			leftWallNode->translate(vec3(-10, 0, 0));
			rightWallNode->translate(vec3(10,0, 0));
			leftWallNode->rotate(-90, vec3(0, 0, 1));
			rightWallNode->rotate(90, vec3(0, 0, 1));
			skyNode->translate(vec3(0, 10, 0));
			

			app_scene->add_mesh_instance(new mesh_instance(floorNode, groundMesh, groundMat));
			app_scene->add_mesh_instance(new mesh_instance(leftWallNode, groundMesh, wall));
			app_scene->add_mesh_instance(new mesh_instance(rightWallNode, groundMesh, wall));
			app_scene->add_mesh_instance(new mesh_instance(skyNode, skyMesh, sky));

		}

		void draw_world(int x, int y, int w, int h)
		{
			int vx = 0, vy = 0;
			get_viewport_size(vx, vy);
			app_scene->begin_render(vx, vy);

			scene_node *cam = app_scene->get_camera_instance(0)->get_node();
			//scene_node *skyNode = 
			//scene_node *ground

			if (is_key_down(key_down))
			{
				cam->translate(vec3(0, 0, 1));
			}
			else if (is_key_down(key_up))
			{
				cam->translate(vec3(0, 0, -1));
			}

			app_scene->update(1.0f / 30);
			app_scene->render((float)vx / vy);
		}
	};
}