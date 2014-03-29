/*  =================== File Information =================
	File Name: ppm.h
	Description:
	Author: Michael Shah
	Last Modified: 3/10/14

	Purpose: 
	Usage:	

	Further Reading resources: 
	http://antongerdelan.net/opengl/raycasting.html
	===================================================== */
#include <string.h>
#include <iostream>
#include <fstream>
#include <GL/glui.h>
#include <math.h>
#include "object.h"
#include "Algebra.h"

/** These are the live variables passed into GLUI ***/
int main_window;
int wireframe = 0;
int filled = 1;
int grid = 0;

int red_Scroll = 0;
int green_Scroll = 255;
int blue_Scroll = 0;
float xy_aspect_glob;

// Definition for rollout for r g b color sliders
#define COLOR_ID 101

float view_rotate[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
float obj_pos[] = { 0.0, 0.0, 0.0 };

// function declarations
void drawRayFunc(float x, float y);

// Flag for OpenGL interaction
bool drawRay = false;
bool paint;

// Window information
double windowXSize = 500;
double windowYSize = 500;

float mouseX = 0;
float mouseY = 0;
float mouseZ = 1.0f;

/* 	======== Our Scene ========
	We are going to create a single sphere that we load
	=========================== */
object* myObject = new object();
/***************************************** myGlutMotion() ***********/

// Updates the mouse cooridnates
// This function turns a x and y screen cooridnate into a world coordinate
// in object space
void updateMouse(int x, int y){
	mouseX = (2.0f * x)/windowXSize - 1.0f;
    mouseY = 1.0f - (2.0f * y)/ windowYSize;
    mouseZ = 1.0f;
    std::cout << "Screen (" << x << "," << y << ") to Object (" << mouseX << "," << mouseY << ")" << std::endl;
}

// This function is called everytime the mouse moves
// In order to get our ray to draw nicely, we update the mouse coordinates
// where the ray is first cast, and then draw the ray, and then draw the rest
// of our scene.
void myGlutMotion(int x, int y )
{
	updateMouse(x,y);
	drawRayFunc(mouseX,mouseY);
    glutPostRedisplay();
}

/***************************************** myGlutMouse() ***********/
/*	This function is called everytime the mouse is clicked

	left click - draw our ray
	right click - color the pixel appropriately.
	
*/
void myGlutMouse(int button, int button_state, int x, int y )
{
	updateMouse(x,y);

    // Cast a ray to the screen
    // Setup a flag so we can see the actual ray that is being cast.
    if(button_state == GLUT_UP && button==GLUT_LEFT_BUTTON){
        drawRay = false;
    }
    if((button_state == GLUT_DOWN && button==GLUT_LEFT_BUTTON) && drawRay==false){
        drawRay = true;
    }


    /*
			This function for how you paint on an object will need to be modified!
	*/
    if((button_state == GLUT_DOWN && button==GLUT_RIGHT_BUTTON) && (drawRay==false) && (paint == true)){
    	// If we intersect, then paint our object with a brush
	    for(int xx=x; xx < x+10; xx++){
	    	for(int yy=y; yy< y+10; yy++){
	    		//CHange HERE!!!!
	    		myObject->paintTexture(xx, yy, red_Scroll, green_Scroll, blue_Scroll);
	    	}
		}
    }

}    


/***************************************** myGlutIdle() ***********/

void myGlutIdle(void)
{
	/* According to the GLUT specification, the current window is
	undefined during an idle callback.  So we need to explicitly change
	it if necessary */
	if (glutGetWindow() != main_window)
		glutSetWindow(main_window);

	glutPostRedisplay();
}


/**************************************** myGlutReshape() *************/

/*
	When the window is resized, update aspect ratio to get correct viewing frustrum.
*/
void myGlutReshape(int x, int y)
{
	float xy_aspect;

	xy_aspect = (float)x / (float)y;
	xy_aspect_glob = xy_aspect;
	// Pixel coordinate system
	glViewport(0, 0, x, y);

	windowXSize = x;
	windowYSize = y;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-xy_aspect*.08, xy_aspect*.08, -.08, .08, .1, 15.0);
	glutPostRedisplay();
}

/*
	Draw a 10x10 grid under our scene
*/
void draw_grid(){
	int grid_size = 10;
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glTranslatef(-grid_size/2,0,-grid_size/2);
    // Draw a grid under the object
        for(int i = 0 ; i < grid_size; i=i+1){
            glBegin(GL_LINES);
                glColor3f( 1.0, 1.0, 1.0 );
                glVertex3f( 0.0, 0.0, i );  glVertex3f( grid_size, 0.0, i ); /* X axis grid */
                glVertex3f( i, 0.0, 0.0 );  glVertex3f( i, 0.0, grid_size ); /* X axis grid */
            glEnd();
        }
    
        glBegin(GL_LINES);
            glColor3f( 1.0, 1.0, 1.0 );
            glVertex3f( 0.0, 0.0, grid_size );  glVertex3f( grid_size, 0.0, grid_size ); /* X axis grid */
            glVertex3f( grid_size, 0.0, 0.0 );  glVertex3f( grid_size, 0.0, grid_size ); /* X axis grid */
        glEnd();
    glPopMatrix();
    glEnable( GL_LIGHTING );

}


/*
		You implement this!

		What it is returning is the depth at where we have intersected.

*/
double Intersect(Point eyePointP, Vector rayV, Matrix transformMatrix) {
	Matrix M_inv = invert(transformMatrix);
	Point  pObj  = M_inv * eyePointP;
	Vector pVec  = pObj - Point();
	//pVec.negate();
	Vector dObj  = M_inv * rayV;
	double A = dot(dObj, dObj);
	double B = 2 * dot(pVec, dObj);
	double C = dot(pVec, pVec) - ((0.5) * (0.5));
	double disc = B * B - (4 * A * C);
	paint = true;
	if (disc < 0){
		//paint = false;
		return -1;
	}
	else if (disc == 0){
		return (eyePointP + ((-B / (2 * A)) * rayV))[3];
	}
	else{
		double b1 = (-B + sqrt(disc)) / (2 * A);
		double b2 = (-B - sqrt(disc)) / (2 * A);
		if (b1 < b2)
			return (eyePointP + (b1 * rayV))[2];
		else
			return (eyePointP + (b2 * rayV))[2];
	}
}

/*
	
	This function you will have to modify

*/
void drawRayFunc(float x, float y){
	if(drawRay==true){
		// Draw a bounding box around the sphere to help debug your intersection
		glColor3f(1,0,0);
		glutWireCube(2.0);
		Point lookAtPoint = Point(1 * x, 1 * y, -1);
		Point eyePoint    = Point(0,0,0);
		Vector lookVector  = lookAtPoint - eyePoint;
		//lookVector.negate();
		lookVector.normalize();


		Matrix transformM  = Matrix();

		Matrix rotationM = Matrix();

		rotationM(0,0) = 1;
		rotationM(0,1) = 0;
		rotationM(0,2) = 0;

		rotationM(1,0) = 0;
		rotationM(1,1) = 1;
		rotationM(1,2) = 0;

		rotationM(2,0) = lookVector[0];
		rotationM(2,1) = lookVector[1];
		rotationM(2,2) = lookVector[2];

		Matrix translateM = Matrix();
		translateM(2,3) = -4;

	 	Matrix scaleM = Matrix();
	 	//scaleM(0,0) = 15;
	 	//scaleM(1,1) = 15;
	 	//scaleM(2,2) = 15;

		transformM = translateM * rotationM;
		transformM = rotationM * translateM;


		lookAtPoint = transformM * lookAtPoint;
		eyePoint    = transformM  * eyePoint;
		Vector ray   = lookAtPoint - eyePoint;
		ray.normalize();
		Matrix transformM2  = Matrix();
		Vector vec = Vector(0,1,0);
		transformM2 = rot_mat(vec, PI/2);

		//glLineWidth(2.5);
		glColor3f(1.0, 1.0, 0.0);
		float d = 5;

		glBegin(GL_LINES);
			glVertex3f(eyePoint[0], eyePoint[1], eyePoint[2]);
			glVertex3f(eyePoint[0] + (d * ray[0]), eyePoint[1] + (d * ray[1]), eyePoint[2] + (d * ray[2]));
		glEnd();

		double closestZ = -1;
		paint = false;
		for (int x = 0; x <= 500; x++){
			for(int y = 0; y<= 500; y++){
				Point rayPoint   = Point((-1 + (x/250)), (-1 + y/250), -1);
				Point eyePoint   = Point(0,0,0);
				//Point rayVector  = rayPoint - eyePoint;

				rayPoint     = transformM  * rayPoint;
				eyePoint     = transformM  * eyePoint;
				Vector rayVector  = rayPoint - eyePoint;
				rayVector.normalize();

				double z = Intersect(eyePoint, ray, transformM2);
				if (((z + 1) > 0.001) || ((z + 1) < -0.001)){
					paint = true;
					if (z < closestZ)
						closestZ = z;
				}
			}
		}
		//double z = Intersect(eyePoint, ray, transformM2);

		/*glBegin(GL_LINES);
			glVertex3f(eyePoint[0], eyePoint[1], eyePoint[2]);
			glVertex3f(eyePoint[0] + (z * ray[0]), eyePoint[1] + (z * ray[1]), eyePoint[2] + (z * ray[2]));
		glEnd();*/

		std::cout << closestZ << std::endl;

	}
}

/***************************************** myGlutDisplay() *****************/
/*
	Renders the scene
*/	
void myGlutDisplay(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef( 0, 0, -4 );
	drawRayFunc(mouseX,mouseY);

	if (filled) {
		glEnable(GL_POLYGON_OFFSET_FILL);
		glColor3f(1.0, 1.0, 1.0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		myObject->render();
		glPushMatrix();
		glRotatef(90,0,1,0);
			myObject->drawTexturedSphere();
		glPopMatrix();
	}
	
	if (wireframe) {
		glDisable(GL_POLYGON_OFFSET_FILL);
		glColor3f(1.0, 1.0, 1.0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		myObject->render();
		glPushMatrix();
		glRotatef(90,0,1,0);
			myObject->drawTexturedSphere();
		glPopMatrix();
	}

	if(grid){
		draw_grid();
	}

	glutSwapBuffers();
}

/*
		If you add any call backs for GLUI, you can put them here
*/
void control_cb( int control )
{

}


/*
	Reclaim memory we've allocated
*/
void onExit()
{
	delete myObject;
}

/**************************************** main() ********************/

int main(int argc, char* argv[])
{

	atexit(onExit);
	/****************************************/
	/*   Initialize GLUT and create window  */
	/****************************************/
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(windowXSize, windowYSize);

	main_window = glutCreateWindow("COMP 175 In Class Assignment 6");
	glutDisplayFunc(myGlutDisplay);
	glutReshapeFunc(myGlutReshape);
	/****************************************/
	/*       Set up OpenGL lighting         */
	/****************************************/
	glShadeModel(GL_SMOOTH);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

	static float one[] = { 1, 1, 1, 1 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, one);
	/****************************************/
	/*          Enable z-buferring          */
	/****************************************/
	glEnable(GL_DEPTH_TEST);
	glPolygonOffset(1, 1);

	/****************************************/
	/*		Setup textured Objects          */
	/****************************************/
	myObject->setTexture(0,".//data//pink.ppm");
	myObject->setTexture(1,".//data//smile.ppm");

	/****************************************/
	/*         Here's the GLUI code         */
	/****************************************/
	GLUI *glui = GLUI_Master.create_glui("GLUI");

    GLUI_Scrollbar *sb;
    sb = new GLUI_Scrollbar( glui, "Red",GLUI_SCROLL_HORIZONTAL, &red_Scroll,COLOR_ID,control_cb);
    sb->set_float_limits(0,255);
    sb = new GLUI_Scrollbar( glui, "Green",GLUI_SCROLL_HORIZONTAL, &blue_Scroll,COLOR_ID,control_cb);
    sb->set_float_limits(0,255);
    sb = new GLUI_Scrollbar( glui, "Blue",GLUI_SCROLL_HORIZONTAL, &green_Scroll,COLOR_ID,control_cb);
    sb->set_float_limits(0,255);

/*
    // Create a rotation widget
    GLUI_Rotation *view_rot = new GLUI_Rotation(glui, "Objects", view_rotate );
    view_rot->set_spin( 1.0 );

    // Navigate our scene
    new GLUI_Column( glui, false );
    GLUI_Translation *trans_x =  new GLUI_Translation(glui, "Objects X", GLUI_TRANSLATION_X, obj_pos );
    trans_x->set_speed( .1 );
    new GLUI_Column( glui, false );
    GLUI_Translation *trans_y =  new GLUI_Translation( glui, "Objects Y", GLUI_TRANSLATION_Y, &obj_pos[1] );
    trans_y->set_speed( .1 );
    new GLUI_Column( glui, false );
    GLUI_Translation *trans_z =  new GLUI_Translation( glui, "Objects Z", GLUI_TRANSLATION_Z, &obj_pos[2] );
    trans_z->set_speed( .1 );
*/
	glui->add_column(true);

	GLUI_Panel *render_panel = glui->add_panel("Render");
	new GLUI_Checkbox(render_panel, "Wireframe", &wireframe);
	new GLUI_Checkbox(render_panel, "Filled", &filled);
	new GLUI_Checkbox(render_panel, "Grid", &grid);

	glui->add_button("Quit", 0, (GLUI_Update_CB)exit);

	glui->set_main_gfx_window(main_window);
	/* We register the idle callback with GLUI, *not* with GLUT */
	GLUI_Master.set_glutIdleFunc(myGlutIdle);
	GLUI_Master.set_glutMouseFunc( myGlutMouse );
	glutMotionFunc( myGlutMotion );

	glutMainLoop();

	return EXIT_SUCCESS;
}



