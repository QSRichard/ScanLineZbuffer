class Vec3f{
    public: 
    float x,y,z;
    // 构造函数
    Vec3f():x(0.0),y(0.0),z(0.0){};
    Vec3f(float _x,float _y,float _z):x(_x),y(_y),z(_z){};
    // 拷贝构造函数
    Vec3f(const Vec3f& t):x(t.x),y(t.y),z(t.z){};
    Vec3f& operator=(Vec3f t){
        x=t.x,y=t.y,z=t.z;
        return *this;
    }
    Vec3f& operator+=(Vec3f t){
        x=t.x+x,y=t.y+y,z=t.z+z;
        return *this;
    }
    Vec3f& operator-=(Vec3f t){
        x=x-t.x,y=y-t.y,z=t.z-z;
        return *this;
    };
};
typedef Vec3f Point3d;
typedef Vec3f Color;
Vec3f operator+(Vec3f left,Vec3f right);
Vec3f operator-(Vec3f left,Vec3f right);
Vec3f operator*(Vec3f v,float num);
Vec3f operator*(float num,Vec3f v);
Vec3f operator/(Vec3f v,double num);
bool operator==(Vec3f left,Vec3f right);
bool operator!=(Vec3f left,Vec3f right);
Vec3f cross(Vec3f left,Vec3f right);
float dot(Vec3f left,Vec3f right);
float cosin(Vec3f left,Vec3f right);
float length(Vec3f v);

