#include <stdio.h>
#include <conio.h>
#include <graphics.h>
#include <algorithm>
#include "Trans.h"

point3d brightness( point3d nrm, point3d pt, point3d lights[][3],
                   point3d amb, point3d view, point3d coeff, float alpha,int n = 1 ) {
    point3d light, refl; float spot;
    amb = amb * coeff.x;
    for( int i = 0; i < n; i++ ) {
        nrm.normalize();
        light = ( lights[i][0] - pt );
        light.normalize();
        view = ( view - pt );
        view.normalize();
        refl = nrm * (nrm * light * 2) - light;
        double bright = max( 0.0, light * nrm ), specular = max( 0.0, view * refl );
        amb = amb + (( lights[i][1] * bright ) * coeff.y + ( lights[i][2] * pow( specular, alpha ) ) * coeff.z);
    }
    return amb;
}
