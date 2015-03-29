////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

#include <fstream>
#include <sstream>
#include <time.h>

namespace octet {
	/// Scene containing a box with octet.
	class water_simulation : public app {
		// scene for drawing box
		ref<visual_scene> app_scene;
		ref<mesh> mWater;
		mesh_builder waterMesh;
		scene_node *cam;
		time_t timer;

		dynarray<char> read;
		dynarray<char> waveLengthStr;
		dynarray<char> amplitudeStr;
		dynarray<char> speedStr;
		dynarray<char> steepStr;

		float wavelength, amplitude, speed, steepness;
		float frame = 1;

		int num = 1;
		int n = 48;

		bool wire = false;
		bool startup = true;

		enum res {
			WIDTH = 40,
			HEIGHT = 40,
			PADDEDWIDTH = WIDTH + 2,
			PADDEDHEIGHT = HEIGHT + 2,
		};

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
			waveStr << "wave" << num << ".txt";
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

			if (length == -1)
			{
				length = 0;
			}

			if (length != 0)
			{
				read.resize(length);
				myFile.read(read.data(), length);
				myFile.close();
				getWaveLength();
				getAmplitude();
				getSpeed();
				getSteepness();
				read.push_back('\0');
			}

		}

		//get the wave length
		void getWaveLength()
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

			wavelength = atof(waveLengthStr.data());
			printf("%f\n", wavelength);
		}

		//get the amplitude
		void getAmplitude()
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

			amplitude = atof(amplitudeStr.data());
			printf("%f\n", amplitude);
		}

		//get the steepness
		void getSteepness()
		{
			steepStr.reset();
			string str(read.data(), read.size());
			int startLoc = str.find("steepness:");
			startLoc += 10;
			int endLoc = str.find("end");
			endLoc -= 2;

			for (int i = startLoc; i < endLoc; ++i)
			{
				steepStr.push_back(read[i]);
			}

			steepness = atof(steepStr.data());
			printf("%f\n", steepness);
		}

		//get the speed
		void getSpeed()
		{
			speedStr.reset();
			string str(read.data(), read.size());
			int startLoc = str.find("speed:");
			startLoc += 6;
			int endLoc = str.find("steepness:");
			endLoc -= 2;

			for (int i = startLoc; i < endLoc; ++i)
			{
				speedStr.push_back(read[i]);
				//read[i] = NULL;
			}

			speed = atof(speedStr.data());
			printf("%f\n", speed);
		}

		//generate a plane and used to update the plane
		void meshGeneration(bool first)
		{
			vec3 vertices[4];
			vec3 normals[4];
			int vertexNum;

			if (!first)
			{
				waterMesh.deleteVertices();
			}

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

			for (int z = 0; z < 40; z++)
			{
				for (int x = 0; x < 40; x++)
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
		void sineWave()
		{
			for (int z = 0; z < PADDEDWIDTH; z++)
			{
				for (int x = 0; x < PADDEDHEIGHT; x++)
				{
					float k, sine;
					k = 2 * 3.14 / wavelength;
					sine = 0.1f * sin(k * MESHHEIGHT[x][z] + (frame * 0));
					MESHHEIGHT[x][z] = sine;
					//printf("%g ", sine);
					frame++;
				}
			}
		}

		//skybox
		void skybox()
		{

		}

		//rocks
		void rocks()
		{

		}


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
			cam->translate(vec3(20, 10, 40));
			loadWave(num);
		}

		/// this is called to draw the world
		void draw_world(int x, int y, int w, int h) {
			int vx = 0, vy = 0;
			get_viewport_size(vx, vy);
			app_scene->begin_render(vx, vy);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			meshGeneration(startup);
			sineWave();
			//frame++;


			//Key input for wireframe
			if (is_key_going_down(key_space))
			{
				if (wire)
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					wire = false;
				}

				else
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					wire = true;
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

			//increase and decrease the water bed
			if (is_key_going_down('Q'))
			{
				for (int i = 50; i < 75; i++)
				{
					for (int j = 2; j < 50; j++)
					{
						MESHHEIGHT[i][j] += 0.1f;
					}
				}
			}

			if (is_key_going_down('A'))
			{
				for (int i = 30; i < 60; i++)
				{
					for (int j = 0; j < 80; j++)
					{
						MESHHEIGHT[i][j] -= 0.1f;
					}
				}
			}

			waterMesh.get_mesh(*mWater);

			// update matrices. assume 30 fps.
			app_scene->update(1.0f / 30);

			// draw the scene
			app_scene->render((float)vx / vy);
		}
	};
}
