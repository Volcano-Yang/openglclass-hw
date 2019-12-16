// rotating cube with two texture objects
// change textures with 1 and 2 keys

#include "Angel.h"
#include "mesh.h"
#include "FreeImage.h"
#include "Mesh_Painter.h"
#include <assert.h>
#include <iostream>
#include <cmath>
#include <GL/glew.h>
#include <glfw3.h>
using namespace std;

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "FreeImage.lib")
typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

std::vector<My_Mesh*> my_meshs;
Mesh_Painter*			mp_;
// Texture objects and storage for texture image

// program �� vao
GLuint program;
GLuint vao[2];

// �϶������ƹ�Դλ�õĲ���
bool mouseLeftDown;
bool mouseRightDown;
float mouseX, mouseY;
float cameraDistance;
float cameraAngleX;
float cameraAngleY;

// modelView
GLuint ModelViewMatrixID;
mat4 ModelViewMatrix;

// shadow
mat4 shadow;
GLuint shadowMatrixID;

// perspective ����
float fov = 100.0;
float aspect = 1.0;
float zN = 0.1;
float zF = 100.0;
//�������
vec4 eye;
vec4 at;
vec4 up;


bool firstMouse = true;
GLfloat lastX = 1300 / 2.0;
GLfloat lastY = 800 / 2.0;
GLfloat yaw = -90.0f;
GLfloat pitch = 0.0f;
// �����ӽǵĽǶȳ�ʼֵ
float rad = 50.0;
float tAngle = 25.0;
float pAngle = 0.0;

// ��Դ
vec3 lightPos(5, 5, 10);
GLuint lightPosID;

// ���ƻ��������ߵĲ���
int runGesture = 1;	// �����л���������
float runX = 0, runY = 0, runZ = 0;	// �����ƶ�������
int cameramouse = 0;	// �����л���һ�ӽ�����ģʽ
int movetorso = 0;	// �����л���һ�ӽ�����ģʽ
int stepSize = 2;	// ���ƻ�����ÿһ���Ĳ���
float increase = 1.0;	// ���ƻ�����ʱ��λ��ƫ��ֵ

						// ���ڴ���ƬԪ��ɫ�����Կ�����Ӱ���ֲ����������䡢���淴��ͻ����� 
GLuint sflag;

namespace Camera
{
	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 projMatrix;

	// ƽ��ͶӰ
	mat4 ortho(const GLfloat left, const GLfloat right,
		const GLfloat bottom, const GLfloat top,
		const GLfloat zNear, const GLfloat zFar)
	{
		mat4 c;
		c[0][0] = 2.0 / (right - left);
		c[1][1] = 2.0 / (top - bottom);
		c[2][2] = 2.0 / (zNear - zFar);
		c[3][3] = 1.0;
		c[0][3] = -(right + left) / (right - left);
		c[1][3] = -(top + bottom) / (top - bottom);
		c[2][3] = -(zFar + zNear) / (zFar - zNear);
		return c;
	}

	// ͸��ͶӰ������ԶС
	mat4 perspective(const GLfloat fovy, const GLfloat aspect,
		const GLfloat zNear, const GLfloat zFar)
	{
		GLfloat top = tan(fovy*DegreesToRadians / 2) * zNear;
		GLfloat right = top * aspect;

		mat4 c;
		c[0][0] = zNear / right;
		c[1][1] = zNear / top;
		c[2][2] = -(zFar + zNear) / (zFar - zNear);
		c[2][3] = -2.0*zFar*zNear / (zFar - zNear);
		c[3][2] = -1.0;
		return c;
	}

	// ����۲캯��
	mat4 lookAt(const vec4& eye, const vec4& at, const vec4& up)
	{
		vec4 n = normalize(at - eye);
		vec4 u = normalize(vec4(cross(n, up), 0.0));
		vec4 v = normalize(vec4(cross(u, n), 0.0));
		vec4 t = vec4(0.0, 0.0, 0.0, 1.0);
		mat4 c = mat4(u, v, -n, t);
		return c * Translate(-eye);
	}
}

const int NumVertices = 36; // ÿ������6���棬ÿ�������2��������Ƭ��ÿ��������Ƭ����3������
point4 points[NumVertices];	// ������
color4 colors[NumVertices];	// ����ɫ
point4 nomal[NumVertices];	// ������
							// ������
point4 vertices[8] = {
	point4(-0.5, -0.5, 0.5, 1.0),
	point4(-0.5, 0.5, 0.5, 1.0),
	point4(0.5, 0.5, 0.5, 1.0),
	point4(0.5, -0.5, 0.5, 1.0),
	point4(-0.5, -0.5, -0.5, 1.0),
	point4(-0.5, 0.5, -0.5, 1.0),
	point4(0.5, 0.5, -0.5, 1.0),
	point4(0.5, -0.5, -0.5, 1.0)
};

// ��ɫ
color4 vertex_colors[8] = {
	color4(0.0, 0.0, 0.0, 1.0),  // ��ɫ��0
	color4(1.0, 0.0, 0.0, 1.0),  // ��ɫ��1,
	color4(1.0, 1.0, 0.0, 1.0),  // ��ɫ��2
	color4(0.0, 1.0, 0.0, 1.0),  // ��ɫ��3
	color4(0.0, 0.0, 1.0, 1.0),  // ��ɫ��4
	color4(1.0, 0.0, 1.0, 1.0),  // Ʒ��ɫ��5
	color4(1.0, 1.0, 1.0, 1.0),  // ��ɫ��6
	color4(0.0, 1.0, 1.0, 1.0)   // ��ɫ��7
};

// �������˵ĸ�����λ������ɫ
point4 color_torso = vertex_colors[4];
point4 color_head = vertex_colors[7];
point4 color_upper_arm = vertex_colors[6];
point4 color_lower_arm = vertex_colors[3];
point4 color_upper_leg = vertex_colors[0];
point4 color_lower_leg = vertex_colors[5];

//----------------------------------------------------------------------------
//����ջ
class MatrixStack {
	int    _index;
	int    _size;
	mat4*  _matrices;

public:
	MatrixStack(int numMatrices = 100) :_index(0), _size(numMatrices)
	{
		_matrices = new mat4[numMatrices];
	}

	~MatrixStack()
	{
		delete[]_matrices;
	}

	void push(const mat4& m) {
		assert(_index + 1 < _size);
		_matrices[_index++] = m;

	}

	mat4& pop(void) {
		assert(_index - 1 >= 0);
		_index--;
		return _matrices[_index];
	}
};

MatrixStack  mvstack;
mat4         model_view;
GLuint       ModelView, Projection;
GLuint       draw_color;
//----------------------------------------------------------------------------

// ��������˸�����λ�Ŀ��͸�
#define TORSO_HEIGHT 5.0
#define TORSO_WIDTH 3.0
#define UPPER_ARM_HEIGHT 3.0
#define LOWER_ARM_HEIGHT 2.0
#define UPPER_LEG_WIDTH  0.5
#define LOWER_LEG_WIDTH  0.5
#define LOWER_LEG_HEIGHT 2.5
#define UPPER_LEG_HEIGHT 3.0
#define UPPER_ARM_WIDTH  0.5
#define LOWER_ARM_WIDTH  0.5
#define HEAD_HEIGHT 1.5
#define HEAD_WIDTH 1.0

// ���������˲������������Ա�ʹ�ùؽڽǶ�
enum {
	Torso,//0
	Head,//1
	RightUpperArm,//2
	RightLowerArm,//3
	LeftUpperArm,//4
	LeftLowerArm,//5
	RightUpperLeg,//6
	RightLowerLeg,//7
	LeftUpperLeg,//8
	LeftLowerLeg,//9
	NumJointAngles,//10
};
//�����˵�����
const int controlcameraon = 11;
const int controlcameraoff = 12;

// ��ʼ���ؽڽǶ�
GLfloat
theta[NumJointAngles] = {
	0.0,    // Torso
	0.0,    // Head
	0.0,    // RightUpperArm
	0.0,    // RightLowerArm
	0.0,    // LeftUpperArm
	0.0,    // LeftLowerArm
	0.0,  // RightUpperLeg
	0.0,    // RightLowerLeg
	0.0,  // LeftUpperLeg
	0.0     // LeftLowerLeg
};

GLint angle = Head;

//----------------------------------------------------------------------------

int Index = 0;

void
quad(int a, int b, int c, int d)
{
	vec3 nomalab = vec3(vertices[a].x - vertices[b].x, vertices[a].y - vertices[b].y, vertices[a].z - vertices[b].z);
	vec3 nomalbc = vec3(vertices[b].x - vertices[c].x, vertices[b].y - vertices[c].y, vertices[b].z - vertices[c].z);
	vec3 nomalcd = vec3(vertices[c].x - vertices[d].x, vertices[c].y - vertices[d].y, vertices[c].z - vertices[d].z);
	vec3 nomalabc = cross(nomalab, nomalbc);//�����������ĸ�����ķ�����

	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; nomal[Index] = nomalabc;

	Index++;

	colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; nomal[Index] = nomalabc; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; nomal[Index] = nomalabc; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; nomal[Index] = nomalabc; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; nomal[Index] = nomalabc; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; nomal[Index] = nomalabc; Index++;
}

void
colorcube(void)//�������ɵ�λ���������������,������ɫ�ͼ��㷨����
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}

//----------------------------------------------------------------------------
// ���ƻ�������Ӱ
void add_robot_shadow()
{
	//����Ӱ��������ɫ��
	glUniformMatrix4fv(shadowMatrixID, 1, GL_TRUE, &shadow[0][0]);
	//������ɫ
	glUniform4fv(draw_color, 1, vertex_colors[0]);
	// �����Ϊ��Ӱ
	glUniform1i(sflag, 1);
	//����
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
}

// ��ʼ����Ӱ����
void init_shadowMatrix()
{
	mat4 shadowMatrix = mat4(1.0);
	glUniformMatrix4fv(shadowMatrixID, 1, GL_TRUE, &shadowMatrix[0][0]);
	glUniformMatrix4fv(ModelViewMatrixID, 1, GL_TRUE, &ModelViewMatrix[0][0]);
}

void
torso()
{
	init_shadowMatrix();

	mvstack.push(model_view);// ���游�ڵ����

	mat4 instance = (Translate(0.0, 0.5 * TORSO_HEIGHT, 0.0) *
		Scale(TORSO_WIDTH, TORSO_HEIGHT, TORSO_WIDTH / 2));// ���ڵ�ֲ��任����

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);// ���ڵ���� * ���ڵ�ֲ��任����
	glUniform4fv(draw_color, 1, color_torso);

	glUniform1i(sflag, 0);	// ����䲻Ϊ��Ӱ����ͬ��
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	add_robot_shadow(); // Ϊ�˲��ֻ�����Ӱ����ͬ��

	model_view = mvstack.pop();// �ָ����ڵ����
}

void
head()
{
	init_shadowMatrix();

	mvstack.push(model_view);// ���游�ڵ����

	mat4 instance = (Translate(0.0, 0.5 * HEAD_HEIGHT, 0.0) *
		Scale(HEAD_WIDTH, HEAD_HEIGHT, HEAD_WIDTH));// ���ڵ�ֲ��任����
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);// ���ڵ����*���ڵ�ֲ��任����
	glUniform4fv(draw_color, 1, color_head);
	glUniform1i(sflag, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	add_robot_shadow();
	model_view = mvstack.pop();// �ָ����ڵ����
}

void
left_upper_arm()
{
	init_shadowMatrix();// ��ʼ����Ӱ����

	mvstack.push(model_view);//���游�ڵ����

	mat4 instance = (Translate(0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0) *
		Scale(UPPER_ARM_WIDTH + 0.3, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH + 0.3));//���ڵ�ֲ��任����

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
	glUniform4fv(draw_color, 1, color_upper_arm);
	glUniform1i(sflag, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	add_robot_shadow();

	model_view = mvstack.pop();//�ָ����ڵ����
}

void
left_lower_arm()
{
	init_shadowMatrix();// ��ʼ����Ӱ����
	mvstack.push(model_view);//���游�ڵ����
	mat4 instance = (Translate(0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0) *
		Scale(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH));//���ڵ�ֲ��任����
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
	glUniform4fv(draw_color, 1, color_lower_arm);	// �󶨻�����ɫ
	glUniform1i(sflag, 0);	 // ��Ǵ˲�����Ӱ
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);	// ����
	add_robot_shadow();	// Ϊ�ⲿ��������ӰЧ��
	model_view = mvstack.pop();//�ָ����ڵ����
}

void
right_upper_arm()
{
	init_shadowMatrix();// ��ʼ����Ӱ����

	mvstack.push(model_view);//���游�ڵ����

	mat4 instance = (Translate(0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0) *
		Scale(UPPER_ARM_WIDTH + 0.3, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH + 0.3));//���ڵ�ֲ��任����

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
	glUniform4fv(draw_color, 1, color_upper_arm);
	glUniform1i(sflag, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	add_robot_shadow();

	model_view = mvstack.pop();//�ָ����ڵ����
}

void
right_lower_arm()
{
	init_shadowMatrix();// ��ʼ����Ӱ����

	mvstack.push(model_view);//���游�ڵ����

	mat4 instance = (Translate(0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0) *
		Scale(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH));//���ڵ�ֲ��任����

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
	glUniform4fv(draw_color, 1, color_lower_arm);
	glUniform1i(sflag, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	add_robot_shadow();

	model_view = mvstack.pop();//�ָ����ڵ����
}

void
left_upper_leg()
{
	init_shadowMatrix();// ��ʼ����Ӱ����
	mvstack.push(model_view);//���游�ڵ����
	mat4 instance = (Translate(0.0, 0.5 * UPPER_LEG_HEIGHT, 0.0) *
		Scale(UPPER_LEG_WIDTH + 0.3, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH + 0.3));//���ڵ�ֲ��任����
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
	glUniform4fv(draw_color, 1, color_upper_leg);//������ɫ
	glUniform1i(sflag, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);//����
	add_robot_shadow();//������Ӱ
	model_view = mvstack.pop();//�ָ����ڵ����
}

void
left_lower_leg()
{
	init_shadowMatrix();// ��ʼ����Ӱ����
	mvstack.push(model_view);//���游�ڵ����
	mat4 instance = (Translate(0.0, 0.5 * LOWER_LEG_HEIGHT, 0.0) *
		Scale(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH));//���ڵ�ֲ��任����
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
	glUniform4fv(draw_color, 1, color_lower_leg);
	glUniform1i(sflag, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	add_robot_shadow();
	model_view = mvstack.pop();//�ָ����ڵ����
}

void
right_upper_leg()
{
	init_shadowMatrix();// ��ʼ����Ӱ����

	mvstack.push(model_view);//���游�ڵ����

	mat4 instance = (Translate(0.0, 0.5 * UPPER_LEG_HEIGHT, 0.0) *
		Scale(UPPER_LEG_WIDTH + 0.3, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH + 0.3));//���ڵ�ֲ��任����

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
	glUniform4fv(draw_color, 1, color_upper_leg);
	glUniform1i(sflag, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	add_robot_shadow();

	model_view = mvstack.pop();//�ָ����ڵ����
}

void
right_lower_leg()
{
	init_shadowMatrix();// ��ʼ����Ӱ����

	mvstack.push(model_view);//���游�ڵ����

	mat4 instance = (Translate(0.0, 0.5 * LOWER_LEG_HEIGHT, 0.0) *
		Scale(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH));//���ڵ�ֲ��任����

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
	glUniform4fv(draw_color, 1, color_lower_leg);
	glUniform1i(sflag, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	add_robot_shadow();

	model_view = mvstack.pop();//�ָ����ڵ����
}

//----------------------------------------------------------------------------

void
menu(int option)
{
	if (option == controlcameraon) {
		cameramouse = 1;//���������������ͷ
	}
	else if (option == controlcameraoff) {
		cameramouse = 0;//�ر������������ͷ
	}
	else
		angle = option;//�л�Ҫ����ת���Ĳ���
}
//----------------------------------------------------------------------------

void
reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	GLfloat left = -10.0, right = 10.0;
	GLfloat bottom = -5.0, top = 15.0;
	GLfloat zNear = -10.0, zFar = 10.0;
	GLfloat aspect = GLfloat(width) / height;
	if (aspect > 1.0) {
		left *= aspect;
		right *= aspect;
	}
	else {
		bottom /= aspect;
		top /= aspect;
	}
	mat4 projection = Ortho(left, right, bottom, top, zNear, zFar);
	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);

	model_view = mat4(1.0);
}

//----------------------------------------------------------------------------

void
init()
{
	colorcube();//���ɵ�λ������
				// ���������������
	glGenVertexArrays(1, &vao[0]);
	glBindVertexArray(vao[0]);
	// ��������ʼ�����㻺�����
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(nomal),
		NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors),
		colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(nomal), nomal);

	// ������ɫ��
	program = InitShader("vshader82.glsl", "fshader82.glsl");

	ModelViewMatrixID = glGetUniformLocation(program, "modelViewMatrix");
	shadowMatrixID = glGetUniformLocation(program, "shadowMatrix");
	glUseProgram(program);
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));
	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points)));

	ModelView = glGetUniformLocation(program, "ModelView");
	Projection = glGetUniformLocation(program, "Projection");
	draw_color = glGetUniformLocation(program, "draw_color");
	lightPosID = glGetUniformLocation(program, "lightPos");
	sflag = glGetUniformLocation(program, "sflag");

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points) + sizeof(colors)));
	//��ʼ���������
	eye = vec4(0.0f, 15.0f, 50.0f, 1);
	at = vec4(0.0f, 0.0f, -1.0f, 1);
	up = vec4(0.0f, 1.0f, 0.0f, 0);

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearColor(0.5, 0.5, 0.5, 1.0);

}
////////////////////////////////////////////
void show_robot()
{
	glUseProgram(program);
	glBindVertexArray(vao[0]);
	model_view = Translate(runX * sin(DegreesToRadians) + runZ, 0, runX * cos(DegreesToRadians))
		* Translate(0.0, increase + UPPER_LEG_HEIGHT + LOWER_LEG_HEIGHT, 0.0)*RotateY(theta[Torso]);//���ɱ任����pAngle*pAngle*
	torso();//���ɻ���
	mvstack.push(model_view);//�������ɱ任����
	model_view *= (Translate(0.0, TORSO_HEIGHT, 0.0) *RotateY(theta[Head]));
	head();//ͷ������
	model_view = mvstack.pop();//�ָ����ɱ任����
	glBindVertexArray(vao[0]);
	mvstack.push(model_view); //�������ɱ任����
							  //�����ϱ۵ı任����(ע��˴�������RotateX�������ı�theta[LeftUpperArm]��ı��ϱ۵���ת�Ƕȣ���X��Ϊ��ת��)
	model_view *= (Translate(0.5*(TORSO_WIDTH + UPPER_ARM_WIDTH), 0.9 * TORSO_HEIGHT, 0.0) *RotateX(theta[LeftUpperArm])*RotateZ(180));
	left_upper_arm();//���ϱۻ���
	model_view *= (Translate(0.0, UPPER_ARM_HEIGHT, 0.0) *RotateX(theta[LeftLowerArm]));
	left_lower_arm();//���±ۻ���
	model_view = mvstack.pop();//�ָ����ɱ任����
	mvstack.push(model_view); //�������ɱ任����
	model_view *= (Translate(-0.5*(TORSO_WIDTH + UPPER_ARM_WIDTH), 0.9 * TORSO_HEIGHT, 0.0) *RotateX(theta[RightUpperArm])*RotateZ(180));
	right_upper_arm();//���ϱۻ���
	model_view *= (Translate(0.0, UPPER_ARM_HEIGHT, 0.0) *RotateX(theta[RightLowerArm]));
	right_lower_arm();//���±ۻ���
	model_view = mvstack.pop();//�ָ����ɱ任����
	mvstack.push(model_view); //�������ɱ任����
	model_view *= (Translate(TORSO_HEIGHT / 6, 0, 0.0) *RotateX(theta[LeftUpperLeg])*RotateZ(180));
	left_upper_leg();//�����Ȼ���
	model_view *= (Translate(0.0, UPPER_LEG_HEIGHT, 0.0) *RotateX(theta[LeftLowerLeg]));
	left_lower_leg();//�����Ȼ���
	model_view = mvstack.pop();//�ָ����ɱ任����
	mvstack.push(model_view); //�������ɱ任����
	model_view *= (Translate(-TORSO_HEIGHT / 6, 0, 0.0) *RotateX(theta[RightUpperLeg])*RotateZ(180));
	right_upper_leg();//�����Ȼ���
	model_view *= (Translate(0.0, UPPER_LEG_HEIGHT, 0.0) *RotateX(theta[RightLowerLeg]));
	right_lower_leg();//�����Ȼ���
	model_view = mvstack.pop();//�ָ����ɱ任����
}

/////////////////////////////////////////////
void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ��Դλ��
	float lx = lightPos[0];
	float ly = lightPos[1];
	float lz = lightPos[2];
	GLfloat shadowface[] = { 0,1,0,-1 };//shadowface��ʾ��Ӱ����ҪͶӰ��ƽ��
	shadow = mat4(
		vec4(-ly*shadowface[1] - lz*shadowface[2], lx*shadowface[1], lx*shadowface[2], lx*shadowface[3]),
		vec4(ly*shadowface[0], -lx*shadowface[0] - lz*shadowface[2], ly*shadowface[2], ly*shadowface[3]),
		vec4(lz*shadowface[0], lz*shadowface[1], -lx*shadowface[0] - ly*shadowface[1], lx*shadowface[3]),
		vec4(0, 0, 0, -lx*shadowface[0] - ly*shadowface[1] - lz*shadowface[2]));

	Camera::modelMatrix = mat4(1.0);
	Camera::viewMatrix = Camera::lookAt(eye, eye + at, up);//���������
	Camera::projMatrix = Camera::perspective(fov, aspect, zN, zF);//����͸��ͶӰ
	glUniformMatrix4fv(Projection, 1, GL_TRUE, &Camera::projMatrix[0][0]);	//����͸��ͶӰ����
	glUniform3fv(lightPosID, 1, &lightPos[0]);	// ����Դ��Ϣ���붥����ɫ��
	ModelViewMatrix = Camera::viewMatrix * Camera::modelMatrix;
	mp_->setCameraMatrix(Camera::modelMatrix, Camera::viewMatrix, Camera::projMatrix);
	mp_->draw_meshes();//��ͼ��
	glUseProgram(program);
	show_robot();	// ���ƻ�����

	glutSwapBuffers();
};

//----------------------------------------------------------------------------
void showTip() {
	cout << "*********************************************" << endl;
}


void
mouse(int button, int state, int x, int y)
{
	//�������Ƿ��µ�״̬������¼���������
	mouseX = x;
	mouseY = y;
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mouseLeftDown = true;
		}
		else if (state == GLUT_UP)
			mouseLeftDown = false;
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mouseRightDown = true;
		}
		else if (state == GLUT_UP)
			mouseRightDown = false;
	}
	glutPostRedisplay();
}
//�Ƕ�ת����
float radians(GLfloat x) {
	return x*M_PI / 180;
}

void mousepassmove(int x, int y) {
	if (cameramouse) {
		//�ƶ�
		if (firstMouse)
		{
			lastX = x;
			lastY = y;
			firstMouse = false;
		}
		x = x * 30;
		y = y * 30;
		GLfloat xoffset = x - lastX;
		GLfloat yoffset = lastY - y;
		lastX = x;
		lastY = y;
		GLfloat sensitivity = 0.05;
		xoffset *= sensitivity;
		yoffset *= sensitivity;
		yaw += xoffset;
		pitch += yoffset;
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
		vec3 front;
		front.x = cos(radians(yaw)) * cos(radians(pitch));// 
		front.y = sin(radians(pitch));
		front.z = sin(radians(yaw)) * cos(radians(pitch));
		at = normalize(front);//��������۲���Դﵽ��������۲�ת����Ŀ��
		glutPostRedisplay();
	}
}

void mouseControllight(int x, int y)
{
	if (mouseLeftDown)
	{
		cameraAngleY += (x - mouseX);
		cameraAngleX += (y - mouseY);
		if (y>mouseY)
			lightPos += vec3(0, (mouseY - y) / 50, 0);
		else if (y < mouseY)
			lightPos += vec3(0, (mouseY - y) / 50, 0);
		if (x>mouseX)
			lightPos += vec3((x - mouseX) / 50, 0, 0);
		else if (x<mouseX)
			lightPos += vec3((x - mouseX) / 50, 0, 0);
		mouseX = x;
		mouseY = y;
	}
	else if (mouseRightDown)
	{
		cameraDistance += (y - mouseY) * 0.2f;
		mouseY = y;
	}
	if (lightPos[1] <= 1)
		lightPos[1] = 1;
	glutPostRedisplay();
}

//----------------------------------------------------------------------------
void
idle(void)
{
	glutPostRedisplay();
}

//----------------------------------------------------------------------------
// ���ƻ��������Ƶĺ���
void robotChangeGesture() {
	if (runGesture == 1) {
		theta[LeftUpperArm] = 40;
		theta[LeftLowerArm] = 115;
		theta[RightUpperArm] = 340;
		theta[RightLowerArm] = 105;
		theta[RightUpperLeg] = 20;
		theta[RightLowerLeg] = 265;
		theta[LeftUpperLeg] = 340;
		theta[LeftLowerLeg] = 0.0;
		runGesture = 0;
	}
	else {
		theta[LeftUpperArm] = 340;
		theta[LeftLowerArm] = 105;
		theta[RightUpperArm] = 40;
		theta[RightLowerArm] = 115;
		theta[RightUpperLeg] = 340;
		theta[RightLowerLeg] = 0;
		theta[LeftUpperLeg] = 20;
		theta[LeftLowerLeg] = 265;
		runGesture = 1;
	}
}



void keyboard(unsigned char key, int mousex, int mousey)
{
	GLfloat cameraSpeed = 1.0f;
	vec4 n = normalize(at - eye);
	vec4 u = normalize(vec4(cross(n, up), 0.0));
	vec4 v = normalize(vec4(cross(u, n), 0.0));
	switch (key)
	{
		// ��������任
	case 'd':
		eye += normalize(cross(at, up)) * cameraSpeed;
		break;
	case 'a':
		eye -= normalize(cross(at, up)) * cameraSpeed;
		break;
	case 'w': tAngle += 5;
		eye += at*cameraSpeed;
		break;
	case 's':
		eye -= at*cameraSpeed;
		break;
	case 'c':
		eye.y += cameraSpeed;
		break;
	case 'C':
		eye.y -= cameraSpeed;
		break;
	case 'x': fov += 5;
		break;
	case 'X': fov -= 5;
		break;

		// ���ƻ������ƶ�
	case 'j':// ����
		robotChangeGesture();	// �л�����
		runZ -= stepSize;	// �߳�һ��������ΪstepSize
		theta[Torso] = 180.0 + 90.0;// ����������ת��ǰ���ķ���
		break;
	case 'l':	// ����
		robotChangeGesture();
		runZ += stepSize;
		theta[Torso] = 180.0 - 90.0;
		break;
	case 'i':	// ǰ��
		robotChangeGesture();
		runX -= stepSize;
		theta[Torso] = 180.0;
		break;
	case 'k':	// ����
		robotChangeGesture();
		runX += stepSize;
		theta[Torso] = 0;
		break;

	case 'f':
		theta[angle] += 2;
		break;

		// ��������������
	case ' ':
		theta[Torso] = 0;
		theta[Head] = 0;
		theta[LeftUpperArm] = 0;
		theta[LeftLowerArm] = 0;
		theta[RightUpperArm] = 0;
		theta[RightLowerArm] = 0;
		theta[RightUpperLeg] = 0;
		theta[RightLowerLeg] = 0;
		theta[LeftUpperLeg] = 0;
		theta[LeftLowerLeg] = 0;
		break;

		// ������Ϣ����
	case 'r':
		// camera
		pAngle = 0.0;

		// perspective
		fov = 100.0;
		aspect = 1.0;
		zN = 0.1;
		zF = 100.0;
		theta[Torso] = 0.0;
		runX = 0;
		runZ = 0;
		lightPos = vec3(5, 5, 10);

		// ����������
		theta[Torso] = 0;
		theta[Head] = 0;
		theta[LeftUpperArm] = 0;
		theta[LeftLowerArm] = 0;
		theta[RightUpperArm] = 0;
		theta[RightLowerArm] = 0;
		theta[RightUpperLeg] = 0;
		theta[RightLowerLeg] = 0;
		theta[LeftUpperLeg] = 0;
		theta[LeftLowerLeg] = 0;
		break;

	default:
		break;
	}
	glutPostRedisplay();

}

//----------------------------------------------------------------------------

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1200, 800);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	int window = glutCreateWindow("2017152032_��־��_��ĩ����ҵ");
	glewExperimental = GL_TRUE;
	glewInit();

	//������պ�
	mp_ = new Mesh_Painter;
	// ����ǽ�ڲ���������
	float wall_height = -50; // ����ǽ��y���ϸ߶ȵ�ƫ��
	My_Mesh* my_mesh5 = new My_Mesh;
	my_mesh5->generate_wallbk(4);
	my_mesh5->set_texture_file("skybox/rainforest_bk.tga");//back
	my_mesh5->set_translate(0, wall_height, -24.25);//ƽ��
	my_mesh5->set_theta(0, 0, 0);//��ת��
	my_mesh5->set_theta_step(0.0, 0.0, 0.0);//��ת�ٶ�
	my_meshs.push_back(my_mesh5);
	mp_->add_mesh(my_mesh5);

	My_Mesh* my_mesh6 = new My_Mesh;
	my_mesh6->generate_walllf(4);
	my_mesh6->set_texture_file("skybox/rainforest_lf.tga");//left
	my_mesh6->set_translate(-24.27, wall_height, 0);
	my_mesh6->set_theta(0, 0, 0);
	my_mesh6->set_theta_step(0.0, 0.0, 0.0);
	my_meshs.push_back(my_mesh6);
	mp_->add_mesh(my_mesh6);

	My_Mesh* my_mesh7 = new My_Mesh;
	my_mesh7->generate_wallft(4);
	my_mesh7->set_texture_file("skybox/rainforest_ft.tga");//front
	my_mesh7->set_translate(0, wall_height, 24.25);
	my_mesh7->set_theta(0, 0, 0);
	my_mesh7->set_theta_step(0.0, 0.0, 0.0);
	my_meshs.push_back(my_mesh7);
	mp_->add_mesh(my_mesh7);

	My_Mesh* my_mesh8 = new My_Mesh;
	my_mesh8->generate_wallrt(4);
	my_mesh8->set_texture_file("skybox/rainforest_rt.tga");//right
	my_mesh8->set_translate(24.0, wall_height, 0);
	my_mesh8->set_theta(0, 0, 0);
	my_mesh8->set_theta_step(0.0, 0.0, 0.0);
	my_meshs.push_back(my_mesh8);
	mp_->add_mesh(my_mesh8);

	// ���ɵ���
	My_Mesh* my_mesh4 = new My_Mesh;
	my_mesh4->generate_floor(4);	// ����ƽ����Ҫ�ĵ㡢�桢����
	my_mesh4->set_texture_file("skybox/rainforest_dn.tga");//ָ������ͼ���ļ�,down
	my_mesh4->set_translate(0, 0, 0);	// ƽ��
	my_mesh4->set_theta(0, 0, 0);	// ��ת��
	my_mesh4->set_theta_step(0.0, 0.0, 0.0);	// ��ת�ٶ� 
	my_meshs.push_back(my_mesh4);
	mp_->add_mesh(my_mesh4);


	//�����컨��
	My_Mesh* my_mesh9 = new My_Mesh;
	my_mesh9->generate_floor(4);
	my_mesh9->set_texture_file("skybox/rainforest_up.tga");//up
	my_mesh9->set_translate(0, 116., 0);  //116  118
	my_mesh9->set_theta(0, 0, 0);
	my_mesh9->set_theta_step(0.0, 0.0, 0.0);
	my_meshs.push_back(my_mesh9);
	mp_->add_mesh(my_mesh9);

	mp_->init_shaders("v_texture.glsl", "f_texture.glsl");
	mp_->update_vertex_buffer();
	mp_->update_texture();

	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);

	glutSetCursor(GLUT_CURSOR_CROSSHAIR);
	glutMotionFunc(mouseControllight);
	glutPassiveMotionFunc(mousepassmove);
	glutMainLoop();

	// �ͷ��ڴ�ռ�
	for (unsigned int i = 0; i < my_meshs.size(); i++)
	{
		delete my_meshs[i];
	}
	delete mp_;

	return 0;
}