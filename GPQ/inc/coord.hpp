#pragma once

//定义基础的类型
//
//


#include "config.hpp"
#include "math.h"





//三维向量
class Vec3 {
    qval vec[3];
public:
    Vec3():vec{ 0, 0, 0 } {}
    Vec3(qval x, qval y, qval z):vec{ x, y, z } {}
    //应用变换矩阵
    void Multiply(TMat& tm);
    inline qval& operator[](quint i) {
        return vec[i];
    }
    //赋值
    inline Vec3& operator=(Vec3& c) {
        vec[0] = c[0];
        vec[1] = c[1];
        vec[2] = c[2];
        return *this;
    }
    //加法
    inline Vec3 operator+(Vec3& c) {
        return Vec3(vec[0] + c[0], vec[1] + c[1], vec[2] + c[2]);
    }
    //减法
    inline Vec3 operator-(Vec3& c) {
        return Vec3(vec[0] - c[0], vec[1] - c[1], vec[2] - c[2]);
    }
    //点乘
    inline qval operator*(Vec3& c) {
        return vec[0] * c[0] + vec[1] * c[1] + vec[2] * c[2];
    }
    //叉乘
    inline Vec3 operator^(Vec3& c) {
        return Vec3(vec[1] * c[2] - vec[2] * c[1], vec[2] * c[0] - vec[0] * c[2], vec[0] * c[1] - vec[1] * c[0]);
    }
    //长度
    inline qval Length() {
        return sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
    }
    //归一化
    inline void Normalize() {
        qval len = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
        vec[0] /= len;
        vec[1] /= len;
        vec[2] /= len;
    }

};

typedef Vec3 Coord;
class Vec2 {
    qval vec[2];
    //qval& x=vec[0],& y=vec[1];
public:
    Vec2():vec{ 0, 0 } {}
    Vec2(qval x, qval y):vec{ x, y } {}
    inline qval& operator[](quint i) {
        return vec[i]; 
    }
    //赋值
    inline Vec2& operator=(Vec2& c) {
        vec[0] = c[0];
        vec[1] = c[1];
        return *this;
    }
    //加法
    inline Vec2 operator+(Vec2& c) {
        return Vec2(vec[0] + c[0], vec[1] + c[1]);
    }
    //减法
    inline Vec2 operator-(Vec2& c) {
        return Vec2(vec[0] - c[0], vec[1] - c[1]);
    }
    //点乘
    inline qval operator*(Vec2& c) {
        return vec[0] * c[0] + vec[1] * c[1];
    }
    //叉乘
    inline qval operator^(Vec2& c) {
        return vec[0] * c[1] - vec[1] * c[0];
    }
    //长度
    inline qval Length() {
        return sqrt(vec[0] * vec[0] + vec[1] * vec[1]);
    }
    //归一化
    inline void Normalize() {
        qval len = sqrt(vec[0] * vec[0] + vec[1] * vec[1]);
        vec[0] /= len;
        vec[1] /= len;
    }
};

//变换矩阵
class TMat {
    qval mat[4][4];
public:
    TMat():mat{
        { 1, 0, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 0, 1 }
    } {}
    TMat(qval a[4][4]):mat{
        { a[0][0], a[0][1], a[0][2], a[0][3] },
        { a[1][0], a[1][1], a[1][2], a[1][3] },
        { a[2][0], a[2][1], a[2][2], a[2][3] },
        { a[3][0], a[3][1], a[3][2], a[3][3] }
    } {}
    //旋转
    void Rotate(qval x, qval y, qval z);
    //平移
    void Translate(qval x, qval y, qval z);
    //缩放
    void Scale(qval x, qval y, qval z);
    //投影
    void Projection(qval d);
    inline qval* operator[](quint i) {
        return mat[i];
    }
    inline TMat& operator=(TMat& a) {
        for (quint i = 0; i < 4; i++) {
            for (quint j = 0; j < 4; j++) {
                mat[i][j] = a[i][j];
            }
        }
        return *this;
    }
};

//摄像机
class Camera {
public:
    //摄像机位置
    Vec3 pos;
    //摄像机视线旋转角度
    Vec3 dir_rol;
    Camera():pos{ 0, 0, 100 }, dir_rol{ 0, 0, 0 } {}
    inline void SetPos(qval x, qval y, qval z) {
        pos[0] = x;
        pos[1] = y;
        pos[2] = z;
    }
    inline void SetDir(qval x, qval y, qval z) {
        dir_rol[0] = x;
        dir_rol[1] = y;
        dir_rol[2] = z;
    }
    //相对于世界坐标系平移
    inline void Translate(qval x, qval y, qval z) {
        pos[0] += x;
        pos[1] += y;
        pos[2] += z;
    }
    //相对于自身旋转
    inline void Rotate_self(qval x, qval y, qval z) {
        dir_rol[0] += x;
        dir_rol[1] += y;
        dir_rol[2] += z;
    }
    //相对于世界坐标系旋转
    inline void Rotate(qval x, qval y, qval z) {
        TMat tm;
        tm.Rotate(x, y, z);
        pos.Multiply(tm);
        Rotate_self(x, y, z);
    }
};

//屏幕
template<quint wid, quint hei>
class Screen {
    qcol Zcol[hei][wid];
    qval Zbuf[hei][wid];
public:
    void Set(void(*set)()) {
        set();
    }
    void Show();
};


//z缓冲
#if color_depth == 24
typedef struct {
    quint col : 24;
}qcol;
//像素点乘法,以字节运算
#define qcol_mul(col,k) (((quint)((((col)>>16)&0xff)*(k))&0xff)<<16) + (((quint)((((col)>>8)&0xff)*(k))&0xff)<<8) + (((quint)(((col)&0xff)*(k))&0xff))
//像素相加
#define qcol_add(col1,col2,col3) (((col1)&0xffffff) + ((col2)&0xffffff) + ((col3)&0xffffff))
#elif color_depth == 16
typedef struct {
    quint16 col : 16;
}qcol;
#define qcol_mul(col,k) (((((col)>>8)&0xff)*(k))&0xff)<<8) + ((((col)&0xff)*(k))&0xff)
#elif color_depth == 8
typedef struct {
    quint8 col : 8;
}qcol;
#define qcol_mul(col,k) ((((col)&0xff)*(k))&0xff)
#endif



void DrawTriangle(Vec3 pos[3], qcol* poscol, bool use_colmix);
void ZbufCls(quint x, quint y);




