namespace octet
{
	class octet_game : public app
	{
		ref<visual_scene> app_scene;
		btDefaultCollisionConfiguration config;
		btCollisionDispatcher *dispatcher;
		btDbvtBroadphase *broadphase;
		btSequentialImpulseConstraintSolver *solver;
		btDiscreteDynamicsWorld *world;

		dynarray<btRigidBody*> rigid_bodies;

		
		collada_builder loader;


		scene_node *top;
		scene_node *bottom;
		scene_node *left;
		scene_node *right;
		scene_node *scene;
		scene_node *ship;

		void add_shape(mat4t_in worldMat, mesh *mesh, material *mat, bool is_dynamic)
		{
			scene_node *node = new scene_node();
			node->access_nodeToParent() = worldMat;
			app_scene->add_child(node);
			app_scene->add_mesh_instance(new mesh_instance(node, mesh, mat));

			btMatrix3x3 matrix(get_btMatrix3x3(worldMat));
			btVector3 pos(get_btVector3(worldMat[3].xyz()));

			btCollisionShape *shape = mesh->get_bullet_shape();
			if (shape)
			{
				btTransform transform(matrix, pos);

				btDefaultMotionState *motionState = new btDefaultMotionState(transform);
				btScalar mass = is_dynamic ? 0.5f : 0.0f;
				btVector3 inertiaTensor;

				shape->calculateLocalInertia(mass, inertiaTensor);

				btRigidBody *rigid_body = new btRigidBody(mass, motionState, shape, inertiaTensor);
				world->addRigidBody(rigid_body);
				rigid_bodies.push_back(rigid_body);
				rigid_body->setUserPointer(node);
			}
		}

		void world_rot()
		{
			/*bottom->translate(vec3(0, 10, 0));
			bottom->rotate(1, vec3(0, 0, 1));
			bottom->translate(vec3(0, -10, 0));

			top->translate(vec3(0, -10, 0));
			top->rotate(1, vec3(0, 0, 1));
			top->translate(vec3(0, 10, 0));

			left->rotate(-90, vec3(0, 0, 1));
			left->translate(vec3(10, 0, 0));
			left->rotate(1, vec3(0, 0, 1));
			left->translate(vec3(-10, 0, 0));
			left->rotate(90, vec3(0, 0, 1));

			right->rotate(-90, vec3(0, 0, 1));
			right->translate(vec3(-10, 0, 0));
			right->rotate(1, vec3(0, 0, 1));
			right->translate(vec3(10, 0, 0));
			right->rotate(90, vec3(0, 0, 1));

			bottom->translate(vec3(0, 0, -0.1f));
			top->translate(vec3(0, 0, -0.1f));
			left->translate(vec3(0, 0, -0.1f));
			right->translate(vec3(0, 0, -0.1f));*/
		}

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
			app_scene->get_camera_instance(0)->set_near_plane(1);
			app_scene->get_camera_instance(0)->set_far_plane(200);

			//mat4t modelToWorld;
			
			/*if (meshes.size())
			{
				material *shipMat = new material(new image("assets/ship.gif"));
				mesh *ship = meshes[0]->get_mesh();
				scene_node *shipNode = new scene_node();
				app_scene->add_child(shipNode);
				app_scene->add_mesh_instance(new mesh_instance(shipNode, ship, shipMat));
			}*/

			material *wall = new material(new image("src/examples/octet_game/material.gif"));
			material *blocks = new material(vec4(1, 1, 1, 1));
			mesh_box *wallMesh = new mesh_box(vec3(10.0f, 0.0f, 10.0f));
			/*bottom = new scene_node();
			bottom->translate(vec3(0, -10, 0));
			app_scene->add_child(bottom);
			app_scene->add_mesh_instance(new mesh_instance(bottom, wallMesh, wall));
			top = new scene_node();
			top->translate(vec3(0, 10, 0));
			app_scene->add_child(top);
			app_scene->add_mesh_instance(new mesh_instance(top, wallMesh, wall));
			left = new scene_node();
			left->translate(vec3(-10, 0, 0));
			left->rotate(90, vec3(0, 0, 1));
			app_scene->add_child(left);
			app_scene->add_mesh_instance(new mesh_instance(left, wallMesh, wall));
			right = new scene_node();
			right->translate(vec3(10, 0, 0));
			right->rotate(90, vec3(0, 0, 1));
			app_scene->add_child(right);
			app_scene->add_mesh_instance(new mesh_instance(right, wallMesh, wall));
			scene = new scene_node();
			scene->translate(vec3(0, 0, -100));
			scene->rotate(90, vec3(1, 0, 0));
			scene->scale(10);
			app_scene->add_child(scene);
			app_scene->add_mesh_instance(new mesh_instance(scene, wallMesh, wall));*/
			
			resource_dict dict;

			if (!loader.load_xml("src/examples/octet_game/pooltable.dae"))
			{
				return;
			}
			loader.get_resources(dict);

			dynarray<resource*> meshes;
			dict.find_all(meshes, atom_mesh);

			if (meshes.size())
			{
				for (int i = 0; i != meshes.size(); ++i)
				{
					material *shipMat = new material(new image("src/examples/octet_game/pooltableMat.gif"));
					mesh *shipMesh = meshes[i]->get_mesh();
					ship = new scene_node();
					ship->translate(vec3(0, -25, -100));
					ship->rotate(90, vec3(0, 1, 0));
					ship->scale(0.1);
					app_scene->add_child(ship);
					app_scene->add_mesh_instance(new mesh_instance(ship, shipMesh, shipMat));

				}
			}

			
			if (!loader.load_xml("src/examples/octet_game/redball.dae"))
			{
				return;
			}
			loader.get_resources(dict);

			dynarray<resource*> redmeshes;
			dict.find_all(redmeshes, atom_mesh);

			if (redmeshes.size())
			{
				for (int i = 0; i != redmeshes.size(); ++i)
				{
					material *shipMat = new material(new image("src/examples/octet_game/redballMat.gif"));
					mesh *shipMesh = redmeshes[i]->get_mesh();
					ship = new scene_node();
					ship->translate(vec3(0, -25, -100));
					ship->rotate(90, vec3(0, 1, 0));
					ship->scale(0.1);
					app_scene->add_child(ship);
					app_scene->add_mesh_instance(new mesh_instance(ship, shipMesh, shipMat));

				}
			}

			if (!loader.load_xml("src/examples/octet_game/yellowball.dae"))
			{
				return;
			}
			loader.get_resources(dict);

			dynarray<resource*> yellowmeshes;
			dict.find_all(yellowmeshes, atom_mesh);

			if (yellowmeshes.size())
			{
				for (int i = 0; i != yellowmeshes.size(); ++i)
				{
					material *shipMat = new material(new image("src/examples/octet_game/yellowballMat.gif"));
					mesh *shipMesh = yellowmeshes[i]->get_mesh();
					ship = new scene_node();
					ship->translate(vec3(0, -25, -100));
					ship->rotate(90, vec3(0, 1, 0));
					ship->scale(0.1);
					app_scene->add_child(ship);
					app_scene->add_mesh_instance(new mesh_instance(ship, shipMesh, shipMat));

				}
			}

			/*if (!loader.load_xml("src/examples/octet_game/cueball.dae"))
			{
				return;
			}
			loader.get_resources(dict);

			dynarray<resource*> cuemeshes;
			dict.find_all(cuemeshes, atom_mesh);

			if (cuemeshes.size())
			{
				for (int i = 0; i != cuemeshes.size(); ++i)
				{
					material *shipMat = new material(new image("src/examples/octet_game/cueballMat.gif"));
					mesh *shipMesh = cuemeshes[i]->get_mesh();
					ship = new scene_node();
					ship->translate(vec3(0, -25, -100));
					ship->rotate(90, vec3(0, 1, 0));
					ship->scale(0.1);
					app_scene->add_child(ship);
					app_scene->add_mesh_instance(new mesh_instance(ship, shipMesh, shipMat));

				}
			}*/

			if (!loader.load_xml("src/examples/octet_game/blackball.dae"))
			{
				return;
			}
			loader.get_resources(dict);

			dynarray<resource*> blackmeshes;
			dict.find_all(blackmeshes, atom_mesh);

			if (blackmeshes.size())
			{
				for (int i = 0; i != blackmeshes.size(); ++i)
				{
					material *shipMat = new material(new image("src/examples/octet_game/blackballMat.gif"));
					mesh *shipMesh = blackmeshes[i]->get_mesh();
					ship = new scene_node();
					ship->translate(vec3(0, -25, -100));
					ship->rotate(90, vec3(0, 1, 0));
					ship->scale(0.1);
					app_scene->add_child(ship);
					app_scene->add_mesh_instance(new mesh_instance(ship, shipMesh, shipMat));

				}
			}

			

		}

		void draw_world(int x, int y, int w, int h)
		{
			int vx = 0, vy = 0;
			get_viewport_size(vx, vy);
			app_scene->begin_render(vx, vy);

			

			//world_rot();
			
			/*world->stepSimulation(1.0f / 30, 1, 1.0f / 30);
			btCollisionObjectArray &colArray = world->getCollisionObjectArray();
			for (unsigned i = 0; i != colArray.size(); ++i)
			{
				btCollisionObject *colObj = colArray[i];
				scene_node *node = (scene_node *)colObj->getUserPointer();
				if (node)
				{
					mat4t &modelToWorld = node->access_nodeToParent();
					colObj->getWorldTransform().getOpenGLMatrix(modelToWorld.get());
				}
			}*/

			app_scene->update(1.0f / 30);
			app_scene->render((float)vx / vy);
		}
	};
}