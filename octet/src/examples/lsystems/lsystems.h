#include <fstream>
#include <sstream>

namespace octet
{
	//the class for the line drawing, help from Mark Woulfe's l-system
	class line {
		mat4t modelToWorld;

		vec4 colour;

		float lineLength;

	public:
		//used to initalize a line
		void init(mat4t model, vec4 linCol, float size)
		{
			modelToWorld = model;
			colour = linCol;
			lineLength = size;
		}

		//used to render a line
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

	//the class for the l-systems
	class lsystems : public app{

		dynarray<char> read;	//dynamic array for the entire txt file
		dynarray<char> tree;	//dynamic array for the chosen tree
		dynarray<char> axiom;	//dynamic array for the axiom of the chosen tree
		dynarray<char> angles;	//dynamic array for the initial angle of the chosen tree
		dynarray<char> iters;	//dynamic array for the iteration number of the chosen tree

		//a struct for the rules
		struct rule{
			char head;			//the character associated to the rule
			std::string body;	//the rule
		};

		dynarray<rule>rules;	//a dynamic array for the rules

		std::string startTree;	//the inital string used for an iteration
		std::string endTree;	//the final string after an iteration

		mat4t modelToWorld;
		mat4t cameraToWorld;
		dynarray<mat4t> modelToWorlds;	
		dynarray<line>lines;	//a dynamic array of the line class
		color_shader shader;	//a shader used for the colour
		
		float angle;			//a float for the angle
		float lineLength;		//a float for the length of each line
		int iteration;			//an int to store the current iteration count
		int max_iters;			//an int to store the max number of iterations
		int doRewrite = 1;		//general int used for rewriting

	public:
		//The following functions are used for reading the file and setting the axiom, rules, 
		//max iterations allowed, inital angle and to create the string for each iteration

		//used to read the whole file to the memory
		void loadFile()
		{
			//Loading the txt file
			std::fstream myfile;

			//Open text file
			myfile.open("lsystems.txt", std::ios::in);

			//Check if file is opened
			if (!myfile.is_open())
			{
				//printf("File not opened/not found \n");
			}
			else
			{
				//printf("File opened \n");
			}

			//Get size of text file
			myfile.seekg(0, myfile.end);
			int length = myfile.tellg();
			//printf("length is %d \n", length);
			myfile.seekg(0, myfile.beg);

			//Resize the array
			read.resize(length);
			myfile.read(read.data(), length);

			//Print out what text file contains
			////printf("Text file contains \n%s", read.data());
		}

		//used to get a tree depending on which number is pressed
		void getTree(int num)
		{
			string str(read.data(), read.size());	//creates a string that is the size of the dynamic array and adds all its elements
			std::stringstream treeNum, treeEnd;		
			tree.reset();							//resets the dynamic array
			treeNum.clear();						//clears the string
			treeEnd.clear();						//clears the string
			treeNum << "tree" << num;				//sets the string
			treeEnd << ";" << num;					//sets the string
			std::string treeStr = treeNum.str();	//finds the start of the tree
			std::string treeStp = treeEnd.str();	//finds the end of the tree

			int startLoc = str.find(treeStr.c_str()); //find the start location of the tree
			//printf("%d\n", startLoc);

			//checks to see if the tree is found
			if (startLoc == -1)
			{
				printf("Not found start of tree");
			}
			startLoc += 7; //skips past unimportant parts of the file						

			int endLoc = str.find(treeStp.c_str()); //find the end location of the tree 

			//checks if the end of the tree is found
			if (endLoc == -1)
			{
				printf("Not found end of tree");
			}

			//adds each element to a new dynamic array from the start location until it reachs the end location
			for (int i = startLoc; i < endLoc; ++i)
			{
				int treeStr = str.find("axiom");
				treeStr += 5;
				if (read[i] != ';')
				{
					tree.push_back(read[i]);
				}
			}

			getAxiom();			//call to get the axiom of the tree
			getAngle();			//call to get the inital angle of the tree
			getIterations();	//call to get the max iterations of the tree
			getRules();			//call to get the rules of the tree
		}

		//used to get the axiom of the tree
		void getAxiom()
		{
			axiom.reset();								//reset the array
			string str(tree.data(), tree.size());		//a new string that contains the tree data
			int startLoc = str.find("axiom");			//find the start location
			startLoc += 5;								//skip part the word axiom	
			int endLoc = str.find("angle");				//find the word angle to mark the end of the axiom
			endLoc -= 1;								//go back one place
			
			//add all the elements between the start and end location to the array
			for (int i = startLoc; i < endLoc; ++i)
			{
				axiom.push_back(tree[i]);
			}
		}

		//used to get the rules of the tree
		void getRules()
		{
			rule r;										//create a new rule called r
			rules.reset();								//reset the dynamic array

			//add the push, pop, clockwise rotation and anticlockwise rotation and the rules
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

			string str(tree.data(), tree.size());		//a new string that contains the tree data
			int startLoc = str.find("rule1");			//find the first rule
			startLoc += 5;								//skip past "rule1"
			r.head = tree[startLoc];					//add the character associated to the rule as the head
			int endLoc = str.find("rule2");				//find the end of the first rule
			
			//a check to see if the end if found
			if (endLoc == -1)
			{
				endLoc = str.find("iterations");
				endLoc -= 1;
			}

			//add the first rule as the body
			for (int i = startLoc+2; i < endLoc; ++i)
			{
				r.body.push_back(tree[i]);
				rules.push_back(r);
				r.body = "";
			}
			
			startLoc = str.find("rule2");				//find the second rule

			//a check to see if there is a second rule
			if (startLoc == -1)
			{
				printf("Does not contain rule 2\n");
			}

			//if there is a second rule add it to the array
			else if (startLoc > -1)
			{
				startLoc += 5;							//skip past "rule2"
				endLoc = str.find("iterations");		//find the end of the rule
				r.head = tree[startLoc];				//add the character associated to the rule as the head
				endLoc -= 1;

				//add the second rule as the body
				for (int i = startLoc+2; i < endLoc; ++i)
				{
					r.body.push_back(tree[i]);
					rules.push_back(r);
					r.body = "";
				}
			}
		}

		//used to get the inital angle of the tree
		void getAngle()
		{
			angles.reset();
			string str(tree.data(), tree.size());
			int startLoc = str.find("angle");
			startLoc += 5;
			//printf("%d\n", startLoc);
			int endLoc = str.find("rule");
			endLoc -= 1;
			//printf("%d\n", endLoc);
			for (int i = startLoc; i < endLoc; ++i)
			{
				angles.push_back(tree[i]);
			}
			//printf("angle is %s\n", angles.data());
			angle = atof(angles.data());
			//printf("%g\n", angle);
		}

		//used to the max iterations for the tree
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
			//printf("max iterations is %d\n", max_iters);
			iteration = max_iters;
		}
	
		//used to rewrite the string for the number of iteration desired 
		void rewrite()
		{
		
			startTree.clear();
			endTree.clear();
			startTree = axiom.data();
			////printf("input is %s\n", input.data());
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
			//printf("new string is %s\n", startTree.data());
			doRewrite = 0;
		}

		//The following functions handle the drawing of the tree to the screen
		//used to create a line 
		void forward(vec4 colour)
		{
			line theLine;
			theLine.init(modelToWorld, colour, lineLength);
			lines.push_back(theLine);
			modelToWorld.translate(0, lineLength, 0);
		}

		//a switch statement for understanding each element of the string for the tree
		void buildTree()
		{
			modelToWorld.loadIdentity();	//reset the modelToWorld matrix
			modelToWorlds.reset();			//reset the dynamic array of matrices
			lines.reset();					//reset the dynamic array of the line class
			for (int i = 0; i < startTree.length(); i++)
			{
				switch (startTree[i])
				{
				case 'F': forward(vec4(0, 1, 0, 1));								//draw forward
					break;
				case 'G': forward(vec4(0, 1, 0, 1));								//special case used for drawing forward for the Sierpinski triangle
					break;
				case'[': modelToWorlds.push_back(modelToWorld);						//create a new branch
					break;
				case']': modelToWorld = modelToWorlds[modelToWorlds.size() - 1];	//go back a branch
					modelToWorlds.pop_back();
					break;
				case '+': modelToWorld.rotateZ(angle);								//rotate the branch in the positive z axis
					break;	
				case '-': modelToWorld.rotateZ(-angle);								//rotat the branch in the negative z axis
					break;
				default: break;
				}
			}
		}

		//used to call the render function and draw each line to the screen
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
			//clear all arrays and set the inital value of everything
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

			//set the location fo the camera
			cameraToWorld.loadIdentity();
			cameraToWorld.translate(0, 100, 150);

			//call to load the file and set the inital tree
			loadFile();
			getTree(1);
		}

		void draw_world(int x, int y, int w, int h)
		{
			glViewport(x, y, w, h);												//set the size of the viewport

			glClearColor(1, 1, 1, 1);											//clear the viewport

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);					//clear the colour and depth buffers

			//If statements for selecting the trees/curves
			if (is_key_going_down('1') || is_key_going_down(VK_NUMPAD1))
			{
				lineLength = 1.f;
				getTree(1);
				doRewrite = 1;
			}

			else if (is_key_going_down('2') || is_key_going_down(VK_NUMPAD2))
			{
				lineLength = 5.f;
				getTree(2);
				doRewrite = 1;
			}

			else if (is_key_going_down('3') || is_key_going_down(VK_NUMPAD3))
			{	
				lineLength = 5.f;
				getTree(3);
				doRewrite = 1;
			}

			else if (is_key_going_down('4') || is_key_going_down(VK_NUMPAD4))
			{
				lineLength = 1.f;
				getTree(4);
				doRewrite = 1;
			}

			else if (is_key_going_down('5') || is_key_going_down(VK_NUMPAD5))
			{
				lineLength = 1.f;
				getTree(5);
				doRewrite = 1;
			}

			else if (is_key_going_down('6') || is_key_going_down(VK_NUMPAD6))
			{
				lineLength = 2.5f;
				getTree(6);
				doRewrite = 1;
			}
			
			else if (is_key_going_down('7') || is_key_going_down(VK_NUMPAD7))
			{
				lineLength = 1.f;
				getTree(7);
				doRewrite = 1;
			}

			else if (is_key_going_down('8') || is_key_going_down(VK_NUMPAD8))
			{
				lineLength = 1.f;
				getTree(8);
				doRewrite = 1;
			}

			//if statements for changing the angles
			if (is_key_down(key_left))
			{
				if (angle <= 0.0f)
				{
					angle += 360.0f;
				}
				angle -= 1.f;
				printf("angle is %g\n ", angle);
				doRewrite = 1;
			}

			else if (is_key_down(key_right))
			{
				if (angle >= 360.0f)
				{
					angle -= 360.0f;
				}
				angle += 1.f;
				printf("angle is %g\n", angle);
				doRewrite = 1;
			}

			//if statements for going through each iteration
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

			//if statements for changing the length of the lines
			if (is_key_down('W'))
			{
				lineLength += .1f;
				doRewrite = 1;
			}

			else if (is_key_down('S'))
			{
				lineLength -= .1f;
				doRewrite = 1;
			}

			//if statement for rewriting a new tree but only once
			if (doRewrite)
			{
				rewrite();
				buildTree();
			}

			drawTree();				//call to draw the tree
		}
	};
}