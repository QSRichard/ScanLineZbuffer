#include <vector>
#include <iostream>
#include "model.h"
#include "Framebuffer.h"
using namespace std;


class nodePyramid{
    public:
    int x_l,x_r;
    int y_b,y_t;
    float z_min;
    nodePyramid* p;
    bool isLeave;
    // 四个孩子节点指针，顺序为 左上 右上 左下 右下
    vector<nodePyramid*> c;
    nodePyramid(int l,int r,int b,int t,nodePyramid* p){
        x_l=l,x_r=r;
        y_b=b,y_t=t;
        p=p;
        c=vector<nodePyramid*>(4,nullptr);
        z_min=FLT_MIN;
        isLeave=true;
    };
};

// 包围盒
class AABB{
    public:
    int xl,xr;
    int yb,yt;
    int id; // 多边形的编号
    float z_max;
    AABB():xl(0),xr(0),yb(0),yt(0),z_max(FLT_MIN){};
    AABB(int _xl,int _xr,int _yb,int _yt,float _z_max,int id):xl(_xl),xr(_xr),yb(_yb),yt(_yt),z_max(_z_max),id(_id){};
};

class Pyramid{
    public:
    nodePyramid* root;
    vector<int> mark_color;
    int width,hight;

    // 一层一层的深度测试，还是直接找到底层节点？？？？？
    // // 找到能包围多边形的最小（底层）Pyramid 节点 (或许会有BUG)
    // nodePyramid* find_node(AABB& aabb,nodePyramid* root){
    //     if(root->isLeave)
    //         return root;        
    //     int x_mid=(root->x_l+root->x_r)/2;
    //     int y_mid=(root->y_b+root->y_t)/2;
    //     if((aabb.xl<x_mid && aabb.xr>x_mid) || (aabb.yb<y_mid && aabb.yt>y_mid))
    //         return root;
    //     if(aabb.xl>=root->x_l && aabb.xr<=x_mid && aabb.yb>=y_mid &&aabb.yt<=root->y_t)
    //         return find_node(aabb,root->c[0]);
    //     else if(aabb.xl>=x_mid && aabb.yb>=y_mid && aabb.xr<=root->x_r && aabb.yt<=root->y_t)
    //         return find_node(aabb,root->c[1]);
    //     else if(aabb.xl>=root->x_l && aabb.xr<=x_mid && aabb.yb>=root->y_b && aabb.yt<=y_mid)
    //         return find_node(aabb,root->c[2]);
    //     else if(aabb.xl>=x_mid && aabb.xr<=root->x_r && aabb.yb>=root->y_b && aabb.yt<=y_mid)
    //         return find_node(aabb,root->c[3]);
    //     return nullptr;
    // }
    
    nodePyramid* bulid_Pyramid(int l,int r,int b,int t,nodePyramid* root){
        if(l>r || b>t)
            return nullptr;
        if(l==r-1 && b==t-1){
            nodePyramid* p=new nodePyramid(l,r,b,t,root);
            return p;
        }
        // 一列
        if(l==r+1){
            int y_mid=(b+t)/2;
            nodePyramid* p=new nodePyramid(l,r,b,t,root);
            p->isLeave=false;
            p->c[0]=bulid_Pyramid(l,r,y_mid,t,p);
            p->c[2]=bulid_Pyramid(l,r,b,y_mid,p);
            return p;
        }
        // 一行
        else if(b==t+1){
            int x_mid=(l+r)/2;
            nodePyramid* p=new nodePyramid(l,r,b,t,root);
            p->isLeave=false;
            p->c[1]=bulid_Pyramid(l,x_mid,b,t,p);
            p->c[3]=bulid_Pyramid(x_mid,r,b,t,p);
            return p;
        }
        else{
            int x_mid=(l+r)/2;
            int y_mid=(b+t)/2;
            nodePyramid* p=new nodePyramid(l,r,b,t,root);
            p->isLeave=false;
            p->c[0]=bulid_Pyramid(l,x_mid,y_mid,t,p);
            p->c[1]=bulid_Pyramid(x_mid,r,y_mid,t,p);
            p->c[2]=bulid_Pyramid(l,x_mid,b,y_mid,p);
            p->c[3]=bulid_Pyramid(x_mid,r,b,y_mid,p);
            return p;
        }
    };


    bool isUpdate(AABB& aabb,nodePyramid* root);


    nodePyramid* get_update_node(int xl,int yl);

    float down_update(nodePyramid* root){
        if(root->isLeave)
            return root->z_min;
        if(root->c[0]!=nullptr)
            root->z_min=min(root->z_min,down_update(root->c[0]));
        if(root->c[1]!=nullptr)
            root->z_min=min(root->z_min,down_update(root->c[1]));
        if(root->c[2]!=nullptr)
            root->z_min=min(root->z_min,down_update(root->c[2]));
        if(root->c[3]!=nullptr)
            root->z_min=min(root->z_min,down_update(root->c[3]));
        return root->z_min;
    }



    AABB get_aabb(vector<Vertex>& vertexs,Face& face){
        vector<int>& ids=face.verids;
        int xl=round(vertexs[ids[0]].point.x);
        int xr=xl;
        int yb=round(vertexs[ids[0]].point.y);
        int yt=yb;
        float z_max=vertexs[ids[0]].point.z;
        for(int i=1;i<ids.size();i++){
            xl=min(xl,round(vertexs[ids[i]].point.x));
            xr=max(xr,round(vertexs[ids[i]].point.x));
            yb=min(yb,round(vertexs[ids[i]].point.y));
            yt=max(yt,round(vertexs[ids[i]].point.y));
        }
        return AABB(xl,xr,yb,yt,z_max,face.id);
    }


    void up_update(nodePyramid* partent);

    
    int round(float val){
        return (int)val+.5;
    }
};
