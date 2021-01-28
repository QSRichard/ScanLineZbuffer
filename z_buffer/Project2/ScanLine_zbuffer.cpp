#include "ScanLine_zbuffer.h"
#include "Rasterizer.h"
using namespace std;



void ScanLine_zbuffer::creatClassifyTab(Model& model) {
	tab_classify_edge.clear();
	tab_classify_poly.clear();
	tab_classify_poly.resize(height);
	tab_classify_edge.resize(height);
	omp_lock_t mylock;
	omp_init_lock(&mylock);
	int faces_size = model.Model_faces.size();
#pragma omp parallel for //num_threads(NUM_THREADS)
	for (int i = 0; i < model.Model_faces.size(); i++) {
		Face& face = model.Model_faces[i];
		float poly_min_y = FLT_MAX, poly_max_y = FLT_MIN;
		ClassifyPoly* poly = new ClassifyPoly();
		poly->id = i;
		for (int j = 0; j < face.verids.size(); j++) {
			int t1 = face.verids[j];
			int t2 = face.verids[(j + 1) % (int)face.verids.size()];
			Point3d p1 = model.Model_vertexs[t1].point;
			Point3d p2 = model.Model_vertexs[t2].point;
			if (p1.y < p2.y) {
				Point3d temp = p1;
				p1 = p2;
				p2 = temp;
			}
			//assert(p1.y >= 0);//假设模型不超出窗口大小
			assert(round(p1.y) < height);
			ClassifyEdge* edge = new ClassifyEdge();
			edge->dy = (round(p1.y) - round(p2.y));
			if (edge->dy <= 0)continue;//避免不必要的计算，无法计算-1/k

			edge->x = p1.x;
			edge->id = poly->id;
			edge->dx = -(p1.x - p2.x) / (p1.y - p2.y);
			omp_set_lock(&mylock);
			tab_classify_edge[round(p1.y)].push_back(edge);
			omp_unset_lock(&mylock);
			poly_min_y = min(p2.y,poly_min_y);
			poly_max_y = max(p1.y, poly_max_y);
		}
		poly->dy = round(poly_max_y) - round(poly_min_y);
		if (poly->dy > 0 && poly_max_y > 0 && poly_min_y < height) {
			poly->a = face.face_normal.x;
			poly->b = face.face_normal.y;
			poly->c = face.face_normal.z;
			Point3d p = model.Model_vertexs[face.verids[0]].point;
			poly->d = -(poly->a * p.x + poly->b * p.y + poly->c * p.z);
			omp_set_lock(&mylock);
			tab_classify_poly[round(poly_max_y)].push_back(poly);
			omp_unset_lock(&mylock);
		}
	}
}



//static bool cmp(ActiveEdge* left, ActiveEdge* right) {
//	if (round(left->x) != round(right->x)) {
//		return (round(left->x) < round(right->x));
//	}
//	return round(left->dx) < round(right->dy);
//}
//static bool edgeSortCmp(ActiveEdge lEdge, ActiveEdge rEdge)
//{
//	cout << "进入cmp函数" << endl;
//	if (round(lEdge.x) < round(rEdge.x))return true;
//	else if (round(lEdge.x) == round(rEdge.x))
//	{
//		if (round(lEdge.dx) < round(rEdge.dx))
//			return true;
//	}
//	return false;
//}
static bool edgeSortCmp(const ActiveEdge& lEdge, const ActiveEdge& rEdge)
{
	if (round(lEdge.x) < round(rEdge.x))return true;
	else if (round(lEdge.x) == round(rEdge.x))
	{
		if (round(lEdge.dx) < round(rEdge.dx))
			return true;
	}
	return false;
}

void ScanLine_zbuffer::activeEdge(int y, ActivePoly* active_poly) {
	//cout << "进入activeEdge函数中" << endl;
	//cout << tab_classify_edge[y].size() << endl;
	for (int i = 0; i < tab_classify_edge[y].size(); i++) {
		if (tab_classify_edge[y][i]->id == active_poly->id) {

			ActiveEdge edge;
			edge.x = tab_classify_edge[y][i]->x;
			edge.dx = tab_classify_edge[y][i]->dx;
			edge.dy = tab_classify_edge[y][i]->dy;

			if (isEqual(active_poly->c, 0))
			{
				edge.z = 0;
				edge.dzx = 0;
				edge.dzy = 0;
			}
			else
			{
				edge.z = -(active_poly->d +
					active_poly->a * tab_classify_edge[y][i]->x +
					active_poly->b * y) / active_poly->c;
				edge.dzx = -(active_poly->a / active_poly->c);
				edge.dzy = active_poly->b / active_poly->c;
			}
			active_poly->activeEdges.push_back(edge);
			tab_classify_edge[y][i]->id = -1;
		}
	}
	//cout << "开始排序" << endl;
	sort(active_poly->activeEdges.begin(), active_poly->activeEdges.end(), edgeSortCmp);
	//cout << "排序完成" << endl;
}
void ScanLine_zbuffer::render(Model& model) {

	Rasterizer raster;
	raster.setSize(width, height);
	raster.run(model);

	creatClassifyTab(model);

	//cout << "分类表建立完成" << endl;
	for (int y = height - 1; y >= 0; y--) {
		for (int m = 0; m < tab_classify_edge.size(); m++) {
			//cout << tab_classify_edge[m].size() << endl;
		}
		Color_id[y] = vector<int>(width, -1);
		fill(z_buffer.begin(), z_buffer.end(), -0xfffffff);
#pragma omp parallel for //num_threads(NUM_THREADS)
		for (auto it = tab_classify_poly[y].begin(); it != tab_classify_poly[y].end(); it++) {
			ActivePoly* polyuu = new ActivePoly();
			polyuu->a = (*it)->a;
			polyuu->b = (*it)->b;
			polyuu->c = (*it)->c;
			polyuu->d = (*it)->d;
			polyuu->id = (*it)->id;
			polyuu->dy = (*it)->dy;
			tab_active_poly.push_back(polyuu);
		}
		//cout << "活化多边形已经加入" << endl;
		for (int i = 0; i < tab_active_poly.size(); i++) {
			ActivePoly* poly = tab_active_poly[i];
			//cout << "多边形已经拿到" << endl;
			activeEdge(y, poly);
			//cout << "边已更新" << endl;
			vector<ActiveEdge>& active_edges = poly->activeEdges;
			assert(active_edges.size() % 2 == 0);
			for (int j = 0; j < active_edges.size(); ++j) {
				ActiveEdge& edge1 = active_edges[j];
				ActiveEdge& edge2 = active_edges[++j];
				float zx = edge1.z;
				//cout << "边已取出" << endl;
				for (int x = round(edge1.x), end = round(edge2.x); x < end; ++x) {
					if (zx > z_buffer[x]) {
						z_buffer[x] = zx;
						Color_id[y][x] = poly->id;
					}
					zx += edge1.dzx;
				}
				--edge1.dy;
				--edge2.dy;
				edge1.x += edge1.dx;
				edge2.x += edge2.dx;
				edge1.z += edge1.dzx * edge1.dx + edge1.dzy;
				edge2.z += edge2.dzx * edge2.dx + edge2.dzy;
			}
			//cout << "活化边已经更新" << endl;
			int last = 0;
			int aesize = active_edges.size();
			for (int t = 0; t < aesize; ++t, ++last)
			{
				while (active_edges[t].dy <= 0)
				{
					++t;
					if (t >= aesize)break;
				}
				if (t >= aesize)break;
				active_edges[last] = active_edges[t];
			}
			//cout << "移除已经结束的活化边" << endl;
			active_edges.resize(last);
			--poly->dy;//活化多边形扫描线向下移动
		}
		int last = 0;
		for (int i = 0, len = tab_active_poly.size(); i < len; ++i, ++last)
		{
			while (tab_active_poly[i]->dy < 0)
			{
				++i;
				if (i >= len) break;
			}
			if (i >= len) break;

			tab_active_poly[last] = tab_active_poly[i];
		}
		tab_active_poly.resize(last);
	}
}
void ScanLine_zbuffer::init(int width, int height) {
	this->width = width;
	this->height = height;
	for (int i = 0; i < height; i++) {
		vector<int> temp;
		temp.resize(width);
		fill(temp.begin(), temp.end(), -1);
		this->Color_id.push_back(temp);
	}
	z_buffer.resize(width);
	fill(z_buffer.begin(), z_buffer.end(), FLT_MIN);
}
void ScanLine_zbuffer::getSize(int& width, int& height) {
	width = this->width;
	height = this->height;
	return;
}
ScanLine_zbuffer::ScanLine_zbuffer() {};