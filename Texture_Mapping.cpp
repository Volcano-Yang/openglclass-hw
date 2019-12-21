//绘制机器人和天空盒

#include "Angel.h"
#include "mesh.h"
#include "FreeImage.h"
#include "Mesh_Painter.h"
#include <assert.h>
#include <iostream>
#include <cmath>
#include <GL/glew.h>
#include <glfw3.h>
#include <gl/glut.h>
#include "Texture_Mapping.h"
#include <windows.h>
using namespace std;

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "FreeImage.lib")
typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

std::vector<My_Mesh*> my_meshs;
Mesh_Painter*			mp_;
// Texture objects and storage for texture image

// program 和 vao
GLuint program;
GLuint vao[2];

// 拖动鼠标控制光源位置的参数
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

// perspective 参数
float fov = 100.0;
float aspect = 1.0;
float zN = 0.1;
float zF = 100.0;

//相机参数
vec4 eye;
vec4 at;
vec4 up;


bool firstMouse = true;
GLfloat lastX = 1300 / 2.0;
GLfloat lastY = 800 / 2.0;
GLfloat yaw = -90.0f;
GLfloat pitch = 0.0f;

// 控制视角的角度初始值
float rad = 50.0;
float tAngle = 25.0;
float pAngle = 0.0;

// 光源
vec3 lightPos(21.92,24.78,10);
GLuint lightPosID;

GLuint sflag;	// 用于传入片元着色器，以控制阴影部分不添加漫反射、镜面反射和环境光 

//控制能量
int energy = 0;

int energy2 = 0;

//天空盒贴图
string pictrue1[6] = { "skybox/rainforest_bk.tga", "skybox/rainforest_lf.tga","skybox/rainforest_ft.tga", "skybox/rainforest_rt.tga","skybox/rainforest_dn.tga","skybox/rainforest_up.tga" };

string pictrue2[6] = { "skybox/rainforest2_bk.tga", "skybox/rainforest2_lf.tga","skybox/rainforest2_ft.tga", "skybox/rainforest2_rt.tga","skybox/rainforest2_dn.tga","skybox/rainforest2_up.tga" };

string pictrue5[6] = { "skybox/hills2_bk.tga", "skybox/hills2_lf.tga","skybox/hills2_ft.tga", "skybox/hills2_rt.tga","skybox/hills2_dn.tga","skybox/hills2_up.tga" };

string pictrue4[6] = { "skybox/hills_bk.tga", "skybox/hills_lf.tga","skybox/hills_ft.tga", "skybox/hills_rt.tga","skybox/hills_dn.tga","skybox/hills_up.tga" };

string pictrue3[6] = { "skybox/plains-of-abraham_bk.tga", "skybox/plains-of-abraham_lf.tga","skybox/plains-of-abraham_ft.tga", "skybox/plains-of-abraham_rt.tga","skybox/plains-of-abraham_dn.tga","skyboxplains-of-abraham_up.tga" };

string pictrue6[6] = { "skybox/island_bk.tga", "skybox/island_lf.tga","skybox/island_ft.tga", "skybox/island_rt.tga","skybox/island_dn.tga","skybox/island_up.tga" };


namespace Camera
{
	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 projMatrix;

	// 平行投影
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

	// 透视投影，近大远小
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

	// 相机观察函数
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

const int NumVertices = 36; // 每个部件6个面，每个面包含2个三角面片，每个三角面片包含3个顶点
point4 points[NumVertices];	// 点坐标
color4 colors[NumVertices];	// 点颜色
point4 nomal[NumVertices];	// 法向量
							// 点坐标
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

// 颜色
color4 vertex_colors[8] = {
	color4(0.0, 0.0, 0.0, 1.0),  // 黑色，0
	color4(1.0, 0.0, 0.0, 1.0),  // 红色，1,
	color4(1.0, 1.0, 0.0, 1.0),  // 黄色，2
	color4(0.3, 1.0, 0.3, 1.0),  // 绿色，3
	color4(0.0, 0.0, 1.0, 1.0),  // 蓝色，4
	color4(1.0, 0.0, 1.0, 1.0),  // 品红色，5
	color4(1.0, 1.0, 1.0, 1.0),  // 白色，6
	color4(0.0, 1.0, 1.0, 1.0)   // 青色，7
};

//----------------------------------------------------------------------------
//矩阵栈
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

MatrixStack  mvstack; //储存机器人的矩阵
mat4         model_view;
GLuint       ModelView, Projection;
GLuint       draw_color;
//----------------------------------------------------------------------------


// 控制机器人行走的参数
int runGesture = 1;	// 控制切换行走姿势
int runGesture2 = 1;	// 控制切换行走姿势
float runX = 10, runY = 0, runZ = -6;	// 控制机器人1移动的坐标
float runX2 = 10, runY2 = 0, runZ2 = 6;	// 控制机器人2移动的坐标
int cameramouse = 0;	// 控制切换第一视角行走模式
int movetorso = 0;	// 控制切换第一视角行走模式
int stepSize = 1;	// 控制机器人1每一步的步长
int stepSize2 = 4;	// 控制机器人2每一步的步长
float increase = 1.0;	// 绘制机器人时的位置偏移值

float L = 0.2;
int endtime = 2;
int start_time = 0;


// 给机器人1的各个部位设置颜色
point4 color_head = vertex_colors[6];
point4 color_torso = vertex_colors[0];

point4 color_upper_arm = vertex_colors[6];
point4 color_lower_arm = vertex_colors[0];

point4 color_upper_leg = vertex_colors[6];
point4 color_lower_leg = vertex_colors[0];

point4 color_cube = vertex_colors[2];

// 给机器人2的各个部位设置颜色
point4 color_head2 = vertex_colors[6];
point4 color_torso2 = vertex_colors[4];

point4 color_upper_arm2 = vertex_colors[6];
point4 color_lower_arm2 = vertex_colors[4];

point4 color_upper_leg2 = vertex_colors[6];
point4 color_lower_leg2 = vertex_colors[4];

point4 color_cube2 = vertex_colors[2];


// 定义机器人1各个部位的大小

float HEAD_HEIGHT = 2.5;
float HEAD_WIDTH = 2.5;

float TORSO_HEIGHT = 5.5;
float TORSO_WIDTH = 4.0;

float UPPER_ARM_HEIGHT = 2.0;
float UPPER_ARM_WIDTH = 1;

float LOWER_ARM_HEIGHT = 1.5;
float LOWER_ARM_WIDTH = 1;

float UPPER_LEG_WIDTH  =1.5;
float UPPER_LEG_HEIGHT =1;

float LOWER_LEG_HEIGHT = 2;
float LOWER_LEG_WIDTH  = 1.5;

float CUBE_HEIGHT = 8;
float CUBE_WIDTH = 8;

// 定义机器人2各个部位的大小

float HEAD_HEIGHT2 = 2.5*0.5;
float HEAD_WIDTH2 = 2.5*0.5;

float TORSO_HEIGHT2 = 5.5*0.5;
float TORSO_WIDTH2 = 4.0*0.5;

float UPPER_ARM_HEIGHT2 = 2.0*0.5;
float UPPER_ARM_WIDTH2 = 1 * 0.5;

float LOWER_ARM_HEIGHT2 = 1.5*0.5;
float LOWER_ARM_WIDTH2 = 1 * 0.5;

float UPPER_LEG_WIDTH2 = 1.5*0.5;
float UPPER_LEG_HEIGHT2 = 1 * 0.5;

float LOWER_LEG_HEIGHT2 = 2 * 0.5;
float LOWER_LEG_WIDTH2 = 1.5*0.5;

float CUBE_HEIGHT2 = 8 * 0.5;
float CUBE_WIDTH2 = 8 * 0.5;


// 建立机器人部件项索引，以便使用关节角度
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

// 初始化关节角度
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

// 初始化关节角度
GLfloat
theta2[NumJointAngles] = {
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
int Index = 0;


void
quad(int a, int b, int c, int d)
{
	vec3 nomalab = vec3(vertices[a].x - vertices[b].x, vertices[a].y - vertices[b].y, vertices[a].z - vertices[b].z);
	vec3 nomalbc = vec3(vertices[b].x - vertices[c].x, vertices[b].y - vertices[c].y, vertices[b].z - vertices[c].z);
	vec3 nomalcd = vec3(vertices[c].x - vertices[d].x, vertices[c].y - vertices[d].y, vertices[c].z - vertices[d].z);
	vec3 nomalabc = cross(nomalab, nomalbc);//计算绘制物体的各个面的法向量

	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; nomal[Index] = nomalabc;

	Index++;

	colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; nomal[Index] = nomalabc; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; nomal[Index] = nomalabc; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; nomal[Index] = nomalabc; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; nomal[Index] = nomalabc; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; nomal[Index] = nomalabc; Index++;
}

void
colorcube(void)//这里生成单位立方体的六个表面,绘制颜色和计算法向量
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}

//----------------------------------------------------------------------------
// 绘制机器人阴影
void add_robot_shadow()
{
	//将阴影矩阵传入着色器
	glUniformMatrix4fv(shadowMatrixID, 1, GL_TRUE, &shadow[0][0]);
	//设置颜色
	glUniform4fv(draw_color, 1, vertex_colors[0]);
	// 标记其为阴影
	glUniform1i(sflag, 1);
	//绘制
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
}

// 初始化阴影矩阵
void init_shadowMatrix()
{
	mat4 shadowMatrix = mat4(1.0);
	glUniformMatrix4fv(shadowMatrixID, 1, GL_TRUE, &shadowMatrix[0][0]);
	glUniformMatrix4fv(ModelViewMatrixID, 1, GL_TRUE, &ModelViewMatrix[0][0]);
}

void
cube()
{
	init_shadowMatrix();
	mvstack.push(model_view);// 保存父节点矩阵
	mat4 instance = (Translate(0.0, 0.5 * CUBE_HEIGHT, 0.0) *
		Scale(CUBE_WIDTH, CUBE_HEIGHT, CUBE_WIDTH/2));// 本节点局部变换矩阵
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);// 父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, color_cube);
	glUniform1i(sflag, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	add_robot_shadow();
	model_view = mvstack.pop();// 恢复父节点矩阵
}

void
torso(float w,float h, point4 color)
{
	init_shadowMatrix();

	mvstack.push(model_view);// 保存父节点矩阵

	mat4 instance = (Translate(0.0, 0.5 * h, 0.0) *
		Scale(w, h, w / 2));// 本节点局部变换矩阵

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);// 父节点矩阵 * 本节点局部变换矩阵
	glUniform4fv(draw_color, 1, color);

	glUniform1i(sflag, 0);	// 标记其不为阴影（下同）
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	add_robot_shadow(); // 为此部分绘制阴影（下同）

	model_view = mvstack.pop();// 恢复父节点矩阵
}

void
head(float w, float h, point4 color)
{
	init_shadowMatrix();

	mvstack.push(model_view);// 保存父节点矩阵

	mat4 instance = (Translate(0.0, 0.5 * h, 0.0) *
		Scale(w, h, w));// 本节点局部变换矩阵
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);// 父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, color);
	glUniform1i(sflag, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	add_robot_shadow();
	model_view = mvstack.pop();// 恢复父节点矩阵
}

void
left_upper_arm(float w, float h, point4 color)
{
	init_shadowMatrix();// 初始化阴影矩阵

	mvstack.push(model_view);//保存父节点矩阵

	mat4 instance = (Translate(0.0, 0.5 * h, 0.0) *
		Scale(w + 0.3, h, w + 0.3));//本节点局部变换矩阵

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, color);
	glUniform1i(sflag, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	add_robot_shadow();

	model_view = mvstack.pop();//恢复父节点矩阵
}

void
left_lower_arm(float w, float h, point4 color)
{
	init_shadowMatrix();// 初始化阴影矩阵
	mvstack.push(model_view);//保存父节点矩阵
	mat4 instance = (Translate(0.0, 0.5 * h, 0.0) *
		Scale(w, h, w));//本节点局部变换矩阵
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, color);	// 绑定绘制颜色
	glUniform1i(sflag, 0);	 // 标记此不是阴影
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);	// 绘制
	add_robot_shadow();	// 为这部分添加阴影效果
	model_view = mvstack.pop();//恢复父节点矩阵
}

void
right_upper_arm(float w, float h, point4 color)
{
	init_shadowMatrix();// 初始化阴影矩阵

	mvstack.push(model_view);//保存父节点矩阵

	mat4 instance = (Translate(0.0, 0.5 * h, 0.0) *
		Scale(w + 0.3, h, w + 0.3));//本节点局部变换矩阵

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, color);
	glUniform1i(sflag, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	add_robot_shadow();

	model_view = mvstack.pop();//恢复父节点矩阵
}

void
right_lower_arm(float w, float h, point4 color)
{
	init_shadowMatrix();// 初始化阴影矩阵

	mvstack.push(model_view);//保存父节点矩阵

	mat4 instance = (Translate(0.0, 0.5 * h, 0.0) *
		Scale(w, h, w));//本节点局部变换矩阵

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, color);
	glUniform1i(sflag, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	add_robot_shadow();

	model_view = mvstack.pop();//恢复父节点矩阵
}

void
left_upper_leg(float w, float h, point4 color)
{
	init_shadowMatrix();// 初始化阴影矩阵
	mvstack.push(model_view);//保存父节点矩阵
	mat4 instance = (Translate(0.0, 0.5 * h, 0.0) *
		Scale(w+ 0.3, h, w + 0.3));//本节点局部变换矩阵
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, color);//传入颜色
	glUniform1i(sflag, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);//绘制
	add_robot_shadow();//添加阴影
	model_view = mvstack.pop();//恢复父节点矩阵
}

void
left_lower_leg(float w, float h, point4 color)
{
	init_shadowMatrix();// 初始化阴影矩阵
	mvstack.push(model_view);//保存父节点矩阵
	mat4 instance = (Translate(0.0, 0.5 * h, 0.0) *
		Scale(w, h, w));//本节点局部变换矩阵
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, color);
	glUniform1i(sflag, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	add_robot_shadow();
	model_view = mvstack.pop();//恢复父节点矩阵
}

void
right_upper_leg(float w, float h, point4 color)
{
	init_shadowMatrix();// 初始化阴影矩阵

	mvstack.push(model_view);//保存父节点矩阵

	mat4 instance = (Translate(0.0, 0.5 * h, 0.0) *
		Scale(w + 0.3, h, w + 0.3));//本节点局部变换矩阵

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, color);
	glUniform1i(sflag, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	add_robot_shadow();

	model_view = mvstack.pop();//恢复父节点矩阵
}

void
right_lower_leg(float w, float h, point4 color)
{
	init_shadowMatrix();// 初始化阴影矩阵

	mvstack.push(model_view);//保存父节点矩阵

	mat4 instance = (Translate(0.0, 0.5 * h, 0.0) *
		Scale(w, h, w));//本节点局部变换矩阵

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, color);
	glUniform1i(sflag, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	add_robot_shadow();

	model_view = mvstack.pop();//恢复父节点矩阵
}

void bamboo(float x, float y, float z)
{
	glUseProgram(program);
	glBindVertexArray(vao[0]);
	model_view = Translate(x,y,z);
	cube();//躯干绘制
}


void show_robot(float x,float y,float z)
{
	glUseProgram(program);
	glBindVertexArray(vao[0]);
	model_view = Translate(x * sin(DegreesToRadians) + z, 0, x* cos(DegreesToRadians))
		* Translate(0.0, increase + UPPER_LEG_HEIGHT + LOWER_LEG_HEIGHT, 0.0)*RotateY(theta[Torso]);//躯干变换矩阵pAngle*pAngle*
	torso(TORSO_WIDTH,TORSO_HEIGHT, color_torso);//躯干绘制


	mvstack.push(model_view);//保存躯干变换矩阵
	model_view *= (Translate(0.0, TORSO_HEIGHT, 0.0) *RotateY(theta[Head]));
	head(HEAD_WIDTH, HEAD_HEIGHT, color_head);//头部绘制
	model_view = mvstack.pop();//恢复躯干变换矩阵
	glBindVertexArray(vao[0]);

	mvstack.push(model_view); //保存躯干变换矩阵
							  //乘以上臂的变换矩阵(注意此处最后乘了RotateX，代表改变theta[LeftUpperArm]会改变上臂的旋转角度，以X轴为旋转轴)
	model_view *= (Translate(0.5*(TORSO_WIDTH + UPPER_ARM_WIDTH), 0.9 * TORSO_HEIGHT, 0.0) *RotateX(theta[LeftUpperArm])*RotateZ(180));
	left_upper_arm(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, color_upper_arm);//左上臂绘制
	model_view *= (Translate(0.0, UPPER_ARM_HEIGHT, 0.0) *RotateX(theta[LeftLowerArm]));
	left_lower_arm(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, color_lower_arm);//左下臂绘制
	model_view = mvstack.pop();//恢复躯干变换矩阵

	mvstack.push(model_view); //保存躯干变换矩阵
	model_view *= (Translate(-0.5*(TORSO_WIDTH + UPPER_ARM_WIDTH), 0.9 * TORSO_HEIGHT, 0.0) *RotateX(theta[RightUpperArm])*RotateZ(180));
	right_upper_arm(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, color_upper_arm);//右上臂绘制
	model_view *= (Translate(0.0, UPPER_ARM_HEIGHT, 0.0) *RotateX(theta[RightLowerArm]));
	right_lower_arm(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, color_lower_arm);//右下臂绘制
	model_view = mvstack.pop();//恢复躯干变换矩阵

	mvstack.push(model_view); //保存躯干变换矩阵
	model_view *= (Translate(TORSO_HEIGHT / 6, 0, 0.0) *RotateX(theta[LeftUpperLeg])*RotateZ(180));
	left_upper_leg(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT,color_upper_leg);//左上腿绘制
	model_view *= (Translate(0.0, UPPER_LEG_HEIGHT, 0.0) *RotateX(theta[LeftLowerLeg]));
	left_lower_leg(LOWER_LEG_WIDTH,LOWER_LEG_HEIGHT,color_lower_leg);//左下腿绘制
	model_view = mvstack.pop();//恢复躯干变换矩阵

	mvstack.push(model_view); //保存躯干变换矩阵
	model_view *= (Translate(-TORSO_HEIGHT / 6, 0, 0.0) *RotateX(theta[RightUpperLeg])*RotateZ(180));
	right_upper_leg(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, color_upper_leg);//右上腿绘制
	model_view *= (Translate(0.0, UPPER_LEG_HEIGHT, 0.0) *RotateX(theta[RightLowerLeg]));
	right_lower_leg(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, color_lower_leg);//右下腿绘制
	model_view = mvstack.pop();//恢复躯干变换矩阵
}


void show_robot2(float x, float y, float z)
{
	glUseProgram(program);
	glBindVertexArray(vao[0]);
	model_view = Translate(x * sin(DegreesToRadians) + z, 0, x* cos(DegreesToRadians))
		* Translate(0.0, increase + UPPER_LEG_HEIGHT2 + LOWER_LEG_HEIGHT2, 0.0)*RotateY(theta2[Torso]);//躯干变换矩阵pAngle*pAngle*
	torso(TORSO_WIDTH2, TORSO_HEIGHT2, color_torso2);//躯干绘制


	mvstack.push(model_view);//保存躯干变换矩阵
	model_view *= (Translate(0.0, TORSO_HEIGHT2, 0.0) *RotateY(theta2[Head]));
	head(HEAD_WIDTH2, HEAD_HEIGHT2, color_head2);//头部绘制
	model_view = mvstack.pop();//恢复躯干变换矩阵
	glBindVertexArray(vao[0]);

	mvstack.push(model_view); //保存躯干变换矩阵
							  //乘以上臂的变换矩阵(注意此处最后乘了RotateX，代表改变theta[LeftUpperArm]会改变上臂的旋转角度，以X轴为旋转轴)
	model_view *= (Translate(0.5*(TORSO_WIDTH2 + UPPER_ARM_WIDTH2), 0.9 * TORSO_HEIGHT2, 0.0) *RotateX(theta2[LeftUpperArm])*RotateZ(180));
	left_upper_arm(UPPER_ARM_WIDTH2, UPPER_ARM_HEIGHT2, color_upper_arm2);//左上臂绘制
	model_view *= (Translate(0.0, UPPER_ARM_HEIGHT2, 0.0) *RotateX(theta2[LeftLowerArm]));
	left_lower_arm(LOWER_ARM_WIDTH2, LOWER_ARM_HEIGHT2, color_lower_arm2);//左下臂绘制
	model_view = mvstack.pop();//恢复躯干变换矩阵

	mvstack.push(model_view); //保存躯干变换矩阵
	model_view *= (Translate(-0.5*(TORSO_WIDTH2 + UPPER_ARM_WIDTH2), 0.9 * TORSO_HEIGHT2, 0.0) *RotateX(theta2[RightUpperArm])*RotateZ(180));
	right_upper_arm(UPPER_ARM_WIDTH2, UPPER_ARM_HEIGHT2, color_upper_arm2);//右上臂绘制
	model_view *= (Translate(0.0, UPPER_ARM_HEIGHT2, 0.0) *RotateX(theta2[RightLowerArm]));
	right_lower_arm(LOWER_ARM_WIDTH2, LOWER_ARM_HEIGHT2, color_lower_arm2);//右下臂绘制
	model_view = mvstack.pop();//恢复躯干变换矩阵

	mvstack.push(model_view); //保存躯干变换矩阵
	model_view *= (Translate(TORSO_HEIGHT2 / 6, 0, 0.0) *RotateX(theta2[LeftUpperLeg])*RotateZ(180));
	left_upper_leg(UPPER_LEG_WIDTH2, UPPER_LEG_HEIGHT2, color_upper_leg2);//左上腿绘制
	model_view *= (Translate(0.0, UPPER_LEG_HEIGHT2, 0.0) *RotateX(theta2[LeftLowerLeg]));
	left_lower_leg(LOWER_LEG_WIDTH2, LOWER_LEG_HEIGHT2, color_lower_leg2);//左下腿绘制
	model_view = mvstack.pop();//恢复躯干变换矩阵

	mvstack.push(model_view); //保存躯干变换矩阵
	model_view *= (Translate(-TORSO_HEIGHT2 / 6, 0, 0.0) *RotateX(theta2[RightUpperLeg])*RotateZ(180));
	right_upper_leg(UPPER_LEG_WIDTH2, UPPER_LEG_HEIGHT2, color_upper_leg2);//右上腿绘制
	model_view *= (Translate(0.0, UPPER_LEG_HEIGHT2, 0.0) *RotateX(theta2[RightLowerLeg]));
	right_lower_leg(LOWER_LEG_WIDTH2, LOWER_LEG_HEIGHT2, color_lower_leg2);//右下腿绘制
	model_view = mvstack.pop();//恢复躯干变换矩阵
}



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
	colorcube();//生成单位六面体
				// 创建顶点数组对象
	glGenVertexArrays(1, &vao[0]);
	glBindVertexArray(vao[0]);
	// 创建并初始化顶点缓存对象
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(nomal),
		NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors),
		colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(nomal), nomal);

	// 加载着色器
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
	//初始化相机参数
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

/////////////////////////////////////////////
void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 光源位置
	float lx = lightPos[0];
	float ly = lightPos[1];
	float lz = lightPos[2];
	GLfloat shadowface[] = { 0,1,0,-1 };//shadowface表示阴影所需要投影的平面
	shadow = mat4(
		vec4(-ly*shadowface[1] - lz*shadowface[2], lx*shadowface[1], lx*shadowface[2], lx*shadowface[3]),
		vec4(ly*shadowface[0], -lx*shadowface[0] - lz*shadowface[2], ly*shadowface[2], ly*shadowface[3]),
		vec4(lz*shadowface[0], lz*shadowface[1], -lx*shadowface[0] - ly*shadowface[1], lx*shadowface[3]),
		vec4(0, 0, 0, -lx*shadowface[0] - ly*shadowface[1] - lz*shadowface[2]));

	Camera::modelMatrix = mat4(1.0);
	Camera::viewMatrix = Camera::lookAt(eye, eye + at, up);//设置照相机
	Camera::projMatrix = Camera::perspective(fov, aspect, zN, zF);//设置透视投影
	glUniformMatrix4fv(Projection, 1, GL_TRUE, &Camera::projMatrix[0][0]);	//传进透视投影矩阵
	glUniform3fv(lightPosID, 1, &lightPos[0]);	// 将光源信息传入顶点着色器
	ModelViewMatrix = Camera::viewMatrix * Camera::modelMatrix;
	mp_->setCameraMatrix(Camera::modelMatrix, Camera::viewMatrix, Camera::projMatrix);
	mp_->draw_meshes();//画图形
	glUseProgram(program);

	//bamboo(20,0,-10);
	show_robot(runX, runY, runZ);	// 绘制机器人
	show_robot2(runX2, runY2, runZ2);	// 绘制机器人
	glutSwapBuffers();
};



void
mouse(int button, int state, int x, int y)
{
	//检测鼠标是否按下的状态，并记录下鼠标坐标
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
//角度转弧度
float radians(GLfloat x) {
	return x*M_PI / 180;
}

void mousepassmove(int x, int y) {
	if (cameramouse) {
		//移动
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
		at = normalize(front);//赋给相机观察点以达到控制相机观察转换的目的
		glutPostRedisplay();
	}
}

//----------------------------------------------------------------------------
//控制光源的函数
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

	//cout << "当前光源的位置：" << lightPos << endl;
}

//----------------------------------------------------------------------------
void
idle(void)
{
	glutPostRedisplay();
}

//----------------------------------------------------------------------------
// 控制机器人姿势的函数 设置两种姿势切换

void robotChangeGesture(int a) {
	//cout << "当前runGesture：" << runGesture << endl;
	if (a == 1) {
		theta[LeftUpperArm] = 40;
		theta[LeftLowerArm] = 115;
		theta[RightUpperArm] = 340;
		theta[RightLowerArm] = 105;
		theta[RightUpperLeg] = 20;
		theta[RightLowerLeg] = 265;
		theta[LeftUpperLeg] = 340;
		theta[LeftLowerLeg] = 0.0;
	}
	else if (a == 2)
	{
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

void robotChangeGesture() {
	//cout << "当前runGesture：" << runGesture << endl;
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

void robotChangeGesture2(int a) {
	//cout << "当前runGesture2：" << runGesture2 << endl;
	if (a == 1) {
		theta2[LeftUpperArm] = 40;
		theta2[LeftLowerArm] = 115;
		theta2[RightUpperArm] = 340;
		theta2[RightLowerArm] = 105;
		theta2[RightUpperLeg] = 20;
		theta2[RightLowerLeg] = 265;
		theta2[LeftUpperLeg] = 340;
		theta2[LeftLowerLeg] = 0.0;
	}
	else if (a == 2)
	{
		theta2[LeftUpperArm] = 340;
		theta2[LeftLowerArm] = 105;
		theta2[RightUpperArm] = 40;
		theta2[RightLowerArm] = 115;
		theta2[RightUpperLeg] = 340;
		theta2[RightLowerLeg] = 0;
		theta2[LeftUpperLeg] = 20;
		theta2[LeftLowerLeg] = 265;
		runGesture2 = 1;
	}
}

void robotChangeGesture2() {
	//cout << "当前runGesture2：" << runGesture2 << endl;
	if (runGesture2 == 1) {
		theta2[LeftUpperArm] = 40;
		theta2[LeftLowerArm] = 115;
		theta2[RightUpperArm] = 340;
		theta2[RightLowerArm] = 105;
		theta2[RightUpperLeg] = 20;
		theta2[RightLowerLeg] = 265;
		theta2[LeftUpperLeg] = 340;
		theta2[LeftLowerLeg] = 0.0;
		runGesture2 = 0;
	}
	else {
		theta2[LeftUpperArm] = 340;
		theta2[LeftLowerArm] = 105;
		theta2[RightUpperArm] = 40;
		theta2[RightLowerArm] = 115;
		theta2[RightUpperLeg] = 340;
		theta2[RightLowerLeg] = 0;
		theta2[LeftUpperLeg] = 20;
		theta2[LeftLowerLeg] = 265;
		runGesture2 = 1;
	}
}

void changeRobotSize(float x) {
	HEAD_HEIGHT *= x;
	HEAD_WIDTH *= x;

	TORSO_HEIGHT *= x;
	TORSO_WIDTH *= x;

	UPPER_ARM_HEIGHT *= x;
	UPPER_ARM_WIDTH *= x;

	LOWER_ARM_HEIGHT *= x;
	LOWER_ARM_WIDTH *= x;

	UPPER_LEG_WIDTH *= x;
	UPPER_LEG_HEIGHT *= x;

	LOWER_LEG_HEIGHT *= x;
	LOWER_LEG_WIDTH *= x;

	energy = 0;
}

void changeRobotSize2(float x) {
	if (x==1)
	{
		HEAD_HEIGHT2 = 1;
		HEAD_WIDTH2 = 1;

		TORSO_HEIGHT2 = 2.5;
		TORSO_WIDTH2 = 2.6;

		UPPER_ARM_HEIGHT2 = 0.5;
		UPPER_ARM_WIDTH2 = 0.5;

		LOWER_ARM_HEIGHT2 = 0.5;
		LOWER_ARM_WIDTH2 = 0.5;

		UPPER_LEG_WIDTH2 = 0.5;
		UPPER_LEG_HEIGHT2 = 0.5;

		LOWER_LEG_HEIGHT2 = 0.5;
		LOWER_LEG_WIDTH2 = 0.5;
		//Sleep(2000);
		float runX = 10, runY = 0, runZ = -6;	// 控制机器人1移动的坐标
		float runX2 = 10, runY2 = 0, runZ2 = 6;	// 控制机器人2移动的坐标
		stepSize2 += 3;
	}
	
	if (0)
	{
		HEAD_HEIGHT2 = 0;
		HEAD_WIDTH2 = 0;

		TORSO_HEIGHT2 = 0;
		TORSO_WIDTH2 = 0;

		UPPER_ARM_HEIGHT2 = 0;
		UPPER_ARM_WIDTH2 = 0;

		LOWER_ARM_HEIGHT2 = 0;
		LOWER_ARM_WIDTH2 = 0;

		UPPER_LEG_WIDTH2 =0;
		UPPER_LEG_HEIGHT2 = 0;

		LOWER_LEG_HEIGHT2 = 0;
		LOWER_LEG_WIDTH2 = 0;
	}

}

void reset()
{
	cout << "游戏重新开始！" << endl;
	// camera
	pAngle = 0.0;

	// perspective
	fov = 100.0;
	aspect = 1.0;
	zN = 0.1;
	zF = 100.0;
	lightPos = vec3(5, 5, 10);
	rad = 50.0;
	tAngle = 25.0;
	pAngle = 0.0;

	runX = 10;
	runZ = -6;
	runX2 = 10;
	runZ2 = 6;


	// 机器人姿势
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

	theta2[Torso] = 0;
	theta2[Head] = 0;
	theta2[LeftUpperArm] = 0;
	theta2[LeftLowerArm] = 0;
	theta2[RightUpperArm] = 0;
	theta2[RightLowerArm] = 0;
	theta2[RightUpperLeg] = 0;
	theta2[RightLowerLeg] = 0;
	theta2[LeftUpperLeg] = 0;
	theta2[LeftLowerLeg] = 0;

	HEAD_HEIGHT = 2.5;
	HEAD_WIDTH = 2.5;

	TORSO_HEIGHT = 5.5;
	TORSO_WIDTH = 4.0;

	UPPER_ARM_HEIGHT = 2.0;
	UPPER_ARM_WIDTH = 1;

	LOWER_ARM_HEIGHT = 1.5;
	LOWER_ARM_WIDTH = 1;

	UPPER_LEG_WIDTH = 1.5;
	UPPER_LEG_HEIGHT = 1;

	LOWER_LEG_HEIGHT = 2;
	LOWER_LEG_WIDTH = 1.5;

	CUBE_HEIGHT = 8;
	CUBE_WIDTH = 8;

	// 定义机器人2各个部位的大小

	HEAD_HEIGHT2 = 2.5*0.5;
	HEAD_WIDTH2 = 2.5*0.5;

	TORSO_HEIGHT2 = 5.5*0.5;
	TORSO_WIDTH2 = 4.0*0.5;

	UPPER_ARM_HEIGHT2 = 2.0*0.5;
	UPPER_ARM_WIDTH2 = 1 * 0.5;

	LOWER_ARM_HEIGHT2 = 1.5*0.5;
	LOWER_ARM_WIDTH2 = 1 * 0.5;

	UPPER_LEG_WIDTH2 = 1.5*0.5;
	UPPER_LEG_HEIGHT2 = 1 * 0.5;

	LOWER_LEG_HEIGHT2 = 2 * 0.5;
	LOWER_LEG_WIDTH2 = 1.5*0.5;

	CUBE_HEIGHT2 = 8 * 0.5;
	CUBE_WIDTH2 = 8 * 0.5;

	stepSize2 = 4;
	endtime = 2;
	start_time = 0;
	L = 1;


}


//奔跑增加能量
void energymaneger2() {
	energy2 += 5;

	if (energy2 >= 500)
	{
		stepSize2 += 2;
		energy2 = 0;
	}
		
	//cout << "当前能量：" << energy2 << endl;

}

void energymaneger() {
	energy += 5;
	start_time += 2;

	if (energy >= 500)
	{
		changeRobotSize(1.5);
		L *= 1.5;
	}

	if (start_time>600)
	{
		cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
		cout << "小布点赢了" << endl;
		cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
		reset();
	}

	if (start_time==100)
	{
		cout << "剩余时间：500times" << endl;
	}

	else if (start_time == 200)
	{
		cout << "剩余时间：400times" << endl;
	}

	else if (start_time == 300)
	{
		cout << "剩余时间：300times" << endl;
	}

	else if (start_time == 400)
	{
		cout << "剩余时间：200times" << endl;
	}
	else if (start_time == 500)
	{
		cout << "剩余时间：100times" << endl;
	}
	else if (start_time == 550)
	{
		cout << "剩余时间：50times" << endl;
	}
	else
	{

	}
		
	//cout << "当前能量：" << energy << endl;

}




Mesh_Painter* skybox(string picture[])
{
	//----------------------------------------------------------------------------
	//制作天空盒
	mp_ = new Mesh_Painter;
	// 绘制墙壁并贴上纹理
	float wall_height = -50; // 控制墙在y轴上高度的偏差
	My_Mesh* my_mesh5 = new My_Mesh;
	my_mesh5->generate_wallbk(4);
	my_mesh5->set_texture_file(picture[0]);//back
	my_mesh5->set_translate(0, wall_height, -24.25);//平移
	my_mesh5->set_theta(0, 0, 0);//旋转轴
	my_mesh5->set_theta_step(0.0, 0.0, 0.0);//旋转速度
	my_meshs.push_back(my_mesh5);
	mp_->add_mesh(my_mesh5);

	My_Mesh* my_mesh6 = new My_Mesh;
	my_mesh6->generate_walllf(4);
	my_mesh6->set_texture_file(picture[1]);//left
	my_mesh6->set_translate(-24.27, wall_height, 0);
	my_mesh6->set_theta(0, 0, 0);
	my_mesh6->set_theta_step(0.0, 0.0, 0.0);
	my_meshs.push_back(my_mesh6);
	mp_->add_mesh(my_mesh6);

	My_Mesh* my_mesh7 = new My_Mesh;
	my_mesh7->generate_wallft(4);
	my_mesh7->set_texture_file(picture[2]);//front
	my_mesh7->set_translate(0, wall_height, 24.25);
	my_mesh7->set_theta(0, 0, 0);
	my_mesh7->set_theta_step(0.0, 0.0, 0.0);
	my_meshs.push_back(my_mesh7);
	mp_->add_mesh(my_mesh7);

	My_Mesh* my_mesh8 = new My_Mesh;
	my_mesh8->generate_wallrt(4);
	my_mesh8->set_texture_file(picture[3]);//right
	my_mesh8->set_translate(24.0, wall_height, 0);
	my_mesh8->set_theta(0, 0, 0);
	my_mesh8->set_theta_step(0.0, 0.0, 0.0);
	my_meshs.push_back(my_mesh8);
	mp_->add_mesh(my_mesh8);

	// 生成地面
	My_Mesh* my_mesh4 = new My_Mesh;
	my_mesh4->generate_floor(4);	// 生成平面需要的点、面、向量
	my_mesh4->set_texture_file(picture[4]);//指定纹理图像文件,down
	my_mesh4->set_translate(0, 0, 0);	// 平移
	my_mesh4->set_theta(0, 0, 0);	// 旋转轴
	my_mesh4->set_theta_step(0.0, 0.0, 0.0);	// 旋转速度 
	my_meshs.push_back(my_mesh4);
	mp_->add_mesh(my_mesh4);


	//生成天花板
	My_Mesh* my_mesh9 = new My_Mesh;
	my_mesh9->generate_floor(4);
	my_mesh9->set_texture_file(picture[5]);//up
	my_mesh9->set_translate(0, 116., 0);  //116  118
	my_mesh9->set_theta(0, 0, 0);
	my_mesh9->set_theta_step(0.0, 0.0, 0.0);
	my_meshs.push_back(my_mesh9);
	mp_->add_mesh(my_mesh9);

	mp_->init_shaders("v_texture.glsl", "f_texture.glsl");
	mp_->update_vertex_buffer();
	mp_->update_texture();

	return mp_;
}

void judugeCross() {
	if (runX < -70)
	{
		mp_ = skybox(pictrue2);
		runX = 30;
		//cout << "机器人穿越啦 "<< endl;
	}
	else if(runX > 30)
	{
		mp_ = skybox(pictrue1);
		runX = -70;
		//cout << "机器人穿越啦 " << endl;
	}
	else if (runZ < -77)
	{
		mp_ = skybox(pictrue3);
		runZ = 77;
		//cout << "机器人穿越啦 " << endl;
	}
	else if (runZ > 77)
	{
		mp_ = skybox(pictrue4);
		runZ = -77;
		//cout << "机器人穿越啦 " << endl;
	}



	if (runX2 < -70)
	{
		mp_ = skybox(pictrue2);
		runX2 = 30;
		//cout << "机器人穿越啦 " << endl;
	}
	else if (runX2 > 30)
	{
		mp_ = skybox(pictrue1);
		runX2 = -70;
		//cout << "机器人穿越啦 " << endl;
	}
	else if (runZ2 < -77)
	{
		mp_ = skybox(pictrue3);
		runZ2 = 77;
		//cout << "机器人穿越啦 " << endl;
	}
	else if (runZ2 > 77)
	{
		mp_ = skybox(pictrue4);
		runZ2 = -77;
		//cout << "机器人穿越啦 " << endl;
	}

	if ((runX-L)<=runX2<=(runX+L)&&(runZ - L) <= runZ2 <= (runZ + L))
	{
		if (endtime==2)
		{
			changeRobotSize2(1);
			endtime = 1;
		}
		else if (endtime == 1)
		{
			cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
			cout << "巨无霸赢了" << endl;
			cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
			changeRobotSize2(0);
			//Sleep(2000);
			reset();
		}
	}
}




//----------------------------------------------------------------------------
//控制相机位置和机器人位置的的函数
void keyboard(unsigned char key, int mousex, int mousey)
{
	GLfloat cameraSpeed = 1.0f;
	vec4 n = normalize(at - eye);
	vec4 u = normalize(vec4(cross(n, up), 0.0));
	vec4 v = normalize(vec4(cross(u, n), 0.0));

	switch (key)
	{
		// 控制相机变换
	case 'h':
		eye += normalize(cross(at, up)) * cameraSpeed;
		//cout << "当前相机的参数： eye:" << eye << " fov:" << fov << endl;
		break;
	case 'f':
		eye -= normalize(cross(at, up)) * cameraSpeed;
		//cout << "当前相机的参数： eye:" << eye << " fov:" << fov << endl;
		break;
	case 't': 
		tAngle += 5;
		eye += at*cameraSpeed;
		//cout << "当前相机的参数： eye:" << eye << " fov:" << fov << endl;
		break;
	case 'g':
		eye -= at*cameraSpeed;
		//cout << "当前相机的参数： eye:" << eye << " fov:" << fov << endl;
		break;
	case 'z':
		eye.y += cameraSpeed;
		//cout << "当前相机的参数： eye:" << eye << " fov:" << fov << endl;
		break;
	case 'x':
		eye.y -= cameraSpeed;
		//cout << "当前相机的参数： eye:" << eye << " fov:" << fov << endl;
		break;
	case 'v': fov += 5;
		//cout << "当前相机的参数： eye:" << eye << " fov:" << fov << endl;
		break;
	case 'c': fov -= 5;
		//cout << "当前相机的参数： eye:" << eye << " fov:" << fov << endl;
		break;

		// 控制机器人2移动
	case 'j':
		robotChangeGesture2();	// 切换姿势
		runZ2 -= stepSize2;	// 走出一步，步长为stepSize
		theta2[Torso] = 180.0 + 90.0;// 机器人身体转向前进的方向
		energymaneger2();
		judugeCross();
		//cout << "机器人当前位置：（" << runX2 << " , " << runY2 << " , " << runZ2 << " )" << endl;
		break;

	case 'l':	// 右移
		robotChangeGesture2();
		runZ2 += stepSize;
		theta2[Torso] = 180.0 - 90.0;
		energymaneger2();
		judugeCross();
		//cout << "机器人当前位置：（" << runX2 << " , " << runY2 << " , " << runZ2 << " )" << endl;
		break;

	case 'i':	// 前进
		robotChangeGesture2();
		runX2 -= stepSize;
		theta2[Torso] = 180.0;
		energymaneger2();
		judugeCross();
		//cout << "机器人当前位置：（" << runX2 << " , " << runY2 << " , " << runZ2 << " )" << endl;
		break;

	case 'k':	// 后退
		robotChangeGesture2();
		runX2 += stepSize;
		theta2[Torso] = 0;
		energymaneger2();
		judugeCross();
		//cout << "机器人当前位置：（" << runX2 << " , " << runY2 << " , " << runZ2 << " )" << endl;
		break;


		// 控制机器人移动
	case 'a':// 左移
		robotChangeGesture();	// 切换姿势
		runZ -= stepSize;	// 走出一步，步长为stepSize
		theta[Torso] = 180.0 + 90.0;// 机器人身体转向前进的方向
		energymaneger();
		judugeCross();
		//cout << "机器人当前位置：（"<<runX<<" , "<<runY<<" , "<<runZ <<" )"<< endl;
		break;
	case 'd':	// 右移
		robotChangeGesture();
		runZ += stepSize;
		theta[Torso] = 180.0 - 90.0;
		energymaneger();
		judugeCross();
		//cout << "机器人当前位置：（" << runX << " , " << runY << " , " << runZ << " )" << endl;
		break;
	case 'w':	// 前进
		robotChangeGesture();
		runX -= stepSize;
		theta[Torso] = 180.0;
		energymaneger();
		judugeCross();
		//cout << "机器人当前位置：（" << runX << " , " << runY << " , " << runZ << " )" << endl;
		break;
	case 's':	// 后退
		robotChangeGesture();
		runX += stepSize;
		theta[Torso] = 0; 
		energymaneger();
		judugeCross();
		//cout << "机器人当前位置：（" << runX << " , " << runY << " , " << runZ << " )" << endl;
		break;

		//改变机器人的大小
	case '7':
		robotChangeGesture(1);
		robotChangeGesture2(1);
		//cout << "机器人姿势改变" << endl;
		break;

	case '8':
		robotChangeGesture(2);
		robotChangeGesture2(2);
		//cout << "机器人姿势改变" << endl;
		break;


	//改变机器人的大小
	case '=':	
		changeRobotSize(1.5);
		//cout << "机器人变大了" << endl;
		break;

	case '-':	
		changeRobotSize(0.8);
		//cout << "机器人变小了" << endl;
		break;

		//改变天空图
	case '1':
		mp_ = skybox(pictrue1);
		//cout << "天空图改变" << endl;
		break;
	case '2':
		mp_ = skybox(pictrue2);
		//cout << "天空图改变" << endl;
		break;
	case '3':
		mp_ = skybox(pictrue3);
		//cout << "天空图改变" << endl;
		break;
	case '4':
		mp_ = skybox(pictrue4);
		//cout << "天空图改变" << endl;
		break;
	case '5':
		mp_ = skybox(pictrue5);
		//cout << "天空图改变" << endl;
		break;
	case '6':
		mp_ = skybox(pictrue6);
		//cout << "天空图改变" << endl;
		break;

		// 所有信息重置
	case ' ':
		reset();
		break;

	default:
		break;
	}
	glutPostRedisplay();

}


int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1200, 800);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	int window = glutCreateWindow("2017152032_杨志发_期末大作业  头发都没了");
	glewExperimental = GL_TRUE;
	glewInit();
	cout << "游戏开始！" << endl;
	mp_ =skybox(pictrue1);

	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);

	glutSetCursor(GLUT_CURSOR_CROSSHAIR);
	glutMotionFunc(mouseControllight);
	glutPassiveMotionFunc(mousepassmove);
	glutMainLoop();

	// 释放内存空间
	for (unsigned int i = 0; i < my_meshs.size(); i++)
	{
		delete my_meshs[i];
	}
	delete mp_;
	//----------------------------------------------------------------------------
}
