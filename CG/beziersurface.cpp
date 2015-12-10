#include <iostream>
#include <graphics.h>
#include <cmath>
#include "Phong.h"
using namespace std;

int main() {
    int n = 4, m = 4;
    initwindow( 500, 500 );
    setbkcolor( WHITE );
    clearviewport();
    point3d** pts = new point3d*[m];
    point3d* qpts = new point3d[m];
    for( int i = 0; i < m; i++ ) {
        pts[i] = new point3d[n];
    }

    //antialias();
    setcolor( COLOR( 128, 128, 0 ) );
    int k = 0, l = 0, z = 0;
    while( 1 ) {
        int a, b;
        while( !ismouseclick( WM_LBUTTONDOWN ) );
        getmouseclick( WM_LBUTTONDOWN, a, b );
        pts[l][k].x = ( double )a, pts[l][k].y = ( double )b;
        if( a != -1 ) {
            fillellipse( a, b, 2, 2 );
            pts[l][k].z = (z - 100);
            k++;
        }
        if( k == n ) {
            l++;
            z -= 100;
            k = 0;
        }
        if( l == m ) break;
    }
    setcolor( COLOR( 255, 0, 0 ) );
    for( int i = 0; i < m; i++ ) for( int j = 0; j < n - 1; j++ ) line( pts[i][j].x, pts[i][j].y, pts[i][j + 1].x, pts[i][j + 1].y );

    setcolor( COLOR( 0, 255, 0 ) );
    for( int i = 0; i < m - 1; i++ ) for( int j = 0; j < n; j++ ) line( pts[i][j].x, pts[i][j].y, pts[i + 1][j].x, pts[i + 1][j].y );

    for( int i = 0; i < m; i++ ) {
        beziercurve( pts[i], n, COLOR( 0, 0, 255 ) );
    }

    point3d*** rpts = new point3d**[1], nrm;
    point3d*** nrms = new point3d**[1];
    for( int i = 0; i < n; i++ ) {
        for( int j = 0; j < m; j++ ) {
            qpts[j] = pts[j][i];
        }
        beziercurve( qpts, m, COLOR( 0, 0, 255 ) );
    }
    rpts[0] = beziersurface( pts, m, n, BLACK, false, true, &nrms[0] );
    float depth[STRIDE][STRIDE] = { { 0.0 } };
    point3d light[1][3] = { { {400.0, 100.0, -150} , {255, 255, 0}, {255, 255, 255} } };
    point3d amb = { 100, 0, 0 }, camera = { 0.0, 0.0, -100 };
    point3d coeff = { 0.4, 0.8, 0.5 };
    double alpha = 10;
    drawgd(nrms[0], rpts[0], m, n, (float* )depth, light, amb, camera, coeff, alpha, 2);
    setcolor(COLOR(0, 0, 255));
    circle(light[0][0].x, light[0][0].y, 2);
    /*for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++) {
            nrm = rpts[0][i][j] + nrms[0][i][j] * 10;
            line(rpts[0][i][j].x, rpts[0][i][j].y, nrm.x, nrm.y);
            nrm.print();
        }
    }*/


    getch();

    return 0;

}
