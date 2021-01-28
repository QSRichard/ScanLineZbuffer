#pragma once

#include <vector>
#include <list>
#include "Model.h"
#include "utils.h"
//#include "Rasterizer.h"
#include <omp.h>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <ctime>



class ClassifyEdge {
public:
	float x;
	float dx;
	int dy;
	int id;
};
class ActiveEdge {
public:
	float x;
	float dx;
	float dy;
	int id;
	float z, dzx, dzy;
};
class ClassifyPoly {
public:
	float a, b, c, d;
	int id;
	int dy;
};
class ActivePoly {
public:
	float a, b, c, d;
	int id;
	int dy;
	vector<ActiveEdge> activeEdges;
};

class ScanLine_zbuffer {
public:
	ScanLine_zbuffer();
	//~ScanLineZBuffer();
	void render(Model& model);
	vector<vector<int>> Color_id;
	vector<float> z_buffer;
	vector<ActivePoly*> tab_active_poly;
	vector<vector<ClassifyEdge*>> tab_classify_edge;
	vector<vector<ClassifyPoly*>> tab_classify_poly;
	int width, height;
	void init(int width, int height);
	void creatClassifyTab(Model& model);
	void activeEdge(int y, ActivePoly* poly);
	void getSize(int& width, int& height);
};