////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

#include <fstream>
#include <sstream>

namespace octet {
  /// Scene containing a box with octet.
  class water : public app {
    // scene for drawing box
    ref<visual_scene> app_scene;
	ref<mesh> mWater;
	mesh_builder waterMesh;
	scene_node *cam;

	dynarray<char> read;
	dynarray<char> waveLength;
	dynarray<char> amplitude;
	dynarray<char> speed;
	dynarray<char> steep;

	int num = 1;

	bool wire = false;

	enum res {
		WIDTH = 46,
		HEIGHT = 46,
		PADDEDWIDTH = WIDTH + 2,
		PADDEDHEIGHT = HEIGHT + 2,
	};

	float H[WIDTH][HEIGHT], U[WIDTH][HEIGHT], V[WIDTH][HEIGHT];

  public:
    /// this is called when we construct the class before everything is initialised.
    water(int argc, char **argv) : app(argc, argv) {
	}

	void loadWave(int waveNum)
	{
		std::fstream myFile;
		std::stringstream waveStr;
		std::string waveFile;
		read.reset();
		waveStr.clear();
		waveFile.clear();
		waveStr << "wave" << num <<".txt";
		waveFile = waveStr.str();
		myFile.open(waveFile, std::ios::in);

		if (!myFile.is_open())
		{
			printf("File is not found/opened\n");
		}
		else
		{
			printf("File found/opened\n");
		}
		myFile.seekg(0, myFile.end);
		int length = myFile.tellg();
		myFile.seekg(0, myFile.beg);

		read.resize(length);
		myFile.read(read.data(), length);
		myFile.close();
	}
	
	void meshGeneration()
	{
		vec4 vertices[4];

		int vertexNum = 0;
		for (int z = 0; z < 40; z++)
		{
			for (int x = 0; x < 40; x++)
			{
				vertices[0] = vec4(x*1.0f, 0.0f, z*1.0f, 1);
				vertices[1] = vec4(x*1.0 + 1.0f, 0.0f, z*1.0f, 1);
				vertices[2] = vec4(x*1.0f + 1.0f, 0.0f, z*1.0f + 1.0f, 1);
				vertices[3] = vec4(x*1.0f, 0.0f, z*1.0f + 1.0f, 1);

				for (size_t i = 0; i < 4; ++i) {
					waterMesh.add_vertex(vertices[i], vec4(1, 1, 1, 1), 1, 1);
				}

				waterMesh.add_index(vertexNum);
				waterMesh.add_index(vertexNum + 1);
				waterMesh.add_index(vertexNum + 2);
				waterMesh.add_index(vertexNum);
				waterMesh.add_index(vertexNum + 2);
				waterMesh.add_index(vertexNum + 3);

				vertexNum += 4;
			}
		}
	}

    /// this is called once OpenGL is initialized
    void app_init() {
      app_scene =  new visual_scene();
      app_scene->create_default_camera_and_lights();
	  scene_node *water_node = app_scene->add_scene_node();
	  material *blue = new material(vec4(0, 0, 1, 1));
	  waterMesh.init();
	  meshGeneration();
	  mWater = new mesh();
	  waterMesh.get_mesh(*mWater);
	  mWater->set_mode(GL_TRIANGLES);
	  app_scene->add_mesh_instance(new mesh_instance(water_node, mWater, blue));
	  cam = app_scene->get_camera_instance(0)->get_node();
	  cam->translate(vec3(20, 10, 40));
	  loadWave(num);
    }

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
	  app_scene->begin_render(vx, vy);
	  glClearColor(0, 0, 0, 1);
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	  glEnable(GL_DEPTH_TEST);

	  if (is_key_going_down(key_space))
	  {
		  if (wire)
		  {
			  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			  wire = false;
		  }

		  else
		  {
			  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			  wire = true;
		  }
	  }

	  if (is_key_going_down(key_left))
	  {
		  if (num > 1)
		  {
			  num--;
			  loadWave(num);
		  }
		  else
		  {
			  printf("No more waves\n");
		  }
	  }

	  if (is_key_going_down(key_right))
	  {
		  num++;
		  loadWave(num);
	  }

      // update matrices. assume 30 fps.
      app_scene->update(1.0f/30);

      // draw the scene
      app_scene->render((float)vx / vy);
    }
  };
}
