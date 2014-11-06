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
				btScalar mass = is_dynamic ? 1.0f : 0.0f;
				btVector3 inertiaTensor;

				shape->calculateLocalInertia(mass, inertiaTensor);

				btRigidBody *rigid_body = new btRigidBody(mass, motionState, shape, inertiaTensor);
				world->addRigidBody(rigid_body);
				rigid_bodies.push_back(rigid_body);
				rigid_body->setUserPointer(node);

			}
		}

	public:
		octet_demo(int argc, char**argv) : app(argc, argv)
		{
			dispatcher = new btCollisionDispatcher(&config);
			broadphase = new btDbvtBroadphase();
			solver = new btSequentialImpulseConstraintSolver();
			world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, &config);
			world->setGravity(btVector3(0, -10, 0));
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
			app_scene = new visual_scene();
			app_scene->create_default_camera_and_lights();
			
			image *transparentImg = new image("assets/transparent.jpg");
			image *transparentmask = new image("assets/transparent.gif");
			param_shader *shader = new param_shader("shaders/default.vs", "shaders/multitexture.fs");
			material *transparent = new material(vec4(1, 1, 1, 1), shader);
			transparent->add_sampler(0, app_utils::get_atom("transparentImg"), transparentImg, new sampler());
			transparent->add_sampler(1, app_utils::get_atom("transparentmask"), transparentmask, new sampler());

			mesh_box *cubeWall = new mesh_box(vec3(10.0f, 0.0f, 10.0f));
			mesh_sphere *sphere = new mesh_sphere(vec3(0, 0, 0), 0.5f);
			mesh_sphere *smallSphere = new mesh_sphere(vec3(0, 0, 0), 0.25f);

			worldCoord.translate(vec3(0, -10, 0));
			add_shape(worldCoord, cubeWall, transparent, false);
			worldCoord.loadIdentity();

			worldCoord.translate(vec3(0, 10, 0));
			add_shape(worldCoord, cubeWall, transparent, false);
			worldCoord.translate(vec3(0, -10, 0));

			worldCoord.translate(vec3(10, 0, 0));
			worldCoord.rotateZ(-90);
			add_shape(worldCoord, cubeWall, transparent, false);
			worldCoord.rotateZ90();
			worldCoord.translate(vec3(-10, 0, 0));

			worldCoord.translate(vec3(-10, 0, 0));
			worldCoord.rotateZ(-90);
			add_shape(worldCoord, cubeWall, transparent, false);
			worldCoord.rotateZ90();
			worldCoord.translate(vec3(10, 0, 0));

			worldCoord.translate(vec3(0, 0, -10));
			worldCoord.rotateX90();
			add_shape(worldCoord, cubeWall, transparent, false);
			worldCoord.rotateX(-90);
			worldCoord.translate(vec3(0, 0, 10));

			worldCoord.translate(vec3(0, 0, 10));
			worldCoord.rotateX90();
			add_shape(worldCoord, cubeWall, transparent, false);
			worldCoord.rotateX(-90);
			worldCoord.translate(vec3(0, 0, -10));			
		}

		void draw_world(int x, int y, int w, int h)
		{
			int vx = 0, vy = 0;
			get_viewport_size(vx, vy);
			app_scene->begin_render(vx, vy);
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
		}
	};
}