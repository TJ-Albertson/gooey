#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>

#include <my_math/basic.h>
#include <my_math/vector.h>

typedef float Mat4[4][4];

void clear_matrix(Mat4* mat)
{
    int i, j;
    for(i = 0; i < 4; i++) {
        for(j = 0; j < 4; j++) {
            if(i == j)
                (*mat)[i][j] = 1.0f;  /* Diagonal elements to 1 */
            else
                (*mat)[i][j] = 0.0f;  /* Non-diagonal elements to 0 */
        }
    }
}

void translateMat4(Mat4* matrix, float tx, float ty, float tz) 
{
    (*matrix)[3][0] += tx;
    (*matrix)[3][1] += ty;
    (*matrix)[3][2] += tz;
}

void scaleMat4(Mat4* matrix, float scaleX, float scaleY, float scaleZ) {
    (*matrix)[0][0] = scaleX;
    (*matrix)[1][1] = scaleY;
    (*matrix)[2][2] = scaleZ;
}

Mat4* perspective(float fovy, float aspect, float near, float far) 
{
    Mat4* matrix = (Mat4*)malloc(sizeof(Mat4));
    float f = 1.0f / my_tanf(fovy / 2.0f);

    (*matrix)[0][0] = f / aspect;
    (*matrix)[0][1] = 0.0f;
    (*matrix)[0][2] = 0.0f;
    (*matrix)[0][3] = 0.0f;

    (*matrix)[1][0] = 0.0f;
    (*matrix)[1][1] = f;
    (*matrix)[1][2] = 0.0f;
    (*matrix)[1][3] = 0.0f;

    (*matrix)[2][0] = 0.0f;
    (*matrix)[2][1] = 0.0f;
    (*matrix)[2][2] = (far + near) / (near - far);
    (*matrix)[2][3] = -1.0f;

    (*matrix)[3][0] = 0.0f;
    (*matrix)[3][1] = 0.0f;
    (*matrix)[3][2] = (2.0f * far * near) / (near - far);
    (*matrix)[3][3] = 0.0f;

    return matrix;
}

Mat4* lookAt(const Vector3D *eye, const Vector3D *center, const Vector3D *up) 
{
    Mat4* matrix = (Mat4*)malloc(sizeof(Mat4));
    Vector3D f, u, r;

    /* Calculate forward vector */ 
    subtract(&f, center, eye);
    normalize(&f);

    /* Calculate right vector */ 
    cross(&r, up, &f);
    normalize(&r);

    /* Calculate up vector */ 
    cross(&u, &f, &r);
    normalize(&u);

    /* Set the matrix elements */ 
    (*matrix)[0][0] = r.x;
    (*matrix)[0][1] = u.x;
    (*matrix)[0][2] = -f.x;
    (*matrix)[0][3] = 0.0f;

    (*matrix)[1][0] = r.y;
    (*matrix)[1][1] = u.y;
    (*matrix)[1][2] = -f.y;
    (*matrix)[1][3] = 0.0f;

    (*matrix)[2][0] = r.z;
    (*matrix)[2][1] = u.z;
    (*matrix)[2][2] = -f.z;
    (*matrix)[2][3] = 0.0f;

    (*matrix)[3][0] = -r.x * eye->x - r.y * eye->y - r.z * eye->z;
    (*matrix)[3][1] = -u.x * eye->x - u.y * eye->y - u.z * eye->z;
    (*matrix)[3][2] = f.x * eye->x + f.y * eye->y + f.z * eye->z;
    (*matrix)[3][3] = 1.0f;

    return matrix;
}

Mat4* ortho(float left, float right, float bottom, float top, float near, float far)
{
    Mat4* matrix = (Mat4*)malloc(sizeof(Mat4));

    (*matrix)[0][0] = 2.0 / (right - left);
    (*matrix)[1][1] = 2.0 / (top - bottom);
    (*matrix)[2][2] = -2.0 / (far - near);
    (*matrix)[3][0] = -(right + left) / (right - left);
    (*matrix)[3][1] = -(top + bottom) / (top - bottom);
    (*matrix)[3][2] = -(far + near) / (far - near);
    (*matrix)[0][1] = (*matrix)[0][2] = (*matrix)[0][3] = (*matrix)[1][0] = (*matrix)[1][2] = (*matrix)[1][3] = (*matrix)[2][0] = (*matrix)[2][1] = (*matrix)[2][3] = 0.0;
    (*matrix)[3][3] = 1.0;

    return matrix;
}

Mat4* generateClipMatrix(float n, float r, float t) {

    Mat4* matrix = (Mat4*)malloc(sizeof(Mat4));

    (*matrix)[0][0] = n / r;
    (*matrix)[0][1] = 0.0f;
    (*matrix)[0][2] = 0.0f;
    (*matrix)[0][3] = 0.0f;

    (*matrix)[1][0] = 0.0f;
    (*matrix)[1][1] = n / t;
    (*matrix)[1][2] = 0.0f;
    (*matrix)[1][3] = 0.0f;

    (*matrix)[2][0] = 0.0f;
    (*matrix)[2][1] = 0.0f;
    (*matrix)[2][2] = -1.0f;
    (*matrix)[2][3] = -2.0f * n;

    (*matrix)[3][0] = 0.0f;
    (*matrix)[3][1] = 0.0f;
    (*matrix)[3][2] = -1.0f;
    (*matrix)[3][3] = 0.0f;

    return matrix;
}



#endif /* MATRIX_H */