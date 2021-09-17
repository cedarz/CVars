/*******************************************************************
 *
 *  CVar and GlutConsole Demo App 1
 *
 *  This demo shows the basics of how to use CVars and the GLConsole
 *
 *******************************************************************/

#include <string>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <map>
#include <cvars/glplatform.h>

//include this header for CVars and GLConsole
#include <GLConsole/GLConsole.h>

//A CVar version of std::vector
#include <cvars/CVarVectorIO.h>

//A CVar version of std::map
#include <cvars/CVarMapIO.h>

#include <GLFW/glfw3.h>


// Single global instance so glut can get access
GLConsole theConsole;  

using CVarUtils::operator<<;
using CVarUtils::operator>>;

using namespace std;

//Function declarations
void display();
void reshape (int w, int h);
void idle();
void special(int key, int px, int py);
// void keyfunc(unsigned char key, int px, int py);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);



/**
 * The Main function
 * Set up the GLUT window and create some CVars
 */
int main (int argc, const char * argv[]) {
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Console Demo", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	//glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//glfwSetCursorPosCallback(window, mouse_callback);
	//glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


  //register functions
  /*glutReshapeFunc (reshape);
  glutDisplayFunc (display);
  glutKeyboardFunc (keyfunc);
  glutSpecialFunc (special);
  glutIdleFunc(idle);*/
	glfwSetKeyCallback(window, key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


  ////// How to make some CVars //////

  //CVars have a name and a default value
  CVarUtils::CreateCVar("myFirstCVar", 1);
  //You can make groups using a dot in the name. Helps keep things tidy
  CVarUtils::CreateCVar("LevelOne.myVar1 ", 2);
  CVarUtils::CreateCVar("LevelOne.myVar2",  3);
  //You can have lots of levels
  CVarUtils::CreateCVar("LevelOne.LevelTwo.another",  3.14);

  //CVars can also have a help string associated
  //Just type help var_name on the console
  CVarUtils::CreateCVar("whatAmIFor", 47, "A test variable to teach you about help");

  //To actually make these CVars useful they need effect something in your program,
  //so you normally create them as follows
  int& nTest = CVarUtils::CreateCVar("testVar", 100, "Another test CVar");

  //nTest is now always equal the value of testVar and vice-vera as it is a reference
  cout << "nTest: " << nTest << endl;
  //This is how you get a CVar value. Note the type specification
  cout << "testVar: " << CVarUtils::GetCVar<int>("testVar") << endl;
  
  nTest = 200;
  cout << "testVar: " << CVarUtils::GetCVar<int>("testVar") << endl;
  
  //this is how to set a CVar
  CVarUtils::SetCVar<int>("testVar", 300);
  cout << "nTest: " << nTest << endl;

  // How to save to text
  CVarUtils::SetStreamType(CVARS_TXT_STREAM);
  const string sOutputFile = "my_cvars.txt";
  cout << "Saving cvars in text format to: " << sOutputFile << endl;
  CVarUtils::Save("my_cvars.txt");

  CVarUtils::Load("my_cvars2.txt");

  //CVars have exception handling
  try {
    cout << "Nonexistant: " << CVarUtils::GetCVar<int>("nonExistant");
  }
  catch(CVarUtils::CVarException e ){
    switch(e) {
      case CVarUtils::CVarNonExistant:
        cout << "CVar does not exist" << endl;
        break;
      default:
        cout << "Unknown exception." << endl;
        break;
    }
  }

  ///@TODO finish off the demo
  //You can also create your own types of CVar
  //so you are not limited to ints, doubles, floats, and strings

  //Included with CVars is an implementation of a CVar vector and a CVar map
  //These will work for vectors and maps of basics types

  //Have a look at demo 2 to see how to make your own types.
  
  // Create a vector of ints, this function uses the
  // serialising/deserialising functions from "CVars/CVarVectorIO.h"
  // obtained using the CVarUtils namespace
  CVarUtils::CreateCVar< std::vector<int> >("stl.vector", std::vector<int>());
    
#if 0
    CVarUtils::CreateCVar< std::vector<int> >("stl.vector", std::vector<int>(), true,
                                               &(CVarUtils::operator<<), &(CVarUtils::operator>>));

    // Create a map of (int,string), this function uses the
    // serialising/deserialising functions from "CVars/CVarMapIO.h"
    // obtained using the CVarUtils namespace
    std::map<int,std::string> mMap;
    mMap[0] = "zero";
    mMap[1] = "one";
    
    CVarUtils::CreateCVar<std::map<int,std::string> >("stl.map", mMap, true,
                                                       &(CVarUtils::operator<<), &(CVarUtils::operator>>));
#endif

    // print out all the CVars (with optional formatting tags -- PMWiki table tags in this case)
//    CVarUtils::PrintCVars("(:cellnr:) ", "", "\n(:cell:) ","");

    //glutMainLoop();
	while (!glfwWindowShouldClose(window)) {
		display();


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
    return 0;
}

/**
 * What to draw each refresh
 * Called by GLUT
 */
void display()
{
  // This is how you register a CVar with the console, having a CVar
  // in an inner loop is a bad idea, however.
  //Hence the try/catch block
  
  float triangleSize = 1.0;
  try {
    //triangleSize is set to the default value of the CVar "triangle.size
    //      Use this syntax to make the CVar    CVar name,    Default value,   Help text,
    triangleSize = CVarUtils::CreateCVar<float>("triangle.size", 1.0f, "Triangle size value");
  }
  catch(CVarUtils::CVarException e) {
    switch(e) {
      case CVarUtils::CVarAlreadyCreated:
        //it already exists, so just assign the latest value
        triangleSize = CVarUtils::GetCVar<float>("triangle.size");
        break;
      default:
        printf("Unknown exception");
      break;
    }
  }

  //set up the scene
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.3, 0.4, 0.5, 1.0);
  //glLoadIdentity();
  //glTranslatef(0,0.0f,-2.0f);

  ////draw the triangle
  //glBegin(GL_TRIANGLES); {
  //  glColor3f(1.0f,0.0f,0.0f);
  //  glVertex3f(0.0f, triangleSize, 0.0f);
  //  glColor3f(0.0f,triangleSize,0.0f);
  //  glVertex3f(-triangleSize,-triangleSize, 0.0f);
  //  glColor3f(0.0f,0.0f,triangleSize);
  //  glVertex3f(triangleSize,-triangleSize, 0.0f);
  //}
  //glEnd();

  //draw the console. always call it last so it is drawn on top of everything
  theConsole.RenderConsole();

  //glutSwapBuffers();
}

/**
 * resize the window to the desired size
 * @param w Window width
 * @param h Window height
 */
void reshape (int w, int h)
{
  glViewport     (0, 0, w, h);
  

 /* if (h == 0)
    gluPerspective (80, (float) w, 1.0, 5000.0);
  else
    gluPerspective (80, (float) w / (float) h, 1.0, 5000.0);*/

 
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	switch (key) {

	case GLFW_KEY_ESCAPE:  //escape
		exit(0);
		break;

	case GLCONSOLE_KEY: //~ key opens console on US keyboards.
								  //On UK keyboards it is the ` key (the one above the Tab and left of the 1 keys)
		theConsole.ToggleConsole();
		break;

	default:
		if (theConsole.IsOpen()) {
			//send keystroke to console
			theConsole.KeyboardFunc(key, scancode, action, mods);
		}
		else {
			//do your own thing with the keys
		}
		break;
	}
}
