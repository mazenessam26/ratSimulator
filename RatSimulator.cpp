#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <iostream>
#include <windows.h>

#include <sstream>
#include <iomanip> // Add this line to include the <iomanip> header

int WIDTH = 1280;
int HEIGHT = 720;

GLuint tex;
char title[] = "Rat Simulator";

// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 100;


#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)
GLuint texID;
#define Rows 30
#define Columns 30
#define Up 0
#define Down 1
#define Left 2
#define Right 3
#define Still 4

int gridx, gridy;
int playerx = 5, playery = 10;
int camx = 5, camy = 1;
short playerd = Still;
bool win = false;
bool gameover = false;
int score = 0;
int seconds = 1;
short demoplayerd = Up;
int demo = 0;



int collx = 3, colly = 4, collx2 = 12, colly2 = 18;
bool coll1 = false, coll2 = false;
bool scale = false;
bool chappened = false;
bool traffic = true;

int obsx = 9, obsx2 = 18, obsy = 12, obsy2 = 8;
bool collobs1 = false, collobs2 = false;

bool lvl2 = false;
int lvl2collx = 4, lvl2colly = 3, lvl2collx2 = 14, lvl2colly2 = 21;
bool lvl2coll1 = false, lvl2coll2 = false;
int lvl2obsx = 7, lvl2obsx2 = 18, lvl2obsy = 24, lvl2obsy2 = 8;
bool lvl2collobs1 = false, lvl2collobs2 = false;
int rotate = 0;
bool jump = false;
int jump1 = 0;

int aloo = 0;
// Model Variables
Model_3DS model_house;
Model_3DS model_tree;
Model_3DS model_cat;
Model_3DS model_dog;
Model_3DS model_door;
Model_3DS model_candy;
Model_3DS model_traffic;
Model_3DS model_cheese;
Model_3DS model_lantern;

// Textures
GLTexture tex_ground;
GLTexture tex_floor;
GLTexture tex_road;
GLTexture tex_buildings;
GLTexture tex_score;
GLTexture tex_score1;
GLTexture tex_score2;
GLTexture tex_score3;
GLTexture tex_score4;

int result1;
LPCSTR myString = "Game Over! ";
LPCSTR resu = "You Won! ";


int lightx = -7, lighty = 6;

float intenisity = 0.0;

void LoadAssets();

void gridinit(int x, int y)
{
	gridx = x;
	gridy = y;
}

void timer(int v) {
	if (!(gameover))
	{
		seconds++;
		if (intenisity <= 1.0)
			intenisity += 0.1;
		else
			intenisity -= 0.1;
		
		
		glutTimerFunc(1000, timer, 0);
	}
}



class Vector3f {
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f& v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f& v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};

class Camera {
public:
	Vector3f eye, center, up;

	Camera(float eyeX = camx * 0.1, float eyeY = camy * 0.1, float eyeZ = 1.0f, float centerX = camx * 0.1, float centerY = camy * 0.1, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}

	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}

	void look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}
};

Camera camera;

bool resetcamera = false;

void drawWall(double thickness) {
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing
	
	if(!lvl2)
		glBindTexture(GL_TEXTURE_2D, tex_floor.texture[0]);
	else
		glBindTexture(GL_TEXTURE_2D, tex_road.texture[0]);
	glPushMatrix();
	glTranslated(0.5, 0.5 * thickness, 0.5);
	glScaled(0.3, thickness, 0.3);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -20);
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(5, 5);
	glVertex3f(20, 0, 20);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 0, 20);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);
}
void drawFence(double thickness) {
	/*glPushMatrix();
	glTranslated(0.1, 0.5 * thickness, 0.5);
	glScaled(0.2, thickness, 1.0);
	glColor3f(0.5, 0.2, 0.0);
	glutSolidCube(1);
	glPopMatrix();*/

	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	if(!lvl2)
		glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);
	else
		glBindTexture(GL_TEXTURE_2D, tex_buildings.texture[0]);

	glPushMatrix();
	glTranslated(0.1, 0.5 * thickness, 2);
	glScaled(0.06, 0.04, 0.1);
	//glColor3f(0.5, 0.3, 0.0);
	//if (seconds % 2 == 0)
		//glColor3f(0.5, 0.2, 0.0);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -20);
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(5, 5);
	glVertex3f(20, 0, 20);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 0, 20);
	glEnd();

	//glutSolidCube(1);
	
	glPopMatrix();

	glEnable(GL_LIGHTING);





}
void drawFence2(double thickness) {
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	if (!lvl2)
		glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);
	else
		glBindTexture(GL_TEXTURE_2D, tex_buildings.texture[0]);
	
	glPushMatrix();
	glTranslated(-0.1, 0.8, 0.0);
	glScaled(0.06, 0.04, 0.1);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -20);
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(5, 5);
	glVertex3f(20, 0, 20);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 0, 20);
	glEnd();
	
	glPopMatrix();

	glEnable(GL_LIGHTING);


}

void drawscore(double thickness) {
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	
		

		if (score == 0)
			glBindTexture(GL_TEXTURE_2D, tex_score.texture[0]);
		else if (score == 1)
			glBindTexture(GL_TEXTURE_2D, tex_score1.texture[0]);
		else if (score == 2)
			glBindTexture(GL_TEXTURE_2D, tex_score2.texture[0]);
		else if (score == 3)
			glBindTexture(GL_TEXTURE_2D, tex_score3.texture[0]);
		else if (score == 4)
			glBindTexture(GL_TEXTURE_2D, tex_score4.texture[0]);
	

	glPushMatrix();
	glTranslated(-0.1, 0.8, 0.0);
	glScaled(0.006, 0.004, 0.01);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -20);
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(5, 5);
	glVertex3f(20, 0, 20);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 0, 20);
	glEnd();

	glPopMatrix();

	glEnable(GL_LIGHTING);


}

class Vector
{
public:
	GLdouble x, y, z;
	Vector() {}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
	//================================================================================================//
	// Operator Overloading; In C++ you can override the behavior of operators for you class objects. //
	// Here we are overloading the += operator to add a given value to all vector coordinates.        //
	//================================================================================================//
	void operator +=(float value)
	{
		x += value;
		y += value;
		z += value;
	}
};

Vector Eye(20, 5, 20);
Vector At(0, 0, 0);
Vector up(0, 1, 0);

int cameraZoom = 0;



//=======================================================================
// Lighting Configuration Function
//=======================================================================
void InitLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);

	// Enable Light Source number 0
	// OpengL has 8 light sources
	glEnable(GL_LIGHT0);

	// Define Light source 0 ambient light
	GLfloat ambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	// Define Light source 0 diffuse light
	GLfloat diffuse[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	// Define Light source 0 Specular light
	GLfloat specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	// Finally, define light source 0 position in World Space
	GLfloat light_position[] = { -7 , 6, 3.0f, 0.0f };
	
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

void changelightposition(int x, int y) {
	GLfloat light_position[] = { -7+lightx , 6+lighty, 3.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************************************************************************//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, up.x, up.y, up.z);
	//*******************************************************************************************//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*******************************************************************************************//

	InitLightSource();

	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}

//=======================================================================
// Render Ground Function
//=======================================================================
void RenderGround()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -20);
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(5, 5);
	glVertex3f(20, 0, 20);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 0, 20);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

//=======================================================================
// Display Function
//=======================================================================
void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



	GLfloat lightIntensity[] = { 0.7, 0.7, 0.7, 1.0f };
	GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);

	// Draw Ground
	RenderGround();

	// Draw Tree Model
	glPushMatrix();
	glTranslatef(10, 0, 0);
	glScalef(0.7, 0.7, 0.7);
	model_tree.Draw();
	glPopMatrix();

	// Draw house Model
	glPushMatrix();
	glRotatef(90.f, 1, 0, 0);
	model_house.Draw();
	glPopMatrix();


	//sky box
	glPushMatrix();

	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	glTranslated(50, 0, 0);
	glRotated(90, 1, 0, 1);
	glBindTexture(GL_TEXTURE_2D, tex);
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 100, 100, 100);
	gluDeleteQuadric(qobj);


	glPopMatrix();



	glutSwapBuffers();
}

//=======================================================================
// Keyboard Function
//=======================================================================
void myKeyboard(unsigned char button, int x, int y)
{
	switch (button)
	{
	case 'w':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'r':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

//=======================================================================
// Motion Function
//=======================================================================
void myMotion(int x, int y)
{
	y = HEIGHT - y;

	if (cameraZoom - y > 0)
	{
		Eye.x += -0.1;
		Eye.z += -0.1;
	}
	else
	{
		Eye.x += 0.1;
		Eye.z += 0.1;
	}

	cameraZoom = y;

	glLoadIdentity();	//Clear Model_View Matrix

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, up.x, up.y, up.z);	//Setup Camera with modified paramters

	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glutPostRedisplay();	//Re-draw scene 
}

//=======================================================================
// Mouse Function
//=======================================================================
void myMouse(int button, int state, int x, int y)
{


	if (state == GLUT_DOWN)
	{
		PlaySound("sound/jump.wav", NULL, SND_ASYNC);
		if (!jump) {
			jump = true;
			
		}
		else {
			jump = false;
			
		}

		
	}
}

//=======================================================================
// Reshape Function
//=======================================================================
void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, up.x, up.y, up.z);
}

//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	// Loading Model files
	model_house.Load("Models/house/house.3DS");
	model_tree.Load("Models/mouse/Toy mouse N250113.3DS");
	model_cat.Load("Models/Cat/Cat N030213.3DS");
	model_dog.Load("Models/Dog/Dog N220420.3DS");
	model_door.Load("Models/Door/Door cls N040722.3ds");
	model_candy.Load("Models/Vase 1/Vase 1.3ds");
	model_traffic.Load("Models/traffic/traffic-light.3ds");
	model_cheese.Load("Models/cheese/Luster massive cheese N190418.3ds");
	model_lantern.Load("Models/lantern/Lantern 2.3DS");
	// Loading texture files
	tex_ground.Load("Textures/wall.bmp");
	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);

	tex_floor.Load("Textures/floor.bmp");
	tex_road.Load("Textures/road.bmp");
	tex_buildings.Load("Textures/buildings.bmp");
	tex_score.Load("Textures/score0.bmp");
	tex_score1.Load("Textures/score1.bmp");
	tex_score2.Load("Textures/score2.bmp");
	tex_score3.Load("Textures/score3.bmp");
	tex_score4.Load("Textures/score4.bmp");
	
	
}

//=======================================================================
// Main Function
//=======================================================================

//``````````````````````````````````````````````````````````````````````````//
void setupLights() {
	GLfloat ambient[] = { 0.7f, 0.7f, 0.7, 1.0f };
	GLfloat diffuse[] = { 0.6f, 0.6f, 0.6, 1.0f };
	GLfloat specular[] = {1.0f, 1.0f, 1.0, 1.0f };
	GLfloat shininess[] = { 50 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	GLfloat lightIntensity[] = { 0.7f, 0.7f, 1, 1.0f };
	GLfloat lightPosition[] = { -7.0 ,6.0f, 3.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightIntensity);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
}
void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 640 / 480, 0.001, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}

void collectables()
{
	if (!lvl2) 
	{
		if (!coll1) { 
			glPushMatrix();
			glTranslated(0.1 * collx, 0.02, 0.1 * colly);
			glScaled(0.009, 0.009, 0.009);
			glRotated(rotate++, 0, 1, 0);
			model_candy.Draw();
			glPopMatrix();
		}

		if (!coll2) { 
			glPushMatrix();
			glTranslated(0.1 * collx2, 0.02, 0.1 * colly2);
			glScaled(0.009, 0.009, 0.009);
			glRotated(rotate++, 0, 1, 0);
			model_candy.Draw();
			glPopMatrix();
		}
	}
	else 
	{
		if (!lvl2coll1) { 
			glPushMatrix();
			glTranslated(0.1 * lvl2collx, 0.14, 0.1 * lvl2colly);
			glScaled(0.0007, 0.0007, 0.0007);
			glRotated(180, 1, 0, 0);
			glRotated(rotate++, 0, 1, 0);
			model_cheese.Draw();
			glPopMatrix();
		}

		if (!lvl2coll2) { 
			glPushMatrix();
			glTranslated(0.1 * lvl2collx2, 0.14, 0.1 * lvl2colly2);
			glScaled(0.0007, 0.0007, 0.0007);
			glRotated(180, 1, 0, 0);
			glRotated(rotate++, 0, 1, 0);
			model_cheese.Draw();
			glPopMatrix();
		}
	}
}

void obstacles()
{
	if (!lvl2) { 
		if (!collobs1) 
		{
			glPushMatrix();
			glTranslated(0.1 * obsx, 0.21, 0.1 * obsy);
			glScalef(0.0001, 0.0001, 0.0001);
			glRotated(rotate++, 0, 1, 0);
			model_cat.Draw();
			glPopMatrix();
		}
		if (!collobs2) 
		{
			glPushMatrix();
			glTranslated(0.1 * obsx2, 0.21, 0.1 * obsy2);
			glScalef(0.0001, 0.0001, 0.0001);
			glRotated(rotate++, 0, 1, 0);
			model_cat.Draw();
			glPopMatrix();
		}
	}
	else 
	{
		if (!lvl2collobs1) 
		{
			glPushMatrix();
			glTranslated(0.1 * lvl2obsx, 0.14, 0.1 * lvl2obsy);
			glScalef(0.002, 0.002, 0.002);
			glRotated(rotate++, 0, 1, 0);
			model_dog.Draw();
			glPopMatrix();
		}
		if (!lvl2collobs2) 
		{
			glPushMatrix();
			glTranslated(0.1 * lvl2obsx2, 0.14, 0.1 * lvl2obsy2);
			glScalef(0.002, 0.002, 0.002);
			glRotated(rotate++, 0, 1, 0);
			model_dog.Draw();
			glPopMatrix();
		}
	}
}
void reset() {
	playerx = 5, playery = 10;
	camx = 5, camy = 1;
	if(!lvl2){
		coll1 = false, coll2 = false;
		collobs1 = false, collobs2 = false;
		score = 0;
	}
	else {
		lvl2coll1 = false, lvl2coll2 = false;
		lvl2collobs1 = false, lvl2collobs2 = false;

		coll1 = true, coll2 = true;
		collobs1 = true, collobs2 = true;
	}
	
	chappened = false;
	


	camera = Camera(camx * 0.1, camy * 0.1, 1.0f, camx * 0.1, camy * 0.1, 0.0f, 0.0f, 1.0f, 0.0f);
}

void player()
{
	if (playerd != Still)
		demoplayerd = playerd;
	if(playery ==gridy -3 || playerx ==3 || playerx ==gridx-3 || playery ==3)
		PlaySound("sound/cat-meow4.wav", NULL, SND_ASYNC);
	if (playerd == Up && playery != gridy - 1) {
		//PlaySound("sound/footstep.wav", NULL, SND_ASYNC);
		playery++;
	
	} //place in this condition a footwalk sound
		
	else if (playerd == Down && playery != 1) {
		//PlaySound("sound/footstep.wav", NULL, SND_ASYNC);
		playery--;
	} //place in this condition a footwalk sound
		
	else if (playerd == Left && playerx != 1) {
		//PlaySound("sound/footstep.wav", NULL, SND_ASYNC);
		playerx--;
	} //place in this condition a footwalk sound
		
	else if (playerd == Right && playerx != gridx - 1) {
		//PlaySound("sound/footstep.wav", NULL, SND_ASYNC);
		playerx++;
	} //place in this condition a footwalk sound
		
	//changelightposition(playerx, playery);

	if (playerx == collx && playery == colly && !(coll1)) { //place eat sound here
		//PlaySound("sound/eating.wav", NULL, SND_ASYNC);
		coll1 = true;
		aloo = 1;

		score++;
	}
	else if (playerx == collx2 && playery == colly2 && !(coll2)) { //place eat sound here
		PlaySound("sound/eating.wav", NULL, SND_ASYNC);
		coll2 = true;
		score++;
		aloo = 1;
	}

	else if (playerx == obsx && playery == obsy && !(collobs1)) { //place cat sound here
		PlaySound("sound/cat-meow4.wav", NULL, SND_ASYNC);
		collobs1 = true;
		reset();
		
		//exit(EXIT_SUCCESS);

	}
	else if (playerx == obsx2 && playery == obsy2 && !(collobs2)) { //place cat sound here
		PlaySound("sound/cat-meow4.wav", NULL, SND_ASYNC);
		collobs2 = true;
		reset();
		//exit(EXIT_SUCCESS);

	}


	else if (playerx == lvl2collx && playery == lvl2colly && !(lvl2coll1)) { //place eat sound here
		PlaySound("sound/eating.wav", NULL, SND_ASYNC);
		lvl2coll1 = true;
		score++;
	}
	else if (playerx == lvl2collx2 && playery == lvl2colly2 && !(lvl2coll2)) { //place eat sound here
		PlaySound("sound/eating.wav", NULL, SND_ASYNC);
		lvl2coll2 = true;
		score++;
	}

	else if (playerx == lvl2obsx && playery == lvl2obsy && !(lvl2collobs1)) { //place dog sound here
		PlaySound("sound/dog-bark4.wav", NULL, SND_ASYNC);
		lvl2collobs1 = true;
		lvl2 = false;
		reset();
		//exit(EXIT_SUCCESS);

	}
	else if (playerx == lvl2obsx2 && playery == lvl2obsy2 && !(lvl2collobs2)) { //place dog sound here
		PlaySound("sound/dog-bark4.wav", NULL, SND_ASYNC);
		lvl2collobs2 = true;
		lvl2 = false;
		reset();
		//exit(EXIT_SUCCESS);

	}



	else if (playerx == 28 && playery == 1) { //the goal
		if (!lvl2) {
			lvl2 = true;
			reset();
		}
		else { //display u win on the screen
			
			traffic = false;
		}
	}


	playerd = Still;





	if (chappened) { 


		GLfloat ambient[] = { intenisity, intenisity, intenisity, 1.0f };
		GLfloat diffuse[] = { 0.02f, 0.02f, 0.02, 1.0f };
		GLfloat specular[] = { 1.0f, 0.0f, aloo, 1.0f };//aloo
		GLfloat shininess[] = { 80 };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

		GLfloat lightIntensity[] = { intenisity, intenisity, intenisity, 1.0f };
		GLfloat lightPosition[] = { 0.1 * playerx ,0, 0.1 * playery, 0.0f };
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);


		glPushMatrix();
		glTranslatef(0.1 * playerx, 0.09, 0.1 * playery);
		glScalef(0.0003, 0.0003, 0.0003);
		model_lantern.Draw();
		glPopMatrix();
		if (demoplayerd == Down) {
			glPushMatrix();
			glTranslatef(0.1 * playerx, 0.05, 0.1 * playery);
			glRotatef(180, 0, 1, 0);
			glScalef(0.0003, 0.0003, 0.0003);
			if (jump) { //place in this condition a jump sound
				//glRotatef(jump1++, 1, 0, 0);
				glTranslatef(0.1 * playerx, 7, 0.1 * playery);
				if (jump1 %360 ==0) {
					jump1 = 0;
					jump = false;
				}
					
			}
			model_tree.Draw();
			glPopMatrix();
		}
			

		
		else if (demoplayerd == Up) { 
			glPushMatrix();
			glTranslatef(0.1 * playerx, 0.05, 0.1 * playery);
			
			glScalef(0.0003, 0.0003, 0.0003);
			if (jump) { 
				//glRotatef(jump1++, 1, 0, 0);
				glTranslatef(0.1 * playerx, 7, 0.1 * playery);
				if (jump1 % 360 == 0) {
					jump1 = 0;
					jump = false;
				}

			}
			model_tree.Draw();
			glPopMatrix();

			
		}
		else if (demoplayerd == Right) {
			glPushMatrix();
			glTranslatef(0.1 * playerx, 0.05, 0.1 * playery);
			glRotatef(90, 0, 1, 0);
			glScalef(0.0003, 0.0003, 0.0003);
			if (jump) { //place in this condition a jump sound
				glTranslatef(0.1 * playerx, 7, 0.1 * playery);
				//glRotatef(jump1++, 0, 1, 0);
				if (jump1 % 360 == 0) {
					jump1 = 0;
					jump = false;
				}

			}
			model_tree.Draw();
			glPopMatrix();
		}
		else if (demoplayerd == Left) {
			glPushMatrix();
			glTranslatef(0.1 * playerx, 0.05, 0.1 * playery);
			glRotatef(-90, 0, 1, 0);
			glScalef(0.0003, 0.0003, 0.0003);
			if (jump) { //place in this condition a jump sound
				glTranslatef(0.1 * playerx, 7, 0.1 * playery);
				//glRotatef(jump1++, 0, 1, 0);
				if (jump1 % 360 == 0) {
					jump1 = 0;
					jump = false;
				}

			}
			
			model_tree.Draw();
			glPopMatrix();
		}

		
		
	}

	


}

std::string getTimeString() {

	std::ostringstream oss;
	oss << std::setfill('0') << std::setw(2) << score;
	if (win) {
		return "You Win!";
	}
	
	return oss.str();
}


void drawScene() {
	// Set the text color to white
	glColor3f(255, 255, 0);

	// Set the text position
	//glWindowPos2d(200, 200);


	// Render the timer text
	std::string timerText = getTimeString();
	for (char c : timerText) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	}
	glutSwapBuffers();
}

void writeText(std::string text, int x, int y) {
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	double* matrix = new double[16];
	glGetDoublev(GL_PROJECTION_MATRIX, matrix);
	glLoadIdentity();
	glOrtho(0, 800, 0, 600, -5, 5);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2i(x, y);
	for (int i = 0; i < text.length(); i++) {
		glColor3f(0.7f, 0.3f, 0.2f);
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, (int)text[i]);
	}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(matrix);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_LIGHTING);
}

void print(int x, int y, char* string)
{
	int len, i;
	glDisable(GL_LIGHTING);
	//set the position of the text in the window using the x and y coordinates
	glRasterPos2d(x, y);
	//glColor3f(0, 0, 0);
	//glWindowPos2i(200.0f,200.0f);

	//get the length of the string to display
	len = (int)strlen(string);

	//loop to display character by character
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
	glEnable(GL_LIGHTING);
}

void show_score() {
	glColor3f(0.0f, 0.0f, 0.0f);
	glRasterPos3f(300.0f, 300.0f, 0.0f); // Adjust the z-coordinate based on your scene
	std::string strr = std::to_string(score);
	std::string result2 = "Total Score:" + strr;

	for (size_t i = 0; i < result2.length(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, result2[i]);
	}
}

void Display() {
	setupCamera();
	setupLights();


	


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*glPushMatrix();
	glTranslated(0.4, 0.4, 0.6);
	glRotated(45, 0, 0, 1);
	glScaled(0.08, 0.08, 0.08);
	drawJack();
	glPopMatrix();
	glPushMatrix();
	glTranslated(0.6, 0.38, 0.5);
	glRotated(30, 0, 1, 0);
	glutSolidTeapot(0.08);
	glPopMatrix();*/
	player();
	collectables();
	obstacles();
	show_score();
	//drawScene();
	//std::string text="score= "+score;
	//writeText(text, 200, 200);
	/*glPushMatrix();
	glTranslatef(0.1*playerx, 0.1, 0.1*playery);
	glScalef(0.04, 0.04, 0.04);
	model_tree.Draw();
	glPopMatrix();*/
	//char* p0s[20];
	//sprintf((char*)p0s, "Score: %d", score);
	

	drawWall(0.02);
	glPushMatrix();
	glRotated(90, 0, 0, 1.0);

	drawFence(0.02);
	glPopMatrix();
	glPushMatrix();
	glRotated(90, 0, 0, 1.0);
	glTranslated(0.0, -3, 0.0);

	drawFence(0.02);

	glPopMatrix();

	glPushMatrix();
	glRotated(-90, 1.0, 0.0, 0.0);
	glTranslated(1.5, -0.85, 0);
	glScaled(1.5, 1, 1);
	drawFence2(0.02);
	glPopMatrix();

	glPushMatrix();
	glRotated(-90, 1.0, 0.0, 0.0);
	//glRotated(-180, 0.0, 0.0, 1.0);
	glTranslated(1, -1, 0.3);
	glScaled(1, 1, 1);
	drawscore(0.02);
	glPopMatrix();

	glPushMatrix();
	glRotated(-90, 1.0, 0.0, 0.0);
	glTranslated(1.5, -3.8, 0);
	glScaled(1.5, 1, 1);
	drawFence2(0.02);
	glPopMatrix();

	if (!lvl2) {
		glPushMatrix();
		glTranslated(0, 1, 4);
		drawWall(0.02);
		glPopMatrix();
	}



	glPushMatrix(); 
	
	if (!lvl2) {
		glTranslated(0.1 * 28, 0.21, 0.1 * 1);
		glScalef(0.00002, 0.00002, 0.00002);
		model_door.Draw();
	}
	else {
		glTranslated(0.1 * 28, 0.33, 0.1 * 1);
		glScalef(0.02, 0.02, 0.02);
		glRotated(180, 0.0, 1.0, 0.0);
		if(traffic)
			model_traffic.Draw();
		
	}
	

	glPopMatrix();

	//print(200.0, 200.0, (char*)p0s);

	glFlush();

}

void Keyboard(unsigned char key, int x, int y) {
	float d = 0.01;
	float a = 1.0;

	switch (key) {
	case 'w':
		camera.moveY(d);
		break;
	case 's':
		camera.moveY(-d);
		break;
	case 'a':
		camera.moveX(d);
		break;
	case 'd':
		camera.moveX(-d);
		break;
	case 'q':
		camera.moveZ(d);
		break;
	case 'e':
		camera.moveZ(-d);
		break;
	case 'u':
		camera.rotateX(a);
		//playerd = Up;
		break;
	case 'i':
		camera.rotateX(-a);
		//playerd = Down;
		break;
	case 'o':
		camera.rotateY(a);
		//playerd = Left;
		break;
	case 'p':
		camera.rotateY(-a);
		//playerd = Right;
		break;
	case 'v':
		
		PlaySound("sound/jump.wav", NULL, SND_ASYNC);
		if (!jump) {
			jump = true;
			break;
		}
		else {
			jump = false;
			break;
		}

		break;
	case 'c':
		if (!chappened) {
			camera.moveZ(-0.115);
			camera.moveY(0.03);
			chappened = true;
			break;
		}
		break;
		

	case 'x':
		
		if(chappened)
		{
			camera.moveZ(0.115);
			camera.moveY(-0.03);
			chappened = false;
		}
		break;


	case GLUT_KEY_ESCAPE:
		exit(EXIT_SUCCESS);
	}

	glutPostRedisplay();
}
void Special(int key, int x, int y) {
	float a = 1.0;
	float d = 0.1;
	switch (key) {
	case GLUT_KEY_UP:
		//camera.rotateX(a);
		if (playery != gridy - 1)
			camera.moveZ(-d);
		playerd = Up;
		break;
	case GLUT_KEY_DOWN:
		//camera.rotateX(-a);

		if (playery != 1)
			camera.moveZ(d);
		playerd = Down;
		break;
	case GLUT_KEY_LEFT:
		//camera.rotateY(a);
		if (playerx != 1)
			camera.moveX(d);
		playerd = Left;
		break;
	case GLUT_KEY_RIGHT:
		//camera.rotateY(-a);
		if (playerx != gridx - 1)camera.moveX(-d);
		playerd = Right;
		break;
	}

	glutPostRedisplay();
}
void timer_callback(int x)
{
	glutPostRedisplay();
	lightx++;
	lighty++;
	glutTimerFunc(85, timer_callback, 0);
}
void animation(int x)
{
	rotate++;
	jump1++;
	glutTimerFunc(100, animation, 0);
}
//``````````````````````````````````````````````````````````````````//
void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(100, 150);

	glutCreateWindow(title);

	glutDisplayFunc(Display);

	glutKeyboardFunc(Keyboard);

	glutSpecialFunc(Special);
	glutMouseFunc(myMouse);

	glutMotionFunc(myMotion);

	//glutMouseFunc(myMouse);

	//glutReshapeFunc(myReshape);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	//myInit();

	//InitLightSource();

	glutTimerFunc(0, timer_callback, 0);
	glutTimerFunc(1000, timer, 0);
	gridinit(Columns, Rows);
	glutTimerFunc(100, animation, 0);
	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}

//place in this condition a footwalk sound
//place in this condition a jump sound
//display u win on the screen
//place eat sound here
//place cat sound here
//place dog sound here


/*note: 1-the score display on the screen put it in the Display function
2- move: up,down,left,right
change 1st and 3rd person view: button c
jump: button v*/