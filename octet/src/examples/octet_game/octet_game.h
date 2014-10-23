namespace octet {

	class octet_game : public app {

		ref <visual_scene> app_scene;
		btDefaultCollisionConfiguration config;
		btCollisionDispatcher *dispatcher;
		btDbvtBroadphase *broadphase;
		btSequentialImpulseConstraintSolver *solver;
		btDiscreteDynamicsWorld *world;

		dynarray<btRigidBody*> rigid_bodies;
		dynarray<scene_node*> nodes;

		void add_box(mat4t_in modelToWorld, vec3_in size, material *mat, bool is_dynamic = true)
		{
			btMatrix3x3 matrix(get_btMatrix3x3(modelToWorld));
			btVector3 pos(get_btVector3(modelToWorld[3].xyz()));

			btCollisionShape *shape = new btBoxShape(get_btVector3(size));

			btTransform transform(matrix, pos);

			btDefaultMotionState *motionState = new btDefaultMotionState(transform);
			btScalar mass = is_dynamic ? 1.0f : 0.0f;
			btVector3 inertiaTensor;

			shape->calculateLocalInertia(mass, inertiaTensor);

			btRigidBody *rigid_body = new btRigidBody(mass, motionState, shape, inertiaTensor);
			world->addRigidBody(rigid_body);
			rigid_bodies.push_back(rigid_body);

			mesh_box *box = new mesh_box(size);
			scene_node *node = new scene_node(modelToWorld, atom_);
			nodes.push_back(node);

			app_scene->add_child(node);
			app_scene->add_mesh_instance(new mesh_instance(node, box, mat));
		}

	public:
		octet_game(int argc, char **argv) : app(argc, argv) 
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

			mat4t modelToWorld;
			material *ground = new material(vec4(1, 1, 0, 0));
			material *border = new material(vec4(1, 0, 1, 1));

			add_box(modelToWorld, vec3(20.0f, 0.0f, 20.0f), ground, false);

			modelToWorld.translate(-11.5f, 5, 0);
			add_box(modelToWorld, vec3(1.0f, 5.0f, 1.0f), border, false);
			modelToWorld.translate(11.5f, -5, 0);

			modelToWorld.translate(11.5f, 5, 0);
			add_box(modelToWorld, vec3(1.0f, 5.0f, 1.0f), border, false);
			modelToWorld.translate(-11.5f, -5, 0);
			
			material *box = new material(vec4(1, 1, 1, 1));
			modelToWorld.translate(-10.0f, 0, 0);
			for (int i = 0; i != 10; ++i)
			{
				modelToWorld.translate(0, 1.0f, 0);
				add_box(modelToWorld, vec3(0.5f), box);
			}
			modelToWorld.translate(1.0f,-10.0f, 0);
			for (int i = 0; i != 10; ++i)
			{
				modelToWorld.translate(0, 1.0f, 0);
				add_box(modelToWorld, vec3(0.5f), box);
			}
			modelToWorld.translate(1.0f, -10.0f, 0);
			for (int i = 0; i != 10; ++i)
			{
				modelToWorld.translate(0, 1.0f, 0);
				add_box(modelToWorld, vec3(0.5f), box);
			}
			modelToWorld.translate(1.0f, -10.0f, 0);
			for (int i = 0; i != 10; ++i)
			{
				modelToWorld.translate(0, 1.0f, 0);
				add_box(modelToWorld, vec3(0.5f), box);
			}
			modelToWorld.translate(1.0f, -10.0f, 0);
			for (int i = 0; i != 10; ++i)
			{
				modelToWorld.translate(0, 1.0f, 0);
				add_box(modelToWorld, vec3(0.5f), box);
			}
			modelToWorld.translate(1.0f, -10.0f, 0);
			for (int i = 0; i != 10; ++i)
			{
				modelToWorld.translate(0, 1.0f, 0);
				add_box(modelToWorld, vec3(0.5f), box);
			}
			modelToWorld.translate(1.0f, -10.0f, 0);
			for (int i = 0; i != 10; ++i)
			{
				modelToWorld.translate(0, 1.0f, 0);
				add_box(modelToWorld, vec3(0.5f), box);
			}
			modelToWorld.translate(1.0f, -10.0f, 0);
			for (int i = 0; i != 10; ++i)
			{
				modelToWorld.translate(0, 1.0f, 0);
				add_box(modelToWorld, vec3(0.5f), box);
			}
			modelToWorld.translate(1.0f, -10.0f, 0);
			for (int i = 0; i != 10; ++i)
			{
				modelToWorld.translate(0, 1.0f, 0);
				add_box(modelToWorld, vec3(0.5f), box);
			}
			modelToWorld.translate(1.0f, -10.0f, 0);
			for (int i = 0; i != 10; ++i)
			{
				modelToWorld.translate(0, 1.0f, 0);
				add_box(modelToWorld, vec3(0.5f), box);
			}
			modelToWorld.translate(1.0f, -10.0f, 0);
			for (int i = 0; i != 10; ++i)
			{
				modelToWorld.translate(0, 1.0f, 0);
				add_box(modelToWorld, vec3(0.5f), box);
			}
			modelToWorld.translate(1.0f, -10.0f, 0);
			for (int i = 0; i != 10; ++i)
			{
				modelToWorld.translate(0, 1.0f, 0);
				add_box(modelToWorld, vec3(0.5f), box);
			}
			modelToWorld.translate(1.0f, -10.0f, 0);
			for (int i = 0; i != 10; ++i)
			{
				modelToWorld.translate(0, 1.0f, 0);
				add_box(modelToWorld, vec3(0.5f), box);
			}
			modelToWorld.translate(1.0f, -10.0f, 0);
			for (int i = 0; i != 10; ++i)
			{
				modelToWorld.translate(0, 1.0f, 0);
				add_box(modelToWorld, vec3(0.5f), box);
			}
			modelToWorld.translate(1.0f, -10.0f, 0);
			for (int i = 0; i != 10; ++i)
			{
				modelToWorld.translate(0, 1.0f, 0);
				add_box(modelToWorld, vec3(0.5f), box);
			}
			modelToWorld.translate(1.0f, -10.0f, 0);
			for (int i = 0; i != 10; ++i)
			{
				modelToWorld.translate(0, 1.0f, 0);
				add_box(modelToWorld, vec3(0.5f), box);
			}
			modelToWorld.translate(1.0f, -10.0f, 0);
			for (int i = 0; i != 10; ++i)
			{
				modelToWorld.translate(0, 1.0f, 0);
				add_box(modelToWorld, vec3(0.5f), box);
			}
			modelToWorld.translate(1.0f, -10.0f, 0);
			for (int i = 0; i != 10; ++i)
			{
				modelToWorld.translate(0, 1.0f, 0);
				add_box(modelToWorld, vec3(0.5f), box);
			}
			modelToWorld.translate(1.0f, -10.0f, 0);
			for (int i = 0; i != 10; ++i)
			{
				modelToWorld.translate(0, 1.0f, 0);
				add_box(modelToWorld, vec3(0.5f), box);
			}
			modelToWorld.translate(1.0f, -10.0f, 0);
			for (int i = 0; i != 10; ++i)
			{
				modelToWorld.translate(0, 1.0f, 0);
				add_box(modelToWorld, vec3(0.5f), box);
			}
			modelToWorld.translate(1.0f, -10.0f, 0);
			for (int i = 0; i != 10; ++i)
			{
				modelToWorld.translate(0, 1.0f, 0);
				add_box(modelToWorld, vec3(0.5f), box);
			}
			
		}

		void draw_world(int x, int y, int w, int h)
		{
			int vx = 0, vy = 0;
			get_viewport_size(vx, vy);
			app_scene->begin_render(vx, vy);

			world->stepSimulation(1.0f / 30);
			for (unsigned i = 0; i != rigid_bodies.size(); ++i)
			{
				btRigidBody *rigid_body = rigid_bodies[i];
				btQuaternion btq = rigid_body->getOrientation();
				btVector3 pos = rigid_body->getCenterOfMassPosition();
				quat q(btq[0], btq[1], btq[2], btq[3]);
				mat4t modelToWorld = q;
				modelToWorld[3] = vec4(pos[0], pos[1], pos[2], 1);
				nodes[i]->access_nodeToParent() = modelToWorld;
			}

			app_scene->update(1.0f / 30);
			app_scene->render((float)vx / vy);
		}
	};
}