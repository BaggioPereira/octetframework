namespace octet
{
	class octet_demo :public app
	{
		ref<visual_scene> app_scene;					//the scene
		btDefaultCollisionConfiguration config;			//setup for the world
		btCollisionDispatcher *dispatcher;				//handler for collisions between objects
		btDbvtBroadphase *broadphase;					//handler for rough collisions
		btSequentialImpulseConstraintSolver *solver;	//handler for resolve collisions
		btDiscreteDynamicsWorld *world;					//the physics world, with rigid bodies

		mat4t worldCoord;								//matrix for the world coordinates

		dynarray<btRigidBody*> rigid_bodies;			//dynamic array for rigid bodies
		material *ballMat;								//material for the balls
		mesh_box *cubeWall;								//box mesh for the cube wall
		mesh_sphere *smallSphere;						//sphere mesh for a small ball
		mesh_sphere *sphere;							//sphere mesh for a large ball
		random r;										//random number generator
		scene_node *cam;								//scene node for the camera

		// function to add a mesh to the physics world
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

		//function to add balls to the world on key input
		void addBalls()
		{
			//get a random x,y,z position inside the cube
			int x = 0, y = 0, z = 0;
			x = r.get(-9, 9);
			y = r.get(-9, 9);
			z = r.get(-9, 9);
			worldCoord.translate(vec3(x, y, z));

			//add a small ball
			if (is_key_down(key_lmb))
			{
				add_shape(worldCoord, smallSphere, ballMat, true);
				rigid_bodies.back()->setFriction(-0.5);
				rigid_bodies.back()->setRestitution(1);
			}

			//add a large ball
			else if (is_key_down(key_rmb))
			{
				add_shape(worldCoord, sphere, ballMat, true);
				rigid_bodies.back()->setFriction(-0.5);
				rigid_bodies.back()->setRestitution(1);
			}
			worldCoord.loadIdentity();
		}

		//function to apply a force to the balls depending on key input
		void applyForce()
		{
			//apply upward force
			if (is_key_going_down(key_up))
			{
				for (int i = 0; i != rigid_bodies.size(); ++i)
				{
					rigid_bodies[i]->applyCentralForce(btVector3(0.0f, 150.0f, 0.0f));
				}
			}

			//apply downward force
			else if (is_key_going_down(key_down))
			{
				for (int i = 0; i != rigid_bodies.size(); ++i)
				{
					rigid_bodies[i]->applyCentralForce(btVector3(0.0f, -150.0f, 0.0f));
				}
			}

			//apply a force to the left
			else if (is_key_going_down(key_left))
			{
				for (int i = 0; i != rigid_bodies.size(); ++i)
				{
					rigid_bodies[i]->applyCentralForce(btVector3(-150.0f, 0.0f, 0.0f));
				}
			}

			//apply a force to the right
			else if (is_key_going_down(key_right))
			{
				for (int i = 0; i != rigid_bodies.size(); ++i)
				{
					rigid_bodies[i]->applyCentralForce(btVector3(150.0f, 0.0f, 0.0f));
				}
			}
		}

		//function to rotate the camera
		void camRotation()
		{
			cam->translate(vec3(0, 0, -40));

			//rotate camera on  x axis by -1 degree
			if (is_key_down('U'))
			{
				cam->rotate(-1, vec3(1, 0, 0));
			}

			//rotate camera on x axis by 1 degree
			else if (is_key_down('J'))
			{
				cam->rotate(1, vec3(1, 0, 0));
			}

			//rotate camera y axis by -1 degree
			if (is_key_down('I'))
			{
				cam->rotate(-1, vec3(0, 1, 0));
			}

			//rotate camera y axis by 1 degree
			else if (is_key_down('K'))
			{
				cam->rotate(1, vec3(0, 1, 0));
			}

			//rotate camera z axis by -1 degree
			if (is_key_down('O'))
			{
				cam->rotate(-1, vec3(0, 0, 1));
			}

			//rotate camera z axis by 1 degree
			else if (is_key_down('L'))
			{
				cam->rotate(1, vec3(0, 0, 1));

			}

			cam->translate(vec3(0, 0, 40));
		}

	public:
		//constructor for the class
		octet_demo(int argc, char**argv) : app(argc, argv)
		{
			dispatcher = new btCollisionDispatcher(&config);
			broadphase = new btDbvtBroadphase();
			solver = new btSequentialImpulseConstraintSolver();
			world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, &config);
		}

		//destructor for the class
		~octet_demo()
		{
			delete world;
			delete solver;
			delete broadphase;
			delete dispatcher;
		}

		//function that is called once OpenGL is initalized
		void app_init()
		{
			r.set_seed(time(NULL));										//sets the random seed
			app_scene = new visual_scene();								//add a visual scene
			app_scene->create_default_camera_and_lights();				//create default and camera for the scene
			cam = app_scene->get_camera_instance(0)->get_node();		//get the node to the camera
			cam->loadIdentity();										//load the identity matrix of the camera
			cam->translate(vec3(0, 0, 40));								//translate the camera
			
			//add a shader to for the material, creates transparency so you can see inside
			image *transparentImg = new image("assets/transparent.jpg");
			image *transparentmask = new image("assets/transparent.gif");
			param_shader *transparentShader = new param_shader("shaders/default.vs", "shaders/multitexture.fs");
			material *transparent = new material(vec4(1, 1, 1, 1), transparentShader);
			transparent->add_sampler(0, app_utils::get_atom("transparentImg"), transparentImg, new sampler());

			cubeWall = new mesh_box(vec3(10.0f, 0.0f, 10.0f));	//set the size of a flat mesh box
			sphere = new mesh_sphere(vec3(0, 0, 0), 1.0f);		//set the size of the large ball
			smallSphere = new mesh_sphere(vec3(0, 0, 0), 0.5f); //set the size of the small ball
			ballMat = new material(vec4(0, 0, 1, 1));			//set the material of the ball

			//creates a box with transparent material and adds friction and restitution
			//bottom wall
			worldCoord.translate(vec3(0, -10, 0));
			add_shape(worldCoord, cubeWall, transparent, false);
			rigid_bodies.back()->setFriction(-0.5);
			rigid_bodies.back()->setRestitution(1);
			worldCoord.loadIdentity();

			//top wall
			worldCoord.translate(vec3(0, 10, 0));
			add_shape(worldCoord, cubeWall, transparent, false);
			rigid_bodies.back()->setFriction(-0.5);
			rigid_bodies.back()->setRestitution(1);
			worldCoord.loadIdentity();

			//right wall
			worldCoord.translate(vec3(10, 0, 0));
			worldCoord.rotateZ(-90);
			add_shape(worldCoord, cubeWall, transparent, false);
			rigid_bodies.back()->setFriction(-0.5);
			rigid_bodies.back()->setRestitution(1);
			worldCoord.loadIdentity();

			//left wall
			worldCoord.translate(vec3(-10, 0, 0));
			worldCoord.rotateZ(-90);
			add_shape(worldCoord, cubeWall, transparent, false);
			rigid_bodies.back()->setFriction(-0.5);
			rigid_bodies.back()->setRestitution(1);
			worldCoord.loadIdentity();

			//back wall
			worldCoord.translate(vec3(0, 0, -10));
			worldCoord.rotateX90();
			add_shape(worldCoord, cubeWall, transparent, false);
			rigid_bodies.back()->setFriction(-0.5);
			rigid_bodies.back()->setRestitution(1);
			worldCoord.loadIdentity();

			//front wall
			worldCoord.translate(vec3(0, 0, 10));
			worldCoord.rotateX90();
			add_shape(worldCoord, cubeWall, transparent, false);
			rigid_bodies.back()->setFriction(-0.5);
			rigid_bodies.back()->setRestitution(1);
			worldCoord.loadIdentity();

			//commands printed out to console, instructions for the demo
			printf("Left mouse click to add a small ball\n");
			printf("Right mouse click to add a bigger ball\n");
			printf("Press Up to add apply a upward force on the balls\n");
			printf("Press Down to add apply a downward force on the balls\n");
			printf("Press Left to add apply a left force on the balls\n");
			printf("Press Right to add apply a right force on the balls\n");
			printf("Press U to rotate on x axis by -1 degree\n");
			printf("Press J to rotate on x axis by 1 degree\n");
			printf("Press I to rotate on y axis by -1 degree\n");
			printf("Press K to rotate on y axis by 1 degree\n");
			printf("Press O to rotate on z axis by -1 degree\n");
			printf("Press L to rotate on z axis by 1 degree\n");
		}

		//function called to draw the world
		void draw_world(int x, int y, int w, int h)
		{
			int vx = 0, vy = 0;
			get_viewport_size(vx, vy);
			app_scene->begin_render(vx, vy);
			
			addBalls();		//calls function to add balls
			applyForce();	//calls function that adds force
			camRotation();	//calls function to rotate the camera around the cube

			//update for the collisions, at 30 frames per second
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

			//update matrices, at 30 frames per second
			app_scene->update(1.0f / 30);

			//redraw the scene
			app_scene->render((float)vx / vy);
		}
	};
}