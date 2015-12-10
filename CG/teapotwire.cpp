#include <iostream>
#include <graphics.h>
#include <cmath>
#include <fstream>
#include "Bezier.h"
using namespace std;

int main() {
    int n = 3, m = 3, ex;
    initwindow( 700, 700, "Win", true );
    setbkcolor(WHITE); clearviewport();
    ifstream in("teapot_nodes.txt");
    ifstream inr("teapot_rectangles.txt");

    int nds = 306, recs = 32;
    point3d* pts = new point3d[nds];
    point3d** rects = new point3d*[recs];
    for(int i = 0; i < nds; i++ ){
        in >> pts[i].x >> pts[i].y >> pts[i].z;
    }
    for(int i = 0; i < recs; i++) {
        rects[i] = new point3d[16];
        for(int j = 0; j < 16; j++) {
            inr >> ex;
            rects[i][j] = pts[ex - 1];
        }
    }
    point3d** qpt = new point3d*[4];
    point3d*** rpts = new point3d**[32];
    int u = 4, v = 4;
    for(int i = 0; i < 4; i++) qpt[i] = new point3d[4];
    for(int i = 0; i < 32; i++) {
        for(int j = 0; j < 4; j++) for(int k = 0; k < 4; k++) qpt[j][k] = rects[i][j*4 + k];
        u = v = 4;
        rpts[i] = beziersurface(qpt, u, v, BLACK, false);
    }
    int page = 0;
    TMatrix view = translate(200, 200, 0) * scale(70, 70, 70);
    setcolor(BLACK);
    point3d a,b;
    while(1) {
        setactivepage(page);
        setvisualpage(1 - page);
        clearviewport();
        int rv[3] = { 0, 0, 0 };
        rv[rand() % 3] = 1;
        TMatrix trans =  rotate(rv[0], rv[1], rv[2], 30);
        for(int i = 0; i < 32; i++) {
            for(int j = 0; j < u; j++) {
                for(int k = 0; k < v; k++) {
                    rpts[i][j][k] = trans * rpts[i][j][k];
                }
            }
            for(int j = 0; j < u; j++) {
                b = view * rpts[i][j][0];
                for( int k = 0; k < v - 1; k++ ) {
                    a = b; b = view * rpts[i][j][k+1];
                    line(a.x, a.y, b.x, b.y);
                }
            }
            for(int j = 0; j < v; j++) {
                b = view * rpts[i][0][j];
                for( int k = 0; k < u - 1; k++ ) {
                    a = b; b = view * rpts[i][k+1][j];
                    line(a.x, a.y, b.x, b.y);
                }
            }

        }
        page = 1 - page;
    }
    getch();

    return 0;

}
