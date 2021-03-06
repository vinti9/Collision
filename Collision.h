
/*
 * Solar.h
 *
 * Author: Samuel R. Buss
 *
 * Software accompanying the book
 *		3D Computer Graphics: A Mathematical Introduction with OpenGL,
 *		by S. Buss, Cambridge University Press, 2003.
 *
 * Software is "as-is" and carries no warranty.  It may be used without
 *   restriction, but if you modify it, please change the filenames to
 *   prevent confusion between different versions.
 * Bug reports: Sam Buss, sbuss@ucsd.edu.
 * Web page: http://math.ucsd.edu/~sbuss/MathCG
 */

#include "Particle.h"   
#include "Vector3D.h";


void OpenGLInit(void);

static void Animate(void );
static void Key_r(void );
static void Key_s(void );
static void ResizeWindow(int w, int h);

static void KeyPressFunc( unsigned char Key, int x, int y );
static void CalcCollision(t_Vector3D *va, t_Vector3D *vb);
