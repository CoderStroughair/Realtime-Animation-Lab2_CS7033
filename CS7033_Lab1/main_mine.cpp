
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
vec3 startingFront = { 0.0f, 0.0f, 1.0f };
GLfloat pitCam = 0, yawCam = 0, rolCam = 0, frontCam = 0, sideCam = 0, speed = 1;
float rotateY = 50.0f, rotateLight = 0.0f;
EulerCamera cam(startingPos, startingFront, vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f, 0.0f);
versor quat;
mat4 rotQuat;

//QuatCam qcam(vec3(0.0f, 1.0f, 0.0f), 0.0, vec3(0.0, 0.0, -15.0));

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
	particle_object.init(PARTICLE_MESH);
}

void display() 
{
	mat4 proj = perspective(87.0, (float)width / (float)(height), 1, 1000.0);
	mat4 view;
	glViewport(0, 0, width, height);
		
	view = look_at(cam.getPosition(), cam.getPosition() + cam.getFront(), cam.getUp());
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

		//if (cam.getFront() != cam.getPosition())
		{

		}
		print(cam.getFront());
		print(cam.getPosition());

/**--------------------QuatCam Stuff--------------------**/
		//Translation
		vec3 movement = vec3(0.0, 0.0, 0.0);
		
		vec3 angVel = vec3(5.0*yawCam, 5.0*pitCam, 5.0*rolCam);

		//Rotation
		//if (yawCam)
		//{
		//	versor yaw = quat_from_axis_deg(yawCam, qcam.up.v[0], qcam.up.v[1], qcam.up.v[2]);
		//	qcam.rotation = slerp(yaw, qcam.rotation, 0.5);
		//}
		//if (pitCam)
		//{
		//	versor pitch = quat_from_axis_deg(pitCam, qcam.up.v[0], qcam.up.v[1], qcam.up.v[2]);
		//	qcam.rotation = slerp(pitch, qcam.rotation, 0.5);
		//}
		//if (rolCam)
		//{
		//	versor roll = quat_from_axis_deg(rolCam, qcam.up.v[0], qcam.up.v[1], qcam.up.v[2]);
		//	qcam.rotation = slerp(roll, qcam.rotation, 0.5);
		//}
		//qcam.rotationMat = quat_to_mat4(qcam.rotation);
		//qcam.forward = qcam.rotation
	}	
}

#pragma region INPUT FUNCTIONS

void keypress(unsigned char key, int x, int y) 
{
	if (key == (char)27)	//Pressing Escape Ends the game
	{
		exit(0);
	}
	else if (key == 'w' || key == 'W')
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
	textID = add_text(
		"Stuff here",
		-0.9, 0.8, fontSize, 1.0f, 1.0f, 1.0f, 1.0f);
	
	quat = quat_from_axis_deg(0.0, 0.0, 1.0, 0.0);

	rotQuat = quat_to_mat4(quat);
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
	

	drawObject(noTextureShaderID, view, proj, identity_mat4(), light, Ls, La, Ld, Ks, Ka, WHITE, specular_exponent, cam, particle_object, coneAngle, coneDirection, GL_QUADS);


}