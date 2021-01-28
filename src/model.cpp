#include "model.h"
using namespace std;


bool Model::load_model(ifstream& in){
    string type;
    while(in>>type){
        if(type=="v"){
            Vertex t;
            in>>t.point.x>>t.point.y>>t.point.z;
            vertexs.push_back(t);
        }
        else if(type=="vn"){
            Vec3f v;
            in>>v.x>>v.y>>v.z;
            vertex_normal.push_back(v);
        }
        else if(type=="f"){
            
        }
    }
}