#include <fstream>

namespace octet
{
	class lsystems : public app{
		ref<visual_scene> app_scene;

		dynarray<char> read;
		dynarray<char> tree;
		dynarray<char> axiom;

		struct rule {
			char head;
			string body;
		};

		dynarray<rule> rules;

		float angle; 
		int iteration;
		int max_iters;

	public:
		void loadFile()
		{
			//Loading the txt file
			std::fstream myfile;

			//Open text file
			myfile.open("lsystems.txt", std::ios::in);

			//Check if file is opened
			if (!myfile.is_open())
			{
				printf("File not opened/not found \n");
			}
			else
			{
				printf("File opened \n");
			}

			//Get size of text file
			myfile.seekg(0, myfile.end);
			int length = myfile.tellg();
			printf("length is %d \n", length);
			myfile.seekg(0, myfile.beg);

			//Resize the array
			read.resize(length);
			myfile.read(read.data(), length);

			//Print out what text file contains
			//printf("Text file contains \n%s", read.data());
		}

		void getTree(int treeNum)
		{
			string str(read.data(), read.size());
			tree.reset();
			//printf("String contains \n%s", string.c_str());
			if (treeNum == 1)
			{
				int startLoc = str.find("tree1");
				printf("%d\n", startLoc);
				if (startLoc == -1)
				{
					printf("Not found start of tree");
				}

				startLoc += 7;
				int endLoc = str.find("};1");
				if (endLoc == -1)
				{
					printf("Not found end of tree");
				}

				for (int i = startLoc; i < endLoc; ++i)
				{
					int treeStr = str.find("axiom");
					treeStr += 5;
					if (read[i] != ';')
					{
						tree.push_back(read[i]);
					}
				}
			}

			if (treeNum == 2)
			{
				int startLoc = str.find("tree2");
				printf("%d\n", startLoc);
				if (startLoc == -1)
				{
					printf("Not found start of tree");
				}

				startLoc += 7;
				int endLoc = str.find("};2");
				if (endLoc == -1)
				{
					printf("Not found end of tree");
				}

				for (int i = startLoc; i < endLoc; ++i)
				{
					int treeStr = str.find("tree");
					treeStr += 5;
					if (read[i] != ';')
					{
						tree.push_back(read[i]);
					}
				}
			}

			if (treeNum == 3)
			{
				int startLoc = str.find("tree3");
				printf("%d\n", startLoc);
				if (startLoc == -1)
				{
					printf("Not found start of tree");
				}

				startLoc += 7;
				int endLoc = str.find("};3");
				if (endLoc == -1)
				{
					printf("Not found end of tree");
				}

				for (int i = startLoc; i < endLoc; ++i)
				{
					int treeStr = str.find("tree");
					treeStr += 5;
					if (read[i] != ';')
					{
						tree.push_back(read[i]);
					}
				}
			}

			if (treeNum == 4)
			{
				int startLoc = str.find("tree4");
				printf("%d\n", startLoc);
				if (startLoc == -1)
				{
					printf("Not found start of tree");
				}

				startLoc += 7;
				int endLoc = str.find("};4");
				if (endLoc == -1)
				{
					printf("Not found end of tree");
				}

				for (int i = startLoc; i < endLoc; ++i)
				{
					int treeStr = str.find("tree");
					treeStr += 5;
					if (read[i] != ';')
					{
						tree.push_back(read[i]);
					}
				}
			}

			if (treeNum == 5)
			{
				int startLoc = str.find("tree5");
				printf("%d\n", startLoc);
				if (startLoc == -1)
				{
					printf("Not found start of tree");
				}

				startLoc += 7;
				int endLoc = str.find("};5");
				if (endLoc == -1)
				{
					printf("Not found end of tree");
				}

				for (int i = startLoc; i < endLoc; ++i)
				{
					int treeStr = str.find("tree");
					treeStr += 5;
					if (read[i] != ';')
					{
						tree.push_back(read[i]);
					}
				}
			}

			if (treeNum == 6)
			{
				int startLoc = str.find("tree6");
				printf("%d\n", startLoc);
				if (startLoc == -1)
				{
					printf("Not found start of tree");
				}

				startLoc += 7;
				int endLoc = str.find("};6");
				if (endLoc == -1)
				{
					printf("Not found end of tree");
				}

				for (int i = startLoc; i < endLoc; ++i)
				{
					int treeStr = str.find("tree");
					treeStr += 5;
					if (read[i] != ';')
					{
						tree.push_back(read[i]);
					}
				}
			}
		}

		void getAxiom()
		{
			axiom.reset();
			string str(tree.data(), tree.size());
			int startLoc = str.find("axiom");
			startLoc += 5;
			printf("%d\n", startLoc);
			int endLoc = str.find("angle");
			endLoc -= 1;
			printf("%d\n", endLoc);
			for (int i = startLoc; i < endLoc; ++i)
			{
				axiom.push_back(tree[i]);
			}
			printf("axiom is %s\n", axiom.data());
		}

		void getRule()
		{}

		void getAngle()
		{}

		void getIterations()
		{}

		void rewrite()
		{

		}

		lsystems(int argc, char **argv) : app(argc, argv)
		{
		}

		void app_init()
		{
			app_scene = new visual_scene();
			app_scene->create_default_camera_and_lights();
			tree.reset();
			axiom.reset();
			read.reset();
			rules.reset();
			loadFile();
		}

		void draw_world(int x, int y, int w, int h)
		{
			int vx = 0, vy = 0;
			get_viewport_size(vx, vy);
			app_scene->begin_render(vx, vy);

			app_scene->update(1.0f / 30);

			app_scene->render((float)vx / vy);

			if (is_key_going_down('1') || is_key_going_down(VK_NUMPAD1))
			{
				getTree(1);
				//printf("%s\n", tree.data());
				getAxiom();
			}

			else if (is_key_going_down('2') || is_key_going_down(VK_NUMPAD2))
			{]
				getTree(2);
				//printf("%s\n", tree.data());
				getAxiom();
			}

			else if (is_key_going_down('3') || is_key_going_down(VK_NUMPAD3))
			{
				getTree(3);
				//printf("%s\n", tree.data());
				getAxiom();
			}

			else if (is_key_going_down('4') || is_key_going_down(VK_NUMPAD4))
			{
				getTree(4);
				//printf("%s\n", tree.data());
				getAxiom();
			}

			else if (is_key_going_down('5') || is_key_going_down(VK_NUMPAD5))
			{
				getTree(5);
				//printf("%s\n", tree.data());
				getAxiom();
			}

			else if (is_key_going_down('6') || is_key_going_down(VK_NUMPAD6))
			{
				getTree(6);
				//printf("%s\n", tree.data());
				getAxiom();
			}
		}

	};
}