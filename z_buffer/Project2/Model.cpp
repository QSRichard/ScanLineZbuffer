#include "Model.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <time.h>
#include <string>
#include <sstream>
using namespace std;

bool Model::load(const string& path) {

	std::cout << "HINT::Model load model." << std::endl;
	Model_normals.clear();
	Model_vertexs.clear();
	Model_faces.clear();
	cout << "开始打开文件" << endl;
	ifstream in;
	in.open(path);
	if (!in.is_open()) {
		std::cout << "ERROR::Importer::loadModel open file:\'" << path << "\' failed." << std::endl;
		return false;
	}
	else {
		string oneline;
		int line_num = 0;
		int v_nums = 0, f_nums = 0;
		while (getline(in, oneline))
		{
			line_num++;
			string type = "";
			int index = 0;
			type = getStr(oneline, index);
			if (type == "")
				continue;
			if (type[0] == '#')
				continue;
			else if (type == "v") {
				Vertex v;
				vector<float> f;
				string str;
				while ((str = getStr(oneline, index)) != "") {
					f.push_back(stof(str.c_str()));
				}
				if (f.size() != 3) {
					cout << "ERROR 顶点坐标参数个数不是三个";
					return false;
				}
				v.point.x = f[0], v.point.y = f[1], v.point.z = f[2];
				v.id = v_nums++;
				Model_vertexs.push_back(v);
			}
			else if (type == "f") {
				Face face;
				vector<int> verids;
				vector<int> norids;
				string str;
				while ((str = getStr(oneline, index)) != "") {
					int i = 0;
					while (i < str.size() && str[i] != '/')
						i++;
					verids.push_back(stoi(str.substr(0, i).c_str()));
					i++; // 此时若i合法则 str[i]=='/' 或str[i]是vt（纹理坐标）索引
					if (i >= str.size())
						continue;
					// 若有纹理坐标索引，此处略过
					while (i < str.size() && str[i] != '/')
						i++;
					i++;
					int j = i;
					while (j < str.size() && str[j] != ' ' && str[j] != '/')
						j++;
					norids.push_back(stoi(str.substr(i, j-i).c_str()));
				}
				if (verids.size() < 3) {
					cout << "ERROR 面的顶点个数小于3" << endl;
					return false;
				}
				// 处理负数顶点索引
				for (int i = 0; i < verids.size(); i++) {
					if (verids[i] < 0) {
						verids[i] = Model_vertexs.size() + 1 + verids[i];
					}
				}
				// 处理负数顶点法向量索引
				for (int i = 0; i < norids.size(); i++) {
					if (norids[i] < 0) {
						norids[i] = Model_normals.size() + 1 + norids[i];
					}
				}
				for (int i = 0; i < verids.size(); i++) {
					face.verids.push_back(verids[i] - 1);
				}
				for (int i = 0; i < norids.size(); i++) {
					face.normals.push_back(norids[i]-1);
				}
				/*
				*
				* Todo
				*
				* Solve Face's Normal Vector and Face's Color
				* 
				* 
				*/
				Point3d& a = Model_vertexs[face.verids[0]].point;
				Point3d& b = Model_vertexs[face.verids[1]].point;
				Point3d& c = Model_vertexs[face.verids[2]].point;
				Vec3f first = b - a, second = c - b;
				Vec3f face_normal = cross(first, second);//求法线，并单位化
				face.face_normal = normalize(face_normal);
				
				/*
				* Todo
				* Color
				* 
				*/
				for (int t = 0; t < face.verids.size(); t++) {
					Vec3f ray = normalize(light_position - Model_vertexs[face.verids[t]].point);
					Vec3f normal = face.normals.size() == 0 ? face.face_normal : Model_normals[face.normals[t]];
					float cos = dot(normal, ray);
					if (cos > 0)face.clolor += cos * light_color;
				}
				face.clolor = face.clolor/face.verids.size();
				Model_faces.push_back(face);
			}
			else if (type == "vn") {
				string str;
				vector<float> temp;
				while ((str = getStr(oneline, index)) != "") {
					temp.push_back(stof(str.c_str()));
				}
				if (temp.size() != 3) {
					cout << "ERROR 顶点法向量参数个数不是三个" << endl;
					return false;
				}
				Vec3f v;
				v.x = temp[0], v.y = temp[1], v.z = temp[2];
				Model_normals.push_back(v);
			}
			else {
			}
		}
	}
	return true;
}
Model::Model(const string& path) {
	if (load(path))
	{
		string face_type;
		cout << "模型" + path + "加载成功！" << endl;
		cout << "面元数：" << Model_faces.size() <<
			"，顶点数：" << Model_vertexs.size() << endl;
	}
	else
	{
		cout << "无法打开obj文件：" + path << endl;
	}
}
string Model:: getStr(string& s, int& index) {
	int t = index;
	while (t < s.size() && s[t] == ' ')
		t++;
	if (t >= s.size())
		return "";
	int m = t;
	while (m < s.size() && s[m] != ' ')
		m++;
	index = m;
	return s.substr(t, m - t);
}

void Model::rotate(float** RotateMat, vector<Vertex>& vertexes)
{
	int vertex_num = vertexes.size();
#pragma omp parallel for
	for (int i = 0; i < vertex_num; ++i)
	{
		//Point3f* point = &vertexes[i].point;
		Point3d point;
		Point3d tmp_point = vertexes[i].point - center_point;
		vertexes[i].point.x = RotateMat[0][0] * tmp_point.x + RotateMat[0][1] * tmp_point.y + RotateMat[0][2] * tmp_point.z;
		vertexes[i].point.y = RotateMat[1][0] * tmp_point.x + RotateMat[1][1] * tmp_point.y + RotateMat[1][2] * tmp_point.z;
		vertexes[i].point.z = RotateMat[2][0] * tmp_point.x + RotateMat[2][1] * tmp_point.y + RotateMat[2][2] * tmp_point.z;
		vertexes[i].point += center_point;
	}

	int normal_num = Model_normals.size();
#pragma omp parallel for
	for (int i = 0; i < normal_num; ++i)
	{
		Point3d tmp_point = Model_normals[i];
		Model_normals[i].x = RotateMat[0][0] * tmp_point.x + RotateMat[0][1] * tmp_point.y + RotateMat[0][2] * tmp_point.z;
		Model_normals[i].y = RotateMat[1][0] * tmp_point.x + RotateMat[1][1] * tmp_point.y + RotateMat[1][2] * tmp_point.z;
		Model_normals[i].z = RotateMat[2][0] * tmp_point.x + RotateMat[2][1] * tmp_point.y + RotateMat[2][2] * tmp_point.z;

	}

	int face_num = Model_faces.size();
#pragma omp parallel for
	for (int i = 0; i < face_num; ++i)
	{
		Point3d tmp_point = Model_faces[i].face_normal;
		Model_faces[i].face_normal.x = RotateMat[0][0] * tmp_point.x + RotateMat[0][1] * tmp_point.y + RotateMat[0][2] * tmp_point.z;
		Model_faces[i].face_normal.y = RotateMat[1][0] * tmp_point.x + RotateMat[1][1] * tmp_point.y + RotateMat[1][2] * tmp_point.z;
		Model_faces[i].face_normal.z = RotateMat[2][0] * tmp_point.x + RotateMat[2][1] * tmp_point.y + RotateMat[2][2] * tmp_point.z;
	}
}