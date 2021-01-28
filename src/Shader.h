#include "model.h"
#include "Vec3.h"
class Shader{
    public:
    int k;
    Color c;
    Point3d source;
    Shader():k(0.2),c(0.1,0.1,0.1),source(300,400,500){};
    Shader(int _k,Color _c,Point3d p):k(_k),c(_c),source(p){};
    void setModelFaceColor(Model model){
        for(int i=0;i<model.faces.size();i++){
            for(int j=0;j<model.faces[i].verids.size();j++){
                Vec3f light_direction=source-model.vertexs[model.faces[i].verids[j]].point;
                
            }
        }
    };
};