#include <fstream>
#include <sstream>
#include <time.h>
#include "sky_box.h"

namespace octet{

#define TWOPI  6.283185
#define PI 3.141592
#define HEIGHT 64
#define WIDTH 64
#define PADDEDHEIGHT HEIGHT+2
#define PADDEDWIDTH WIDTH+2

	class water_simulation : public app {
		ref<visual_scene> app_scene;
		ref<mesh> mWater;
		mesh_builder waterMesh;
		scene_node *cam;

		//struct for normals
		struct vertexNormal{
			float x; 
			float y; 
			float z;

			vertexNormal() = default;

			vertexNormal(float xIn, float yIn, float zIn)
			{
				x = xIn;
				y = yIn;
				z = zIn;
			}
		};

		//dynarray for txt files and normals
		dynarray<char> read;
		dynarray<char> waveLengthStr;
		dynarray<char> amplitudeStr;
		dynarray<char> speedStr;
		dynarray<vertexNormal> norms;

		//array of floats and vec3 for sine wave parameters
		float wavelength[8], amplitude[8], speed[8], frequency[8];
		vec3 direction[8];

		//random number generator
		random rand;

		//array for vertex
		float MESHHEIGHT[WIDTH][HEIGHT];

		//boolean for inital run
		bool start = true;

	public:
		water_simulation(int argc, char **argv) : app(argc, argv) 
		{

		}

		//load the wave file
		void loadWave(int waveNum)
		{
			std::fstream myFile;
			std::stringstream waveStr;
			std::string waveFile;
			read.reset();
			waveStr.clear();
			waveFile.clear();
			waveStr << "wave" << waveNum << ".txt";
			waveFile = waveStr.str();
			myFile.open(waveFile, std::ios::in);

			if (!myFile.is_open())
			{
				printf("File is not found/opened\n");
			}
			else
			{
				printf("File found/opened\n");
			}

			myFile.seekg(0, myFile.end);
			unsigned int length = myFile.tellg();
			myFile.seekg(0, myFile.beg);
			waveNum -= 1;

			if (length == -1)
			{
				length = 0;
			}

			if (length != 0)
			{
				read.resize(length);
				myFile.read(read.data(), length);
				myFile.close();
				getWaveLength(waveNum);
				getAmplitude(waveNum);
				getSpeed(waveNum);
				getDirection(waveNum);
				read.push_back('\0');
			}
		}

		//get the wavelength and frequency
		void getWaveLength(int i)
		{
			waveLengthStr.reset();
			string str(read.data(), read.size());
			int startLoc = str.find("wavelength:");
			startLoc += 11;
			int endLoc = str.find("amplitude:");
			endLoc -= 2;

			for (int i = startLoc; i < endLoc; ++i)
			{
				waveLengthStr.push_back(read[i]);
				//read[i] = NULL;
			}

			wavelength[i] = atof(waveLengthStr.data());
			frequency[i] = TWOPI / wavelength[i];
			printf("%g\n", wavelength[i]);
		}

		//get the amplitude
		void getAmplitude(int i)
		{
			amplitudeStr.reset();
			string str(read.data(), read.size());
			int startLoc = str.find("amplitude:");
			startLoc += 10;
			int endLoc = str.find("speed:");
			endLoc -= 2;

			for (int i = startLoc; i < endLoc; ++i)
			{
				amplitudeStr.push_back(read[i]);
				//read[i] = NULL;
			}

			amplitude[i] = atof(amplitudeStr.data());
			printf("%g\n", amplitude[i]);
		}

		//get the speed
		void getSpeed(int i)
		{
			speedStr.reset();
			string str(read.data(), read.size());
			int startLoc = str.find("speed:");
			startLoc += 6;
			int endLoc = str.find("end");
			endLoc -= 2;

			for (int i = startLoc; i < endLoc; ++i)
			{
				speedStr.push_back(read[i]);
				//read[i] = NULL;
			}

			speed[i] = atof(speedStr.data());
			printf("%g\n", speed[i]);
		}

		//get the direction
		void getDirection(int i)
		{
			direction[i] = vec3(rand.get(-1.0f, 1.0f), rand.get(-1.0f, 1.0f), 0.0f);
		}

		//generate the mesh
		void meshGeneration(bool first)
		{
			vec3 vertices[4];
			vec3 normals[4];
			vec2 uvs[4];
			int vertexNum;

			//delete old vertices
			if (!first) 
			{
				waterMesh.deleteVertices();
			}

			//for first run, set default variables for flat plane
			if (first)
			{
				vertexNum = 0;
				for (int i = 0; i < PADDEDWIDTH; i++)
				{
					for (int j = 0; j < PADDEDHEIGHT; j++)
					{
						MESHHEIGHT[i][j] = 0.0f;
						norms.push_back(vertexNormal(0.0f, -1.0f, 0.0f));
					}
				}
			}

			for (int z = 0; z < WIDTH; z++)
			{
				for (int x = 0; x < HEIGHT; x++)
				{
					//setup vertices
					vertices[0] = vec3(x*1.0f, MESHHEIGHT[x][z], z*1.0f);
					vertices[1] = vec3(x*1.0 + 1.0f, MESHHEIGHT[x + 1][z], z*1.0f);
					vertices[2] = vec3(x*1.0f + 1.0f, MESHHEIGHT[x + 1][z + 1], z*1.0f + 1.0f);
					vertices[3] = vec3(x*1.0f, MESHHEIGHT[x][z + 1], z*1.0f + 1.0f);

					//normals at first run
					if (start)
					{
						normals[0] = cross((vertices[1] - vertices[0]), (vertices[3] - vertices[0]));
						normals[1] = cross((vertices[2] - vertices[1]), (vertices[0] - vertices[1]));
						normals[2] = cross((vertices[3] - vertices[2]), (vertices[1] - vertices[2]));
						normals[3] = cross((vertices[0] - vertices[3]), (vertices[2] - vertices[3]));
					}

					//updated normals
					else
					{
						normals[0] = vec3(norms[x].x, norms[x].y,norms[x].z);
						normals[1] = vec3(norms[x+1].x, norms[x].y, norms[x].z);
						normals[2] = vec3(norms[x+66].x, norms[x].y, norms[x].z);
						normals[3] = vec3(norms[x+67].x, norms[x].y, norms[x].z);
					}
					
					//add vertex to mesh
					for (size_t i = 0; i < 4; i++)
					{
						waterMesh.add_vertex(vec4(vertices[i], 1.0f), vec4(normals[i], 1.0f), 1, 1);
					}

					//create the triangles
					// 0--1
					// | \|
					// 3--2
					if (first)
					{
						waterMesh.add_index(vertexNum);
						waterMesh.add_index(vertexNum + 1);
						waterMesh.add_index(vertexNum + 2);
						waterMesh.add_index(vertexNum);
						waterMesh.add_index(vertexNum + 2);
						waterMesh.add_index(vertexNum + 3);
						vertexNum += 4;
					}
				}
			}
			//set false once first run is complete
			start = false;
		}

		//this is called once OpenGL is initalized
		void app_init()
		{
			app_scene = new visual_scene;
			app_scene->create_default_camera_and_lights();
			scene_node *water_node = app_scene->add_scene_node();
			rand.set_seed(time(0));
			material *blue = new material(vec4(0, 0, 1, 1));
			waterMesh.init();
			meshGeneration(start);
			mWater = new mesh();
			waterMesh.get_mesh(*mWater);
			mWater->set_mode(GL_TRIANGLES);
			app_scene->add_mesh_instance(new mesh_instance(water_node, mWater, blue));
			cam = app_scene->get_camera_instance(0)->get_node();
			cam->translate(vec3(32, 64, 70));
			cam->rotate(-45, vec3(1, 0, 0));
			app_scene->get_camera_instance(0)->set_far_plane(100);
			for (int i = 0; i < 8; ++i)
			{
				loadWave(i);
			}
		}

		//this is called to draw the world
		void draw_world(int x, int y, int w, int h)
		{
			int vx = 0, vy = 0;
			get_viewport_size(vx, vy);
			app_scene->begin_render(vx, vy);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			//generate new plane with new points
			meshGeneration(start);

			//get the new mesh
			waterMesh.get_mesh(*mWater);

			// update matrices. assume 30 fps.
			app_scene->update(1.0f / 30);

			// draw the scene
			app_scene->render((float)vx / vy);
		}
	};
}