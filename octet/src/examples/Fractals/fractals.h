////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
namespace octet {
  /// Scene containing a box with octet.
  class fractals : public app {
	// scene for drawing box
    ref<visual_scene> app_scene;

	float heightmap[x][y];

  public:
    /// this is called when we construct the class before everything is initialised.
    fractals(int argc, char **argv) : app(argc, argv) {
    }

    /// this is called once OpenGL is initialized
    void app_init() {
 
    }

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {

    }
  };
}
