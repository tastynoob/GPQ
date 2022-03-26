#pragma once
#include "mam.hpp"

//摄像机默认的z坐标
#define camera_default_z 100


//屏幕宽
#define screen_width 800
//屏幕高
#define screen_height 600
//色深(24/16/8)
#define color_depth 24

//屏幕分割模式(有助于屏幕刷新速度的提升)
#define screen_splitmode_00 0//不分割
#define screen_splitmode_22 1//2x2分割(宽高)
#define screen_splitmode_42 2//4x2分割

#define screen_splitmode screen_splitmode_00



//用于坐标计算
typedef float qval;
typedef unsigned char quint8;
typedef unsigned short quint16;
typedef unsigned int quint;



inline void fill_color(quint x, quint y, quint w, quint h, qcol* col) {
    
}