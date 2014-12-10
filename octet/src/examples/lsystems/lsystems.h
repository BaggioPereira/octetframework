#include <fstream>
#include <sstream>

namespace octet
{
	class line {
		mat4t modelToWorld;

		vec4 colour;

		float lineLength;

	public:
		void init(mat4t model, vec4 linCol, float size)
		{
			modelToWorld = model;
			colour = linCol;
			lineLength = size;
		}

		void render(color_shader &shader, mat4t &cameraToWorld)
		{
			mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);

			shader.render(modelToProjection, colour);

			float points[] = {
				0, 0, 0,
				0, lineLength, 0,
			};

			glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)points);
			glEnableVertexAttribArray(attribute_pos);

			glDrawArrays(GL_LINES, 0, 2);
		}
	};

	class lsystems : public app{
		ref<visual_scene> app_scene;

		dynarray<char> read;
		dynarray<char> tree;
		dynarray<char> axiom;
		dynarray<char> angles;
		dynarray<char> iters;

		struct rule{
			char head;
			std::string body;
		};

		dynarray<rule>rules;

		std::string startTree;
		std::string endTree;

		mat4t modelToWorld;
		mat4t cameraToWorld;
		dynarray<mat4t> modelToWorlds;
		dynarray<line>lines;
		color_shader shader;
		
		float angle; 
		float lineLength;
		int iteration;
		int max_iters;
		int doRewrite = 1;

	public:
		void loadFile()
		{
			//Loading the txt file
			std::fstream myfile;

			//Open text file
			myfile.open("lsystems.txt", std::ios::in);

			//Check if file is opened
			if (!myfile.is_open())
			{
				printf("File not opened/not found \n");
			}
			else
			{
				printf("File opened \n");
			}

			//Get size of text file
			myfile.seekg(0, myfile.end);
			int length = myfile.tellg();
			printf("length is %d \n", length);
			myfile.seekg(0, myfile.beg);

			//Resize the array
			read.resize(length);
			myfile.read(read.data(), length);

			//Print out what text file contains
			//printf("Text file contains \n%s", read.data());
		}

		void getTree(int num)
		{
			string str(read.data(), read.size());
			std::stringstream treeNum, treeEnd;
			tree.reset();
			treeNum.clear();
			treeEnd.clear();
			treeNum << "tree" << num;
			treeEnd << ";" << num;
			std::string treeStr = treeNum.str();
			std::string treeStp = treeEnd.str();
			printf("%s\n", treeStr.c_str());

			//printf("String contains \n%s", string.c_str());
			int startLoc = str.find(treeStr.c_str());
			printf("%d\n", startLoc);
			if (startLoc == -1)
			{
				printf("Not found start of tree");
			}

			startLoc += 7;
			int endLoc = str.find(treeStp.c_str());
			if (endLoc == -1)
			{
				printf("Not found end of tree");
			}

			for (int i = startLoc; i < endLoc; ++i)
			{
				int treeStr = str.find("axiom");
				treeStr += 5;
				if (read[i] != ';')
				{
					tree.push_back(read[i]);
				}
			}
			getAxiom();
			getAngle();
			getIterations();
			getRules();
		}

		void getAxiom()
		{
			axiom.reset();
			string str(tree.data(), tree.size());
			int startLoc = str.find("axiom");
			startLoc += 5;
			printf("%d\n", startLoc);
			int endLoc = str.find("angle");
			endLoc -= 1;
			printf("%d\n", endLoc);
			for (int i = startLoc; i < endLoc; ++i)
			{
				axiom.push_back(tree[i]);
			}
			printf("axiom is %s\n", axiom.data());
		}

		void getRules()
		{
			rule r;
			rules.reset();
			r.head = '[';
			r.body = '[';
			rules.push_back(r);

			r.head = ']';
			r.body = ']';
			rules.push_back(r);

			r.head = '+';
			r.body = '+';
			rules.push_back(r);

			r.head = '-';
			r.body = '-';
			rules.push_back(r);

			r.body = "";
			string str(tree.data(), tree.size());
			int startLoc = str.find("rule1");
			startLoc += 5;
			r.head = tree[startLoc];
			int endLoc = str.find("rule2");
			if (endLoc == -1)
			{
				endLoc = str.find("iterations");
				endLoc -= 1;
			}
			for (int i = startLoc+2; i < endLoc; ++i)
			{
				r.body.push_back(tree[i]);
				rules.push_back(r);
				r.body = "";
			}
			
			printf("rule1 is %s\n", rules.data());
			
			startLoc = str.find("rule2");
			if (startLoc == -1)
			{
				printf("Does not contain rule 2\n");
			}
			else if (startLoc > -1)
			{
				startLoc += 5;
				endLoc = str.find("iterations");
				r.head = tree[startLoc];
				endLoc -= 1;
				for (int i = startLoc+2; i < endLoc; ++i)
				{
					r.body.push_back(tree[i]);
					rules.push_back(r);
					r.body = "";
				}
				printf("rule2 is %s\n", rules.data());
			}
		}

		void getAngle()
		{
			angles.reset();
			string str(tree.data(), tree.size());
			int startLoc = str.find("angle");
			startLoc += 5;
			printf("%d\n", startLoc);
			int endLoc = str.find("rule");
			endLoc -= 1;
			printf("%d\n", endLoc);
			for (int i = startLoc; i < endLoc; ++i)
			{
				angles.push_back(tree[i]);
			}
			printf("angle is %s\n", angles.data());
			angle = atof(angles.data());
			printf("%g\n", angle);
		}

		void getIterations()
		{
			iters.reset();
			string str(tree.data(), tree.size());
			int startLoc = str.find("iterations");
			startLoc += 10;
			int endLoc = str.find("}");
			endLoc -= 1;
			for (int i = startLoc; i < endLoc; ++i)
			{
				iters.push_back(tree[i]);
			}
			max_iters = atoi(iters.data());
			printf("max iterations is %d\n", max_iters);
			iteration = max_iters;
		}
	
		void rewrite()
		{
		
			startTree.clear();
			endTree.clear();
			startTree = axiom.data();
			//printf("input is %s\n", input.data());
			for (int i = 0; i < iteration; ++i)
			{
				for (int j = 0; j < startTree.length(); ++j)
				{
					for (int k = 0; k < rules.size(); ++k)
					{
						if (startTree[j] == rules[k].head)
						{
							endTree.insert(endTree.length(), rules[k].body);
						}
					}
				}
				startTree = endTree;
				endTree = "";
			}
			printf("new string is %s\n", startTree.data());
			doRewrite = 0;
		}

		void forward(vec4 colour)
		{
			line theLine;
			theLine.init(modelToWorld, colour, lineLength);
			lines.push_back(theLine);
			modelToWorld.translate(0, lineLength, 0);
		}

		void buildTree()
		{
			modelToWorld.loadIdentity();
			modelToWorlds.reset();
			lines.reset();
			for (int i = 0; i < startTree.length(); i++)
			{
				switch (startTree[i])
				{
				case 'F': forward(vec4(0, 1, 0, 1));
					break;
				case'[': modelToWorlds.push_back(modelToWorld);
					break;
				case']': modelToWorld = modelToWorlds[modelToWorlds.size() - 1];
					modelToWorlds.pop_back();
					break;
				case '+': modelToWorld.rotateZ(angle);
					break;
				case '-': modelToWorld.rotateZ(-angle);
					break;
				default: break;
				}
			}
		}

		void drawTree()
		{
			for (int i = 0; i < lines.size(); ++i)
			{
				lines[i].render(shader, cameraToWorld);
			}
		}

		lsystems(int argc, char **argv) : app(argc, argv)
		{
		}

		void app_init()
		{
			tree.reset();
			axiom.reset();
			angles.reset();
			read.reset();
			rules.reset();
			angle = 0.0f;
			iteration = 0;
			max_iters = 0;
			lineLength = 1.f;
			shader.init();

			cameraToWorld.loadIdentity();
			cameraToWorld.translate(0, 250, 300);
			loadFile();
			getTree(1);
		}

		void draw_world(int x, int y, int w, int h)
		{
			glViewport(x, y, w, h);

			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			if (is_key_going_down('1') || is_key_going_down(VK_NUMPAD1))
			{
				getTree(1);
				//printf("%s\n", tree.data());
				doRewrite = 1;
			}

			else if (is_key_going_down('2') || is_key_going_down(VK_NUMPAD2))
			{
				getTree(2);
				//printf("%s\n", tree.data());
				doRewrite = 1;
			}

			else if (is_key_going_down('3') || is_key_going_down(VK_NUMPAD3))
			{
				getTree(3);
				//printf("%s\n", tree.data());#
				doRewrite = 1;
			}

			else if (is_key_going_down('4') || is_key_going_down(VK_NUMPAD4))
			{
				getTree(4);
				//printf("%s\n", tree.data());
				doRewrite = 1;
			}

			else if (is_key_going_down('5') || is_key_going_down(VK_NUMPAD5))
			{
				getTree(5);
				//printf("%s\n", tree.data());
				doRewrite = 1;
			}

			else if (is_key_going_down('6') || is_key_going_down(VK_NUMPAD6))
			{
				getTree(6);
				//printf("%s\n", tree.data());
				doRewrite = 1;
			}

			if (is_key_down(key_left))
			{
				angle -= .1f;
				printf("angle is %g\n ", angle);
				doRewrite = 1;
			}

			else if (is_key_down(key_right))
			{
				angle += .1f;
				printf("angle is %g\n", angle);
				doRewrite = 1;
			}

			if (is_key_going_down(key_up))
			{
				if (iteration < max_iters)
				{
					iteration += 1;
					doRewrite = 1;
				}
				else
				{
					iteration = max_iters;
					doRewrite = 1;
				}
				printf("iteration is %d\n", iteration);
			}

			else if (is_key_going_down(key_down))
			{
				if (iteration > 0)
				{
					iteration -= 1;
					doRewrite = 1;
				}
				else
				{
					iteration = 0;
					doRewrite = 1;

				}
				printf("iteration is %d\n", iteration);
			}

			if (is_key_down('w'))
			{
				lineLength += .1f;
				doRewrite = 1;
			}

			else if (is_key_down('s'))
			{
				lineLength -= .1f;
				doRewrite = 1;
			}

			if (doRewrite)
			{
				rewrite();
				buildTree();
			}


			drawTree();
		}
	};
}