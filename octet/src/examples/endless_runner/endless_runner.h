////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
namespace octet {
  /// Scene containing a box with octet.
  class endless_runner : public app 
  {
    // scene for drawing box
    ref<visual_scene> app_scene;
	scene_node *node;
	random rand;
	material *mat;
	mesh *plane;

  public:
    /// this is called when we construct the class before everything is initialised.
    endless_runner(int argc, char **argv) : app(argc, argv) 
	{
    }

    /// this is called once OpenGL is initialized
    void app_init() 
	{
      app_scene =  new visual_scene();
	  app_scene->create_default_camera_and_lights();

	  rand.set_seed(time(NULL));
	  node = new scene_node();
	  mat = new material(new image("assets/galaxy.jpg"));
	  app_scene->add_child(node);

	  /*mesh *plane = new mesh_box(vec3(4));
	  
	  
	  app_scene->add_mesh_instance(new mesh_instance(node, plane, tex));*/
    }

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) 
	{
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);

      // update matrices. assume 30 fps.
      app_scene->update(1.0f/30);

      // draw the scene
      app_scene->render((float)vx / vy);

	  if(is_key_going_down('D'))
		  runRandomNum();

      // tumble the box  (there is only one mesh instance)
      /*scene_node *node = app_scene->get_mesh_instance(0)->get_node();
      node->rotate(1, vec3(1, 1, 0));*/
    }

	void runRandomNum()
	{
		int i = rand.get(0, 4);
		createObj(i);
	}

	void createObj(int i)
	{
		if (i == 0)
		{
			plane = new mesh_box(vec3(1,1,0));
		}
		else if (i == 1)
		{
			plane = new mesh_box(vec3(2));
		}
		else if (i == 2)
		{
			plane = new mesh_sphere(vec3(rand.get(0, 3), rand.get(0, 3), rand.get(0, 3)), 3);
		}
		else if (i == 4)
		{
			plane = new mesh_sphere(vec3(rand.get(3, 6), rand.get(3, 6), rand.get(3, 6)), 5);
		}
		node->loadIdentity();
		node->translate(vec3(rand.get(0, 3), rand.get(0, 3), rand.get(0, 3)));
		app_scene->add_mesh_instance(new mesh_instance(node, plane, mat));
	}
  };
}
