namespace octet
{
	class octet_demo :public app
	{
		ref<visual_scene> app_scene;
		btDefaultCollisionConfiguration config;
		btCollisionDispatcher *dispatcher;
		btDbvtBroadphase *broadphase;
		btSequentialImpulseConstraintSolver *solver;
		btDiscreteDynamicsWorld *world;

		mat4t worldCoord;

		dynarray<btRigidBody*> rigid_bodies;
		material *ballMat;
		mesh_sphere *smallSphere;
		mesh_sphere *sphere;
		random r;

		void add_shape(mat4t_in mat, mesh *msh, material *mtl, bool is_dynamic)
		{
			scene_node *node = new scene_node();
			node->access_nodeToParent() = mat;
			app_scene->add_child(node);
			app_scene->add_mesh_instance(new mesh_instance(node, msh, mtl));

			btMatrix3x3 matrix(get_btMatrix3x3(mat));
			btVector3 pos(get_btVector3(mat[3].xyz()));

			btCollisionShape *shape = msh->get_bullet_shape();
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

		void addBalls()
		{
			int x = 0, y = 0, z = 0;
			x = r.get(-9, 9);
			y = r.get(-9, 9);
			z = r.get(-9, 9);
			worldCoord.translate(vec3(x, y, z));
			if (is_key_down('A'))
			{
				add_shape(worldCoord, smallSphere, ballMat, true);
				rigid_bodies.back()->setFriction(-0.5);
				rigid_bodies.back()->setRestitution(1);
			}
			else if (is_key_down('S'))
			{
				add_shape(worldCoord, sphere, ballMat, true);
				rigid_bodies.back()->setFriction(-0.5);
				rigid_bodies.back()->setRestitution(1);
			}
			worldCoord.loadIdentity();
		}

		void applyForce()
		{
			if (is_key_going_down(key_up))
			{
				for (int i = 0; i != rigid_bodies.size(); ++i)
				{
					rigid_bodies[i]->applyCentralForce(btVector3(0.0f, 150.0f, 0.0f));
				}
			}
			else if (is_key_going_down(key_down))
			{
				for (int i = 0; i != rigid_bodies.size(); ++i)
				{
					rigid_bodies[i]->applyCentralForce(btVector3(0.0f, -150.0f, 0.0f));
				}
			}
			else if (is_key_going_down(key_left))
			{
				for (int i = 0; i != rigid_bodies.size(); ++i)
				{
					rigid_bodies[i]->applyCentralForce(btVector3(-150.0f, 0.0f, 0.0f));
				}
			}
			else if (is_key_going_down(key_right))
			{
				for (int i = 0; i != rigid_bodies.size(); ++i)
				{
					rigid_bodies[i]->applyCentralForce(btVector3(150.0f, 0.0f, 0.0f));
				}
			}
		}

	public:
		octet_demo(int argc, char**argv) : app(argc, argv)
		{
			dispatcher = new btCollisionDispatcher(&config);
			broadphase = new btDbvtBroadphase();
			solver = new btSequentialImpulseConstraintSolver();
			world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, &config);
		}

		~octet_demo()
		{
			delete world;
			delete solver;
			delete broadphase;
			delete dispatcher;
		}

		void app_init()
		{
			r.set_seed(time(NULL));
			app_scene = new visual_scene();
			app_scene->create_default_camera_and_lights();
			
			image *transparentImg = new image("assets/transparent.jpg");
			image *transparentmask = new image("assets/transparent.gif");
			param_shader *shader = new param_shader("shaders/default.vs", "shaders/multitexture.fs");
			material *transparent = new material(vec4(1, 1, 1, 1), shader);
			transparent->add_sampler(0, app_utils::get_atom("transparentImg"), transparentImg, new sampler());
			transparent->add_sampler(1, app_utils::get_atom("transparentmask"), transparentmask, new sampler());

			mesh_box *cubeWall = new mesh_box(vec3(10.0f, 0.0f, 10.0f));
			sphere = new mesh_sphere(vec3(0, 0, 0), 1.0f);
			smallSphere = new mesh_sphere(vec3(0, 0, 0), 0.5f);
			ballMat = new material(vec4(0, 0, 1, 1));

			worldCoord.translate(vec3(0, -10, 0));
			add_shape(worldCoord, cubeWall, transparent, false);
			rigid_bodies.back()->setFriction(-0.5);
			rigid_bodies.back()->setRestitution(1);
			worldCoord.loadIdentity();

			worldCoord.translate(vec3(0, 10, 0));
			add_shape(worldCoord, cubeWall, transparent, false);
			rigid_bodies.back()->setFriction(-0.5);
			rigid_bodies.back()->setRestitution(1);
			worldCoord.loadIdentity();

			worldCoord.translate(vec3(10, 0, 0));
			worldCoord.rotateZ(-90);
			add_shape(worldCoord, cubeWall, transparent, false);
			rigid_bodies.back()->setFriction(-0.5);
			rigid_bodies.back()->setRestitution(1);
			worldCoord.loadIdentity();

			worldCoord.translate(vec3(-10, 0, 0));
			worldCoord.rotateZ(-90);
			add_shape(worldCoord, cubeWall, transparent, false);
			rigid_bodies.back()->setFriction(-0.5);
			rigid_bodies.back()->setRestitution(1);
			worldCoord.loadIdentity();

			worldCoord.translate(vec3(0, 0, -10));
			worldCoord.rotateX90();
			add_shape(worldCoord, cubeWall, transparent, false);
			rigid_bodies.back()->setFriction(-0.5);
			rigid_bodies.back()->setRestitution(1);
			worldCoord.loadIdentity();

			worldCoord.translate(vec3(0, 0, 10));
			worldCoord.rotateX90();
			add_shape(worldCoord, cubeWall, transparent, false);
			rigid_bodies.back()->setFriction(-0.5);
			rigid_bodies.back()->setRestitution(1);
			worldCoord.loadIdentity();

			printf("Press A to add a small ball\n");
			printf("Press S to add a bigger ball\n");
			printf("Press Up to add apply a upward force on the balls\n");
			printf("Press Down to add apply a downward force on the balls\n");
			printf("Press Left to add apply a left force on the balls\n");
			printf("Press Right to add apply a right force on the balls\n");
		}

		void draw_world(int x, int y, int w, int h)
		{
			int vx = 0, vy = 0;
			get_viewport_size(vx, vy);
			app_scene->begin_render(vx, vy);
			
			addBalls();
			applyForce();

			world->stepSimulation(1.0f / 30, 1.0f / 30, 1.0f / 30);
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
		}
	};
}