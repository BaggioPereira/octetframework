////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
namespace octet {
  /// Scene containing a box with octet.
  class water : public app {
    // scene for drawing box
    ref<visual_scene> app_scene;
	mesh_builder waterMesh;
	octet::ref<mesh> mWater;
	enum Resolutions {
		WIDTH = 66,
		HEIGHT = 66,
		PADDED_WIDTH = WIDTH + 2,
		PADDED_HEIGHT = HEIGHT + 2
	};

	float H[WIDTH][HEIGHT], V[WIDTH][HEIGHT], U[WIDTH][HEIGHT];
	float Hx[PADDED_WIDTH][PADDED_HEIGHT], Hy[PADDED_WIDTH][PADDED_HEIGHT], Ux[PADDED_WIDTH][PADDED_HEIGHT], Uy[PADDED_WIDTH][PADDED_HEIGHT], Vx[PADDED_WIDTH][PADDED_HEIGHT], Vy[PADDED_WIDTH][PADDED_HEIGHT];
	int n;
	float g;
	float dt;
	float dx;
	float dy;

	octet::mat4t& getCameraMatrix() {
		camera_instance* camera = app_scene->get_camera_instance(0);
		return camera->get_node()->access_nodeToParent();
	};

  public:
    /// this is called when we construct the class before everything is initialised.
    water(int argc, char **argv) : app(argc, argv) {
    }

	void generateMesh() {
		vec4 vertices[4];
		vec4 normals[4];
		vec2 uvs[4];

		const float deltaZ = 1.0f / 60.0f;
		const float deltaX = 1.0f / 60.0f;

		int numVertices = 0;
		for (int z = 0; z < 60; z++) {
			for (int x = 0; x < 60; x++) {
				vertices[0] = vec4(x*1.0f, H[x][z], z*1.0f, 1);
				vertices[1] = vec4(x*1.0f + 1.0f, H[x + 1][z], z*1.0f, 1);
				vertices[2] = vec4(x*1.0f + 1.0f, H[x + 1][z + 1], z*1.0f + 1.0f, 1);
				vertices[3] = vec4(x*1.0f, H[x][z + 1], z*1.0f + 1.0f, 1);

				normals[0] = (vertices[1] - vertices[0]).cross(vertices[3] - vertices[0]);
				normals[1] = (vertices[2] - vertices[1]).cross(vertices[0] - vertices[1]);
				normals[2] = (vertices[3] - vertices[2]).cross(vertices[1] - vertices[2]);
				normals[3] = (vertices[0] - vertices[3]).cross(vertices[2] - vertices[3]);

				uvs[0] = vec2(x * deltaX, z * deltaZ);
				uvs[1] = vec2(x * deltaX + deltaX, z * deltaZ);
				uvs[2] = vec2(x * deltaX + deltaX, z * deltaZ + deltaZ);
				uvs[3] = vec2(x * deltaX, z * deltaZ + deltaZ);

				for (size_t i = 0; i < 4; ++i) {
					waterMesh.add_vertex(vertices[i], normals[i], uvs[i].x(), uvs[i].y());
				}

				//waterMesh.add_vertex(vec4(x*1.0f,     H[x][z],    z*1.0f,     1), normals, 0,0);
				//waterMesh.add_vertex(vec4(x*1.0f+1.0f,H[x+1][z],  z*1.0f,     1), normals, 1,0);
				//waterMesh.add_vertex(vec4(x*1.0f+1.0f,H[x+1][z+1],z*1.0f+1.0f,1), normals, 1,1);
				//waterMesh.add_vertex(vec4(x*1.0f,     H[x][z+1],  z*1.0f+1.0f,1), normals, 0,1);
				//printf("%f\n",H[z][x]);

				waterMesh.add_index(numVertices);
				waterMesh.add_index(numVertices + 1);
				waterMesh.add_index(numVertices + 2);
				waterMesh.add_index(numVertices);
				waterMesh.add_index(numVertices + 2);
				waterMesh.add_index(numVertices + 3);

				numVertices += 4;
			}
		}
	}

	void setup(){
		n = 64;
		g = -9.8f;
		dt = 0.15f;
		dx = 1.0f;
		dy = 1.0f;

		for (int i = 0; i < WIDTH; i++){
			for (int j = 0; j < HEIGHT; j++){
				Hx[i][j] = 0.0f;
				Ux[i][j] = 0.0f;
				Vx[i][j] = 0.0f;
				Hy[i][j] = 0.0f;
				Uy[i][j] = 0.0f;
				Vy[i][j] = 0.0f;
			}
		}
		for (int i = 0; i < PADDED_WIDTH; i++){
			for (int j = 0; j < PADDED_HEIGHT; j++){
				H[i][j] = 1.0f;
				U[i][j] = 0.0f;
				V[i][j] = 0.0f;
			}
		}
	}

    /// this is called once OpenGL is initialized
    void app_init() {
      app_scene =  new visual_scene();
      app_scene->create_default_camera_and_lights();
	  scene_node* water_node = app_scene->add_scene_node();
	  material *blue = new material(vec4(0, 0, 1, 1));
	  setup();
	  waterMesh.init();
	  generateMesh();
	  mWater = new mesh();
	  waterMesh.get_mesh(*mWater);
	  mWater->set_mode(GL_TRIANGLES);
	  app_scene->add_mesh_instance(new mesh_instance(water_node, mWater, blue));
	  camera_instance* cam = app_scene->get_camera_instance(0);
	  cam->set_perspective(0, 45, 1, 10.0f, 1000.0f);
	  getCameraMatrix().translate(20,10,50);
    }

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);
	  glClearColor(0, 0, 0, 1);
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	  glEnable(GL_DEPTH_TEST);

      // update matrices. assume 30 fps.
      app_scene->update(1.0f/30);

      // draw the scene
      app_scene->render((float)vx / vy);
    }
  };
}
