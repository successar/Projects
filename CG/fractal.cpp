#include <graphics.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
using namespace std;

#define XMAX 300 /* change these to change size */
#define YMAX 300	/*	of picture */
#define MAXCOUNT 256 /* number of iterations */

int colorc(int c) {
    //cout << c << " " << ((c & 0xFF0000) >> 16) << " " << ((c & 0x00FF00) >> 8) << " " << (c & 0x0000FF) << endl;
    return c % 16; //COLOR(0, 0, c % 256);
}

void triangle(double x1, double y1, double x2, double y2)
{
    int x, y;				/* location of pixel on screen */
	double xscale, yscale;	/* distance between pixels */
	double zx, zy;			/* real and imag parts of z */
	double cx, cy;			/* real and imag parts of c */
	double tempx;			/* briefly holds zx */
	int count;				/* number of iterations */

	xscale = (x2 - x1) / XMAX;
	yscale = (y2 - y1) / YMAX;
     for( x = 1, zx = x1; x <= XMAX; x++, zx += xscale ) {			/* for each pixel column */
		for( y = 1, zy = y1; y <= YMAX; y++, zy += yscale ) {		/* for each pixel row */
			cx = zx;			/* set c to pixel location */
			cy = zy;
			count = 0;						/* reset count */
											/* size of z < 2 */
			while( cx*cx + cy*cy < 4 && count < MAXCOUNT ) {
				tempx = cx*cx - cy*cy + zx;	/* set z = z^2 + c */
				cy = 2*cx*cy + zy;
				cx = tempx;
				count += 1;					/* another iteration */
			}
			putpixel( x, y, colorc(count) ); 		/* color is count */
		}
	}
}

int main() {
    int gd = DETECT, gm;
    initgraph( &gd, &gm, "C:\\TC\\BGI" );
    setcolor(WHITE);
    double xs, ys, x1 = -2.25, x2 = 0.75, y1 = -1.25, y2 = 1.25, xt1, xt2, yt1, yt2;
    while(true) {
        cout << x1 << " " << x2 << " " << y1 << " " << y2 << endl;
        triangle(x1, y1, x2, y2);
        xs = (x2 - x1)/XMAX; ys = (y2 - y1)/YMAX;
        while(true) {
            int a, b;
            getmouseclick(WM_LBUTTONDOWN, a, b);
            if(a != -1) {
                if( a > XMAX || b > YMAX ) { x1 = -2.25, x2 = 0.75, y1 = -1.25, y2 = 1.25; goto labl; }
                xt1 = x1 + a * xs;
                yt1 = y1 + b * ys;
                break;
            }
        }
        while(true) {
            int a, b;
            getmouseclick(WM_LBUTTONDOWN, a, b);
            if(a != -1) {
                if( a > XMAX || b > YMAX ) { x1 = -2.25, x2 = 0.75, y1 = -1.25, y2 = 1.25; break; }
                xt2 = x1 + a * xs;
                yt2 = y1 + b * ys;
                x1 = xt1, x2 = xt2, y1 = yt1, y2 = yt2;
                break;
            }
        }
        labl : clearviewport();
    }
    getch();
    closegraph();
    cout << COLOR(127, 127, 127);
    return 0;
}

