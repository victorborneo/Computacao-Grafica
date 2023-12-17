// Victor Borneo
// Leonardo Josephson

#define _CRT_SECURE_NO_WARNINGS
#define PI 3.141592654

#include <windows.h>
#include <iostream>
#include <stdlib.h>
#include <GL/glut.h>
#include "RgbImage.h"

using namespace std;

const char* filenameTexWall = "./wall.bmp";
const char* filenameTexGrass = "./grass.bmp";
const char* filenameTexSphere = "./sphereTexture.bmp";
const char* filenameTexBody = "./electronicTexture.bmp";

GLuint _textureIdSphere;
GLuint _textureIdCylinder;
GLuint _textureIdWall;
GLuint _textureIdGrass;
GLUquadric* quadSphere;
GLUquadric* quadCylinder;

bool textureOn = true;

int min_ang;

float diameterCylinder = 0.3;
float diameterSphere = 0.4;
float sizeArm = 4.5;
float sizeForearm = 3.0;
float sizeHand = 2.0;
float sizeClampPart = 1.0;
float diameterBase = 2.0;
float heightBase = 0.5;

float eyeDistance = 35.0;
float eyeX;
float eyeY;
float eyeZ;
float viewAngleX = 0.0;
float viewAngleZ = 15.0;

float angleBase = 90.0;
float angleArm = 90.0;
float angleForearm = 90.0;
float angleHand = 0.0;
float angleClampZ = 0.0;
float angleClampY = 0.0;

// 0 = standing, 1 = sitting, 2 = laying
int state = 0;
const char* motion;

float rearAngle = -15.0;
float rearLeg1Angle1 = -15.0;
float rearLeg1Angle2 = 30.0;
float rearLeg2Angle1 = -15.0;
float rearLeg2Angle2 = 30.0;

float frontAngle = -15.0;
float frontLeg1Angle1 = -15.0;
float frontLeg1Angle2 = 30.0;
float frontLeg2Angle1 = -15.0;
float frontLeg2Angle2 = 30.0;

float bodyMotion = 0.0f;

int walking = 0;
int walkingStep = 10;
int tailAngle = 0;

int clampAngle = 0;
int drilling = 0;
int drillSpeed = 0;

const int FPS = 30;
const int angleStep = 5;
bool anim = false;

//makes the image into a texture, and returns the id of the texture
GLuint loadTexture(const char* filename) {
	GLuint textureId;

	RgbImage theTexMap(filename); //Image with texture

	glGenTextures(1, &textureId); //Make room for our texture
	glBindTexture(GL_TEXTURE_2D, textureId);	//Tell OpenGL which texture to edit
	//Map the image to the texture
	glTexImage2D(GL_TEXTURE_2D,	//Always GL_TEXTURE_2D
		0,						//0 for now
		GL_RGB,					//Format OpenGL uses for image
		theTexMap.GetNumCols(),	//Width 
		theTexMap.GetNumRows(),	//Height
		0,						//The border of the image
		GL_RGB,					//GL_RGB, because pixels are stored in RGB format
		GL_UNSIGNED_BYTE,		//GL_UNSIGNED_BYTE, because pixels are stored as unsigned numbers
		theTexMap.ImageData());	//The actual pixel data
	return textureId; //Returns the id of the texture
}

void initRendering(void) {
	GLfloat luzAmbiente[4] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat luzDifusa[4] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat luzEspecular[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat posicaoLuz[4] = { 0.0, 1.0, 0.0, 1.0 };

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);
	glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
	glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular);
	glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	quadSphere = gluNewQuadric();
	quadCylinder = gluNewQuadric();

	_textureIdCylinder = loadTexture(filenameTexBody);
	_textureIdSphere = loadTexture(filenameTexSphere);

	_textureIdGrass = loadTexture(filenameTexGrass);
	_textureIdWall = loadTexture(filenameTexWall);
}

void moveTail(int) {
	static int dir = 1;

	tailAngle += 3 * dir;

	if (abs(tailAngle) >= 30) {
		dir *= -1;
	}

	glutPostRedisplay();
	glutTimerFunc(1000 / FPS, moveTail, 0);
}

void sit(int value) {
	if (value > 65 - 1) {
		anim = false;
		return;
	}

	rearLeg1Angle1 -= angleStep;
	rearLeg2Angle1 -= angleStep;
	rearLeg1Angle2 += 2 * angleStep;
	rearLeg2Angle2 += 2 * angleStep;
	rearAngle -= angleStep;
	glutPostRedisplay();
	glutTimerFunc(1000 / FPS, sit, value + 5);
}

void layDown(int value) {

	if (angleBase < 15) {
		angleBase += 5;
	}

	if (value > 65 - 1) {
		anim = false;
		return;
	}

	frontLeg1Angle1 -= angleStep;
	frontLeg2Angle1 -= angleStep;
	frontLeg1Angle2 += 2 * angleStep;
	frontLeg2Angle2 += 2 * angleStep;
	frontAngle -= angleStep;
	glutPostRedisplay();
	glutTimerFunc(1000 / FPS, layDown, value + 5);
}

void standUp(int value) {
	if (value > 65 - 1) {
		anim = false;
		return;
	}

	if (state == 1) {
		frontLeg1Angle1 += angleStep;
		frontLeg2Angle1 += angleStep;
		frontLeg1Angle2 -= 2 * angleStep;
		frontLeg2Angle2 -= 2 * angleStep;
		frontAngle += angleStep;
	}
	else {
		rearLeg1Angle1 += angleStep;
		rearLeg2Angle1 += angleStep;
		rearLeg1Angle2 -= 2 * angleStep;
		rearLeg2Angle2 -= 2 * angleStep;
		rearAngle += angleStep;
	}

	glutPostRedisplay();
	glutTimerFunc(1000 / FPS, standUp, value + 5);
}

void walk(int value) {
	static int dir = 1;
	static int count = 0;

	if (value > 65 - 1) {
		if (motion == "stop" && dir < 0) {
			dir = 1;
			count = 0;
			anim = false;
			return;
		}
		dir *= -1;
		value = 0;
		count = (count + 1) % 4;
	}

	if (count <= 1) {
		rearLeg1Angle1 += 0.5 * angleStep * dir;
		frontLeg1Angle1 += 0.5 * angleStep * dir;
	} else {
		rearLeg2Angle1 += 0.5 * angleStep * dir;
		frontLeg2Angle1 += 0.5 * angleStep * dir;
	}

	bodyMotion += 0.05 * dir;
	walking = (walking + 1) % walkingStep;

	glutPostRedisplay();
	glutTimerFunc(1000 / FPS, walk, value + 5);
}

void eat(int) {
	static int dir = 1;
	static bool eating = true;
	eating = true;

	if (motion == "stopEat") {
		return;
	}

	if (angleBase > -45) {
		eating = false;
		angleBase -= 3;
	}

	if (angleForearm != 45) {
		eating = false;
		angleForearm += angleForearm - 45 > 0 ? -3 : 3;
	}

	if (angleArm != 90) {
		eating = false;
		angleArm += angleArm - 90 > 0 ? -3 : 3;
	}

	if (angleClampY > 0 && !eating) {
		eating = false;
		angleClampY--;
	}

	if (eating) {
		if (angleClampY >= 20 || angleClampY < 0) {
			dir *= -1;
		}

		angleClampY += 2 * dir;
	}

	glutPostRedisplay();
	glutTimerFunc(1000 / FPS, eat, 0);
}

void raiseHead(int) {
	bool done = true;

	if (angleBase < 90) {
		done = false;
		angleBase += 3;
	}

	if (angleForearm < 90) {
		done = false;
		angleForearm += 3;
	}

	if (angleClampY > 0) {
		angleClampY -= 2;
		done = false;
	}

	if (done) {
		anim = false;
		return;
	}

	glutPostRedisplay();
	glutTimerFunc(1000 / FPS, raiseHead, 0);
}

void animate(const char* dir) {
	if (dir == "eat") {
		eat(0);
		return;
	}

	if (dir == "stopEat") {
		raiseHead(0);
		return;
	}

	if (dir == "up") {
		standUp(0);
		return;
	}

	switch (state) {
	case 1:
		sit(0);
		return;
	case 2:
		layDown(0);
		return;
	default:
		break;
	}

	walk(0);
}

void handleSpecialKeypress(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		if (motion == "walk") {
			motion = "stop";
		}
		break;
	case GLUT_KEY_RIGHT:
		if (state == 0 && anim == false) {
			anim = true;
			motion = "walk";
			animate(motion);
		}
		break;
	case GLUT_KEY_UP:
		if (state >= 1 && anim == false) {
			state--;
			anim = true;
			motion = "up";
			animate(motion);
		}
		break;
	case GLUT_KEY_DOWN:
		if (state <= 1 && anim == false) {
			state++;
			anim = true;
			motion = "down";
			animate(motion);
		}
		break;
	default:
		return;
	}
}

void drill(int) {
	if (drilling == 0 && drillSpeed == 0) {
		return;
	}

	if (drillSpeed <= 50 || drilling == 0) {
		drillSpeed += max(drillSpeed / 10, 1) * (drilling == 1 ? 1 : -1);
	}
	clampAngle += drillSpeed;

	glutPostRedisplay();
	glutTimerFunc(1000 / FPS, drill, 0);
}

void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
	case 27: //Escape key
		exit(0);
	case 'w': //Increase view angle z axis
		if (viewAngleZ < 90) viewAngleZ += 3;
		break;
	case 's': //Decrease view angle z axis
		if (viewAngleZ > 0) viewAngleZ -= 3;
		break;
	case 'a': //Decrease view angle x axis
		if (viewAngleX > -90) viewAngleX -= 3;
		break;
	case 'd': //Increase view angle x axis
		if (viewAngleX < 90) viewAngleX += 3;
		break;
	case 't': //Use texture or not
		textureOn = !textureOn;
		break;
	case '1': //Increase arm angle
		if (angleBase < 120 && !anim ) angleBase += 3;
		break;
	case '2': //Decrease arm angle
		min_ang = state != 2 ? -35 : 15;
		if (angleBase >= min_ang && !anim) angleBase -= 3;
		break;
	case '3': //Increase arm angle
		if (anim) break;
		angleArm += 3;
		if (angleArm >= 360) angleArm = 0;
		break;
	case '4': //Decrease arm angle
		if (anim) break;
		angleArm -= 3;
		if (angleArm <= 0) angleArm = 360;
		break;
	case '5': //Increase forearm angle
		if (angleForearm < 90 && !anim) angleForearm += 3;
		break;
	case '6': //Decrease forearm angle
		if (angleForearm > -90 && !anim) angleForearm -= 3;
		break;
	case '7': //Increase clamp angle y axis
		if (angleClampY < 60 && !anim) angleClampY += 3;
		break;
	case '8': //Decrease clamp angle y axis
		if (angleClampY > 0 && !anim) angleClampY -= 3;
		break;
	case '0':
		if (anim) {
			break;
		}
		if (drilling == 0 && drillSpeed == 0) {
			drilling = 1;
			drill(0);
		}
		else {
			drilling = 0;
		}
		break;
	case ' ':
		if (!anim && state != 2) {
			anim = true;
			motion = "eat";
			animate(motion);
			break;
		}

		if (anim && motion == "eat") {
			motion = "stopEat";
			animate(motion);
		}

		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void handleMouse(int button, int state, int x, int y) {
	if (button == 3 && state == 0 && eyeDistance > 20) {
		eyeDistance--;
	}
	else if (button == 4 && state == 0 && eyeDistance < 50) {
		eyeDistance++;
	}
}

void handleResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (float)w / (float)h, 1.0, 100.0);
}

void drawCylinder(float diameter, float lenght) {
	if (textureOn) {
		glBindTexture(GL_TEXTURE_2D, _textureIdCylinder);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		gluQuadricTexture(quadCylinder, 1);
	}
	else
		gluQuadricTexture(quadCylinder, 0);
	gluCylinder(quadCylinder, diameter, diameter, lenght, 40.0, lenght * 30.0);
}

void drawCone(float diameter, float lenght) {
	if (textureOn) {
		glBindTexture(GL_TEXTURE_2D, _textureIdCylinder);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		gluQuadricTexture(quadCylinder, 1);
	}
	else
		gluQuadricTexture(quadCylinder, 0);
	gluCylinder(quadCylinder, diameter, 0, lenght, 40.0, lenght * 30.0);
}

void drawDisk(float diameterInner, float diameterOuter) {
	if (textureOn) {
		glBindTexture(GL_TEXTURE_2D, _textureIdCylinder);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		gluQuadricTexture(quadCylinder, 1);
	}
	else
		gluQuadricTexture(quadCylinder, 0);
	gluDisk(quadCylinder, diameterInner, diameterOuter, 40.0, 30.0);
}

void drawSphere(float diameter) {
	if (textureOn) {
		glBindTexture(GL_TEXTURE_2D, _textureIdSphere);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		gluQuadricTexture(quadSphere, 1);
	}
	else
		gluQuadricTexture(quadSphere, 0);
	gluSphere(quadSphere, diameter, 40.0, 40.0);
}

void drawFloor() {
	glBindTexture(GL_TEXTURE_2D, _textureIdGrass);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glPushMatrix();
	for (int i = -8; i < 8; i++) {
		for (int j = -5; j < 5; j++) {
			glBegin(GL_QUADS);
			glTexCoord2f(0, 1);
			glVertex3f(j * 10, (i * 10 - walking) / 2.0, -7.5);

			glTexCoord2f(1, 1);
			glVertex3f(j * 10 + 10, (i * 10 - walking) / 2.0, -7.5);

			glTexCoord2f(1, 0);
			glVertex3f(j * 10 + 10, (i * 10 + 10 - walking) / 2.0, -7.5);

			glTexCoord2f(0, 0);
			glVertex3f(j * 10, (i * 10 + 10 - walking) / 2.0, -7.5);
			glEnd();
		}
	}
	glPopMatrix();
}

void drawWall() {
	glBindTexture(GL_TEXTURE_2D, _textureIdWall);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glPushMatrix();
	for (int i = -3; i < 3; i++) {
		for (int j = -8; j < 8; j++) {
			glBegin(GL_QUADS);
			glTexCoord2f(0, 1);
			glVertex3f(-10, (j * 10 - walking) / 2.0, i * 10);

			glTexCoord2f(1, 1);
			glVertex3f(-10, (j * 10 + 10 - walking) / 2.0, i * 10);

			glTexCoord2f(1, 0);
			glVertex3f(-10, (j * 10 + 10 - walking) / 2.0, i * 10 + 10);

			glTexCoord2f(0, 0);
			glVertex3f(-10, (j * 10 - walking) / 2.0, i * 10 + 10);
			glEnd();
		}
	}
	glPopMatrix();
}

void printCurrentMatrix() {
	GLfloat matrix[16];

	glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			cout << matrix[4 * i + j] << " ";
		}
		cout << endl;
	}
}

void drawScene(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	eyeX = eyeDistance * cos(viewAngleZ * PI / 180) * cos(viewAngleX * PI / 180);
	eyeY = eyeDistance * cos(viewAngleZ * PI / 180) * sin(viewAngleX * PI / 180);
	eyeZ = eyeDistance * sin(viewAngleZ * PI / 180);

	if (viewAngleZ < 90)
		gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0);
	else
		gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 1.0, 0.0, 0.0, -1.0);


	drawFloor();
	drawWall();

	glTranslatef(0.0f, 0.0f, (rearAngle + 15) / 15.0f);

	glRotatef(-90, 1.0f, 0.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, bodyMotion);
	glRotatef((-rearAngle - 15) / 2.5, 1.0f, 0.0f, 0.0f);
	glRotatef((frontAngle + 15) / 2.5, 1.0f, 0.0f, 0.0f);

	glPushMatrix();
	// draw body
	drawCylinder(diameterBase, 15 * heightBase);
	drawDisk(diameterCylinder, diameterBase);
	glTranslatef(0.0f, 0.0f, 15 * heightBase);
	drawDisk(diameterCylinder, diameterBase);
	drawSphere(diameterSphere);
	glPushMatrix();

	// move to head's referential
	glTranslatef(0.0f, -diameterBase, 0.0f);
	glRotatef(angleBase, 1.0f, 0.0f, 0.0f);

	// draws the base
	//drawCylinder(diameterBase, heightBase);
	//glTranslatef(0.0f, 0.0f, heightBase);
	//drawDisk(diameterCylinder, diameterBase);

	// move to arm referential
	glRotatef(angleArm, 0.0f, 0.0f, 1.0f);

	//draws the arm
	drawSphere(diameterSphere);
	drawCylinder(diameterCylinder, sizeArm);

	// move to forearm referential
	glTranslatef(0.0f, 0.0f, sizeArm + diameterSphere / 5);
	glRotatef(angleForearm, 0.0f, 1.0f, 0.0f);

	//draws the forearm
	drawSphere(diameterSphere);
	glTranslatef(0.0f, 0.0f, diameterSphere / 5);
	drawCylinder(diameterCylinder, sizeForearm);

	//move to clamp referential
	glRotatef(clampAngle, 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, 0.0f, sizeForearm + diameterSphere / 5);
	glRotatef(angleClampZ, 0.0f, 0.0f, 1.0f);

	//draws the clamp sphere
	drawSphere(diameterSphere);
	glTranslatef(0.0f, 0.0f, diameterSphere / 2);

	glPushMatrix();

	//draws top part of clamp
	glRotatef(-angleClampY - 60, 0.0f, 1.0f, 0.0f);

	drawCylinder(diameterCylinder / 3, sizeClampPart);
	glTranslatef(0.0f, 0.0f, sizeClampPart + diameterSphere / 15);
	drawSphere(diameterSphere / 3);

	glTranslatef(0.0f, 0.0f, diameterSphere / 15);
	glRotatef(60, 0.0f, 1.0f, 0.0f);

	drawCylinder(diameterCylinder / 3, sizeClampPart);
	glTranslatef(0.0f, 0.0f, sizeClampPart + diameterSphere / 15);
	drawSphere(diameterSphere / 3);

	glTranslatef(0.0f, 0.0f, diameterSphere / 15);
	glRotatef(60, 0.0f, 1.0f, 0.0f);
	drawCone(diameterCylinder / 3, sizeClampPart);

	glPopMatrix();
	glPushMatrix();

	//rotate z axis for 1st bottom clamp referential
	glRotatef(angleClampZ - 45, 0.0f, 0.0f, 1.0f);

	//draws 1st bottom part of clamp
	glRotatef(angleClampY + 60, 0.0f, 1.0f, 0.0f);

	drawCylinder(diameterCylinder / 3, sizeClampPart);
	glTranslatef(0.0f, 0.0f, sizeClampPart + diameterSphere / 15);
	drawSphere(diameterSphere / 3);

	glTranslatef(0.0f, 0.0f, diameterSphere / 15);
	glRotatef(-60, 0.0f, 1.0f, 0.0f);

	drawCylinder(diameterCylinder / 3, sizeClampPart);
	glTranslatef(0.0f, 0.0f, sizeClampPart + diameterSphere / 15);
	drawSphere(diameterSphere / 3);

	glTranslatef(0.0f, 0.0f, diameterSphere / 15);
	glRotatef(-60, 0.0f, 1.0f, 0.0f);
	drawCone(diameterCylinder / 3, sizeClampPart);

	glPopMatrix();

	//rotate z axis for 2nd bottom clamp referential
	glRotatef(angleClampZ + 45, 0.0f, 0.0f, 1.0f);

	//draws 2nd bottom part of clamp
	glRotatef(angleClampY + 60, 0.0f, 1.0f, 0.0f);

	drawCylinder(diameterCylinder / 3, sizeClampPart);
	glTranslatef(0.0f, 0.0f, sizeClampPart + diameterSphere / 15);
	drawSphere(diameterSphere / 3);

	glTranslatef(0.0f, 0.0f, diameterSphere / 15);
	glRotatef(-60, 0.0f, 1.0f, 0.0f);

	drawCylinder(diameterCylinder / 3, sizeClampPart);
	glTranslatef(0.0f, 0.0f, sizeClampPart + diameterSphere / 15);
	drawSphere(diameterSphere / 3);

	glTranslatef(0.0f, 0.0f, diameterSphere / 15);
	glRotatef(-60, 0.0f, 1.0f, 0.0f);
	drawCone(diameterCylinder / 3, sizeClampPart);

	glPopMatrix();
	glPushMatrix();

	// draw 1st front leg
	glTranslatef(diameterBase * cos(45), diameterBase * sin(45), 0.0f);
	drawSphere(diameterSphere);
	glRotatef(-90, 1.0f, 0.0f, 0.0f);
	glRotatef(frontLeg1Angle1, 1.0f, 0.0f, 0.0f);
	drawCylinder(diameterCylinder, 2.5 * sizeClampPart);
	glTranslatef(0.0f, 0.0f, 2.5 * sizeClampPart);
	drawSphere(diameterSphere);
	glRotatef(frontLeg1Angle2, 1.0f, 0.0f, 0.0f);
	drawCylinder(diameterCylinder, 2.5 * sizeClampPart);
	glTranslatef(0.0f, 0.0f, 2.5 * sizeClampPart);
	drawSphere(diameterSphere);

	glPopMatrix();

	// draw 2nd front leg
	glTranslatef(diameterBase * cos(90), diameterBase * sin(90), 0.0f);
	drawSphere(diameterSphere);
	glRotatef(-90, 1.0f, 0.0f, 0.0f);
	glRotatef(frontLeg2Angle1, 1.0f, 0.0f, 0.0f);
	drawCylinder(diameterCylinder, 2.5 * sizeClampPart);
	glTranslatef(0.0f, 0.0f, 2.5 * sizeClampPart);
	drawSphere(diameterSphere);
	glRotatef(frontLeg2Angle2, 1.0f, 0.0f, 0.0f);
	drawCylinder(diameterCylinder, 2.5 * sizeClampPart);
	glTranslatef(0.0f, 0.0f, 2.5 * sizeClampPart);
	drawSphere(diameterSphere);

	glPopMatrix();
	glPushMatrix();

	glPushMatrix();
	drawSphere(diameterSphere);
	glRotatef(180, 1, 0, 0);
	glRotatef(60, 1, 0, 0);
	glRotatef(tailAngle, 0, 1, 0);
	for (int i = 0; i < 10; i++) {
		drawCylinder(0.5, 0.5);
		glTranslatef(0, 0, 0.5);
		glRotatef(-10, 1, 0, 0);
	}
	drawCone(0.5, 1);
	glPopMatrix();

	// draw 1st rear leg
	glTranslatef(diameterBase * cos(90), diameterBase * sin(90), 0.0f);
	drawSphere(diameterSphere);
	glRotatef(-90, 1.0f, 0.0f, 0.0f);
	glRotatef(rearLeg1Angle1, 1.0f, 0.0f, 0.0f);
	drawCylinder(diameterCylinder, 2.5 * sizeClampPart);
	glTranslatef(0.0f, 0.0f, 2.5 * sizeClampPart);
	drawSphere(diameterSphere);
	glRotatef(rearLeg1Angle2, 1.0f, 0.0f, 0.0f);
	drawCylinder(diameterCylinder, 2.5 * sizeClampPart);
	glTranslatef(0.0f, 0.0f, 2.5 * sizeClampPart);
	drawSphere(diameterSphere);

	glPopMatrix();

	// draw 2nd rear leg
	glTranslatef(diameterBase * cos(45), diameterBase * sin(45), 0.0f);
	drawSphere(diameterSphere);
	glRotatef(-90, 1.0f, 0.0f, 0.0f);
	glRotatef(rearLeg2Angle1, 1.0f, 0.0f, 0.0f);
	drawCylinder(diameterCylinder, 2.5 * sizeClampPart);
	glTranslatef(0.0f, 0.0f, 2.5 * sizeClampPart);
	drawSphere(diameterSphere);
	glRotatef(rearLeg2Angle2, 1.0f, 0.0f, 0.0f);
	drawCylinder(diameterCylinder, 2.5 * sizeClampPart);
	glTranslatef(0.0f, 0.0f, 2.5 * sizeClampPart);
	drawSphere(diameterSphere);

	glPopMatrix();
	glutSwapBuffers();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Cachorro (ou cavalo)");
	glEnable(GL_DEPTH_TEST);

	initRendering();
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutSpecialFunc(handleSpecialKeypress);
	glutReshapeFunc(handleResize);
	glutMouseFunc(handleMouse);

	moveTail(0);
	glutMainLoop();
	return 0;
}