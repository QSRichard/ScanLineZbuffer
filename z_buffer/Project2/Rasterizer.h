#pragma once

#include "Model.h"

class Rasterizer
{
public:
	//��ʾ���ڵĴ�С
	int width;
	int height;

	Rasterizer();
	//Rasterisation(Model& model);
	void setSize(int width, int height);//���ô��ڴ�С
	void getSize(int& width, int& height);

	void run(Model& model);//����model��ʹ��rasterize()���model�ı�
};

