#include <fstream>

namespace octet
{
	class lsystems : public app{
		ref<visual_scene> app_scene;

	public:
		lsystems(int argc, char **argv) : app(argc, argv)
		{
		}

		void app_init()
		{
			app_scene = new visual_scene();
			app_scene->create_default_camera_and_lights();
			std::ifstream myfile ("lsystems.txt");
			std::string line;
			if (myfile.is_open())
			{
				printf("file opened \n");
				while (std::getline(myfile, line))
				{

					printf("string is %s \n", line.c_str());
				}
				
				myfile.close();
				printf("file closed \n");
			}
			else
			{
				printf("Cannot open file \n");
			}
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