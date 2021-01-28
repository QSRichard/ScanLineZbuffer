#include "Vec3.h"
#include <math.h>
Vec3f operator+(Vec3f left,Vec3f right){
    Vec3f ans=Vec3f();
    ans.x=left.x+right.x;
    ans.y=left.y+right.y;
    ans.z=left.z+right.z;
    return ans;
};
Vec3f operator-(Vec3f left,Vec3f right){
    Vec3f ans=Vec3f();
    ans.x=left.x-right.x;
    ans.y=left.y-right.y;
    ans.z=left.z-right.z;
    return ans;
};
Vec3f operator*(Vec3f v,double num){
    Vec3f ans=Vec3f();
    ans.x=v.x*num;
    ans.y=v.y*num;
    ans.z=v.z*num;
    return ans;
};
Vec3f operator*(double num,Vec3f v){
    Vec3f ans=Vec3f();
    ans.x=v.x*num;
    ans.y=v.y*num;
    ans.z=v.z*num;
    return ans;
};
// Vec3f operator/(Vec3f v,double num){
//     Vec3f ans=Vec3f();
//     ans.x=v.x/num;
//     ans.y=v.y/num;
//     ans.z=v.z/num;
//     return ans;
// };
bool operator==(Vec3f left,Vec3f right){
    if(left.x==right.x&&left.y==right.y&&left.z==right.z)
    return true;
    return false;
};
bool operator!=(Vec3f left,Vec3f right){
    return !(left==right);
};
Vec3f cross(Vec3f left,Vec3f right){
    Vec3f ans=Vec3f();
    ans.x=left.y*right.z-left.z*right.y;
    ans.y=left.z*right.x-left.x*right.z;
    ans.z=left.x*right.y-left.y*right.x;
    return ans;
};
float dot(Vec3f left,Vec3f right){
    return left.x*right.x+left.y*right.y+left.z*right.z;
};
float length(Vec3f v){
    return (float)sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
};
float cosin(Vec3f left,Vec3f right){
    return dot(left,right)/(length(left)*length(right));
};