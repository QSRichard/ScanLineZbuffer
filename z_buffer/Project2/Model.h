#pragma once

#include <iostream>
#include <vector>
#include <string>
#include "Vec3f.h"

class Vertex {
public:
	int id;
	Point3d point;
	
};
class Face {
public :
	vector<int> verids;
	vector<int> normals;

	Vec3f face_normal; // �淨����
	Color clolor; // ����ɫ��Ϊ��������ɫ��ֵ

};

class Model {
public:
	vector<Vertex> Model_vertexs;
	vector<Face> Model_faces;
	Vec3f center_point; // ��Ԫ���ߵ�
	vector<Vec3f> Model_normals;
	// ���Դ��Ϣ
	Point3d light_position = Point3d(400.0f, 600.0f, 500.0f);
	Color light_color = Color(0.3, 0.3, 0.3);
	Model() {};
	Model(const string& path);
	bool load(const string& path);
	void rotate(float** rotateMat, vector<Vertex>& vertexs);
private:
	string getStr(string& s, int& index);
};