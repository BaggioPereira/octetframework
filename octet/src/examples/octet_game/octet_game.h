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

		scene_node *scene;
		scene_node *ship;

		mesh_box *leftObstacle;
		mesh_box *rightObstacle;
		mesh_box *bar;
		material *blocks;

		random rand;

		mat4t enemy;

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

		void worldRotation()
		{
			scene_node *cam = app_scene->get_camera_instance(0)->get_node();

			scene->translate(vec3(0, -0.005f, 0));

			cam->translate(vec3(0, 0, -0.1f));

			rigid_bodies[0]->translate(btVector3(0, 0, -0.1f));
			rigid_bodies[1]->translate(btVector3(0, 0, -0.1f));
		}

		void  playerMovement()
		{
			if (is_key_going_down(key_up))
			{
				rigid_bodies[1]->translate(btVector3(0, 1.0f, 0));
			}

			else if (is_key_down(key_down))
			{
				rigid_bodies[1]->translate(btVector3(0, -1.0f, 0));
			}

			if (is_key_down(key_right))
			{
				rigid_bodies[1]->translate(btVector3(0.1f, 0, 0));
			}

			else if (is_key_down(key_left))
			{
				rigid_bodies[1]->translate(btVector3(-0.1f, 0, 0));
			}
			else if (is_key_going_down(key_space))
			{
				rigid_bodies[1]->applyCentralForce(btVector3(0.0f, 150.0f, 0.0f));
			}
		}

		void enemies(int enemyNum)
		{
			enemy.translate(0, 0, -10);
			if (enemyNum == 1)
			{	
				enemy.translate(-5, -5, -10);
				add_shape(enemy, leftObstacle, blocks, true);
				enemy.translate(5, 5, 10);
			}

			else if (enemyNum == 2)
			{
				enemy.translate(5, -5, -10);
				add_shape(enemy, rightObstacle, blocks, true);
				enemy.translate(-5, 5, 10);
			}

			else if (enemyNum == 3)
			{
				enemy.translate(0, 2.5, -10);
				add_shape(enemy, bar, blocks, true);
				enemy.translate(0, -2.5, 10);
			}
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
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 0, -10));

			rand.set_seed(time(NULL));

			mat4t modelToWorld;

			material *wall = new material(new image("src/examples/octet_game/material.gif"));
			blocks = new material(vec4(1, 1, 1, 1));
			material *playerMat = new material(vec4(1, 0, 0, 1));
			mesh_box *wallMesh = new mesh_box(vec3(10.0f, 0.0f, 1000.0f));
			mesh_box *player = new mesh_box(1.0f);

			leftObstacle = new mesh_box(vec3(5.0f, 5.0f, 0.1f));
			rightObstacle = new mesh_box(vec3(5.0f, 5.0f, 0.1f));
			bar = new mesh_box(vec3(10.0f, 0.5f, 1.0f));
			modelToWorld.translate(vec3(0, -7.5f, 0));
			add_shape(modelToWorld, wallMesh, wall, false);
			modelToWorld.translate(vec3(0, 7.5f, 0));
			scene = new scene_node();
			scene->translate(vec3(0, 0, -100));
			scene->rotate(90, vec3(1, 0, 0));
			scene->scale(10);
			app_scene->add_child(scene);
			app_scene->add_mesh_instance(new mesh_instance(scene, wallMesh, wall));

			modelToWorld.translate(vec3(0, -5, 0));

			add_shape(modelToWorld, player, playerMat, true);
		}

		void draw_world(int x, int y, int w, int h)
		{
			int vx = 0, vy = 0;
			
			get_viewport_size(vx, vy);
			app_scene->begin_render(vx, vy);

			scene_node *cam = app_scene->get_camera_instance(0)->get_node();

			worldRotation();
			playerMovement();



			world->stepSimulation(1.0f / 30, 1, 1.0f / 30);
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
			}

			app_scene->update(1.0f / 30);
			app_scene->render((float)vx / vy);

			int enemyNum;
			enemyNum = rand.get(1, 4);
			printf("enemy number is %d\n", enemyNum);

			enemies(enemyNum);
		}
	};
}