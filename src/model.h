#include "Vec3.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
typedef Vec3f Point3d;
typedef Vec3f Color;
using namespace std;
class Vertex{
public:
    int id;
    Point3d point;
    Color color;  // 顶点颜色
    Vec3f normal; // 顶点法向量
};

class Face{
public:
    vector<int> verids; // 顶点索引
    vector<int> verNorIds; // 顶点法向量索引
    Color color; // 面元颜色 （顶点颜色平均值）
    int id;

};
class Model{
public:
    vector<Vertex> vertexs;
    vector<Vec3f> vertex_normal;
    vector<Face> faces;
    bool load_model(ifstream& in);
    Model(string path);
};