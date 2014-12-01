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
			std::string line;
			std::ifstream myfile ("src\examples\lsystems\lsystems.txt");
			if (myfile.is_open())
			{
				while (std::getline(myfile, line))
				{
					printf("", line, '\n');
				}
				myfile.close();
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