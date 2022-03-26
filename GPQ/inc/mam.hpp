#pragma once

//定义基础算法
//三角形绘制


#include "config.hpp"
#include "coord.hpp"
#define angle(x)    ((x/180.0)*3.1415926)

extern Camera camera;


class Triangle {
public:
    Coord v[3];
    Triangle(Coord a, Coord b, Coord c): v{ a, b, c } {}
    //渲染三角形
    //参数:色彩混合,顶点颜色,渲染变换矩阵
    void Render(TMat& tm, bool use_mix, qcol* poscol);
};


//所有3d物体的基类
class GPobj {
    //包含的三角形
    Triangle* tri;
    //三角形数量
    quint tri_num;
public:
    GPobj() {}
    //平移
    void Translate(qval x, qval y, qval z);
    //相对于世界坐标系旋转
    void Rotate(qval x, qval y, qval z);
    //相对于某一个轴旋转
    void Rotate(Vec3 axis,qval angle);
    //缩放
    void Scale(qval x, qval y, qval z);
    //变换
    void Multiply(TMat& tm);
    //渲染
    void Render(TMat& tm, bool use_mix, qcol* poscol);
};





//GPQ 3d渲染执行器
//tick:每一帧的时间
void GPQ_tick(quint tick);





