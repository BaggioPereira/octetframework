#include <string>

namespace octet{
	class seabed_rocks{
		collada_builder loader; //loader for the dae file

	public:
		material *seabed;
		mesh *seaMesh;
		scene_node *seaNode;

		int height = 0;

		seabed_rocks()
		{

		}

		void render()
		{
			//load the dae file for the rocks
			resource_dict dict;
			if (!loader.load_xml("src/examples/water/seabed.dae"))
			{
				return;
			}
			loader.get_resources(dict);

			//add mesh to array
			dynarray<resource*> meshes;
			dict.find_all(meshes, atom_mesh);

			if (meshes.size())
			{
				seabed = new material(vec4(0.65, 0.16,0.16,1)); //set brown colour
				seaMesh = meshes[0]->get_mesh();				//get the mesh from the array
				seaNode = new scene_node();						
				seaNode->translate(vec3(32, -10, 32));
				seaNode->scale(0.5);
			}
		}

		//function to raise the rocks
		void increment()
		{
			if (height < 15)
			{
				seaNode->translate(vec3(0, 1, 0));
				height++;
			}	
		}

		//function to lower the rocks
		void decrement()
		{
			if (height > 0)
			{
				seaNode->translate(vec3(0, -1, 0));
				height--;
			}
		}
	};
}