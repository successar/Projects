#include <graphics.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <algorithm>
using namespace std;

struct point {
    double x;
    double y;
    double z;
};

class TMatrix {
    float mat[4][4];
public :
    TMatrix( float m[][4] ) {
        for( int i = 0; i < 4; i++ )
            for( int j = 0; j < 4; j++ )
                mat[i][j] = m[i][j];

        //this->print();
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
    point operator*( point a ) {
        point b;
        b.x = mat[0][0] * a.x + mat[0][1] * a.y + mat[0][2] * a.z + mat[0][3];
        b.y = mat[1][0] * a.x + mat[1][1] * a.y + mat[1][2] * a.z + mat[1][3];
        b.z = mat[2][0] * a.x + mat[2][1] * a.y + mat[2][2] * a.z + mat[2][3];
        return b;
    }
};

TMatrix translate( float x, float y, float z ) {
    float m[4][4] = { {1, 0, 0, x}, {0, 1, 0, y}, {0, 0, 1, z}, {0, 0, 0, 1} };
    return TMatrix( m );
}
TMatrix rotate( float u, float v, float w, float a ) {
    float norm = sqrt( u * u + v * v + w * w );
    u = u / norm, v = v / norm, w = w / norm;
    a *= 2 * 3.14 / 360;
    float m[4][4] = { { u*u + ( 1 - u * u ) * cos( a ), u*v*( 1 - cos( a ) ) - w * sin( a ), u*w*( 1 - cos( a ) ) + v * sin( a ), 0 },
        { u*v*( 1 - cos( a ) ) + w * sin( a ), v*v + ( 1 - v * v ) * cos( a ), v*w*( 1 - cos( a ) ) - u * sin( a ), 0},
        {u*w*( 1 - cos( a ) ) - v * sin( a ), w*v*( 1 - cos( a ) ) + u * sin( a ), w*w + ( 1 - w * w ) * cos( a ), 0},
        {0, 0, 0, 1}
    };
    return TMatrix( m );
}
TMatrix scale( float sx, float sy , float sz ) {
    float m[4][4] = { {sx, 0, 0, 0}, {0, sy, 0, 0}, {0, 0, sz, 0}, {0, 0, 0, 1} };
    return TMatrix( m );
}

void transpoly( point a[], int n, TMatrix trans ) {
    for( int i = 0; i < n; i++ ) {
        a[i] = trans * a[i];
    }
}

void triangle( point xa[], int n, double a, double b, double c ) {

    double x = 0.1;
    double y = 0;
    double z = 0;
    double t = 0.01;
    int lorenzIterationCount = n;
    int i;

    for ( i = 0; i < lorenzIterationCount; i++ ) {
        double xt = x + t * a * ( y - x );
        double yt = y + t * ( x * ( b - z ) - y );
        double zt = z + t * ( x * y - c * z );
        x = xt;
        y = yt;
        z = zt;
        xa[i].x = x;
        xa[i].y = y;
        xa[i].z = z;
        //cout << x << " " << y << " " << z << endl;
    }

}
void drawcube(point cube[]) {
    for(int i = 0; i < 4; i++) {
        setcolor(GREEN);
        line(cube[i + 4].x, cube[i + 4].y, cube[(i + 1)%4 + 4].x, cube[(i + 1)%4 + 4].y);
        line(cube[i].x, cube[i].y, cube[i + 4].x, cube[i + 4].y);
        line(cube[i].x, cube[i].y, cube[(i + 1)%4].x, cube[(i + 1)%4].y);
    }
}

void drawcoord(point coord[]) {
    setcolor(RED);
    line(coord[0].x, coord[0].y, coord[1].x, coord[1].y);
    line(coord[0].x, coord[0].y, coord[2].x, coord[2].y);
    line(coord[0].x, coord[0].y, coord[3].x, coord[3].y);
}

int main() {
    int gd = DETECT, gm;
    //initgraph( &gd, &gm, "C:\\TC\\BGI" );
    int win = initwindow(900, 700, "Lorenz", 0, 0);
    setcurrentwindow(win);
    setbkcolor(WHITE);
    setcolor( BLUE );
    int ch = KEY_UP;
    int chs[6] = { KEY_UP, KEY_PGUP, KEY_LEFT };

    int h = 0, n = 4000;

    point pa[10000], xa[10000];
    triangle( pa, n, 10.0, 10.0, 8.0/3.0 );
    float xmin = pa[0].x, xmax = pa[0].x, ymin = pa[0].y, ymax = pa[0].y, zmin = pa[0].z, zmax = pa[0].z;
    for( int i = 0; i < n; i++ ) {
        if( pa[i].x < xmin ) xmin = pa[i].x;
        if( pa[i].x > xmax ) xmax = pa[i].x;
        if( pa[i].y < ymin ) ymin = pa[i].y;
        if( pa[i].y > ymax ) ymax = pa[i].y;
        if( pa[i].z < zmin ) zmin = pa[i].z;
        if( pa[i].z > zmax ) zmax = pa[i].z;
    }
    float xc = ( xmax + xmin ) / 2, yc = ( ymax + ymin ) / 2, zc = ( zmax + zmin ) / 2;

    point cube[8] = { { xmin, ymin, zmin}, { xmax, ymin, zmin}, {xmax, ymax, zmin},  { xmin, ymax, zmin },
                        { xmin, ymin, zmax}, { xmax , ymin, zmax}, {xmax, ymax, zmax}, { xmin, ymax, zmax} };
    point cubea[8];

    point coord[4] = { {xc, yc, zc} , {xc + 10, yc, zc}, {xc, yc + 10, zc}, {xc, yc, zc + 10} };
    point coorda[4];
    int page = 0;
    int c = BLUE;
    while( true ) {
        setactivepage(page);
        setvisualpage(1 - page);
        clearviewport();
        ch = (h == 0) ? chs[(rand() % 10000) % 3] : ch;
        h = (h + 1) % 5;
        int xr = 0, yr = 0, zr = 0, a = 0;
        if( ch == KEY_UP ) xr = 1, a = 10;
        else if( ch == KEY_DOWN ) xr = 1, a = -10;
        else if( ch == KEY_LEFT ) yr = 1, a = 10;
        else if( ch == KEY_RIGHT ) yr = 1, a = -10;
        else if( ch == KEY_PGUP ) zr = 1, a = 10;
        else if( ch == KEY_PGDN ) zr = 1, a = -10;
        transpoly( pa, n, translate( xc, yc, zc ) * rotate( xr, yr, zr, a ) * translate( -xc, -yc, -zc ) );
        transpoly( cube, 8, translate( xc, yc, zc ) * rotate( xr, yr, zr, a ) * translate( -xc, -yc, -zc ) );
        transpoly( coord, 4, translate( xc, yc, zc ) * rotate( xr, yr, zr, a ) * translate( -xc, -yc, -zc ) );
        for( int j = 0; j < n; j++ ) xa[j] = pa[j];
        for( int j = 0; j < 8; j++ ) cubea[j] = cube[j];
        for( int j = 0; j < 4; j++ ) coorda[j] = coord[j];
        transpoly( xa, n, translate( xc, yc, zc ) * scale( 5, 5, 5 ) * translate( -xc, -yc, -zc ) );
        transpoly( cubea, 8, translate(350, 200, 0) * translate( xc, yc, zc ) * scale( 5, 5, 5 ) * translate( -xc, -yc, -zc ) );
        transpoly( coorda, 4, translate(350, 200, 0) * translate( xc, yc, zc ) * scale( 5, 5, 5 ) * translate( -xc, -yc, -zc ) );

        point cen = { xc, yc, zc };
        cen = translate( xc, yc, zc ) * rotate( xr, yr, zr, a ) * translate( -xc, -yc, -zc ) * cen;
        setcolor(BLUE);
        //circle(cen.x + 350, cen.y + 200, 10);
        drawcube(cubea);
        setcolor(BLUE);
        for( int i = 0; i < n-1; i++ ) { line( xa[i].x + 350, xa[i].y + 200, xa[i+1].x + 350, xa[i+1].y + 200 ); }
        drawcoord(coorda);
        page = 1 - page;
    }
    getch();
    closegraph();
    return 0;
}

