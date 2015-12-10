#pragma once
#include <graphics.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
using namespace std;

//..........................Point API..........................//

struct point3d {
    double x;
    double y;
    double z;
public :
    point3d operator*( double u ) {
        point3d npt = {u * x, u * y, u * z};
        return npt;
    }
    point3d operator/( double u ) {
        point3d npt = { x/u , y/u , z/u };
        return npt;
    }
    point3d operator+( point3d b ) {
        point3d npt = {b.x + x, b.y + y, b.z + z};
        return npt;
    }
    point3d operator-( point3d b ) {
        point3d npt = {x - b.x, y - b.y, z - b.z};
        return npt;
    }

    point3d operator%( point3d b ) {
        point3d npt;
        npt.x = y * b.z - z * b.y;
        npt.y = z * b.x - x * b.z;
        npt.z = x * b.y - y * b.x;
        return npt;
    }
    void print() { cout << "(" << x << ", " << y << ", " << z << ")" << endl; }
    void normalize() { float s = sqrt(x*x + y*y + z*z); x = x/s, y = y/s, z = z/s; }
    double operator*( point3d b ) { return x*b.x + y*b.y + z*b.z; }
    void draw(int color) { putpixel(x, y, color); }
};

struct ptcl : public point3d {
    point3d c;
};

bool sortbyy( point3d a, point3d b ) {
    return ( a.y < b.y );
}

bool sortbyx( point3d a, point3d b ) {
    return ( a.x < b.x );
}

//.....................Transform API ................................ //

class TMatrix {
    float mat[4][4];
public :
    TMatrix( float m[][4] ) {
        for( int i = 0; i < 4; i++ )
            for( int j = 0; j < 4; j++ )
                mat[i][j] = m[i][j];
    }
    TMatrix operator*( TMatrix m ) {
        float n[4][4] = { { 0 } };
        for( int i = 0; i < 4; i++ )
            for( int j = 0; j < 4; j++ )
                for( int k = 0; k < 4; k++ )
                    n[i][j] += mat[i][k] * m.mat[k][j];
        return TMatrix( n );
    }
    TMatrix print() {
        for( int i = 0; i < 4; i++ ) {
            for( int j = 0; j < 4; j++ ) {
                cout << mat[i][j] << "\t";
            }
            cout << endl;
        }
    }
    point3d operator*( point3d a );
};

//..............................Basic Transformations.................................//

point3d TMatrix::operator*( point3d a ) {
        point3d b;
        b.x = mat[0][0] * a.x + mat[0][1] * a.y + mat[0][2] * a.z + mat[0][3];
        b.y = mat[1][0] * a.x + mat[1][1] * a.y + mat[1][2] * a.z + mat[1][3];
        b.z = mat[2][0] * a.x + mat[2][1] * a.y + mat[2][2] * a.z + mat[2][3];
        return b;
    }

TMatrix translate( double x, double y, double z ) {
    float m[4][4] = { {1, 0, 0, x}, {0, 1, 0, y}, {0, 0, 1, z}, {0, 0, 0, 1} };
    return TMatrix( m );
}
TMatrix rotate( float u, float v, float w, float a ) {
    float norm = sqrt(u*u + v*v + w*w);
    u = u/norm, v = v/norm, w = w/norm;
    a *= 2 * 3.14 / 360;
    float m[4][4] = { { u*u + (1 - u*u) * cos(a), u*v*(1 - cos(a)) - w*sin(a), u*w*(1 - cos(a)) + v*sin(a), 0 },
                    { u*v*(1 - cos(a)) + w*sin(a), v*v + (1 - v*v) * cos(a), v*w*(1 - cos(a)) - u*sin(a), 0},
                    {u*w*(1 - cos(a)) - v*sin(a), w*v*(1 - cos(a)) + u*sin(a), w*w + (1 - w*w) * cos(a), 0},
                    {0, 0, 0, 1} };
    return TMatrix( m );
}
TMatrix scale( float sx, float sy , float sz) {
    float m[4][4] = { {sx, 0, 0, 0}, {0, sy, 0, 0}, {0, 0, sz, 0}, {0, 0, 0, 1} };
    return TMatrix( m );
}

void transpoly(point3d a[], int n, TMatrix trans) {
    for( int i = 0; i < n; i++ ) {
        a[i] = trans * a[i];
    }
}
