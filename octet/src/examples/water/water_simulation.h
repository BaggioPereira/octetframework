////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

#include <fstream>
#include <sstream>
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
		float totalSteep = 0.5f;
		int numWaves = 8;
		float amplitudeAdd = 0.0f;

		bool wire = false;
		bool startup = true;

		random rand;

		float MESHHEIGHT[WIDTH][HEIGHT];
		vec3 MESHNORMALS[WIDTH][HEIGHT];

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
			vec2 uv[4];
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

					normals[0] = MESHNORMALS[x][z];
					normals[1] = MESHNORMALS[x + 1][z];
					normals[2] = MESHNORMALS[x + 1][z + 1];
					normals[3] = MESHNORMALS[x][z + 1];

					/*normals[0] = cross((vertices[1] - vertices[0]), (vertices[3] - vertices[0]));
					normals[1] = cross((vertices[2] - vertices[1]), (vertices[0] - vertices[1]));
					normals[2] = cross((vertices[3] - vertices[2]), (vertices[1] - vertices[2]));
					normals[3] = cross((vertices[0] - vertices[3]), (vertices[2] - vertices[3]));*/

					uv[0] = vec2(x*(1.0f / 40), z*(1.0f / 40));
					uv[1] = vec2(x *(1.0f / 40) + (1.0f / 40), z*(1.0f / 40));
					uv[2] = vec2(x *(1.0f / 40)+(1.0f / 40), z *(1.0f / 40) + (1.0f / 40));
					uv[3] = vec2(x*(1.0f / 40), z *(1.0f / 40)+(1.0f / 40));

					for (size_t i = 0; i < 4; ++i) {
						waterMesh.add_vertex(vec4(vertices[i].x(), vertices[i].y(), vertices[i].z(), 1), vec4(normals[i].x(), normals[i].y(), normals[i].z(), 1), uv[i].x(), uv[i].y());
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
				/*float xOff = x - direction[i].x();
				float zOff = z - direction[i].z();
				float theta = sqrt(xOff*xOff + zOff*zOff);
				float sine = (amplitude[i] + amplitudeAdd)* sin(frequency[i] * theta - frame * speed[i] *0.5f);*/
				float steep = totalSteep / (frequency[i] * amplitude[i] * numWaves);
				float radians = frequency[i] * direction[i].dot(vec3(x, 0.0f, z)) +  frame * speed[i];
				float yPos = steep * direction[i].z() * cosf(radians);
				height += yPos;
				//printf("%g\n", height);
			}
			MESHHEIGHT[x][z] = height;
		}

		void getNormals(int x, int z)
		{
			vec3 normals = vec3(0.0f, 0.0f, 1.0f);
			for (int i = 0; i < numWaves - 1; i++)
			{
				//float heightTerm =  speed [i] * amplitude[i];
				float steep = totalSteep / (frequency[i] * amplitude[i] * numWaves);
				float radians = frequency[i] * direction[i].dot(MESHHEIGHT[x][z]) + frame* speed[i];
				float xPos = -direction[i].x()*amplitude[i]*frequency[i]*cosf(radians);
				float yPos = -direction[i].z()*amplitude[i] * frequency[i] * cosf(radians);
				float zPos = 1 - steep * amplitude[i] * frequency[i] * sinf(radians);
				normals += vec3(xPos, yPos, zPos);
			}
			MESHNORMALS[x][z] = normals;
			//printf("%g, %g, %g\n", normals.x(), normals.y(), normals.z());
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
			image *img = new image("src/examples/water/water.gif");
			//material *blue = new material(img);
			material *blue = new material(vec4(0, 0, 1, 1));
			waterMesh.init();
			meshGeneration(startup);
			mWater = new mesh();
			waterMesh.get_mesh(*mWater);
			mWater->set_mode(GL_TRIANGLES);
			app_scene->add_mesh_instance(new mesh_instance(water_node, mWater, blue));
			cam = app_scene->get_camera_instance(0)->get_node();
			cam->translate(vec3(20, 40, 30));
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
					getNormals(x, z);
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

			//TODO
			//increase and decrease the water bed
			if (is_key_going_down('Q'))
			{
				amplitudeAdd += 0.01f;
			}

			if (is_key_going_down('A'))
			{
				amplitudeAdd -= 0.01f;	
			}

			waterMesh.get_mesh(*mWater);

			// update matrices. assume 30 fps.
			app_scene->update(1.0f / 30);

			// draw the scene
			app_scene->render((float)vx / vy);
		}
	};
}