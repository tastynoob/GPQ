#include "coord.hpp"
#include "tool.hpp"

//中心投影
//认为摄像机位于(0,0,d)的位置
Vec3 Project(Vec3& a, qval d) {
    Vec3 n;
    n[0] /= (1 - a[2] / d);
    n[1] /= (1 - a[2] / d);
    n[2] /= (1 - a[2] / d);
    return n;
}

//矩阵乘法
void Mat4Mul(TMat& a, TMat& b, TMat& c) {
    TMat temp;
    for (quint i = 0; i < 4; i++) {
        for (quint j = 0; j < 4; j++) {
            temp[i][j] = a[i][0] * b[0][j] + a[i][1] * b[1][j] + a[i][2] * b[2][j] + a[i][3] * b[3][j];
        }
    }
    c = temp;
}

//绕x,y,z轴旋转
void TMat::Rotate(qval x, qval y, qval z) {
    //绕x轴旋转
    float ax[4][4]{
        { 1, 0, 0, 0 },
        { 0, cos(x), -sin(x), 0 },
        { 0, sin(x), cos(x), 0 },
        { 0, 0, 0, 1 }
    };
    //绕y轴旋转
    float ay[4][4]{
        { cos(y), 0, sin(y), 0 },
        { 0, 1, 0, 0 },
        { -sin(y), 0, cos(y), 0 },
        { 0, 0, 0, 1 }
    };
    //绕z轴旋转
    float az[4][4]{
        { cos(z), -sin(z), 0, 0 },
        { sin(z), cos(z), 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 0, 1 }
    };
    TMat tempx(ax);
    TMat tempy(ay);
    TMat tempz(az);
    TMat temp;
    Mat4Mul(tempx, tempy, temp);
    Mat4Mul(temp, tempz, temp);
    Mat4Mul(temp, *this, *this);
}

//平移
void TMat::Translate(qval x, qval y, qval z) {
    float a[4][4]{
        { 1, 0, 0, x },
        { 0, 1, 0, y },
        { 0, 0, 1, z },
        { 0, 0, 0, 1 }
    };
    TMat temp(a);
    Mat4Mul(temp, *this, *this);
}
//缩放
void TMat::Scale(qval x, qval y, qval z) {
    float a[4][4]{
        { x, 0, 0, 0 },
        { 0, y, 0, 0 },
        { 0, 0, z, 0 },
        { 0, 0, 0, 1 }
    };
    TMat temp(a);
    Mat4Mul(temp, *this, *this);
}

//中心投影
//认为摄像机位于(0,0,d)的位置
void TMat::Projection(qval d) {
    float a[4][4]{
        { 1, 0, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, -1/d, 1}
    };
    TMat temp(a);
    Mat4Mul(temp, *this, *this);
}

void Vec3::Multiply(TMat& tm) {
    qval x = vec[0] * tm[0][0] + vec[1] * tm[0][1] + vec[2] * tm[0][2] + tm[0][3];
    qval y = vec[0] * tm[1][0] + vec[1] * tm[1][1] + vec[2] * tm[1][2] + tm[1][3];
    qval z = vec[0] * tm[2][0] + vec[1] * tm[2][1] + vec[2] * tm[2][2] + tm[2][3];
    qval w = vec[0] * tm[3][0] + vec[1] * tm[3][1] + vec[2] * tm[3][2] + tm[3][3];
    vec[0] = x / w;
    vec[1] = y / w;
    vec[2] = z / w;
}

//z缓冲
qcol Zcol[screen_height][screen_width];
qval Zbuf[screen_height][screen_width];

//清楚某一点的z缓冲
void ZbufCls(quint x, quint y) {
    Zcol[y][x].col = 0;
    Zbuf[y][x] = 0;
}


//利用重心坐标判断点是否在三角形内部
bool barycentric(Vec3& a, Vec3& b, Vec3& c, Vec2& p) {
    Vec3 _x(b[0] - a[0], c[0] - a[0], a[0] - p[0]);
    Vec3 _y(b[1] - a[1], c[1] - a[1], a[1] - p[1]);

    Vec3 u = _x ^ _y;

    if ((u[2] < 1) && (u[2] > -1)) return false;
    Vec3 un(1.0 - (u[0] + u[1]) / u[2], u[0] / u[2], u[1] / u[2]);
    return (un[0] >= 0) && (un[1] >= 0) && (un[2] >= 0);

}

//利用叉乘判断点是否在三角形内部
//vec3,vec3,vec3,vec2
#define crossProduct(a, b, c, p) \
    Vec2 pa(a[0] - p[0], a[1] - p[1]);\
    Vec2 pb(b[0] - p[0], b[1] - p[1]);\
    Vec2 pc(c[0] - p[0], c[1] - p[1]);\
    bool k1 = (pa ^ pb) >= 0;\
    bool k2 = (pb ^ pc) >= 0;\
    bool k3 = (pc ^ pa) >= 0;\
    avalid = (k1 && k2 && k3) || (!(k1||k2||k3));


//在zbuff中绘制三角形
//poscol:三个顶点的颜色(如果不使用颜色混合则使用第一个顶点的颜色)
//use_colmix:是否使用颜色混合
void DrawTriangle(Vec3 pos[3], qcol* poscol = nullptr, bool use_colmix = false) {
    qval x1, y1, z1, x2, y2, z2, x3, y3, z3;
    x1 = pos[0][0] + screen_width / 2;
    y1 = screen_height / 2 - pos[0][1];
    z1 = pos[0][2];
    x2 = pos[1][0] + screen_width / 2;
    y2 = screen_height / 2 - pos[1][1];
    z2 = pos[1][2];
    x3 = pos[2][0] + screen_width / 2;
    y3 = screen_height / 2 - pos[2][1];
    z3 = pos[2][2];

    Vec3 npos[3];
    npos[0][0] = x1;
    npos[0][1] = y1;
    npos[0][2] = z1;
    npos[1][0] = x2;
    npos[1][1] = y2;
    npos[1][2] = z2;
    npos[2][0] = x3;
    npos[2][1] = y3;
    npos[2][2] = z3;

    qval minX = min(x1, min(x2, x3));
    minX = minX < 0 ? 0 : minX;
    qval maxX = max(x1, max(x2, x3));
    maxX = maxX > screen_width ? screen_width : maxX;

    qval minY = min(y1, min(y2, y3));
    minY = minY < 0 ? 0 : minY;
    qval maxY = max(y1, max(y2, y3));
    maxY = maxY > screen_height ? screen_height : maxY;

    //求平面方程
    qval ak, bk, ck, dk;
    // A = (y2 - y1)(z3 - z1) -  (z2 - z1)(y3 - y1);
    ak = (npos[1][1] - npos[0][1]) * (npos[2][2] - npos[0][2]) - (npos[1][2] - npos[0][2]) * (npos[2][1] - npos[0][1]);
    // B = (x3 - x1)(z2 - z1) -  (z3 - z1)(x2 - x1);
    bk = (npos[2][0] - npos[0][0]) * (npos[1][2] - npos[0][2]) - (npos[2][2] - npos[0][2]) * (npos[1][0] - npos[0][0]);
    // C = (x2 - x1)(y3 - y1) -  (y2 - y1)(x3 - x1);
    ck = (npos[1][0] - npos[0][0]) * (npos[2][1] - npos[0][1]) - (npos[1][1] - npos[0][1]) * (npos[2][0] - npos[0][0]);
    // D = -(A * x1 + B * y1 + C * z1);
    dk = -(ak * npos[0][0] + bk * npos[0][1] + ck * npos[0][2]);

    //屏幕分割
#if screen_splitmode == screen_splitmode_00
    for (quint py = minY;py < maxY;py++) {
        for (quint px = minX;px < minX;px++) {
            Vec2 p_(px, py);
            //判断点是否在三角形内部
            bool avalid;
            crossProduct(npos[0], npos[1], npos[2], p_);
            if (avalid) {
                //计算p点的深度
                qval pz = -(ak * px + bk * py + dk) / ck;
                //判断深度是否在视锥体内
                if (pz > Zbuf[py][px]) {
                    //计算深度
                    Zbuf[py][px] = pz;
                    //计算颜色
                    if (use_colmix) {
                        //计算比例,以p点距离其他三个点的距离比例
                        qval ap_d = pa.Length();
                        qval bp_d = pb.Length();
                        qval cp_d = pc.Length();
                        qcol acol = { poscol[0].col };
                        acol.col = qcol_mul(acol.col, ap_d / ((ap_d + bp_d + cp_d)));
                        qcol bcol = { poscol[1].col };
                        bcol.col = qcol_mul(bcol.col, bp_d / ((ap_d + bp_d + cp_d)));
                        qcol ccol = { poscol[2].col };
                        ccol.col = qcol_mul(ccol.col, cp_d / ((ap_d + bp_d + cp_d)));
                        qcol pcol;
                        pcol.col = qcol_add(acol.col, bcol.col, ccol.col);
                        Zcol[py][px] = pcol;
                    }
                    else {
                        Zcol[py][px] = poscol[0];
                    }
                }
            }
        }
    }
#endif

}

