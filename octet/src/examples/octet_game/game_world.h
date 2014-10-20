#include "../../octet.h"
namespace octet  {

	class game_world : public octet_game {

	public:
		static material *ground;
		static mesh *box;
		static scene_node *node;

		void app_init()
		{
			ground = new material(vec4(0, 1, 0, 1));
			box = new mesh_box(vec3(1000, 0, 1000));
			node = new scene_node();
		}
	};
}