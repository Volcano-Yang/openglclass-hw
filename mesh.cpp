#include "mesh.h"
#include <sstream>
#include <fstream>
#include <iosfwd>
#include <algorithm>
#include <gl/GL.h>
#include <math.h>
#include <algorithm>
#include <iostream>
#include "Angel.h"
#include <assert.h>

using namespace std;
#pragma comment(lib, "glew32.lib")

// ��������ε�ÿ���Ƕ�
double getSquareAngle(int point)
{
	return M_PI / 4 + (M_PI / 2 * point);
}

My_Mesh::My_Mesh()
{
	vTranslation[0] = Theta[0] = 0;
	vTranslation[1] = Theta[1] = 0;
	vTranslation[2] = Theta[2] = 0;
	Theta[0] = 45;
}

My_Mesh::~My_Mesh()
{

}

void My_Mesh::normal_to_color(float nx, float ny, float nz, float& r, float& g, float& b)
{
	r = float(std::min(std::max(0.5 * (nx + 1.0), 0.0), 1.0));
	g = float(std::min(std::max(0.5 * (ny + 1.0), 0.0), 1.0));
	b = float(std::min(std::max(0.5 * (nz + 1.0), 0.0), 1.0));
};

const VtList&  My_Mesh::get_vts()
{
	return this->m_vt_list_;
};
void My_Mesh::clear_data()
{
	m_vertices_.clear();
	m_normals_.clear();
	m_faces_.clear();
	m_color_list_.clear();
	m_vt_list_.clear();
};
void My_Mesh::get_boundingbox(point3f& min_p, point3f& max_p) const
{
	min_p = this->m_min_box_;
	max_p = this->m_max_box_;
};
const STLVectorf&  My_Mesh::get_colors()
{
	return this->m_color_list_;
};
const VertexList& My_Mesh::get_vertices()
{
	return this->m_vertices_;
};
const NormalList& My_Mesh::get_normals()
{
	return this->m_normals_;
};
const FaceList&   My_Mesh::get_faces()
{
	return this->m_faces_;
};

int My_Mesh::num_faces()
{
	return this->m_faces_.size() / 3;
};
int My_Mesh::num_vertices()
{
	return this->m_vertices_.size() / 3;
};

const point3f& My_Mesh::get_center()
{
	return this->m_center_;
};

void My_Mesh::generate_cylinder(int num_division, float height)
{
	this->clear_data();
	this->m_center_ = point3f(0, 0, 0);
	this->m_min_box_ = point3f(-1, -1, -height);
	this->m_max_box_ = point3f(1, 1, height);

	int scale = 200;	// ����Բ�����С
	int num_samples = num_division;	// ������
	float z = -height;	// ����Բ����ĵ�ƽ��߶�
						// ��������
	float pi = 3.14159265;
	float step = 1.0 * 360 / num_samples;
	float rr = pi / 180;
	//Բ����Z�����ϣ���cos��sin����x��y����
	for (int i = 0; i < num_samples; i++)
	{
		float r_r_r = i * step * rr;	// ���㻡��
		float x = cos(r_r_r);
		float y = sin(r_r_r);
		m_vertices_.push_back(x*scale);	// ѹ�������
		m_vertices_.push_back(y*scale);
		m_vertices_.push_back(z*scale);

		m_normals_.push_back(x);	// ѹ�뷨����
		m_normals_.push_back(y);
		m_normals_.push_back(0);
		//������������
		float r;
		float g;
		float b;
		My_Mesh::normal_to_color(x, y, z, r, g, b);
		//������÷�����������ɫ��ѧ�������Զ����Լ�����ɫ���ɷ�ʽ
		m_color_list_.push_back(r);
		m_color_list_.push_back(g);
		m_color_list_.push_back(b);
	}

	z = height;
	//Բ����Z�����ϣ���cos��sin����x��y���꣬
	for (int i = 0; i < num_samples; i++)
	{
		float r_r_r = i * step * rr;
		float x = cos(r_r_r);
		float y = sin(r_r_r);
		m_vertices_.push_back(x*scale);
		m_vertices_.push_back(y*scale);
		m_vertices_.push_back(z*scale);

		m_normals_.push_back(x);
		m_normals_.push_back(y);
		m_normals_.push_back(0);
		//������������
		float r;
		float g;
		float b;
		My_Mesh::normal_to_color(x, y, z, r, g, b);
		//������÷�����������ɫ��ѧ�������Զ����Լ�����ɫ���ɷ�ʽ
		m_color_list_.push_back(r);
		m_color_list_.push_back(g);
		m_color_list_.push_back(b);

	}
	//����������Ƭ
	for (int i = 0; i < num_samples; i++)
	{
		m_faces_.push_back(i);
		m_faces_.push_back((i + 1) % num_samples);
		m_faces_.push_back((i)+num_samples);

		m_faces_.push_back((i)+num_samples);
		m_faces_.push_back((i + 1) % num_samples);
		m_faces_.push_back((i + num_samples + 1) % (num_samples)+num_samples);
		//����������Ƭ

		m_vt_list_.push_back(1.0 * i / num_samples);
		m_vt_list_.push_back(0.0);
		//��������
		m_vt_list_.push_back(1.0 * ((i + 1)) / num_samples);
		m_vt_list_.push_back(0.0);
		//��������
		m_vt_list_.push_back(1.0 * i / num_samples);
		m_vt_list_.push_back(1.0);
		//��������

		m_vt_list_.push_back(1.0 * i / num_samples);
		m_vt_list_.push_back(1.0);
		//��������
		m_vt_list_.push_back(1.0 * ((i + 1)) / num_samples);
		m_vt_list_.push_back(0.0);
		//��������
		m_vt_list_.push_back(1.0 * ((i + 1)) / num_samples);
		m_vt_list_.push_back(1.0);
		//��������
	}

};

void My_Mesh::generate_disk(int num_division)
{
	//���ڴ���Ӵ�������Բ��
	this->clear_data();
	this->m_center_ = point3f(0, 0, 0);//ȷ������
	this->m_min_box_ = point3f(-1, -1, 0);
	this->m_max_box_ = point3f(1, 1, 0);

	int num_samples = num_division;//����
	float z = 0;
	float pi = 3.14159265;
	float step = 1.0 * 360 / num_samples;
	float rr = pi / 180;
	//Բ��Z�����ϣ���cos��sin����x��y����
	for (int i = 0; i < num_samples; i++)
	{
		float r_r_r = i * step * rr;//���ɻ���
		float x = cos(r_r_r);
		float y = sin(r_r_r);
		m_vertices_.push_back(x);//ѹ�������
		m_vertices_.push_back(y);
		m_vertices_.push_back(z);

		m_normals_.push_back(x);//ѹ�뷨����
		m_normals_.push_back(y);
		m_normals_.push_back(0);
		//������������
		float r;
		float g;
		float b;
		My_Mesh::normal_to_color(x, y, z, r, g, b);
		//������÷�����������ɫ��ѧ�������Զ����Լ�����ɫ���ɷ�ʽ
		m_color_list_.push_back(r);
		m_color_list_.push_back(g);
		m_color_list_.push_back(b);
	}
	//ѹ��Բ��
	m_vertices_.push_back(0);
	m_vertices_.push_back(0);
	m_vertices_.push_back(0);

	m_normals_.push_back(0);
	m_normals_.push_back(0);
	m_normals_.push_back(0);
	//������������
	float r;
	float g;
	float b;
	My_Mesh::normal_to_color(0, 0, 0, r, g, b);
	//������÷�����������ɫ��ѧ�������Զ����Լ�����ɫ���ɷ�ʽ
	m_color_list_.push_back(r);
	m_color_list_.push_back(g);
	m_color_list_.push_back(b);

	//����������Ƭ
	for (int i = 0; i < num_samples; i++)
	{
		m_faces_.push_back(i % num_samples);
		m_faces_.push_back((i + 1) % num_samples);
		m_faces_.push_back(num_samples);

		//����������Ƭ����������

		m_vt_list_.push_back(1.0*cos(i% num_samples * step * rr) / 2 + 0.5);
		m_vt_list_.push_back(1.0*sin(i % num_samples* step * rr) / 2 + 0.5);
		//��������
		m_vt_list_.push_back(1.0*cos(((i + 1)) % num_samples* step * rr) / 2 + 0.5);
		m_vt_list_.push_back(1.0*sin(((i + 1)) % num_samples *step * rr) / 2 + 0.5);
		//��������
		m_vt_list_.push_back(0.5);
		m_vt_list_.push_back(0.5);
		//��������
	}
};

void My_Mesh::generate_cone(int num_division, float height)
{
	//���ڴ���Ӵ�������Բ׶��
	this->clear_data();
	this->m_center_ = point3f(0, 0, 0);//����
	this->m_min_box_ = point3f(-1, -1, -height);
	this->m_max_box_ = point3f(1, 1, height);


	int scale = 20;
	int num_samples = num_division;
	float z = -height;
	float pi = 3.14159265;
	float step = 1.0 * 360 / num_samples;
	float rr = pi / 180;
	//Բ׶��Z�����ϣ���cos��sin����x��y����
	for (int i = 0; i < num_samples; i++)
	{
		float r_r_r = i * step * rr;//���ɻ���
		float x = cos(r_r_r);
		float y = sin(r_r_r);
		m_vertices_.push_back(x);//ѹ�������
		m_vertices_.push_back(y);
		m_vertices_.push_back(z);

		m_normals_.push_back(x);//ѹ�뷨����
		m_normals_.push_back(y);
		m_normals_.push_back(0);
		//������������
		float r;
		float g;
		float b;
		My_Mesh::normal_to_color(x, y, z, r, g, b);
		//������÷�����������ɫ��ѧ�������Զ����Լ�����ɫ���ɷ�ʽ
		m_color_list_.push_back(r);
		m_color_list_.push_back(g);
		m_color_list_.push_back(b);
	}
	//ѹ�붥��
	m_vertices_.push_back(0);
	m_vertices_.push_back(-height);
	m_vertices_.push_back(-height);

	m_normals_.push_back(0);
	m_normals_.push_back(0);
	m_normals_.push_back(0);
	//������������
	float r;
	float g;
	float b;
	My_Mesh::normal_to_color(0, 0, -height, r, g, b);
	//������÷�����������ɫ��ѧ�������Զ����Լ�����ɫ���ɷ�ʽ
	m_color_list_.push_back(r);
	m_color_list_.push_back(g);
	m_color_list_.push_back(b);

	//����������Ƭ
	for (int i = 0; i < num_samples; i++)
	{
		m_faces_.push_back(i);
		m_faces_.push_back((i + 1) % num_samples);
		m_faces_.push_back(num_samples);

		//����������Ƭ����������

		m_vt_list_.push_back((1.0 * i / num_samples)*0.8 + 0.1);
		m_vt_list_.push_back(0.0);
		//��������
		m_vt_list_.push_back((1.0 * ((i + 1)) / num_samples)*0.8 + 0.1);
		m_vt_list_.push_back(0.0);
		//��������
		m_vt_list_.push_back(0.5);
		m_vt_list_.push_back(1.0);
		//��������

	}

};

void My_Mesh::generate_wallbk(int num_division)
{
	this->clear_data();
	this->m_center_ = point3f(0, 0, 0);
	this->m_min_box_ = point3f(-1, -1, 0);
	this->m_max_box_ = point3f(1, 1, 0);
	int num_samples = num_division;
	double scale = 500;
	double currentAngle;
	int z = -0.5*scale;
	for (int i = 0; i < num_samples; i++)
	{
		currentAngle = getSquareAngle(i);
		int x = cos(-currentAngle)*scale;
		int y = abs(sin(currentAngle)*scale) + sin(currentAngle)*scale;
		m_vertices_.push_back(x);
		m_vertices_.push_back(y);
		m_vertices_.push_back(z);
		m_normals_.push_back(x);
		m_normals_.push_back(y);
		m_normals_.push_back(0);
		//������������
		float r;
		float g;
		float b;
		My_Mesh::normal_to_color(x, y, z, r, g, b);
		//������÷�����������ɫ��ѧ�������Զ����Լ�����ɫ���ɷ�ʽ
		m_color_list_.push_back(r);
		m_color_list_.push_back(g);
		m_color_list_.push_back(b);
	}
	m_vertices_.push_back(0);
	m_vertices_.push_back(0);
	m_vertices_.push_back(0);
	m_normals_.push_back(0);
	m_normals_.push_back(0);
	m_normals_.push_back(1);
	//������������
	float r;
	float g;
	float b;
	My_Mesh::normal_to_color(0, 0, 1, r, g, b);
	m_color_list_.push_back(r);
	m_color_list_.push_back(g);
	m_color_list_.push_back(b);

	//////////////////////////////////////////
	//����������Ƭ
	m_faces_.push_back(0);//˳ʱ��0,1,2  ��ʱ��2,1,0
	m_faces_.push_back(3);
	m_faces_.push_back(2);
	//����������Ƭ		
	m_vt_list_.push_back(1);
	m_vt_list_.push_back(1);
	//��������

	m_vt_list_.push_back(1);
	m_vt_list_.push_back(0);
	//��������

	m_vt_list_.push_back(0);
	m_vt_list_.push_back(0);
	//��������
	//////////////////////////////////////
	m_faces_.push_back(0);
	m_faces_.push_back(2);
	m_faces_.push_back(1);//˳ʱ��0,2,3  ��ʱ��0,3,2
						  //����������Ƭ		
	m_vt_list_.push_back(1);
	m_vt_list_.push_back(1);
	//��������

	m_vt_list_.push_back(0);
	m_vt_list_.push_back(0);
	//��������

	m_vt_list_.push_back(0);
	m_vt_list_.push_back(1);
	//��������

};
void My_Mesh::generate_wallft(int num_division)
{
	this->clear_data();
	this->m_center_ = point3f(0, 0, 0);
	this->m_min_box_ = point3f(-1, -1, 0);
	this->m_max_box_ = point3f(1, 1, 0);

	int num_samples = num_division;

	double scale = 500;
	double currentAngle;

	//	int z = sin(getSquareAngle(2))*scale;
	int z = 0.5*scale;
	for (int i = 0; i < num_samples; i++)
	{
		currentAngle = getSquareAngle(i);
		int x = cos(currentAngle)*scale;
		int y = abs(sin(currentAngle)*scale) + sin(currentAngle)*scale;

		//z = tan(currentAngle);

		m_vertices_.push_back(x);
		m_vertices_.push_back(y);
		m_vertices_.push_back(z);

		m_normals_.push_back(x);
		m_normals_.push_back(y);
		m_normals_.push_back(0);
		//������������
		float r;
		float g;
		float b;
		My_Mesh::normal_to_color(x, y, z, r, g, b);
		//������÷�����������ɫ��ѧ�������Զ����Լ�����ɫ���ɷ�ʽ
		m_color_list_.push_back(r);
		m_color_list_.push_back(g);
		m_color_list_.push_back(b);
	}
	m_vertices_.push_back(0);
	m_vertices_.push_back(0);
	m_vertices_.push_back(0);
	m_normals_.push_back(0);
	m_normals_.push_back(0);
	m_normals_.push_back(1);
	//������������
	float r;
	float g;
	float b;
	My_Mesh::normal_to_color(0, 0, 1, r, g, b);
	m_color_list_.push_back(r);
	m_color_list_.push_back(g);
	m_color_list_.push_back(b);

	//////////////////////////////////////////
	//����������Ƭ
	m_faces_.push_back(2);//˳ʱ��0,1,2  ��ʱ��2,1,0
	m_faces_.push_back(1);
	m_faces_.push_back(0);
	//����������Ƭ		
	m_vt_list_.push_back(1);
	m_vt_list_.push_back(0);
	//��������

	m_vt_list_.push_back(1);
	m_vt_list_.push_back(1);
	//��������

	m_vt_list_.push_back(0);
	m_vt_list_.push_back(1);
	//��������
	//////////////////////////////////////
	m_faces_.push_back(2);
	m_faces_.push_back(0);
	m_faces_.push_back(3);//˳ʱ��0,2,3  ��ʱ��0,3,2
						  //����������Ƭ		
	m_vt_list_.push_back(1);
	m_vt_list_.push_back(0);
	//��������

	m_vt_list_.push_back(0);
	m_vt_list_.push_back(1);
	//��������

	m_vt_list_.push_back(0);
	m_vt_list_.push_back(0);
	//��������

};
void My_Mesh::generate_wallrt(int num_division)
{
	this->clear_data();
	this->m_center_ = point3f(0, 0, 0);
	this->m_min_box_ = point3f(-1, -1, 0);
	this->m_max_box_ = point3f(1, 1, 0);

	int num_samples = num_division;

	double scale = 500;
	double currentAngle;

	//	int x = sin(getSquareAngle(2))*scale;
	int x = 0.5*scale;
	for (int i = 0; i < num_samples; i++)
	{
		currentAngle = getSquareAngle(i);
		int z = cos(-currentAngle)*scale;
		int y = abs(sin(currentAngle)*scale) + sin(currentAngle)*scale;

		m_vertices_.push_back(x);
		m_vertices_.push_back(y);
		m_vertices_.push_back(z);

		m_normals_.push_back(x);
		m_normals_.push_back(y);
		m_normals_.push_back(0);
		//������������
		float r;
		float g;
		float b;
		My_Mesh::normal_to_color(x, y, z, r, g, b);
		m_color_list_.push_back(r);
		m_color_list_.push_back(g);
		m_color_list_.push_back(b);
	}
	m_vertices_.push_back(0);
	m_vertices_.push_back(0);
	m_vertices_.push_back(0);
	m_normals_.push_back(0);
	m_normals_.push_back(0);
	m_normals_.push_back(1);
	//������������
	float r;
	float g;
	float b;
	My_Mesh::normal_to_color(0, 0, 1, r, g, b);
	//������÷�����������ɫ��ѧ�������Զ����Լ�����ɫ���ɷ�ʽ
	m_color_list_.push_back(r);
	m_color_list_.push_back(g);
	m_color_list_.push_back(b);

	//////////////////////////////////////////
	//����������Ƭ
	m_faces_.push_back(0);//˳ʱ��0,1,2  ��ʱ��2,1,0
	m_faces_.push_back(3);
	m_faces_.push_back(2);
	//����������Ƭ		
	m_vt_list_.push_back(1);
	m_vt_list_.push_back(1);
	//��������

	m_vt_list_.push_back(1);
	m_vt_list_.push_back(0);
	//��������

	m_vt_list_.push_back(0);
	m_vt_list_.push_back(0);
	//��������
	//////////////////////////////////////
	m_faces_.push_back(0);
	m_faces_.push_back(2);
	m_faces_.push_back(1);//˳ʱ��0,2,3  ��ʱ��0,3,2
						  //����������Ƭ		
	m_vt_list_.push_back(1);
	m_vt_list_.push_back(1);
	//��������

	m_vt_list_.push_back(0);
	m_vt_list_.push_back(0);
	//��������

	m_vt_list_.push_back(0);
	m_vt_list_.push_back(1);
	//��������

};

void My_Mesh::generate_walllf(int num_division)
{
	this->clear_data();
	this->m_center_ = point3f(0, 0, 0);
	this->m_min_box_ = point3f(-1, -1, 0);
	this->m_max_box_ = point3f(1, 1, 0);

	int num_samples = num_division;
	double scale = 500;
	double k = scale / 2;
	const vec3 vertices[4] = {
		vec3(k, k, k),
		vec3(k,k, -k),
		vec3(k,  -k,k),
		vec3(k,  -k, -k),
	};

	double currentAngle;

	//	int x = sin(getSquareAngle(2))*scale;
	int x = -0.5*scale;
	for (int i = 0; i < num_samples; i++)
	{
		currentAngle = getSquareAngle(i);
		int z = cos(-currentAngle)*scale;
		int y = abs(sin(currentAngle)*scale) + sin(currentAngle)*scale;

		m_vertices_.push_back(x);
		m_vertices_.push_back(y);
		m_vertices_.push_back(z);

		m_normals_.push_back(x);
		m_normals_.push_back(y);
		m_normals_.push_back(0);
		//������������
		float r;
		float g;
		float b;
		My_Mesh::normal_to_color(x, y, z, r, g, b);
		m_color_list_.push_back(r);
		m_color_list_.push_back(g);
		m_color_list_.push_back(b);
	}
	m_vertices_.push_back(0);
	m_vertices_.push_back(0);
	m_vertices_.push_back(0);
	m_normals_.push_back(0);
	m_normals_.push_back(0);
	m_normals_.push_back(-1);
	//������������
	float r;
	float g;
	float b;
	My_Mesh::normal_to_color(0, 0, 1, r, g, b);
	//������÷�����������ɫ��ѧ�������Զ����Լ�����ɫ���ɷ�ʽ
	m_color_list_.push_back(r);
	m_color_list_.push_back(g);
	m_color_list_.push_back(b);

	//////////////////////////////////////////
	//����������Ƭ
	m_faces_.push_back(2);//˳ʱ��0,1,2  ��ʱ��2,1,0
	m_faces_.push_back(1);
	m_faces_.push_back(0);
	//����������Ƭ		
	m_vt_list_.push_back(1);
	m_vt_list_.push_back(0);
	//��������

	m_vt_list_.push_back(1);
	m_vt_list_.push_back(1);
	//��������

	m_vt_list_.push_back(0);
	m_vt_list_.push_back(1);
	//��������
	//////////////////////////////////////
	m_faces_.push_back(2);
	m_faces_.push_back(0);
	m_faces_.push_back(3);//˳ʱ��0,2,3  ��ʱ��0,3,2
						  //����������Ƭ		
	m_vt_list_.push_back(1);
	m_vt_list_.push_back(0);
	//��������

	m_vt_list_.push_back(0);
	m_vt_list_.push_back(1);
	//��������

	m_vt_list_.push_back(0);
	m_vt_list_.push_back(0);
	//��������

};

// ���ɵ���
void My_Mesh::generate_floor(int num_division)
{
	this->clear_data();
	this->m_center_ = point3f(0, 0, 0);
	this->m_min_box_ = point3f(-1, -1, 0);
	this->m_max_box_ = point3f(1, 1, 0);
	double currentAngle;

	int num_samples = num_division;	// �������
	double scale = 500;	// ����ƽ��Ĵ�С
	int y = 0;	// ��Ϊ��ƽ�棬�������������y��Ϊ��ֵ����������Ϊ0 
	for (int i = 0; i < num_samples; i++)
	{
		// ��ȡ�������ĸ��ǵĽǶ�
		currentAngle = getSquareAngle(i);
		// �ýǶȼ���x��z������
		int x = cos(-currentAngle)*scale;
		int z = sin(currentAngle)*scale;

		m_vertices_.push_back(x);
		m_vertices_.push_back(y);
		m_vertices_.push_back(z);

		m_normals_.push_back(x);
		m_normals_.push_back(y);
		m_normals_.push_back(0);
		//������������
		float r;
		float g;
		float b;
		My_Mesh::normal_to_color(x, y, z, r, g, b);
		m_color_list_.push_back(r);
		m_color_list_.push_back(g);
		m_color_list_.push_back(b);
	}
	m_vertices_.push_back(0);
	m_vertices_.push_back(0);
	m_vertices_.push_back(0);
	m_normals_.push_back(0);
	m_normals_.push_back(0);
	m_normals_.push_back(1);
	//������������
	float r;
	float g;
	float b;
	My_Mesh::normal_to_color(0, 0, 1, r, g, b);
	//������÷�����������ɫ��ѧ�������Զ����Լ�����ɫ���ɷ�ʽ
	m_color_list_.push_back(r);
	m_color_list_.push_back(g);
	m_color_list_.push_back(b);

	//////////////////////////////////////////
	//����������Ƭ
	m_faces_.push_back(2);//˳ʱ��0,1,2  ��ʱ��2,1,0
	m_faces_.push_back(1);
	m_faces_.push_back(0);
	//����������Ƭ		
	m_vt_list_.push_back(1);
	m_vt_list_.push_back(0);
	//��������

	m_vt_list_.push_back(1);
	m_vt_list_.push_back(1);
	//��������

	m_vt_list_.push_back(0);
	m_vt_list_.push_back(1);
	//��������
	//////////////////////////////////////
	m_faces_.push_back(2);
	m_faces_.push_back(0);
	m_faces_.push_back(3);//˳ʱ��0,2,3  ��ʱ��0,3,2
						  //����������Ƭ		
	m_vt_list_.push_back(1);
	m_vt_list_.push_back(0);
	//��������

	m_vt_list_.push_back(0);
	m_vt_list_.push_back(1);
	//��������

	m_vt_list_.push_back(0);
	m_vt_list_.push_back(0);
	//��������

};

void My_Mesh::set_texture_file(std::string s)
{
	this->texture_file_name = s;
};
std::string My_Mesh::get_texture_file()
{
	return this->texture_file_name;
};

void My_Mesh::set_translate(float x, float y, float z)
{
	vTranslation[0] = x;
	vTranslation[1] = y;
	vTranslation[2] = z;

};
void My_Mesh::get_translate(float& x, float& y, float& z)
{
	x = vTranslation[0];
	y = vTranslation[1];
	z = vTranslation[2];
};

void My_Mesh::set_theta(float x, float y, float z)
{
	Theta[0] = x;
	Theta[1] = y;
	Theta[2] = z;
};
void My_Mesh::get_theta(float& x, float& y, float& z)
{
	x = Theta[0];
	y = Theta[1];
	z = Theta[2];
};

void My_Mesh::set_theta_step(float x, float y, float z)
{
	Theta_step[0] = x;
	Theta_step[1] = y;
	Theta_step[2] = z;
};

void My_Mesh::add_theta_step()
{
	/*Theta[0] = Theta[0] + Theta_step[0];
	Theta[1] = Theta[1] + Theta_step[1];
	Theta[2] = Theta[2] + Theta_step[2];*/
};