/*
 */

#include <stdio.h>
#include "Collision.h"   
#include <stdlib.h> 
#include <time.h>
#include <math.h>
#include "glut.h"	// OpenGL Graphics Utility Library
#include "Vector3D.h";

#define PARTICLES_NUMBER 100
#define PARTICLE_RADIUS 1
#define BOX_SIZE 200

static GLenum spinMode = GL_TRUE;
static GLenum singleStep = GL_FALSE;

static float AnimateIncrement;
static float boxSize = BOX_SIZE;
static int particlesNumber = PARTICLES_NUMBER;
static struct particle particles[PARTICLES_NUMBER];
static float PI = 3.1415;

// glutKeyboardFunc is called below to set this function to handle
//		all normal key presses.  
static void KeyPressFunc( unsigned char Key, int x, int y )
{
	switch ( Key ) {
	case 'R':
	case 'r':
		Key_r();
		break;
	case 's':
	case 'S':
		Key_s();
		break;
	case 27:	// Escape key
		exit(1);
	}
}

// glutSpecialFunc is called below to set this function to handle
//		all special key presses.  See glut.h for the names of
//		special keys.
static void SpecialKeyFunc( int Key, int x, int y )
{
	switch ( Key ) {
	case GLUT_KEY_UP:		
		Key_up();
		break;
	case GLUT_KEY_DOWN:
		Key_down();
		break;
	}
}


static void Key_r(void)
{
	if ( singleStep ) {			// If ending single step mode
		singleStep = GL_FALSE;
		spinMode = GL_TRUE;		// Restart animation
	}
	else {
		spinMode = !spinMode;	// Toggle animation on and off.
	}
}

static void Key_s(void)
{
	singleStep = GL_TRUE;
	spinMode = GL_TRUE;
}

static void Key_up(void)
{
    AnimateIncrement *= 2.0;			// Double the animation time step
}

static void Key_down(void)
{
    AnimateIncrement /= 2.0;			// Halve the animation time step	
}

/*
 * Animate() handles the animation and the redrawing of the
 *		graphics window contents.
 */

static int existsParticleInPos(int i, float x, float y, float z)
{
	int result = 0;
	int j;
	for (j = 0; j < particlesNumber; j++)
		{
			if (i != j)
			{
				float d = sqrt(pow(particles[i].x - particles[j].x, 2) + 
					pow(particles[i].y - particles[j].y, 2) +
					pow(particles[i].z - particles[j].z, 2));
				if (d < particles[i].radius + particles[j].radius)
				{
					result = 1;
					break;
				}
			}
		}
	return result;
}

static void CreateParticles()
{
	int i;
	srand((unsigned)(time(0))); 
	for (i = 0; i < particlesNumber; i++)
	{
		int size = BOX_SIZE;
		particles[i].x = (gen_rand(size)-(size/2))/1.0;
		particles[i].y = (gen_rand(size)-(size/2))/1.0;
		particles[i].z = (gen_rand(size)-(size/2))/1.0;
		while (existsParticleInPos(i, particles[i].x, particles[i].y, particles[i].z))
		{
			particles[i].x = (gen_rand(size)-(size/2))/1.0;
			particles[i].y = (gen_rand(size)-(size/2))/1.0;
			particles[i].z = (gen_rand(size)-(size/2))/1.0;
		}
		particles[i].radius = PARTICLE_RADIUS;
		particles[i].slices = 20;
		particles[i].stacks = 20;
		particles[i].r = (gen_rand(10))/10.0;
		particles[i].g = (gen_rand(10))/10.0;
		particles[i].b = (gen_rand(10))/10.0;
		particles[i].vx = (gen_rand(10)-5)/1.0;
		particles[i].vy = (gen_rand(10)-5)/1.0;
		particles[i].vz = (gen_rand(10)-5)/1.0;
	}
}

static void DrawBox()
{
	glColor3f(255, 255, 255);
	glutWireCube(boxSize);
}

static void DetectCollision()
{
	int i;
	int j;
	struct Vector3D velocities[PARTICLES_NUMBER];
	for (i = 0; i < particlesNumber; i++)
	{
		float sumvx = 0;
		float sumvy = 0;
		float sumvz = 0;
		for (j = 0; j < particlesNumber; j++)
		{
			if (i != j)
			{
				float d = sqrt(pow(particles[i].x - particles[j].x, 2) + 
					pow(particles[i].y - particles[j].y, 2) +
					pow(particles[i].z - particles[j].z, 2));
				if (d <= particles[i].radius + particles[j].radius)
				{
					sumvx += particles[j].vx;
					sumvy += particles[j].vy;
					sumvz += particles[j].vz;
				}
			}
		}
		if (sumvx != 0)
			velocities[i].x = sumvx;
		else
			velocities[i].x = particles[i].vx;
		if (sumvy != 0)
			velocities[i].y = sumvy;
		else
			velocities[i].y = particles[i].vy;
		if (sumvz != 0)
			velocities[i].z = sumvz;
		else
			velocities[i].z = particles[i].vz;
	}
	for (i = 0; i < particlesNumber; i++)
	{
		particles[i].vx = velocities[i].x;
		particles[i].vy = velocities[i].y;
		particles[i].vz = velocities[i].z;

		if (particles[i].x + particles[i].vx - particles[i].radius < -boxSize/2 || particles[i].x + particles[i].vx + particles[i].radius > boxSize/2)
		{
			if (particles[i].x + particles[i].vx - particles[i].radius < -boxSize/2)
			{
				float offset = 0;
				particles[i].x = -boxSize/2 + particles[i].radius + offset;
				while (existsParticleInPos(i, particles[i].x, particles[i].y, particles[i].z))
				{
					offset += particles[i].radius / 10;
					particles[i].x = -boxSize/2 + particles[i].radius + offset;
				}
			}
			if (particles[i].x + particles[i].vx + particles[i].radius > boxSize/2)
			{
				float offset = 0;
				particles[i].x = boxSize/2 - particles[i].radius - offset;
				while (existsParticleInPos(i, particles[i].x, particles[i].y, particles[i].z))
				{
					offset += particles[i].radius / 10;
					particles[i].x = boxSize/2 - particles[i].radius - offset;
				}
			}

			particles[i].vx = - particles[i].vx;
		}
		else
		{
			particles[i].x += particles[i].vx;
		}
		if (particles[i].y + particles[i].vy - particles[i].radius < -boxSize/2 || particles[i].y + particles[i].vy + particles[i].radius > boxSize/2)
		{
			if (particles[i].y + particles[i].vy - particles[i].radius < -boxSize/2)
			{
				float offset = 0;
				particles[i].y = -boxSize/2 + particles[i].radius + offset;
				while (existsParticleInPos(i, particles[i].x, particles[i].y, particles[i].z))
				{
					offset += particles[i].radius / 10;
					particles[i].y = -boxSize/2 + particles[i].radius + offset;
				}
			}
			if (particles[i].y + particles[i].vy + particles[i].radius > boxSize/2)
			{
				float offset = 0;
				particles[i].y = boxSize/2 - particles[i].radius - offset;
				while (existsParticleInPos(i, particles[i].x, particles[i].y, particles[i].z))
				{
					offset += particles[i].radius / 10;
					particles[i].y = boxSize/2 - particles[i].radius - offset;
				}
			}

			particles[i].vy = - particles[i].vy;
		}
		else
		{
			particles[i].y += particles[i].vy;
		}
		if (particles[i].z + particles[i].vz - particles[i].radius < -boxSize/2 || particles[i].z + particles[i].vz + particles[i].radius > boxSize/2)
		{
			if (particles[i].z + particles[i].vz - particles[i].radius < -boxSize/2)
			{
				float offset = 0;
				particles[i].z = -boxSize/2 + particles[i].radius + offset;
				while (existsParticleInPos(i, particles[i].x, particles[i].y, particles[i].z))
				{
					offset += particles[i].radius / 10;
					particles[i].z = -boxSize/2 + particles[i].radius + offset;
				}
			}
			if (particles[i].z + particles[i].vz + particles[i].radius > boxSize/2)
			{
				float offset = 0;
				particles[i].z = boxSize/2 - particles[i].radius - offset;
				while (existsParticleInPos(i, particles[i].x, particles[i].y, particles[i].z))
				{
					offset += particles[i].radius / 10;
					particles[i].z = boxSize/2 - particles[i].radius - offset;
				}
			}

			particles[i].vz = - particles[i].vz;
		}
		else
		{
			particles[i].z += particles[i].vz;
		}
	}
}

static void renderBitmapString(
		float x,
		float y,
		float z,
		void *font,
		char *string) {

  char *c;
  glRasterPos3f(x, y,z);
  for (c=string; *c != '\0'; c++) {
    glutBitmapCharacter(font, *c);
  }
}

static void DrawParticles()
{
	char text [50];
	int i;
	float sumSpeedx = 0;
	for (i = 0; i < particlesNumber; i++)
	{
		sumSpeedx += abs(particles[i].vx) + abs(particles[i].vy) + abs(particles[i].vz);
		glPushMatrix();
		glTranslatef(particles[i].x, particles[i].y, particles[i].z);
		glColor3f(particles[i].r, particles[i].g, particles[i].b);
		glutSolidSphere(particles[i].radius, particles[i].slices, particles[i].stacks);
		glPopMatrix();
	}
	
	sprintf(text, "Speeds sum: %3.2f", sumSpeedx);
	renderBitmapString(BOX_SIZE, BOX_SIZE - 50, 0, GLUT_BITMAP_HELVETICA_10, text);
}

static int gen_rand(int max)
{
	int n;
	n = max*rand()/(RAND_MAX+1.0); 
	return(n);
}

static void Animate(void)
{
	// Clear the redering window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Clear the current matrix (Modelview)
    glLoadIdentity();

	// Back off eight units to be able to view from the origin.
    glTranslatef ( 0.0, 0.0, 0.0);

	// Rotate the plane of the elliptic
	// (rotate the model's plane about the x axis by fifteen degrees)
	glRotatef( 0.0, 0.0, 0.0, 0.0 );
	
	DrawBox();
	DetectCollision();
    DrawParticles();

	// Flush the pipeline, and swap the buffers
    glFlush();
    glutSwapBuffers();

	if ( singleStep ) {
		spinMode = GL_FALSE;
	}

	glutPostRedisplay();		// Request a re-draw for animation purposes

}

// Initialize OpenGL's rendering modes
void OpenGLInit(void)
{
    glShadeModel( GL_FLAT );
    glClearColor( 0.0, 0.0, 0.0, 0.0 );
    glClearDepth( 1.0 );
    glEnable( GL_DEPTH_TEST );
	CreateParticles();
}

// ResizeWindow is called when the window is resized
static void ResizeWindow(int w, int h)
{
    float aspectRatio;
	h = (h == 0) ? 1 : h;
	w = (w == 0) ? 1 : w;
	glViewport( 0, 0, w, h );	// View port uses whole window
	aspectRatio = (float)w/(float)h;

	// Set up the projection view matrix (not very well!)
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 50.0, aspectRatio, 1.0, 1000.0 );
	gluLookAt(0.0, 0.0, BOX_SIZE * 2,
          0.0, 0.0, 0.0,
          0.0, 1.0, 0.0);
	// Select the Modelview matrix
    glMatrixMode( GL_MODELVIEW );
}


// Main routine
// Set up OpenGL, hook up callbacks, and start the main loop
int main( int argc, char** argv )
{
	// Need to double buffer for animation
	glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

	// Create and position the graphics window
    glutInitWindowPosition( 0, 0 );
    glutInitWindowSize( 1280, 1024 );
    glutCreateWindow( "Collision Detector" );

	// Initialize OpenGL.
    OpenGLInit();

	// Set up callback functions for key presses
	glutKeyboardFunc( KeyPressFunc );
	glutSpecialFunc( SpecialKeyFunc );

	// Set up the callback function for resizing windows
    glutReshapeFunc( ResizeWindow );

	// Callback for graphics image redrawing
    glutDisplayFunc( Animate );
	
	// Start the main loop.  glutMainLoop never returns.
	glutMainLoop(  );

    return(0);			// Compiler requires this to be here. (Never reached)
}
