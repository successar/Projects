#pragma once
#include <iostream>
#include <graphics.h>
#include <cmath>
#include "Bezier.h"
using namespace std;

int main() {
    int n = 4, i = 0;
    initwindow(700, 700);
    setbkcolor(WHITE); clearviewport();
    point3d* pts = new point3d[n];

    setcolor(COLOR(255, 0, 0));
    while(1) {
        int a, b;
        getmouseclick(WM_LBUTTONDOWN, a, b);
        pts[i].x = a; pts[i].y = b;
        if( a != -1 ) { circle(a, b, 2); pts[i].z = 0; i++; }
        if( i == n ) break;
    }
    setcolor(COLOR(0, 0, 255));
    for(int i = 0; i < n - 1; i++) line(pts[i].x, pts[i].y, pts[i + 1].x, pts[i + 1].y);
    beziercurve(pts, n, BLACK);
    getch();

    return 0;

}
