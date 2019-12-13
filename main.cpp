
#include "Angel.h"
#include <assert.h>
#include <iostream>
using namespace std;
#pragma comment(lib, "glew32.lib")
typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
color4 colors[NumVertices];

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

// RGBA olors
color4 vertex_colors[8] = {
	color4(0.0, 0.0, 0.0, 1.0),  // black
	color4(0.0, 0.0, 0.0, 1.0),  // red
	color4(0.0, 0.0, 0.0, 1.0),  // yellow
	color4(0.0, 0.0, 0.0, 1.0),  // green
	color4(0.0, 0.0, 0.0, 1.0),  // blue
	color4(0.0, 0.0, 0.0, 1.0),  // magenta
	color4(0.0, 0.0, 0.0, 1.0),  // white
	color4(0.0, 0.0, 0.0, 1.0)   // cyan
};
point4 color_torso = point4(0, 0, 1, 1);
//----------------------------------------------------------------------------

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

#define TORSO_HEIGHT 5.0
#define TORSO_WIDTH 3.0
#define UPPER_ARM_HEIGHT 3.0
#define LOWER_ARM_HEIGHT 2.0
#define UPPER_LEG_WIDTH  0.5
#define LOWER_LEG_WIDTH  0.5
#define LOWER_LEG_HEIGHT 2.0
#define UPPER_LEG_HEIGHT 3.0
#define UPPER_LEG_WIDTH  0.5
#define UPPER_ARM_WIDTH  0.5
#define LOWER_ARM_WIDTH  0.5
#define HEAD_HEIGHT 1.5
#define HEAD_WIDTH 1.0

// Set up menu item indices, which we can alos use with the joint angles
enum {
	Torso,
	Head1,
	Head2,
	RightUpperArm,
	RightLowerArm,
	LeftUpperArm,
	LeftLowerArm,
	RightUpperLeg,
	RightLowerLeg,
	LeftUpperLeg,
	LeftLowerLeg,
	NumJointAngles,
	Quit
};

// Joint angles with initial values
GLfloat
theta[NumJointAngles] = {
	60.0,    // Torso
	0.0,    // Head1
	0.0,    // Head2
	30.0,    // RightUpperArm
	-105.0,    // RightLowerArm
	-30.0,    // LeftUpperArm
	-105.0,    // LeftLowerArm
	150.0,  // RightUpperLeg
	0.0,    // RightLowerLeg
	210.0,  // LeftUpperLeg
	0.0     // LeftLowerLeg
};

GLint angle = Head2;

//----------------------------------------------------------------------------

int Index = 0;

void
quad(int a, int b, int c, int d)
{
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; Index++;
}

void
colorcube(void)//这里生成单位立方体的六个表面
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}

//----------------------------------------------------------------------------

void
torso()
{
	mvstack.push(model_view);//保存父节点矩阵

	mat4 instance = (Translate(0.0, 0.5 * TORSO_HEIGHT, 0.0) *
		Scale(TORSO_WIDTH, TORSO_HEIGHT, TORSO_WIDTH));//本节点局部变换矩阵

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, color_torso);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	//glUniform4fv(draw_color, 1, color_torso);
	model_view = mvstack.pop();//恢复父节点矩阵
}

void
head()
{
	//mvstack.push(model_view);//保存父节点矩阵

	mat4 instance = (Translate(0.0, 0.5 * HEAD_HEIGHT, 0.0) *
		Scale(HEAD_WIDTH, HEAD_HEIGHT, HEAD_WIDTH));//本节点局部变换矩阵

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, vertex_colors[3]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	//glUniform4fv(draw_color, 1, vertex_colors[1]);
	//model_view = mvstack.pop();//恢复父节点矩阵
}

void
left_upper_arm()
{
	//mvstack.push(model_view);//保存父节点矩阵

	mat4 instance = (Translate(0.0, -0.5 * UPPER_ARM_HEIGHT, 0.0) *
		Scale(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH));//本节点局部变换矩阵

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, vertex_colors[2]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	//glUniform4fv(draw_color, 1, vertex_colors[2]);
	//model_view = mvstack.pop();//恢复父节点矩阵
}

void
left_lower_arm()
{
	//mvstack.push(model_view);//保存父节点矩阵

	mat4 instance = (Translate(0.0, -0.5 * LOWER_ARM_HEIGHT, 0.0) *
		Scale(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH));//本节点局部变换矩阵

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, vertex_colors[1]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	//glUniform4fv(draw_color, 1, vertex_colors[1]);
	//model_view = mvstack.pop();//恢复父节点矩阵
}

void
right_upper_arm()
{
	//mvstack.push(model_view);//保存父节点矩阵

	mat4 instance = (Translate(0.0, -0.5 * UPPER_ARM_HEIGHT, 0.0) *
		Scale(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH));//本节点局部变换矩阵

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, vertex_colors[2]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	//glUniform4fv(draw_color, 1, vertex_colors[2]);
	//model_view = mvstack.pop();//恢复父节点矩阵
}

void
right_lower_arm()
{
	//mvstack.push(model_view);//保存父节点矩阵

	mat4 instance = (Translate(0.0, -0.5 * LOWER_ARM_HEIGHT, 0) *
		Scale(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH));//本节点局部变换矩阵

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, vertex_colors[1]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	//glUniform4fv(draw_color, 1, vertex_colors[1]);
	//model_view = mvstack.pop();//恢复父节点矩阵
}

void
left_upper_leg()
{
	mat4 instance = (Translate(0.0, 0.5 * UPPER_LEG_HEIGHT, 0.0) *
		Scale(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH));//本节点局部变换矩阵

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, vertex_colors[5]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniform4fv(draw_color, 1, vertex_colors[2]);
	//model_view = mvstack.pop();//恢复父节点矩阵
}

void
left_lower_leg()
{
	mat4 instance = (Translate(0.0, 0.5 * LOWER_LEG_HEIGHT, 0.0) *
		Scale(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH));//本节点局部变换矩阵

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, vertex_colors[7]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	//model_view = mvstack.pop();//恢复父节点矩阵
}

void
right_upper_leg()
{
	mat4 instance = (Translate(0.0, 0.5 * UPPER_LEG_HEIGHT, 0.0) *
		Scale(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH));//本节点局部变换矩阵

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, vertex_colors[5]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
}

void
right_lower_leg()
{
	mat4 instance = (Translate(0.0, 0.5 * LOWER_LEG_HEIGHT, 0.0) *
		Scale(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH));//本节点局部变换矩阵

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, vertex_colors[7]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	//glUniform4fv(draw_color, 1, vertex_colors[2]);
}

//----------------------------------------------------------------------------

/*
Torso,
Head1,
Head2,
RightUpperArm,
RightLowerArm,
LeftUpperArm,
LeftLowerArm,
RightUpperLeg,
RightLowerLeg,
LeftUpperLeg,
LeftLowerLeg,
NumJointAngles,
Quit
*/


void
display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	model_view = RotateY(theta[Torso]);//躯干变换矩阵
	torso();//躯干绘制

	mvstack.push(model_view);//保存躯干变换矩阵
	model_view *= (Translate(0.0, TORSO_HEIGHT, 0.0) * RotateY(theta[Head1]));
	head();//头部绘制
	model_view = mvstack.pop();//恢复躯干变换矩阵

	mvstack.push(model_view); //保存躯干变换矩阵
	model_view *= (Translate(-0.5 * (TORSO_WIDTH + UPPER_ARM_WIDTH), TORSO_HEIGHT, 0.0) * RotateX(theta[LeftUpperArm]));
	left_upper_arm();//左上臂绘制

	model_view *= (Translate(0.0, -UPPER_ARM_HEIGHT, 0.0) * RotateX(theta[LeftLowerArm]));
	left_lower_arm();//左下臂绘制
	model_view = mvstack.pop();//恢复躯干变换矩阵


	mvstack.push(model_view); //保存躯干变换矩阵
	model_view *= (Translate(0.5 * (TORSO_WIDTH + UPPER_ARM_WIDTH), TORSO_HEIGHT, 0.0) * RotateX(theta[RightUpperArm]));
	right_upper_arm();//右上臂绘制
	model_view *= (Translate(0.0, -UPPER_ARM_HEIGHT, 0.0) * RotateX(theta[RightLowerArm]));
	right_lower_arm();//右下臂绘制
	model_view = mvstack.pop();//恢复躯干变换矩阵


	mvstack.push(model_view); //保存躯干变换矩阵
	model_view *= (Translate(-0.5 * (TORSO_WIDTH + UPPER_LEG_WIDTH), 0.0, 0.0) * RotateX(theta[LeftUpperLeg]));
	left_upper_leg();//左上腿绘制
	model_view *= (Translate(0.0, UPPER_LEG_HEIGHT, 0.0) * RotateX(theta[LeftLowerLeg]));
	left_lower_leg();//左下腿绘制
	model_view = mvstack.pop();//恢复躯干变换矩阵


	mvstack.push(model_view); //保存躯干变换矩阵
	model_view *= (Translate(0.5 * (TORSO_WIDTH + UPPER_LEG_WIDTH), 0.0, 0.0) * RotateX(theta[RightUpperLeg]));
	right_upper_leg();//右上腿绘制
	model_view *= (Translate(0.0, UPPER_LEG_HEIGHT, 0.0) * RotateX(theta[RightLowerLeg]));
	right_lower_leg();//右下腿绘制
	model_view = mvstack.pop();//恢复躯干变换矩阵

	glutSwapBuffers();
}

//-----------------------------------------------------------------------------
int times1 = 0;
int times2 = 0;
bool flag = true;
void idle(void)
{
	if (flag && times1++ == 30) {
		theta[LeftUpperArm] += 5;
		//theta[LeftLowerArm] += 3;
		theta[RightUpperArm] -= 5;
		//theta[RightLowerArm] -= 3;
		theta[LeftUpperLeg] -= 5;
		theta[RightUpperLeg] += 5;
		if (theta[LeftUpperArm] < 1.0)
			theta[LeftLowerLeg] += 15;
		else
			theta[LeftLowerLeg] -= 15;
		times1 = 0;
		if (theta[LeftUpperArm] > 25.0) {
			flag = false;
		}
	}


	if (!flag && times2++ == 30) {
		theta[LeftUpperArm] -= 5;
		//theta[LeftLowerArm] -= 3;
		theta[RightUpperArm] += 5;
		//theta[RightLowerArm] += 3;
		theta[LeftUpperLeg] += 5;
		theta[RightUpperLeg] -= 5;

		if (theta[RightUpperArm] < 1.0)
			theta[RightLowerLeg] += 15;
		else
			theta[RightLowerLeg] -= 15;
		times2 = 0;
		if (theta[RightUpperArm] > 25.0) {
			flag = true;
		}
	}

	glutPostRedisplay();
}

//----------------------------------------------------------------------------

void
mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		theta[angle] += 5.0;
		if (theta[angle] > 360.0) { theta[angle] -= 360.0; }
	}

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		theta[angle] -= 5.0;
		if (theta[angle] < 0.0) { theta[angle] += 360.0; }
	}

	glutPostRedisplay();
}

//----------------------------------------------------------------------------

void
menu(int option)
{
	if (option == Quit) {
		exit(EXIT_SUCCESS);
	}

	angle = option;
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

	model_view = mat4(1.0);   // An Identity matrix
}

//----------------------------------------------------------------------------

void
init(void)
{
	colorcube();

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
		NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors),
		colors);

	// Load shaders and use the resulting shader program
	GLuint program = InitShader("vshader82.glsl", "fshader82.glsl");
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
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClearColor(1.0, 1.0, 1.0, 1.0);

}

//----------------------------------------------------------------------------

void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033: // Escape Key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	}
}

//----------------------------------------------------------------------------

int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("robot");
	glewExperimental = GL_TRUE;
	glewInit();

	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);

	glutCreateMenu(menu);
	glutAddMenuEntry("torso", Torso);
	glutAddMenuEntry("head1", Head1);
	glutAddMenuEntry("head2", Head2);
	glutAddMenuEntry("right_upper_arm", RightUpperArm);
	glutAddMenuEntry("right_lower_arm", RightLowerArm);
	glutAddMenuEntry("left_upper_arm", LeftUpperArm);
	glutAddMenuEntry("left_lower_arm", LeftLowerArm);
	glutAddMenuEntry("right_upper_leg", RightUpperLeg);
	glutAddMenuEntry("right_lower_leg", RightLowerLeg);
	glutAddMenuEntry("left_upper_leg", LeftUpperLeg);
	glutAddMenuEntry("left_lower_leg", LeftLowerLeg);
	glutAddMenuEntry("quit", Quit);
	glutAttachMenu(GLUT_MIDDLE_BUTTON);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}
