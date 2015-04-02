#include <fstream>
#include <sstream>
#include <ctime>
#include "seabedrocks.h"

namespace octet{
	class water_simulation : public app {
		
		//defines
		#define TWOPI  6.283185
		#define PI 3.141592
		#define HEIGHT 64
		#define WIDTH 64
		#define PADDEDHEIGHT HEIGHT+2
		#define PADDEDWIDTH WIDTH+2

		ref<visual_scene> app_scene;
		ref<mesh> mWater;
		mesh_builder waterMesh;
		scene_node *cam, *water_node;
		material *blue, *waterMat;
		image *waterImg;

		seabed_rocks sea;

		//dynarray for txt files and normals
		dynarray<char> read;
		dynarray<char> waveLengthStr;
		dynarray<char> amplitudeStr;
		dynarray<char> speedStr;

		//array of floats and vec3 for sine wave parameters
		float wavelength[8], amplitude[8], speed[8], frequency[8], phase[8];
		vec3 direction[8];

		//random number generator
		random rand;

		//number of sine waves
		int numWaves = 8;

		//steepness of a wave
		float steep = 0.5f;

		//array for vertex
		float MESHHEIGHT[WIDTH][HEIGHT];

		//boolean for inital run
		bool start = true;

		//boolean for wireframe mode
		bool wire = false;

		//boolean for solid or texture
		bool solid = false;

		//boolean for gerstner calculations
		bool gerstner = true;

		//frame counter
		int frame = 1;

		//floats for additional amplitude and frequency
		float addedAmp = 0.0f;
		float addedFreq = 0.0f;

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
			speed[i] = speed[i] * 0.1f;
			phase[i] = speed[i] * TWOPI / wavelength[i];
		}

		//get the direction
		void getDirection(int i)
		{
			direction[i] = vec3(rand.get(-1.0f, 1.0f), 0.0f, rand.get(-1.0f, 1.0f));
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
					}
				}
			}

			for (int z = 0; z < HEIGHT; z++)
			{
				for (int x = 0; x < WIDTH; x++)
				{
					//setup vertices
					vertices[0] = vec3(x*1.0f, MESHHEIGHT[x][z], z*1.0f);
					vertices[1] = vec3(x*1.0 + 1.0f, MESHHEIGHT[x + 1][z], z*1.0f);
					vertices[2] = vec3(x*1.0f + 1.0f, MESHHEIGHT[x + 1][z + 1], z*1.0f + 1.0f);
					vertices[3] = vec3(x*1.0f, MESHHEIGHT[x][z + 1], z*1.0f + 1.0f);

					//calculate the normals
					normals[0] = cross((vertices[1] - vertices[0]), (vertices[3] - vertices[0]));
					normals[1] = cross((vertices[2] - vertices[1]), (vertices[0] - vertices[1]));
					normals[2] = cross((vertices[3] - vertices[2]), (vertices[1] - vertices[2]));
					normals[3] = cross((vertices[0] - vertices[3]), (vertices[2] - vertices[3]));

					uvs[0] = vec2(x*(1.0f / HEIGHT), z*(1.0f / WIDTH));
					uvs[1] = vec2(x *(1.0f / HEIGHT) + (1.0f / HEIGHT), z*(1.0f / WIDTH));
					uvs[2] = vec2(x *(1.0f / HEIGHT) + (1.0f / HEIGHT), z *(1.0f / WIDTH) + (1.0f / WIDTH));
					uvs[3] = vec2(x*(1.0f / HEIGHT), z *(1.0f / WIDTH) + (1.0f / WIDTH));
					
					//add vertex to mesh
					for (size_t i = 0; i < 4; i++)
					{
						waterMesh.add_vertex(vec4(vertices[i].x(), vertices[i].y(), vertices[i].z(), 1.0f), vec4(normals[i].x(), normals[i].y(), normals[i].z(), 1.0f), uvs[i].x(), uvs[i].y());
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

		//calculate new height for the point
		void gerstnerWaves(int x, int z)
		{
			float yCoord = 0.0f;
			
			for (int i = 0; i < numWaves; ++i)
			{	
				//sum sine waves
				float xOff = x - direction[i].x();
				float zOff = z - direction[i].y();
				float angular = sqrt(pow(xOff,2)+pow(zOff,2));
				float sine = 0.1f * sin((frequency[i] + addedFreq) * angular + frame * phase[i]);
				yCoord += sine;

				if (gerstner)
				{
					//gerstner waves
					float steepness = steep / (frequency[i] + addedFreq) * (amplitude[i] + addedAmp) * numWaves;
					float angle = (frequency[i] + addedFreq) * direction[i].dot(vec3(x, MESHHEIGHT[x][z], z)) + frame * phase[i];
					float yPos = steepness * (amplitude[i] + addedAmp) * direction[i].z() * cosf(angle);
					yCoord += yPos;
				}
			}
			MESHHEIGHT[x][z] = yCoord;
		}

		//this is called once OpenGL is initalized
		void app_init()
		{
			app_scene = new visual_scene;
			app_scene->create_default_camera_and_lights();
			water_node = app_scene->add_scene_node();
			rand.set_seed(time(0));
			waterImg = new image("src/examples/water/water.gif");
			blue = new material(vec4(0, 0, 1, 1));
			waterMat = new material(waterImg);
			waterMesh.init();
			meshGeneration(start);
			mWater = new mesh();
			waterMesh.get_mesh(*mWater);
			mWater->set_mode(GL_TRIANGLES);
			app_scene->add_mesh_instance(new mesh_instance(water_node, mWater, blue));
			cam = app_scene->get_camera_instance(0)->get_node();
			cam->translate(vec3(32, 64, 70));
			cam->rotate(-45, vec3(1, 0, 0));
			app_scene->get_camera_instance(0)->set_far_plane(1000);

			//load all the files
			for (int i = 1; i < numWaves; ++i)
			{
				loadWave(i);
			}

			//for rendering the rocks
			sea.render();
			app_scene->add_mesh_instance(new mesh_instance(sea.seaNode, sea.seaMesh, sea.seabed));
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

			//call to calculate new points
			for (int z = 0; z < HEIGHT; z++)
			{
				for (int x = 0; x < WIDTH; x++)
				{
					gerstnerWaves(x, z);
				}
			}

			//frame counter increment;
			++frame;

			//key input for wireframe
			if (is_key_going_down(key_space))
			{
				if (!wire)
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					wire = !wire;
				}

				else
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					wire = !wire;
				}
			}

			//key input to change between texture and solid
			if (is_key_going_down('M'))
			{
				if (solid)
				{
					blue = new material(vec4(0, 0, 1, 1));
					app_scene->get_mesh_instance(0)->set_material(blue);
					solid = !solid;
				}

				else
				{
					waterMat = new material(waterImg);
					app_scene->get_mesh_instance(0)->set_material(waterMat);
					solid = !solid;
				}	
			}

			//key input to alter amplitude
			if (is_key_going_down('A'))
			{
				addedFreq += 0.01f;
			}

			else if (is_key_going_down('D'))
			{
				addedFreq -= 0.01f;
			}

			//key input to alter frequency
			if (is_key_going_down('W'))
			{
				addedAmp += 0.01f;
			}

			else if (is_key_going_down('S'))
			{
				addedAmp -= 0.01f;
			}

			//key input for a different wave
			if (is_key_going_down('N'))
			{
				for (int i = 0; i < numWaves; ++i)
				{
					loadWave(i);
				}
			}

			//key input to enable or disable gerstner waves calculations
			if (is_key_going_down('G'))
			{
				gerstner = !gerstner;
			}

			//key input to raise and lower "Rocks"
			if (is_key_going_down(key_up))
			{
				sea.increment();
			}

			else if (is_key_going_down(key_down))
			{
				sea.decrement();
			}

			//get the new mesh
			waterMesh.get_mesh(*mWater);

			// update matrices. assume 30 fps.
			app_scene->update(1.0f / 30);

			// draw the scene
			app_scene->render((float)vx / vy);
		}
	};
}