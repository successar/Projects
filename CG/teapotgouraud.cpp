#include <iostream>
#include <graphics.h>
#include <cmath>
#include <fstream>
#include "Phong.h"
using namespace std;

TMatrix cdt = rotate( 1.0, 0.0, 0.0, 30 ) * rotate( 0, 1, 0, 30 );

void dorotate( int rot[] ) {
    char ch = getch();
    ch = getch();
    rot[0] = rot[1] = rot[2] = 0;
    switch( ch ) {
    case KEY_UP :
        rot[0] = 1;
        cout << "KEY_UP" << endl;
        break;
    case KEY_DOWN :
        rot[0] = -1;
        cout << "KEY_DOWN" << endl;
        break;
    case KEY_LEFT :
        rot[1] = 1;
        cout << "KEY_LEFT" << endl;
        break;
    case KEY_RIGHT :
        rot[1] = -1;
        cout << "KEY_RIGHT" << endl;
        break;
    case KEY_HOME :
        rot[2] = 1;
        cout << "KEY_HOME" << endl;
        break;
    case KEY_END :
        rot[2] = -1;
        cout << "KEY_END" << endl;
        break;
    default :
        rot[0] = 1;
        cout << "DEFAULT" << endl;
        break;
    }
}

void drawcoords( point3d origin ) {
    point3d coord[3] = {{ 50, 0, 0}, {0, 50, 0}, {0, 0, 50} };
    int color[3] = { COLOR( 255, 0, 0 ), COLOR( 0, 255, 0 ), COLOR( 0, 0, 255 ) };
    setlinestyle( SOLID_LINE, '-', 2 );
    for( int i = 0; i < 3; i++ ) {
        coord[i] = origin + ( cdt * coord[i] );
        setcolor( color[i] );
        line( origin.x, origin.y, coord[i].x, coord[i].y );
    }
}

void drawvector( point3d vec, point3d origin, int c ) {
    vec = origin + ( cdt * ( vec / 4 ) );
    setlinestyle( SOLID_LINE, '-', 1 );
    setcolor( c );
    line( origin.x, origin.y, vec.x, vec.y );
}

int main() {
    int n = 3, m = 3, ex;
    initwindow( 700, 700, "Win" );
    setbkcolor( BLACK );
    clearviewport();
    ifstream in( "E:/CG/teacup.bpt.txt" );

    int recs = 0;
    in >> recs;
    point3d** qpt = new point3d*[4];
    point3d*** rpts = new point3d**[recs];
    point3d*** nrms = new point3d**[recs];
    int u = 4, v = 4;

    for( int i = 0; i < 4; i++ ) qpt[i] = new point3d[4];
    for( int i = 0; i < recs; i++ ) {
        in >> u >> v;
        u++; v++;
        for( int j = 0; j < u; j++ ) for( int k = 0; k < v; k++ ) in >> qpt[j][k].x >> qpt[j][k].y >> qpt[j][k].z;
        u = v = 4;
        rpts[i] = beziersurface( qpt, u, v, WHITE, false, true, &nrms[i] );
    }

    point3d** nrpts = new point3d*[u];
    for( int i = 0; i < u; i++ ) nrpts[i] = new point3d[v];

    point3d light[1][3] = { { { 0.0 , 200.0 , 200.0 } , { 100 , 170 , 0 }, { 255, 255, 255} } };//,
                          //  { { 0.0 , -300.0 , -400.0 } , { 130 , 0 , 150 }, { 255, 255, 255} } };

    point3d amb = { 20, 20, 20 }, camera = { 0.0, 200.0, 200.0 };
    point3d coeff = { 0.5, 0.5, 0.5 };
    double alpha = 18;

    point3d up = { 0.0, 40000.0, 0.0 }, focus = { 0.0, 0.0, 0.0 };
    point3d nV = (focus - camera), up_nrm = (up - camera);
    nV.normalize(); up_nrm.normalize();
    nV = nV;
    point3d vV = (up_nrm - nV * (up_nrm * nV)); vV.normalize();
    point3d uV = (nV % vV);
    float view_rot[4][4] = { {uV.x, uV.y, uV.z, 0}, {vV.x, vV.y, vV.z, 0} , {nV.x, nV.y, nV.z, 0}, {0, 0, 0, 1} };
    uV.print(); vV.print(); nV.print();
    TMatrix rotate_ori(view_rot);
    TMatrix camera_trans = rotate_ori * translate(-camera.x, -camera.y, -camera.z);

    TMatrix scaleup = scale( 200, 200, 200 );
    TMatrix scaleup_nrm = scale( 1 / 200.0, 1 / 200.0, 1 / 200.0 );
    TMatrix trans_ori = translate(300, 300, 0) * camera_trans;

    camera = trans_ori * camera;
    light[0][0] = trans_ori * light[0][0];
    //light[1][0] = trans_ori * light[1][0];

    float depthbuf[XMAX - XMIN + 1][YMAX - YMIN + 1];
    for( int i = 0 ; i < XMAX - XMIN + 1; i++ ) {
        for( int j = 0; j < YMAX - YMIN + 1; j++ ) {
            depthbuf[i][j] = -100000000000;
        }
    }

// (-300, 343), (-200, 200), (0, 315)
// (-195 195), (-195, 195), (0, 171),
    int xmin = 10000000, ymin = 10000000, xmax = -10000000, ymax = -1000000, zmin = 1000000, zmax = -1000000;
    for( int i = 0; i < recs; i++ ) {
        for( int j = 0; j < u; j++ ) {
            for( int k = 0; k < v; k++ ) {
                nrms[i][j][k] = trans_ori * rotate(1, 0, 0, 90) * scaleup_nrm * nrms[i][j][k];
                nrpts[j][k] = trans_ori * rotate(1, 0, 0, 90) * scaleup * rpts[i][j][k];
                xmin = ( xmin > nrpts[j][k].x ) ? nrpts[j][k].x : xmin;
                xmax = ( xmax < nrpts[j][k].x ) ? nrpts[j][k].x : xmax;
                ymin = ( ymin > nrpts[j][k].y ) ? nrpts[j][k].y : ymin;
                ymax = ( ymax < nrpts[j][k].y ) ? nrpts[j][k].y : ymax;
                zmin = ( zmin > nrpts[j][k].z ) ? nrpts[j][k].z : zmin;
                zmax = ( zmax < nrpts[j][k].z ) ? nrpts[j][k].z : zmax;
            }
        }
        drawgd( nrms[i], nrpts, u, v, ( float* ) depthbuf, light, amb, camera, coeff, alpha, 1 );
    }
    cout << xmin << " " << xmax << " " << ymin << " " << ymax << " " << zmin << " " << zmax << endl;
    getch();

    getch();

    return 0;

}
