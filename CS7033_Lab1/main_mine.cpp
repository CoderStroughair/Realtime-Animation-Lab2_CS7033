
#include <windows.h>
#include "Antons_maths_funcs.h"
#include "teapot.h" // teapot mesh
#include "Utilities.h"

using namespace std;




/*----------------------------------------------------------------------------
						Mesh and Shader Variables
----------------------------------------------------------------------------*/
Mesh monkeyhead_object, torch_object, wall_object, cube, bear_object, signReflect_object, signRefract_object, signNormal_object;
Mesh gem_object, sphere_object, particle_object;

GLuint noTextureShaderID, textureShaderID, cubeMapTextureID, refractiveShaderID, cubeMapShaderID;
GLuint testID, normalisedShaderID, reflectiveShaderID, multiTextureShaderID, mirrorShaderID, debugSkyboxID;

Shader shaderFactory;

//Cube Vertices for the Skybox
GLfloat vertices[] = {
	-10.0f, -10.0f, -10.0f,  0.0f,  0.0f, -1.0f,
	10.0f, -10.0f, -10.0f,  0.0f,  0.0f, -1.0f,
	10.0f,  10.0f, -10.0f,  0.0f,  0.0f, -1.0f,
	10.0f,  10.0f, -10.0f,  0.0f,  0.0f, -1.0f,
	-10.0f,  10.0f, -10.0f,  0.0f,  0.0f, -1.0f,
	-10.0f, -10.0f, -10.0f,  0.0f,  0.0f, -1.0f,

	-10.0f, -10.0f,  10.0f,  0.0f,  0.0f,  1.0f,
	10.0f, -10.0f,  10.0f,  0.0f,  0.0f,  1.0f,
	10.0f,  10.0f,  10.0f,  0.0f,  0.0f,  1.0f,
	10.0f,  10.0f,  10.0f,  0.0f,  0.0f,  1.0f,
	-10.0f,  10.0f,  10.0f,  0.0f,  0.0f,  1.0f,
	-10.0f, -10.0f,  10.0f,  0.0f,  0.0f,  1.0f,

	-10.0f,  10.0f,  10.0f, -1.0f,  0.0f,  0.0f,
	-10.0f,  10.0f, -10.0f, -1.0f,  0.0f,  0.0f,
	-10.0f, -10.0f, -10.0f, -1.0f,  0.0f,  0.0f,
	-10.0f, -10.0f, -10.0f, -1.0f,  0.0f,  0.0f,
	-10.0f, -10.0f,  10.0f, -1.0f,  0.0f,  0.0f,
	-10.0f,  10.0f,  10.0f, -1.0f,  0.0f,  0.0f,

	10.0f,  10.0f,  10.0f,  1.0f,  0.0f,  0.0f,
	10.0f,  10.0f, -10.0f,  1.0f,  0.0f,  0.0f,
	10.0f, -10.0f, -10.0f,  1.0f,  0.0f,  0.0f,
	10.0f, -10.0f, -10.0f,  1.0f,  0.0f,  0.0f,
	10.0f, -10.0f,  10.0f,  1.0f,  0.0f,  0.0f,
	10.0f,  10.0f,  10.0f,  1.0f,  0.0f,  0.0f,

	-10.0f, -10.0f, -10.0f,  0.0f, -1.0f,  0.0f,
	10.0f, -10.0f, -10.0f,  0.0f, -1.0f,  0.0f,
	10.0f, -10.0f,  10.0f,  0.0f, -1.0f,  0.0f,
	10.0f, -10.0f,  10.0f,  0.0f, -1.0f,  0.0f,
	-10.0f, -10.0f,  10.0f,  0.0f, -1.0f,  0.0f,
	-10.0f, -10.0f, -10.0f,  0.0f, -1.0f,  0.0f,

	-10.0f,  10.0f, -10.0f,  0.0f,  1.0f,  0.0f,
	10.0f,  10.0f, -10.0f,  0.0f,  1.0f,  0.0f,
	10.0f,  10.0f,  10.0f,  0.0f,  1.0f,  0.0f,
	10.0f,  10.0f,  10.0f,  0.0f,  1.0f,  0.0f,
	-10.0f,  10.0f,  10.0f,  0.0f,  1.0f,  0.0f,
	-10.0f,  10.0f, -10.0f,  0.0f,  1.0f,  0.0f
}; 


/*----------------------------------------------------------------------------
							CAMERA VARIABLES
----------------------------------------------------------------------------*/

vec3 startingPos = { 0.0f, 0.0f, 20.0f };
vec3 startingFront = { 0.0f, 0.0f, -1.0f };
GLfloat pitCam = 0, yawCam = 0, rolCam = 0, frontCam = 0, sideCam = 0, speed = 1;
float rotateY = 50.0f, rotateLight = 0.0f;
EulerCamera cam(startingPos, 0.0f, 0.0f, 0.0f);

QuatCam qcam(vec4(0.0f, 1.0f, 0.0f, 0.0), 0.0, vec4(0.0, 0.0, -15.0, 0.0));

bool qmode = false;

GLfloat objYaw = 0, objPit = 0, objRol = 0;

/*----------------------------------------------------------------------------
							OTHER VARIABLES
----------------------------------------------------------------------------*/

const char* atlas_image = "../freemono.png";
const char* atlas_meta = "../freemono.meta";

float fontSize = 25.0f;
int textID = -1;
const float width = 800, height = 800;
void drawloop(mat4 view, mat4 proj, GLuint framebuffer);


/*--------------------------------------------------------------------------*/

void init()
{
	if (!init_text_rendering(atlas_image, atlas_meta, width, height)) 
	{
		fprintf(stderr, "ERROR init text rendering\n");
		exit(1);
	}
	cam.setSensitivity(2.0f);
	//*************************//
	//*****Compile Shaders*****//
	//*************************//
	refractiveShaderID = shaderFactory.CompileShader(NOTEXTURE_VERT, FRESNEL_FRAG);
	noTextureShaderID = shaderFactory.CompileShader(NOTEXTURE_VERT, NOTEXTURE_FRAG);
	cubeMapShaderID = shaderFactory.CompileShader(SKY_VERT, SKY_FRAG);
	textureShaderID = shaderFactory.CompileShader(TEXTURE_VERT, TEXTURE_FRAG);
	normalisedShaderID = shaderFactory.CompileShader(NORMAL_VERT, NORMAL_FRAG);
	reflectiveShaderID = shaderFactory.CompileShader(NOTEXTURE_VERT, REFLECTIVE_FRAG);
	multiTextureShaderID = shaderFactory.CompileShader(TEXTURE_VERT, TEXTURE_FRAG);
	mirrorShaderID = shaderFactory.CompileShader(MIRROR_VERT, MIRROR_FRAG);
	debugSkyboxID = shaderFactory.CompileShader(DEBUG_VERT, DEBUG_FRAG);
	testID = shaderFactory.CompileShader(TEST_VERT, TEST_FRAG);
	//*********************//
	//*****Init Objects*****//
	//*********************//
	monkeyhead_object.init(MONKEYHEAD_MESH);
	cube.initCubeMap(vertices, 36, "desert");
	torch_object.init(TORCH_MESH);
	bear_object.init(BEAR_MESH, BEAR_TEXTURE, BEAR_TEXTURE2);
	wall_object.init(WALL_MESH, NULL, BRICK_NORMAL);
	signReflect_object.init(SIGN_MESH, REFLECT_TEXTURE);
	signRefract_object.init(SIGN_MESH, REFRACT_TEXTURE);
	signNormal_object.init(SIGN_MESH, NORMAL_TEXTURE);
	sphere_object.init(SPHERE_MESH);
	gem_object.init(GEM_MESH);
	particle_object.init(GEM_MESH);
}

void display() 
{
	mat4 proj = perspective(87.0, (float)width / (float)(height), 1, 1000.0);
	mat4 view;
	glViewport(0, 0, width, height);
		
	//view = look_at(cam.getPosition(), cam.getPosition() + cam.getFront(), cam.getUp());
	view = qcam.viewMat;
	proj = perspective(60.0, (float)width / (float)height, 1, 1000.0);
	glViewport(0, 0, width, height);
	drawloop(view, proj, 0);
	draw_texts();
	glutSwapBuffers();
}

void updateScene() {
	static DWORD  last_frame;	//time when last frame was drawn
	static DWORD last_timer = 0;	//time when timer was updated
	DWORD  curr_time = timeGetTime();//for frame Rate Stuff.
	static bool first = true;
	if (first)
	{
		last_frame = curr_time;
		first = false;
	}
	float  delta = (curr_time - last_frame) * 0.001f;
	if (delta >= 0.03f) 
	{
		last_frame = curr_time;
		glutPostRedisplay();
		rotateY = rotateY + 0.5f;
		rotateLight = rotateLight + 0.01f;
		if (rotateY >= 360.0f)
			rotateY = 0.0f;
		if (rotateLight >= 360.0f)
			rotateLight = 0.0f;
		cam.movForward(frontCam*speed);
		cam.movRight(sideCam*speed);
		cam.changeFront(pitCam, yawCam, rolCam);

		qcam.yaw += yawCam;
		qcam.pitch += pitCam;
		qcam.roll += rolCam;

		versor q = quat_from_axis_deg(qcam.yaw, qcam.up.v[0], qcam.up.v[1], qcam.up.v[2]);
		q = quat_from_axis_deg(-qcam.pitch, qcam.right.v[0], qcam.right.v[1], qcam.right.v[2]) * q;
		q = quat_from_axis_deg(qcam.roll, qcam.front.v[0], qcam.front.v[1], qcam.front.v[2]) * q;

		qcam.heading += yawCam;

		qcam.R = quat_to_mat4(normalise(q));

		qcam.front = qcam.R * vec4(0.0, 0.0, 1.0, 0.0);
		qcam.right = qcam.R * vec4(1.0, 0.0, 0.0, 0.0);
		qcam.up = qcam.R * vec4(0.0, 1.0, 0.0, 0.0);

		qcam.cam_pos = qcam.cam_pos + vec3(qcam.front) * frontCam;
		qcam.cam_pos = qcam.cam_pos + vec3(qcam.right) * sideCam;
		qcam.T = translate(identity_mat4(), vec3(qcam.cam_pos));

		qcam.viewMat = (qcam.R) * (qcam.T);

		//Orthonormalisation
		vec3 Cx = vec3(qcam.viewMat.m[0], qcam.viewMat.m[1], qcam.viewMat.m[2]) / length(vec3(qcam.viewMat.m[0], qcam.viewMat.m[1], qcam.viewMat.m[2]));
		vec3 Cz = vec3(qcam.viewMat.m[8], qcam.viewMat.m[9], qcam.viewMat.m[10]);
		vec3 Cy = cross(Cz, Cx);
		Cy = Cy / length(Cy);
		Cz = cross(Cx, Cy);
		Cz = Cz / length(Cz);
		qcam.viewMat.m[0] = Cx.v[0];
		qcam.viewMat.m[1] = Cx.v[1];
		qcam.viewMat.m[2] = Cx.v[2];

		qcam.viewMat.m[4] = Cy.v[0];
		qcam.viewMat.m[5] = Cy.v[1];
		qcam.viewMat.m[6] = Cy.v[2];

		qcam.viewMat.m[8] = Cz.v[0];
		qcam.viewMat.m[9] = Cz.v[1];
		qcam.viewMat.m[10] = Cz.v[2];

		if (qmode)
		{
			string output = "QUAT_MODE: Front: [" + to_string(qcam.getFront().v[0]) + ", " + to_string(qcam.getFront().v[1]) + ", " + to_string(qcam.getFront().v[2]) + "]\n";
			output += "Position: [" + to_string(qcam.getPosition().v[0]) + ", " + to_string(qcam.getPosition().v[1]) + ", " + to_string(qcam.getPosition().v[2]) + "]\n";
			output += "Up: [" + to_string(qcam.getUp().v[0]) + ", " + to_string(qcam.getUp().v[1]) + ", " + to_string(qcam.getUp().v[2]) + "]\n";
			output += "Object Position: [" + to_string(particle_object.position.v[0]) + ", " + to_string(particle_object.position.v[1]) + ", " + to_string(particle_object.position.v[2]) + "]\n";
			output += "Pitch: " + to_string(qcam.pitch) + "\n";
			output += "Yaw: " + to_string(qcam.yaw) + "\n";
			output += "Roll: " + to_string(qcam.roll) + "\n";
			update_text(textID, output.c_str());
		}

		else
		{
			string output = "EULER_MODE: Front: [" + to_string(cam.getFront().v[0]) + ", " + to_string(cam.getFront().v[1]) + ", " + to_string(cam.getFront().v[2]) + "]\n";
			output += "Position: [" + to_string(cam.getPosition().v[0]) + ", " + to_string(cam.getPosition().v[1]) + ", " + to_string(cam.getPosition().v[2]) + "]\n";
			output += "Up: [" + to_string(cam.getUp().v[0]) + ", " + to_string(cam.getUp().v[1]) + ", " + to_string(cam.getUp().v[2]) + "]\n";
			output += "Object Position: [" + to_string(particle_object.position.v[0]) + ", " + to_string(particle_object.position.v[1]) + ", " + to_string(particle_object.position.v[2]) + "]\n";
			output += "Pitch: " + to_string(cam.pitch) + "\n";
			output += "Yaw: " + to_string(cam.yaw) + "\n";
			output += "Roll: " + to_string(cam.roll) + "\n";
			update_text(textID, output.c_str());
		}

		particle_object.moveObject(vec3(0.0, 0.0, 0.0), vec3(objPit*10.0f, objYaw*10.0f, objRol*10.0f), delta);

/**--------------------QuatCam Stuff--------------------**/
		//Translation
		vec3 movement = vec3(0.0, 0.0, 0.0);
		
		vec3 angVel = vec3(5.0*yawCam, 5.0*pitCam, 5.0*rolCam);
	}	
}

#pragma region INPUT FUNCTIONS

void keypress(unsigned char key, int x, int y) 
{
	if (key == (char)27)	//Pressing Escape Ends the game
	{
		exit(0);
	}

	if (key == 'w' || key == 'W')
	{
		frontCam = 1;
		printf("Moving Forward\n");
	}
	else if ((key == 's') || (key == 'S'))
	{
		frontCam = -1;
		printf("Moving Backward\n");
	}

	if ((key == 'a') || (key == 'A'))
	{
		sideCam = -1;
		printf("Moving Left\n");
	}
	else if ((key == 'd') || (key == 'D'))
	{
		sideCam = 1;
		printf("Moving Right\n");
	}

	if ((key == 'q') || (key == 'Q'))
	{
		rolCam = -1;
		printf("Spinning Negative Roll\n");
	}
	else if ((key == 'e') || (key == 'E'))
	{
		rolCam = 1;
		printf("Spinning Positive Roll\n");
	}

	if ((key == 'r') || (key == 'R'))
	{
		objPit = 1;
	}
	else if ((key == 'f') || (key == 'F'))
	{
		objPit = -1;
	}

	if ((key == 't') || (key == 'T'))
	{
		objYaw = 1;
	}
	else if ((key == 'g') || (key == 'G'))
	{
		objYaw = -1;
	}

	if ((key == 'y') || (key == 'Y'))
	{
		objRol = 1;
	}
	else if ((key == 'h') || (key == 'H'))
	{
		objRol = -1;
	}
}

void keypressUp(unsigned char key, int x, int y)
{
	if ((key == 'w') || (key == 'W'))
		frontCam = 0;
	else if ((key == 's') || (key == 'S'))
		frontCam = 0;
	if ((key == 'a') || (key == 'A'))
		sideCam = 0;
	else if ((key == 'd') || (key == 'D'))
		sideCam = 0;
	if ((key == 'q') || (key == 'Q'))
		rolCam = 0;
	if ((key == 'e') || (key == 'E'))
		rolCam = 0;
	if (key == ' ')
		qmode = !qmode;

	if ((key == 'r') || (key == 'R') || (key == 'f') || (key == 'F'))
	{
		objPit = 0;
	}

	if ((key == 't') || (key == 'T') || (key == 'g') || (key == 'G'))
	{
		objYaw = 0;
	}

	if ((key == 'y') || (key == 'Y') || (key == 'h') || (key == 'H'))
	{
		objRol = 0;
	}
}

void specialKeypress(int key, int x, int y) 
{
	switch (key)
	{
	case (GLUT_KEY_SHIFT_L):
	case (GLUT_KEY_SHIFT_R):
		speed = 4;
		break;
	case (GLUT_KEY_LEFT):
		printf("Spinning Negative Yaw\n");
		yawCam = -1;
		break;
	case (GLUT_KEY_RIGHT):
		printf("Spinning Positive Yaw\n");
		yawCam = 1;
		break;
	case (GLUT_KEY_UP):
		printf("Spinning Positive Pit\n");
		pitCam = 1;
		break;
	case (GLUT_KEY_DOWN):
		printf("Spinning Negative Pit\n");
		pitCam = -1;
		break;
	}
}

void specialKeypressUp(int key, int x, int y) 
{
	switch (key)
	{
	case (GLUT_KEY_SHIFT_L):
	case (GLUT_KEY_SHIFT_R):
		speed = 1;
		break;
	case (GLUT_KEY_LEFT):
		yawCam = 0;
		break;
	case (GLUT_KEY_RIGHT):
		yawCam = 0;
		break;
	case (GLUT_KEY_UP):
		pitCam = 0;
		break;
	case (GLUT_KEY_DOWN):
		pitCam = 0;
		break;
	}
}

void (mouse)(int x, int y)
{
	
}

#pragma endregion INPUT FUNCTIONS

int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("GameApp");
	glutSetCursor(GLUT_CURSOR_CROSSHAIR);


	// Tell glut where the display function is
	glutWarpPointer(width / 2, height / 2);
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);

	// Input Function Initialisers //
	glutKeyboardFunc(keypress);
	glutPassiveMotionFunc(mouse);
	glutSpecialFunc(specialKeypress);
	glutSpecialUpFunc(specialKeypressUp);
	glutKeyboardUpFunc(keypressUp);

	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	init();
	textID = add_text("Stuff here",
		-0.9, 0.8, fontSize, 1.0f, 1.0f, 1.0f, 1.0f);
	
	glutMainLoop();
	return 0;
}

void drawloop(mat4 view, mat4 proj, GLuint framebuffer)
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glEnable(GL_DEPTH_TEST);								// enable depth-testing
	glDepthFunc(GL_LESS);									// depth-testing interprets a smaller value as "closer"
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear the color and buffer bits to make a clean slate
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);					//Create a background


	mat4 model;
	model = identity_mat4();
	// light properties
	vec3 Ls = vec3(1.0f, 1.0f, 1.0f);	//Specular Reflected Light
	vec3 Ld = vec3(0.99f, 0.99f, 0.99f);	//Diffuse Surface Reflectance
	vec3 La = vec3(1.0f, 1.0f, 1.0f);	//Ambient Reflected Light
	vec3 light = vec3(5 * sin(rotateLight), 10, -5.0f*cos(rotateLight));//light source location
	vec3 coneDirection = light + vec3(0.0f, -1.0f, 0.0f);
	float coneAngle = 10.0f;
	// object colour
	vec3 Ks = vec3(0.1f, 0.1f, 0.1f); // specular reflectance
	vec3 Kd = BROWN;
	vec3 Ka = vec3(0.05f, 0.05f, 0.05f); // ambient reflectance
	float specular_exponent = 0.5f; //specular exponent - size of the specular elements

	drawCubeMap(cubeMapShaderID, cube.tex, view, proj, identity_mat4(), Ld, La, cam, cube, GL_TRIANGLES);
	drawObject(noTextureShaderID, view, proj, identity_mat4(), light, Ls, La, Ld, Ks, Ka, WHITE, specular_exponent, cam, particle_object, coneAngle, coneDirection, GL_TRIANGLES);
}