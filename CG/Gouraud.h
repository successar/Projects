#include <stdio.h>
#include <conio.h>
#include <graphics.h>
#include <algorithm>
#include "Bezier.h"
#include "Light.h"
using namespace std;

#define XMIN 0
#define XMAX 600
#define YMIN 0
#define YMAX 600

#define STRIDE (XMAX-XMIN+1)

void scanlinepoly( point3d pts[], int n, float * depth, point3d color[] ) {
    ptcl cpts[10];
    double x[11], y[11], z[11];
    double m[10], zd, incz;
    point3d cd, incc, cy[10], inccy[10];
    for( int i = 0; i < n; i++ ) {
        x[i] = pts[i].x;
        y[i] = pts[i].y;
        z[i] = pts[i].z;
    }
    x[n] = x[0], y[n] = y[0], z[n] = z[0];
    for( int i = 0; i < n; i++ ) {
        if( y[i + 1] == y[i] ) m[i] = 1.0;
        else if( x[i + 1] == x[i] ) m[i] = 0.0;
        else m[i] = ( x[i + 1] - x[i] ) / ( y[i + 1] - y[i] );
        cy[i] = color[i];
        inccy[i] = ( color[i + 1] - color[i] ) / ( y[i + 1] - y[i] );
    }
    cy[n] = color[0];
    double ymin = min( ( *min_element( y, y + n ) ) - 1, ( double )YMIN ), ymax = max( ( *max_element( y, y + n ) ) + 1, ( double )YMAX );
    int j = 0;
    while( ymin <= ymax ) {
        j = 0;
        for( int i = 0; i < n; i++ ) {
            if( ( y[i] <= ymin && y[i + 1] > ymin ) || ( y[i] > ymin && y[i + 1] <= ymin )  ) {
                cpts[j].x = x[i] + m[i] * ( ymin - y[i] );
                cpts[j].z = z[i];
                cpts[j].c = cy[i];
                cy[i] = cy[i] + inccy[i];
                j++;
            }
        }
        sort( cpts, cpts + j, sortbyx );
        for( int i = 0; i < j; i += 2 ) {
            zd = cpts[i].z;
            incz = ( cpts[i + 1].z - cpts[i].z ) / ( cpts[i + 1].x + 1 - ( cpts[i].x - 1 ) );
            cd = cpts[i].c;
            incc = ( cpts[i + 1].c - cpts[i].c ) / ( cpts[i + 1].x + 1 - ( cpts[i].x - 1 ) );
            float xmi = max( cpts[i].x - 1, ( double )XMIN );
            float xma = min( cpts[i + 1].x + 2, ( double )XMAX );
            float cpt;
            for( cpt = xmi; cpt < xma; cpt += 1 ) {
                int a = int( cpt ) - XMIN, b = int( ymin ) - YMIN;
                if( *( depth + a * STRIDE + b ) > zd ) {
                    putpixel( int( cpt ), int( ymin ), COLOR( cd.x, cd.y, cd.z ) );
                    *( depth + a * STRIDE + b ) = zd;
                }
                zd += incz;
                cd = cd + incc;
            }
        }
        ymin++;
    }
}

void drawgd( point3d** nrms, point3d** rpts, int u, int v, float * depth,
             point3d light[][3], point3d amb, point3d view, point3d coeff, float alpha, int n = 1 ) {
    for( int j = 0; j < u - 1; j++ ) {
        for( int k = 0; k < v - 1; k++ ) {
            point3d color[7] = { nrms[j][k] , nrms[j + 1][k] , nrms[j + 1][k + 1] , nrms[j][k], nrms[j][k + 1], nrms[j + 1][k + 1], nrms[j][k] };
            point3d ppts[7] = { rpts[j][k], rpts[j + 1][k], rpts[j + 1][k + 1], rpts[j][k], rpts[j][k + 1], rpts[j + 1][k + 1], rpts[j][k]  };
            for( int i = 0; i < 5; i++ ) {
                color[i] = brightness( color[i], ppts[i], light, amb, view, coeff, alpha, n );
            }
            scanlinepoly( ppts, 3, ( float* ) depth, color );
            scanlinepoly( ppts + 3, 3, ( float* ) depth, color ) ;
        }
    }

}
