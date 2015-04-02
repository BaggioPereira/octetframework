#include <string>

namespace octet{
	class seabed_rocks{
		collada_builder loader;
	public:
		material *seabed;
		mesh *seaMesh;
		scene_node *seaNode;

		seabed_rocks()
		{

		}

		void render()
		{
			resource_dict dict;
			if (!loader.load_xml("src/examples/water/seabed.dae"))
			{
				return;
			}
			loader.get_resources(dict);

			dynarray<resource*> meshes;
			dict.find_all(meshes, atom_mesh);

			if (meshes.size())
			{
				seabed = new material(vec4(0.65, 0.16,0.16,1));
				seaMesh = meshes[0]->get_mesh();
				seaNode = new scene_node();
				seaNode->translate(vec3(32, -10, 32));
				seaNode->scale(0.5);
			}
		}

		void increment()
		{
			seaNode->translate(vec3(0, 1, 0));
		}

		void decrement()
		{
			seaNode->translate(vec3(0, -1, 0));
		}
	};
}