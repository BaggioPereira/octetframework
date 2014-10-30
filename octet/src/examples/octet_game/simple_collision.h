namespace octet {
	class simple_collision : public app {

		ref <visual_scene> app_scene;
		btDefaultCollisionConfiguration config;
		btCollisionDispatcher *dispatcher;
		btDbvtBroadphase *broadphase;
		btSequentialImpulseConstraintSolver *solver;
		btDiscreteDynamicsWorld *world;

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
				btScalar mass = is_dynamic ? 0.25f : 0.0f;
				btVector3 inertiaTensor;

				shape->calculateLocalInertia(mass, inertiaTensor);

				btRigidBody *rigid_body = new btRigidBody(mass, motionState, shape, inertiaTensor);
				world->addRigidBody(rigid_body);
				rigid_bodies.push_back(rigid_body);				
				rigid_body->setUserPointer(node);

			}
		}

		void mouse()
		{
			int x, y;
			int vx = 0, vy = 0;

			mat4t mouseToWorld;
			get_viewport_size(vx, vy);
			get_mouse_pos(x, y);
			mouseToWorld.loadIdentity();
			mouseToWorld.translate(vec3(0, 15, 50));
			
			mouseToWorld.rotateY((float)-x*2.0f);
			if (vy / 2 - y<40 && vy / 2 - y>-40)
			{
				mouseToWorld.rotateX((float)vy / 2 - y);
			}
			else if (vy / 2 - y >= 40)
			{
				mouseToWorld.rotateX(40);
			}
			else if (vy / 2 - y <= -40)
			{
				mouseToWorld.rotateX(40);
			}
			
			if (is_key_down(key_lmb))
			{
				add_shape(mouseToWorld, new mesh_sphere(vec3(2), 2), new material(vec4(0, 1, 0, 1)), true);
				rigid_bodies.back()->applyCentralImpulse(btVector3(0, 0, -10));
				rigid_bodies.back()->setMassProps(20.0f, btVector3(0, 0, 0));
			}

			app_scene->get_camera_instance(0)->get_node()->access_nodeToParent() = mouseToWorld;
		}

	public:
		simple_collision(int argc, char **argv) : app(argc, argv) 
		{
			dispatcher = new btCollisionDispatcher(&config);
			broadphase = new btDbvtBroadphase();
			solver = new btSequentialImpulseConstraintSolver();
			world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, &config);
		}

		~simple_collision()
		{
			delete world;
			delete solver;
			delete broadphase;
			delete dispatcher;
		}

		void app_init()
		{
			int count = 0;
			app_scene = new visual_scene();
			app_scene->create_default_camera_and_lights();
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 15, 20));
			app_scene->get_camera_instance(0)->set_near_plane(1);
			app_scene->get_camera_instance(0)->set_far_plane(20000);

			mat4t modelToWorld;
			
			material *ground = new material(vec4(1, 1, 0, 0));
			material *border = new material(vec4(1, 0, 1, 1));
			material *box = new material(vec4(1, 1, 1, 1));
			material *ball = new material(vec4(0, 0, 1, 1));

			add_shape(modelToWorld, new mesh_box(vec3(20.0f, 0.0f, 20.0f)), ground, false);

			modelToWorld.translate(-11.5f, 5, 0);
			add_shape(modelToWorld, new mesh_box(vec3(1.0f, 5.0f, 1.0f)), border, false);
			modelToWorld.translate(11.5f, -5, 0);

			modelToWorld.translate(11.5f, 5, 0);
			add_shape(modelToWorld, new mesh_box(vec3(1.0f, 5.0f, 1.0f)), border, false);
			modelToWorld.translate(-11.5f, -5, 0);
			
			modelToWorld.translate(-10.0f, 0, 0);

			for (int j = -10; j != 11; ++j)
			{
				for (int i = 0; i != 10; ++i)
				{
					modelToWorld.translate(0, 1.0f, 0);
					add_shape(modelToWorld, new mesh_box(0.5f), box,true);
					count++;
					//printf("count %d", count);

				}
				modelToWorld.translate(1.0f, -10.0f, 0);
			}

			modelToWorld.translate(-10, 7.5f, -15);
			modelToWorld.rotate(-45, 1,0,0);
			add_shape(modelToWorld, new mesh_box(vec3(20.0f, 0.0f, 20.0f)), ground, false);

			modelToWorld.rotate(45, 1, 0, 0);
			/*modelToWorld.translate(0,250,-10);
			add_shape(modelToWorld, new mesh_sphere(vec3(2), 2), ball, true);
			rigid_bodies[214]->setMassProps(2.0f, btVector3(0, 0, 0));

			modelToWorld.translate(-5, 0, 0);
			add_shape(modelToWorld, new mesh_sphere(vec3(2), 2), ball, true);
			rigid_bodies[215]->setMassProps(2.0f, btVector3(0, 0, 0));

			modelToWorld.translate(10, 0, 0);
			add_shape(modelToWorld, new mesh_sphere(vec3(2), 2), ball, true);
			rigid_bodies[216]->setMassProps(2.0f, btVector3(0, 0, 0));


			modelToWorld.translate(-20, -250, 10);*/
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
			scene_node *cam_node = app_scene->get_camera_instance(0)->get_node();
			app_scene->update(1.0f / 30);
			app_scene->render((float)vx / vy);

			if (is_key_down('A'))
			{
				cam_node->rotate(-1, vec3(0, 1, 0));
			}
			else if (is_key_down('D'))
			{
				cam_node->rotate(1, vec3(0, 1, 0));
			}

			else if (is_key_down(key_left))
			{
				cam_node->translate(vec3(-1, 0, 0));
			}
			else if (is_key_down(key_right))
			{
				cam_node->translate(vec3(1, 0, 0));
			}
			else if (is_key_down(key_up))
			{
				cam_node->translate(vec3(0, 0, -1));
			}
			else if (is_key_down(key_down))
			{
				cam_node->translate(vec3(0, 0, 1));
			}

			mouse();
		}
	};
}