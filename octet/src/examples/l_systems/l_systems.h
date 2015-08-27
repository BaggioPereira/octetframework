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
		//dynarray<char> axiom;
		//dynarray<char> alphabet;
		//dynarray<float> angle;
		//dynarray<int> interations;

		std::stringstream file;
		string contents;
		std::string alphabet, axiom, angle, iteration;

		float angles;
		int iterations;

		hash_map<char, string> rules;

		// scene for drawing box
		ref<visual_scene> app_scene;

		int defaultTree = 1;
		int prevTree;
		std::stringstream treeString;

		//Tweakbar
		TwBar *myBar;

	public:
		/// this is called when we construct the class before everything is initialised.
		l_systems(int argc, char **argv) : app(argc, argv) {
		}

		void fileLoading()
		{
			std::fstream myFile;
			std::stringstream fileName;
			fileName << "Tree " << defaultTree << ".txt";
			myFile.open(fileName.str().c_str(), std::ios::in);
			if (!myFile.is_open())
			{
				printf("File not opened/not found \n");
			}

			else
			{
				printf("File is open/found\n");
				myFile.seekg(0, myFile.end);
				int length = myFile.tellg();
				myFile.seekg(0, myFile.beg);

				file.str(std::string());
				file << myFile.rdbuf();
				myFile.close();

				contents = file.str().c_str();

				//printf("%s\n", contents.c_str());

				getAlphabet();
				getAxiom();
				getAngle();
				getIteration();
			}
		}

		void getAlphabet()
		{
			alphabet.clear();
			int startLocation = contents.find("alphabet");
			startLocation += 8;
			int endLocation = contents.find("axiom");
			endLocation -= 1;

			for (int i = startLocation; i < endLocation; i++)
			{
				alphabet.push_back(contents[i]);
			}
		}

		void getAxiom()
		{
			axiom.clear();
			int startLocation = contents.find("axiom");
			startLocation += 5;
			int endLocation = contents.find("angle");
			endLocation -= 1;

			for (int i = startLocation; i < endLocation; i++)
			{
				axiom.push_back(contents[i]);
			}
		}

		void getAngle()
		{
			angle.clear();
			int startLocation = contents.find("angle");
			startLocation += 5;
			int endLocation = contents.find("rule1");
			endLocation -= 1;

			for (int i = startLocation; i < endLocation; i++)
			{
				angle.push_back(contents[i]);
			}

			angles = atof(angle.c_str());
		}

		void getIteration()
		{
			iteration.clear();
			int startLocation = contents.find("iterations");
			startLocation += 10;
			int endLocation = contents.find(";");

			for (int i = startLocation; i < endLocation; i++)
			{
				iteration.push_back(contents[i]);
			}

			iterations = atoi(iteration.c_str());
		}

		//Mouse input
		void mouseInputControl()
		{
			int mX = 0, mY = 0;
			get_mouse_pos(mX, mY);
			TwMouseMotion(mX, mY);
			if (is_key_going_down(key_lmb))
			{
				TwMouseButton(TW_MOUSE_PRESSED, TW_MOUSE_LEFT);
			}
			if (!is_key_down(key_lmb) && get_prev_keys()[key_lmb] != 0)
			{
				TwMouseButton(TW_MOUSE_RELEASED, TW_MOUSE_LEFT);
			}
		}

		//key input
		void keyboardTranslation()
		{
			//Numbers
			if (is_key_going_down('1') || is_key_going_down(VK_NUMPAD1))
			{
				TwKeyPressed('1', 0);
			}
			if (is_key_going_down('2') || is_key_going_down(VK_NUMPAD2))
			{
				TwKeyPressed('2', 0);
			}
			if (is_key_going_down('3') || is_key_going_down(VK_NUMPAD3))
			{
				TwKeyPressed('3', 0);
			}
			if (is_key_going_down('4') || is_key_going_down(VK_NUMPAD4))
			{
				TwKeyPressed('4', 0);
			}
			if (is_key_going_down('5') || is_key_going_down(VK_NUMPAD5))
			{
				TwKeyPressed('5', 0);
			}
			if (is_key_going_down('6') || is_key_going_down(VK_NUMPAD6))
			{
				TwKeyPressed('6', 0);
			}
			if (is_key_going_down('7') || is_key_going_down(VK_NUMPAD7))
			{
				TwKeyPressed('7', 0);
			}
			if (is_key_going_down('8') || is_key_going_down(VK_NUMPAD8))
			{
				TwKeyPressed('8', 0);
			}
			if (is_key_going_down('9') || is_key_going_down(VK_NUMPAD9))
			{
				TwKeyPressed('9', 0);
			}
			if (is_key_going_down('0') || is_key_going_down(VK_NUMPAD0))
			{
				TwKeyPressed('0', 0);
			}
			//control values using ascii found here
			//http://www.asciitable.com/
			if (is_key_going_down(key_backspace))
			{
				TwKeyPressed(8, 0);
			}
		}

		/// this is called once OpenGL is initialized
		void app_init() 
		{
			app_scene =  new visual_scene();
			app_scene->create_default_camera_and_lights();
			TwInit(TW_OPENGL, NULL);
			TwWindowSize(750, 720);
			myBar = TwNewBar("L Systems");
			TwAddVarRW(myBar, "Tree Selected", TW_TYPE_INT32, &defaultTree, "help ='Type in the tree you would like to load'");
			TwAddVarRO(myBar, "Alphabets", TW_TYPE_STDSTRING, &alphabet,"help = ' '");
			TwAddVarRO(myBar, "Axiom", TW_TYPE_STDSTRING, &axiom, "help = ' '");
			TwAddVarRO(myBar, "Angle", TW_TYPE_FLOAT, &angles, "help = ' '");
			TwAddVarRO(myBar, "Iteration", TW_TYPE_INT32, &iterations, "help = ' '");
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
			mouseInputControl();
			keyboardTranslation();

			if (prevTree != defaultTree)
			{
				printf("Tree selected %d\n", defaultTree);
				prevTree = defaultTree;
				fileLoading();
			}
		}
	};
}
