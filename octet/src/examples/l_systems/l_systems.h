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

		//strings to hold file information
		std::stringstream file, newrule[5];
		string contents;
		std::string alphabet, axiom, angle, iteration, rulesNum, rule_head, rule_body, rule[5], startTree, endTree;

		float angles;
		int currentIteration, maxIterations, lastIter, numberOfRules;

		//hashmap to hold rules
		hash_map<char, std::string> rules;

		// scene for drawing box
		ref<visual_scene> app_scene;

		int defaultTree = 1;
		int prevTree;
		bool rulesExists = false;

		//Tweakbar
		TwBar *myBar;

	public:
		/// this is called when we construct the class before everything is initialised.
		l_systems(int argc, char **argv) : app(argc, argv) {
		}

		//function to find a position with a given start position
		int findPosition(string contents, char letter, int start)
		{
			for (int i = start; i < contents.size(); i++)
			{
				if (contents[i] == letter)
				{
					return i;
				}
			}
			return -1;
		}

		//load file and put into memory
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

				checkKeywords();
			}
		}

		//Check to see all variables are found
		void checkKeywords()
		{
			if (contents.find("Alphabet") == -1 || contents.find("Axiom") == -1 || contents.find("Angle") == -1 || contents.find("Rules") == -1 || contents.find("Iterations") == -1)
			{
				printf("Check text file, missing keywords\n");
			}

			else
			{
				printf("Keywords are found\n");
				getAlphabet();
				getAxiom();
				getAngle();
				getRules();
				getIteration();
			}
		}

		//get the alphabets
		void getAlphabet()
		{
			alphabet.clear();
			int startLocation = contents.find("Alphabet") + 8;
			int endLocation = findPosition(contents, ';', startLocation);

			for (int i = startLocation; i < endLocation; i++)
			{
				alphabet.push_back(contents[i]);
			}
		}

		//get the axiom
		void getAxiom()
		{
			axiom.clear();
			int startLocation = contents.find("Axiom") + 5;
			int endLocation = findPosition(contents, ';', startLocation);

			for (int i = startLocation; i < endLocation; i++)
			{
				axiom.push_back(contents[i]);
			}
		}

		//get the angle
		void getAngle()
		{
			angle.clear();
			int startLocation = contents.find("Angle") + 5;
			int endLocation = findPosition(contents, ';', startLocation);

			for (int i = startLocation; i < endLocation; i++)
			{
				angle.push_back(contents[i]);
			}

			angles = atof(angle.c_str());
		}

		//get the rules
		void getRules()
		{
			rulesNum.clear();
			rules.clear();
			int startLocation = contents.find("Rules") + 6;
			int endLocation = findPosition(contents, ';', startLocation);
			for (int i = startLocation; i < endLocation; i++)
			{
				rulesNum.push_back(contents[i]);
			}
			numberOfRules = atoi(rulesNum.c_str());
			startLocation = endLocation+2;

			//reset the rules
			for (int i = 0; i < 5; i++)
			{
				newrule[i].str(std::string());
				newrule[i] << "No Rule";
				rule[i] = newrule[i].str();
			}

			//set the new rules
			for (int i = 0; i < numberOfRules; i++)
			{
				rule_head.clear();
				rule_body.clear();
				newrule[i].str(std::string());
				rule_head.push_back(contents[startLocation]);

				//printf("%s\n", rule_head.c_str());
				startLocation += 2;
				endLocation = findPosition(contents,';', startLocation);
				for (int i = startLocation; i < endLocation; i++)
				{
					rule_body.push_back(contents[i]);
				}

				//printf("%s\n", rule_body.c_str());
				rules[rule_head[0]] = rule_body;

				//test to check the rules are being applied to the hashmap
				//std::cout <<rules.get_key(i)<<"="<< rules.get_value(i)<<"\n";

				newrule[i] << rule_head << "=" << rule_body;
				rule[i] = newrule[i].str();
				startLocation = endLocation + 2;
			}
			rules[']'] = "]";
			rules['['] = "[";
			rules['+'] = "+";
			rules['-'] = "-";
		}

		//get the max iteration
		void getIteration()
		{
			iteration.clear();
			int startLocation = contents.find("Iterations") + 10;
			int endLocation = findPosition(contents, ';', startLocation);

			for (int i = startLocation; i < endLocation; i++)
			{
				iteration.push_back(contents[i]);
			}

			maxIterations = atoi(iteration.c_str());
			currentIteration = maxIterations;
		}

		void reWrite()
		{
			startTree.clear();
			endTree.clear();
			startTree = axiom;
			for (int i = 0; i < currentIteration; i++)
			{
				for (int j = 0; j < startTree.length(); j++)
				{
					for (int k = 0; k < rules.size(); k++)
					{
						if (startTree[j] == rules.get_key(k))
						{
							endTree.insert(endTree.length(), rules.get_value(k));
						}
					}
				}
				startTree = endTree;
				endTree = "";
			}
			printf("This iteration sting is %s\n", startTree.c_str());
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

			if (is_key_going_down(VK_OEM_PERIOD))
			{
				TwKeyPressed(46, 0);
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
			TwAddVarRW(myBar, "Angle", TW_TYPE_FLOAT, &angles, "help = ' '");
			TwAddVarRO(myBar, "Rule 1", TW_TYPE_STDSTRING, &rule[0], "help = ' '"); 
			TwAddVarRO(myBar, "Rule 2", TW_TYPE_STDSTRING, &rule[1], "help = ' '");
			TwAddVarRO(myBar, "Rule 3", TW_TYPE_STDSTRING, &rule[2], "help = ' '");
			TwAddVarRO(myBar, "Rule 4", TW_TYPE_STDSTRING, &rule[3], "help = ' '");
			TwAddVarRO(myBar, "Rule 5", TW_TYPE_STDSTRING, &rule[4], "help = ' '");
			TwAddVarRW(myBar, "Iteration", TW_TYPE_INT32, &currentIteration, "help = ' '");
			fileLoading();
			prevTree = defaultTree;
			lastIter = currentIteration;
			reWrite();
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
				fileLoading();
				prevTree = defaultTree;
				reWrite();
			}

			if (lastIter != currentIteration)
			{ 
				if (currentIteration > 0 && currentIteration <= maxIterations)
				{
					lastIter = currentIteration;
					reWrite();
				}
			}
		}
	};
}
