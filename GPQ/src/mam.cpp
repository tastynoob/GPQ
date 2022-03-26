#include "mam.hpp"
//核心代码

//全局摄像机
Camera camera;

//使用摄像头获取世界坐标到屏幕坐标的转换矩阵
void Get_Camera_TMat(Camera& camera,TMat& tm_out) {
    
}




//在渲染三角形时,需要相对摄像头的位置进行移动
//先变换,再仿射
void Triangle::Render(TMat& tm, bool use_mix=false, qcol* poscol) {

    Vec3 nv[3] = { v[0], v[1], v[2] };//拷贝一份作为渲染顶点缓存
    //渲染变换
    //相对于摄像机进行变换投影
    for (quint i = 0;i < 3;i++) {
        nv[i].Multiply(tm);
    }
    DrawTriangle(nv, poscol, use_mix);
}













