#include <fstream>

namespace octet
{
	class lsystems : public app{
		ref<visual_scene> app_scene;

		dynarray<char> read;
		dynarray<char> axiom;
		hash_map<char, string> rules;

		dynarray<char> rule;

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

		void getVariables(int treeNum)
		{
			string str(read.data(), read.size());
			//printf("String contains \n%s", string.c_str());
			if (treeNum == 1)
			{
				int startLoc = str.find("tree1");
				printf("%d\n", startLoc);
				if (startLoc == -1)
				{
					printf("Not found start of tree");
				}

				startLoc += 12;
				int endLoc = str.find("};1");
				if (endLoc == -1)
				{
					printf("Not found end of tree");
				}

				for (int i = startLoc; i < endLoc; ++i)
				{
					int axiomStr = str.find("axiom");
					axiomStr += 5;
					if (read[i] != ';')
					{
						axiom.push_back(read[i]);
					}

					else if (read[i] == ';')
					{
						break;
					}
				}
			}
		}

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
			axiom.empty();
			loadFile();
			getVariables(1);
			printf("%s\n", axiom.data());
		}

		void draw_world(int x, int y, int w, int h)
		{
			int vx = 0, vy = 0;
			get_viewport_size(vx, vy);
			app_scene->begin_render(vx, vy);

			app_scene->update(1.0f / 30);

			app_scene->render((float)vx / vy);
		}

	};
}