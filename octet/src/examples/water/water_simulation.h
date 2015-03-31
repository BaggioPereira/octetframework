////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

#include <fstream>
#include <sstream>
#include <time.h>
#include "sky_box.h"

namespace octet {
	/// Scene containing a box with octet.
	class water_simulation : public app {

		#define TWOPI  6.283185
		#define PI 3.141592
		#define HEIGHT 40
		#define WIDTH 40
		#define PADDEDHEIGHT HEIGHT+2
		#define PADDEDWIDTH WIDTH+2

		// scene for drawing box
		ref<visual_scene> app_scene;
		ref<mesh> mWater;
		mesh_builder waterMesh;
		scene_node *cam;

		dynarray<char> read;
		dynarray<char> waveLengthStr;
		dynarray<char> amplitudeStr;
		dynarray<char> speedStr;


		float wavelength[8], amplitude[8], speed[8], frequency[8];
		vec3 direction[8];
		float frame = 1;

		int num = 1;
		int numWaves = 8;

		bool wire = false;
		bool startup = true;

		random rand;

		float MESHHEIGHT[WIDTH][HEIGHT], U[WIDTH][HEIGHT], V[WIDTH][HEIGHT];

	public:
		/// this is called when we construct the class before everything is initialised.
		water_simulation(int argc, char **argv) : app(argc, argv) {
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

		//get the amplitude and normalized between 0 and 1
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
			speed[i] = speed[i] / wavelength[i];
			printf("%g\n", speed[i]);
		}

		//get the direction
		void getDirection(int i)
		{
			direction[i] = vec3(rand.get(-1.0f, 1.0f), 0.0f, rand.get(-1.0f, 1.0f));
		}

		//generate a plane and used to update the plane
		void meshGeneration(bool first)
		{
			vec3 vertices[4];
			vec3 normals[4];
			int vertexNum;

			//delete the vertices for redrawing a new wave
			if (!first)
			{
				waterMesh.deleteVertices();
			}

			//on startup set the height of the wave to 0
			if (first)
			{
				vertexNum = 0;
				for (int i = 0; i < PADDEDWIDTH; i++)
				{
					for (int j = 0; j < PADDEDHEIGHT; j++)
					{
						MESHHEIGHT[i][j] = 1.0f;
					}
				}
			}

			//go through each (x,z) coordinate and set the vertices.
			for (int z = 0; z < WIDTH; z++)
			{
				for (int x = 0; x < HEIGHT; x++)
				{
					vertices[0] = vec3(x*1.0f, MESHHEIGHT[x][z], z*1.0f);
					vertices[1] = vec3(x*1.0 + 1.0f, MESHHEIGHT[x + 1][z], z*1.0f);
					vertices[2] = vec3(x*1.0f + 1.0f, MESHHEIGHT[x + 1][z + 1], z*1.0f + 1.0f);
					vertices[3] = vec3(x*1.0f, MESHHEIGHT[x][z + 1], z*1.0f + 1.0f);

					normals[0] = cross((vertices[1] - vertices[0]), (vertices[3] - vertices[0]));
					normals[1] = cross((vertices[2] - vertices[1]), (vertices[0] - vertices[1]));
					normals[2] = cross((vertices[3] - vertices[2]), (vertices[1] - vertices[2]));
					normals[3] = cross((vertices[0] - vertices[3]), (vertices[2] - vertices[3]));

					for (size_t i = 0; i < 4; ++i) {
						waterMesh.add_vertex(vec4(vertices[i].x(), vertices[i].y(), vertices[i].z(), 1), vec4(normals[i].x(), normals[i].y(), normals[i].z(), 1), 1, 1);
					}

					if (startup)
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

			startup = false;
		}

		//TODO
		//calculate the sine wave
		void sineWave(int x, int z)
		{
			float height = 0.0f;
			for (int i = 1; i < numWaves-1; i++)
			{
				float xOff = x - direction[i].x();
				float zOff = z - direction[i].z();
				float theta = sqrt(xOff*xOff + zOff*zOff);
				float sine = amplitude[i]* sin(frequency[i] * theta + frame * speed[i] *0.5f);
				height += sine;
				//printf("%g\n", height);
			}
			MESHHEIGHT[x][z] = height;
		}

		//rocks
		/*void rocks()
		{

		}*/
		
		/// this is called once OpenGL is initialized
		void app_init() {


			app_scene = new visual_scene();
			app_scene->create_default_camera_and_lights();
			scene_node *water_node = app_scene->add_scene_node();
			material *blue = new material(vec4(0, 0, 1, 1));
			waterMesh.init();
			meshGeneration(startup);
			mWater = new mesh();
			waterMesh.get_mesh(*mWater);
			mWater->set_mode(GL_TRIANGLES);
			app_scene->add_mesh_instance(new mesh_instance(water_node, mWater, blue));
			cam = app_scene->get_camera_instance(0)->get_node();
			cam->translate(vec3(20, 45, 30));
			cam->rotate(-45, vec3(1, 0, 0));
			app_scene->get_camera_instance(0)->set_far_plane(1000);
			for (int i = 0; i < numWaves; i++)
			{
				loadWave(i);
			}
		}

		/// this is called to draw the world
		void draw_world(int x, int y, int w, int h) {
			int vx = 0, vy = 0;
			get_viewport_size(vx, vy);
			app_scene->begin_render(vx, vy);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			meshGeneration(startup);

			for (int z = 0; z < PADDEDWIDTH; z++)
			{
				for (int x = 0; x < PADDEDHEIGHT; x++)
				{
					sineWave(x, z);
				}
			}
			frame++;

			//Key input for wireframe
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

			//change the wave file
			if (is_key_going_down(key_left))
			{
				if (num > 1)
				{
					num--;
					loadWave(num);
				}
				else
				{
					printf("No more waves\n");
				}
			}

			if (is_key_going_down(key_right))
			{
				num++;
				loadWave(num);
			}

			//TODO
			//increase and decrease the water bed
			/*if (is_key_going_down('Q'))
			{
				amplitude += 0.1f;
			}

			if (is_key_going_down('A'))
			{

				amplitude -=0.1f;
			}*/

			waterMesh.get_mesh(*mWater);

			// update matrices. assume 30 fps.
			app_scene->update(1.0f / 30);

			// draw the scene
			app_scene->render((float)vx / vy);
		}
	};
}