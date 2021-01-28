#include <vector>
#include "utils.h"
#include "Vec3f.h"
#include "Model.h"
#include "Display.h"
#include "ScanLine_zbuffer.h"
#include "Rasterizer.h"

int main(int argc, char** argv) {
	cout << "开始加载模型" << endl;
	cout << "开始加载模型" << endl;
	cout << "开始加载模型" << endl;
	cout << "开始加载模型" << endl;
	cout << "开始加载模型" << endl;
	Model model("C:\\Users\\86173\\Desktop\\ScanLine_zbuffer\\z_buffer\\Project2\\models\\bunny.obj");

	int width = 800, height = 600;
	ScanLine_zbuffer scanline_zbuffer;
	scanline_zbuffer.init(width,height);
	cout << scanline_zbuffer.width << "  " << scanline_zbuffer.height << " scanline_zbuffer 初始化完成" << endl;
	scanline_zbuffer.render(model);
	cout << "render完成 " << endl;
	Display display(&model, &scanline_zbuffer);
	display.run(&argc,argv);
}