////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
#include <fstream>
#include <sstream>
#include <AntTweakBar.h>

namespace octet 
{
	/// Scene containing a box with octet.
	class l_systems : public app 
	{
		//dynamic arrays
		dynarray<char> readfile;
		dynarray<char> axiom;
		dynarray<char> alphabet;
		dynarray<float> angle;
		dynarray<int> interations;

		hash_map<char, string> rules;

		// scene for drawing box
		ref<visual_scene> app_scene;

		char treenum[64];
		std::stringstream tree;

		//Tweakbar
		TwBar *myBar;
	public:
		/// this is called when we construct the class before everything is initialised.
		l_systems(int argc, char **argv) : app(argc, argv) {
		}

		void loadfile()
		{

		}

		/// this is called once OpenGL is initialized
		void app_init() 
		{
			app_scene =  new visual_scene();
			app_scene->create_default_camera_and_lights();
			TwInit(TW_OPENGL, NULL);
			TwWindowSize(100, 100);
			myBar = TwNewBar("L Systems");
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
			TwDraw();
		}

	};
}
