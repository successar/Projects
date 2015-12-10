#include <iostream>
#include <graphics.h>
#include <cmath>
#include "Trans.h"
using namespace std;

point3d normal(double u, double v, point3d** pts) {
    double u2 = u*u, u3 = u*u*u, v2 = v*v, v3 = v*v*v;
    point3d ut = {0.0, 0.0, 0.0}, vt = {0.0, 0.0, 0.0}, nrm;
    double left1[4] = { -3.0*u2+6.0*u-3.0, 9*u2-12*u+3, 6*u-9*u2, 3*u2},
    left2[4] = { 1-3*u+3*u2-u3, 3*u3-6*u2+3*u, 3*u2-3*u3, u3 },
    right1[4] = { 1-3*v+3*v2-v3, 3*v3-6*v2+3*v, 3*v2-3*v3, v3 },
    right2[4] = { -3*v2+6*v-3, 9*v2-12*v+3, 6*v-9*v2, 3*v2};
    for(int i = 0; i < 4; i++) {
        double x = 0, y = 0, z = 0;
        for(int j = 0; j < 4; j++) { x += pts[i][j].x * right1[j]; y += pts[i][j].y * right1[j]; z += pts[i][j].z * right1[j]; }
        ut.x += left1[i]*x; ut.y += left1[i]*y; ut.z += left1[i]*z;

        x = 0, y = 0, z = 0;
        for(int j = 0; j < 4; j++) { x += pts[i][j].x * right2[j]; y += pts[i][j].y * right2[j]; z += pts[i][j].z * right2[j]; }
        vt.x += left2[i]*x; vt.y += left2[i]*y; vt.z += left2[i]*z;
    }

    nrm.x = ut.y*vt.z - ut.z*vt.y;
    nrm.y = ut.z*vt.x - ut.x*vt.z;
    nrm.z = ut.x*vt.y - ut.y*vt.x;
    double nrmsq = sqrt(nrm.x*nrm.x + nrm.y*nrm.y + nrm.z*nrm.z);
    nrm = nrm * (1.0/nrmsq);
    return nrm;
}

void decastle( int n, point3d pts[], point3d &pr, double u ) {
    point3d* qpts = new point3d[n];

    for( int i = 0; i < n; i++ ) {
        qpts[i] = pts[i];
    }
    for( int k = 1; k < n; k++ ) {
        for( int i = 0; i < n - k; i++ ) {
            qpts[i] = ( qpts[i] * ( 1 - u ) ) + ( qpts[i + 1] * u );
        }
    }
    pr = qpts[0];
}

void beziercurve( point3d pts[], int n, int color ) {
    point3d pr;
    for( float u = 0.0; u < 1.0; u += 0.0005 ) {
        decastle( n, pts, pr, u );
        pr.draw( color );
    }
}

point3d** beziersurface( point3d** pts, int &m, int &n, int color, bool draw=true, bool norm=false, point3d*** nrms = NULL ) {
    point3d pr, tu, tv;
    float step = 0.01, u, v;
    int k, l;
    point3d* qpts = new point3d[m];
    point3d** rpts = new point3d*[int(1.0/step) + 10];
    if(norm) *nrms = new point3d*[int(1.0/step) + 10];
    setcolor(color);
    for( k = 0, u = 0.0; u <= 1.0001; u += step, k++ ) {
        rpts[k] = new point3d[int(1.0/step) + 10];
        if(norm) (*nrms)[k] = new point3d[int(1.0/step) + 10];
        for( l = 0, v = 0.0; v <= 1.00001; v += step, l++ ) {
            for( int i = 0; i < m; i++ ) {
                decastle( n, pts[i], qpts[i], v );
            }
            decastle( m, qpts, pr, u );
            rpts[k][l] = pr;
            if(norm) { (*nrms)[k][l] = normal(u, v, pts); }
            if(draw) (pr * 100).draw(color);
        }
    }
    m = k; n = l;
    return rpts;
}
