#include "Vec3.h"
#include "model.h"
#include "Framebuffer.h"
#include <vector>
#include <math.h>
#include <string>
#include <map>
#include <algorithm>
#include "Shader.h"
using namespace std;
class nodeClassifyEdge{
    public:
    float x; // 边上端点x坐标
    float dx;
    float dy; // 边跨越的扫描线数目
    int id;
    nodeClassifyEdge* next;
};
class nodeClassifyPoly{
    public:
    float a,b,c,d;
    int id;
    int dy;
    Color clolor;
    nodeClassifyPoly* next;
};
class nodeActiveEdge{
    public:
    float x;
    float dx;
    int dy;
    float z,dzx,dzy;
    int id;
    // nodeActiveEdge* next;
};
class nodeActivePoly{
    public:
    float a,b,c,d;
    int id;
    int dy;
    vector<nodeActiveEdge> active_edge;
    nodeActivePoly(nodeClassifyPoly* cur){
        a=cur->a;
        b=cur->b;
        c=cur->c;
        d=cur->d;
        id=cur->id;
        dy=cur->dy;
    };
};
class scan_line_Zbuffer{
    vector<nodeClassifyPoly*> ClassifyPloyTab;
    vector<nodeClassifyEdge*> ClassifyEdgeTab;
    vector<nodeActivePoly> ActivePloyTab;
    // nodeActiveEdge* ActiveEdgeHead;
    Framebuffer& buffer;
    vector<float> depth;
    // 构造函数，并初始化
    scan_line_Zbuffer(Framebuffer buffer){
        int width=buffer.getBufferWidth();
        int hight=buffer.getBufferHight();
        depth=vector<float>(width,-0xffffffff);
        ClassifyPloyTab=vector<nodeClassifyPoly*>(hight,nullptr);
        ClassifyEdgeTab=vector<nodeClassifyEdge*>(hight,nullptr);
    }


    // 建立分类边表和分类多边形表
    void createClassifyTab(Model& model){
        int fnum=model.faces.size();
        for(int i=0;i<fnum;i++){
            nodeClassifyPoly* poly=(nodeClassifyPoly*)malloc(sizeof(nodeClassifyPoly));
            vector<float> Para(4,0.0);
            getPolyPara(Para,model.faces[i],model);
            // 记录分类多边形表节点的属性
            poly->a=Para[0];
            poly->b=Para[1];
            poly->c=Para[2];
            poly->d=Para[3];
            poly->id=i;
            poly->next=nullptr;
            int vnum=model.faces[i].verids.size();
            int y_min=INT_MAX,y_max=INT_MIN;
            //  遍历多边形的各条边
            for(int j=0;j<vnum;j++){
                int id1=model.faces[i].verids[j];
                int id2=model.faces[i].verids[(j+1)%vnum];
                Point3d p1=model.vertexs[id1].point;
                Point3d p2=model.vertexs[id2].point;
                nodeClassifyEdge* edge=(nodeClassifyEdge*)malloc(sizeof(nodeActiveEdge));
                if(p1.y<p2.y){
                    Point3d temp=p1;
                    p1=p2;
                    p2=temp;
                }
                // 更新多边形的最大 y 坐标与最小 y 坐标（取整）
                y_min=min(y_min,round(p2.y));
                y_max=max(y_max,round(p1.y));
                // 记录此条边的属性
                edge->id=i;
                edge->dx=(p1.x-p2.x)/(p1.y-p2.y);
                edge->x=p1.x;
                edge->dy=p1.y-p2.y;
                // 把边指针加入分类边表中
                edge->next=ClassifyEdgeTab[p1.y];
                ClassifyEdgeTab[p1.y]=edge;
            }
            poly->dy=y_max-y_min;
            // 添加多边形的颜色

            // 把指向多边形的指针放入分类多边形链表中
            poly->next=ClassifyPloyTab[y_max];
            ClassifyPloyTab[y_max]=poly;
        }
    };




    bool cmp(nodeActiveEdge first,nodeActiveEdge second){
        if(round(first.x)!=round(second.x)){
            return round(first.x)<round(second.x);
        }
        return first.dx<second.dy;
    };



    void activeNewEdge(int y,nodeActivePoly poly){
        nodeClassifyEdge* cur=ClassifyEdgeTab[y];
        while(cur!=nullptr){
            if(cur->id==poly.id){
                nodeActiveEdge edge;
                edge.id=poly.id;
                edge.x=cur->x;
                edge.dy=cur->dy;
                edge.dx=cur->dx;
                edge.dzx=poly.a/poly.c;
                edge.dzy=poly.a/poly.b;
                poly.active_edge.push_back(edge);
            }
            cur=cur->next;
        }
        sort(poly.active_edge.begin(),poly.active_edge.end(),cmp);
    };


    
    void build(Model& model){
        int fnum=model.faces.size();
        int y=buffer.getBufferHight();
        for(;y>=0;y--){
            // 加入新的激活多边形
            nodeClassifyPoly* poly=ClassifyPloyTab[y];
            while (poly!=nullptr)
            {
                nodeActivePoly p(poly);
                ActivePloyTab.push_back(p);
                poly=poly->next;
            }
            // 加入新的激活边
            for(int i=0;i<ActivePloyTab.size();i++){
                activeNewEdge(y,ActivePloyTab[i]);
            }
            for(int i=0;i<ActivePloyTab.size();i++){
                nodeActivePoly& act_pol=ActivePloyTab[i];
                int e_num=act_pol.active_edge.size();
                if(e_num%2==0){
                    for(int j=0;j<e_num/2;j++){
                        nodeActiveEdge& edge1=act_pol.active_edge[j];
                        nodeActiveEdge& edge2=act_pol.active_edge[(j+1)%e_num];
                        for(int t=round(edge1.x);t<round(edge2.x);t++){
                            edge1.z=edge1.z+edge1.dzx;
                            if(depth[round(t)]<edge1.z){
                                // 更新颜色
                                int width=buffer.getBufferWidth();
                                buffer.setColor(t,y,model.faces[act_pol.id].color);
                            }

                        }
                        // 更新活化边的属性
                        edge1.dy--;
                        edge2.dy--;
                        edge1.x+=edge1.dx;
                        edge2.x+=edge2.dx;
                        edge1.z=edge1.z+edge1.dzy;
                        edge2.z=edge2.z+edge2.dzy;
                    }
                    // 删除已完成边
                    int last=0;
                    for(int j=0;j<e_num;j++){
                        if(act_pol.active_edge[j].dy<0){
                            continue;
                        }
                        act_pol.active_edge[last]=act_pol.active_edge[j];
                        last++;
                    }
                    act_pol.active_edge.resize(last);
                }
                act_pol.dy--;
            }
            // 删除已完成多边形
            int last=0;
            for(int m=0;m<ActivePloyTab.size();m++){
                if(ActivePloyTab[m].dy<0){
                    continue;
                }
                ActivePloyTab[last]=ActivePloyTab[m];
                last++;
            }
            ActivePloyTab.resize(last);
        }
    }
    // // 此函数添加第一次活化的多边形与当前扫描线相交的边进入活化边表
    // void addEdge(int y,nodeActivePoly* poly){
    //     int id=poly->id;
    //     nodeClassifyEdge* cur=ClassifyEdgeTab[y];
    //     while (cur!=nullptr)
    //     {
    //             nodeActiveEdge* edge=(nodeActiveEdge*)malloc(sizeof(nodeActiveEdge));
    //             nodeClassifyEdge* first=nullptr;
    //             nodeClassifyEdge* second=nullptr;
    //             // 寻找多边形的两条边
    //             findEdge(first,id,cur);
    //             findEdge(second,id,cur);
    //             // 记录左边和右边的信息
    //             edge->xl=first->x;
    //             edge->xr=second->x;
    //             edge->dxl=first->dx;
    //             edge->dxr=second->dx;
    //             edge->dyl=first->dy;
    //             edge->dyr=second->dy;
    //             // 记录有关深度的信息 （注意此处是否取整）
    //             edge->zl=-(poly->d+poly->a*first->x+poly->b*y)/poly->c;
    //             edge->dzx=-(poly->a/poly->c);
    //             edge->dzy=(poly->b/poly->c);
    //             edge->id=id;
    //             edge->next=nullptr;
    //             edge->next=ActiveEdgeHead->next;
    //             ActiveEdgeHead->next=edge;
    //     }
    // };

    // // 增量式深度更新
    // void deepth_update(){
    //     int y=buffer.getBufferHight();
    //     for(;y>=0;y--){
    //         // 申请深度数组
    //         vector<float> depth(buffer.getBufferWidth(),-9999999.0);
    //         // 添加首次激活的多边形
    //         if(ClassifyPloyTab[y]!=nullptr){
    //             ActivePoly(y);
    //         }
    //         auto it=ActivePloyHead->next;
    //         auto pre=ActivePloyHead;
    //         // 更新多边形的 dy 当 dy 小于 0 时删除
    //         // 可能会有BUG !!!!!!!!!!!!!!!!
    //         while(it!=nullptr){
    //             it->dy--;
    //             if(it->dy<0){
    //                 pre->next=it->next;
    //                 delete it;
    //                 it=pre->next;
    //             }
    //             else{
    //                 it=it->next;
    //                 pre=pre->next;
    //             }
    //         }
    //         // 添加已激活多边形的激活边
    //         nodeClassifyEdge* cur=ClassifyEdgeTab[y];

    //     }
    // };
    // 寻找多边形的一条边
    // void findEdge(nodeClassifyEdge*&edge,int id,nodeClassifyEdge*&head){
    //     while(head!=nullptr && head->id!=id) head=head->next;
    //     edge=head;
    //     edge->id=-1;
    //     return;
    // };
    // 该函数得到多边形所在平面的一般式方程系数
    void getPolyPara(vector<float>& Para,Face& face,Model& model){
        int num1=face.verids[0];
        int num2=face.verids[1];
        int num3=face.verids[2];
        Vec3f first= model.vertexs[num1].point-model.vertexs[num2].point;
        Vec3f second=model.vertexs[num1].point-model.vertexs[num3].point;
        Para[0]=first.y*second.z-first.z*second.y;
        Para[1]=first.x*second.z-first.z*second.y;
        Para[2]=first.x*second.y-first.y*second.x;
        Para[3]=-(Para[0]*model.vertexs[num1].point.x+Para[1]*model.vertexs[num1].point.y+Para[2]*model.vertexs[num1].point.z);
        return;
    };
    int round(float val){
        return int(val+.5);
    };
};
