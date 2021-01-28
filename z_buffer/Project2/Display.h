#pragma once
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/freeglut_std.h>
#include "Model.h"
#include "ScanLine_zbuffer.h"
#include <iostream>
#include <omp.h>
#include <iostream>
#include <cmath>
#include "utils.h"

class Display
{
public:

	Display(Model* model, ScanLine_zbuffer* slzBuffer);
	~Display();
	//void setSize(int w, int h);
	void run(int* argc, char** argv);

	static Model* model;
	//int width, height;
	static ScanLine_zbuffer* engine;
	static Point3d point_1st, point_2rd;
	static float** RotateMat;
	static int idx1, idx2;

	//通过拖动,获取旋转矩阵，并旋转模型
	static void loop();
	static void reshape(int w, int h);
	static void drag_mouse(int button, int state, int x, int y);
	static void keyboard(int key, int x, int y);
	static void screen_menu(int value);

};