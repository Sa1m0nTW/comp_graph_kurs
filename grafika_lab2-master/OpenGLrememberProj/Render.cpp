#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"


void get_normal(double a1[3], double b1[3], double c1[3], double normal[3]) {
	double a[3] = { b1[0] - a1[0], b1[1] - a1[1], b1[2] - a1[2] };
	double b[3] = { c1[0] - a1[0], c1[1] - a1[1], c1[2] - a1[2] };
	double length = 0;
	normal[0] = a[1] * b[2] - b[1] * a[2];
	normal[1] = -a[0] * b[2] + b[0] * a[2];
	normal[2] = a[0] * b[1] - b[0] * a[1];
	length = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
	normal[0] = normal[0] / length;
	normal[1] = normal[1] / length;
	normal[2] = normal[2] / length;
}

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;


	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}


	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist * cos(fi2) * cos(fi1),
			camDist * cos(fi2) * sin(fi1),
			camDist * sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}


	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);


		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale * 0.08;
		s.Show();

		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale * 1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL* ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01 * dx;
		camera.fi2 += -0.01 * dy;
	}


	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k * r.direction.X() + r.origin.X();
		y = k * r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02 * dy);
	}


}

void mouseWheelEvent(OpenGL* ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01 * delta;

}

void keyDownEvent(OpenGL* ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL* ogl, int key)
{

}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL* ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);


	//������ ����������� ���������  (R G B)
	RGBTRIPLE* texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);



	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH);


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}





void Render(OpenGL* ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ��� 
	double a[] = { 0, 0, 0 }; double a1[] = { 0, 0, 3 };
	double b[] = { 6, -5, 0 }; double b1[] = { 6, -5, 3 };
	double c[] = { 11, -3, 0 }; double c1[] = { 11, -3, 3 };
	double d[] = { 6, -7, 0 }; double d1[] = { 6, -7, 3 };
	double e[] = { 11, -11, 0 }; double e1[] = { 11, -11, 3 };
	double f[] = { 8,-15, 0 }; double f1[] = { 8, -15, 3 };
	double g[] = { 4,-8, 0 }; double g1[] = { 4, -8, 3 };
	double h[] = { -2, -10, 0 }; double h1[] = { -2, -10, 3 };

	double normal[3];


	glColor3d(0.55, 0.38, 0.45);
	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, -1);
	glVertex3d(0, 0, 0);
	glVertex3d(-2, -10, 0);
	glVertex3d(4, -8, 0);
	glEnd();

	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, -1);
	glVertex3d(0, 0, 0);
	glVertex3d(6, -5, 0);
	glVertex3d(4, -8, 0);
	glEnd();

	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, -1);
	glVertex3d(6, -7, 0);
	glVertex3d(6, -5, 0);
	glVertex3d(4, -8, 0);
	glEnd();

	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, -1);
	glVertex3d(6, -7, 0);
	glVertex3d(6, -5, 0);
	glVertex3d(11, -3, 0);
	glEnd();

	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, -1);
	glVertex3d(6, -7, 0);
	glVertex3d(11, -11, 0);
	glVertex3d(4, -8, 0);
	glEnd();

	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, -1);
	glVertex3d(8, -15, 0);
	glVertex3d(11, -11, 0);
	glVertex3d(4, -8, 0);
	glEnd();

	glColor3d(0.87, 0.92, 0.21);
	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, 1);
	glVertex3d(0, 0, 3);
	glVertex3d(-2, -10, 3);
	glVertex3d(4, -8, 3);
	glEnd();

	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, 1);
	glVertex3d(0, 0, 3);
	glVertex3d(6, -5, 3);
	glVertex3d(4, -8, 3);
	glEnd();

	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, 1);
	glVertex3d(6, -7, 3);
	glVertex3d(6, -5, 3);
	glVertex3d(4, -8, 3);
	glEnd();

	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, 1);
	glVertex3d(6, -7, 3);
	glVertex3d(6, -5, 3);
	glVertex3d(11, -3, 3);
	glEnd();

	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, 1);
	glVertex3d(6, -7, 3);
	glVertex3d(11, -11, 3);
	glVertex3d(4, -8, 3);
	glEnd();

	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, 1);
	glVertex3d(8, -15, 3);
	glVertex3d(11, -11, 3);
	glVertex3d(4, -8, 3);
	glEnd();

	glBegin(GL_QUADS);
	get_normal(a, h, h1, normal);
	glNormal3dv(normal);
	glColor3d(1, 1, 0.564);
	glVertex3d(0, 0, 0);
	glVertex3d(-2, -10, 0);
	glVertex3d(-2, -10, 3);
	glVertex3d(0, 0, 3);
	glEnd();

	glBegin(GL_QUADS);
	get_normal(h, g, g1, normal);
	glNormal3dv(normal);
	glColor3d(0.478, 1, 0.564);
	glVertex3d(-2, -10, 0);
	glVertex3d(4, -8, 0);
	glVertex3d(4, -8, 3);
	glVertex3d(-2, -10, 3);
	glEnd();

	glBegin(GL_QUADS);
	get_normal(g, f, f1, normal);
	glNormal3dv(normal);
	glColor3d(0.278, 0.25, 0.564);
	glVertex3d(4, -8, 0);
	glVertex3d(8, -15, 0);
	glVertex3d(8, -15, 3);
	glVertex3d(4, -8, 3);
	glEnd();

	glBegin(GL_QUADS);
	get_normal(f, e, e1, normal);
	glNormal3dv(normal);
	glColor3d(0.58, 0.35, 0.264);
	glVertex3d(11, -11, 0);
	glVertex3d(8, -15, 0);
	glVertex3d(8, -15, 3);
	glVertex3d(11, -11, 3);
	glEnd();

	glBegin(GL_QUADS);
	get_normal(e, d, d1, normal);
	glNormal3dv(normal);
	glColor3d(0.555, 0.35, 0.864);
	glVertex3d(11, -11, 0);
	glVertex3d(6, -7, 0);
	glVertex3d(6, -7, 3);
	glVertex3d(11, -11, 3);
	glEnd();

	glBegin(GL_QUADS);
	get_normal(d, c, c1, normal);
	glNormal3dv(normal);
	glColor3d(0.178, 0.75, 0.4);
	glVertex3d(6, -7, 0);
	glVertex3d(11, -3, 0);
	glVertex3d(11, -3, 3);
	glVertex3d(6, -7, 3);
	glEnd();

	glBegin(GL_QUADS);
	get_normal(c, b, b1, normal);
	glNormal3dv(normal);
	glColor3d(0.778, 0.55, 0.4);
	glVertex3d(6, -5, 0);
	glVertex3d(11, -3, 0);
	glVertex3d(11, -3, 3);
	glVertex3d(6, -5, 3);
	glEnd();

	glBegin(GL_QUADS);
	get_normal(b, a, a1, normal);
	glNormal3dv(normal);
	glColor3d(0.28, 0.35, 0.4);
	glVertex3d(0, 0, 0);
	glVertex3d(6, -5, 0);
	glVertex3d(6, -5, 3);
	glVertex3d(0, 0, 3);
	glEnd();
	//��������� ������ ������


	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	//(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R=" << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;

	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}