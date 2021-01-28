#include "Vec3.h"
#include <vector>
using namespace std;

typedef Vec3f  Color;
class Framebuffer{
    public:
    vector<Color> buffer; // 帧缓存
    int width,hight;
    Framebuffer():width(0),hight(0){};
    Framebuffer(int _width,int _hight):width(_width),hight(_hight){
        buffer.resize(width*hight*3);
    };

    // 设置像素颜色
    void setColor(int x,int y,Color color){
        if(x<0 || x>width)
            return;
        if(y<0 || y>hight)
            return;
        int index=y*width+x;
        buffer[index]=color;
    };
    int getBufferWidth(){
        return width;
    };
    int getBufferHight(){
        return hight;
    };
};