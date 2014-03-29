/*  =================== File Information =================
	File Name: object.h
	Description:
	Author: Michael Shah
	Last Modified: 3/10/14

	Purpose: 
	Usage:	

	Further Reading resources: 
	===================================================== */
#ifndef OBJECT_H
#define OBJECT_H

#include "Algebra.h"
#include "ppm.h"

/*
	This object renders a piece of geometry ('a sphere by default')
	that has one texture that can be drawn on.

*/
class object{
	public:
		/*	===============================================
		Desc:
		Precondition: 
		Postcondition:
		=============================================== */ 
		object();
		/*	===============================================
		Desc:
		Precondition: 
		Postcondition:
		=============================================== */ 
		~object();
		/*	===============================================
		Desc:
		Precondition: 
		Postcondition:
		=============================================== */
		GLuint loadTexture(int width, int height, char* pixels);
		/*	===============================================
		Desc:	This instantiates an image to be rendered.
				
				If memory has already been allocated for this,
				then our contract is to delete the previous image,
				and overwrite it with the new one.
		Precondition: 
		Postcondition:
		=============================================== */ 
		void setTexture(int textureNumber,std::string _fileName);
		/*	===============================================
		Desc:	Draw two planes for each of the ppms texture
		Precondition: 
		Postcondition:
		=============================================== */ 
		void render();
		/*	===============================================
		Desc:	Draw the actual rendered spheres
		Precondition: 
		Postcondition:
		=============================================== */ 
		void drawTexturedSphere();
		/*	===============================================
		Desc:			Calls into this function modify a previously loaded ppm's
						color array.

						Note that this does NOT change the original ppm image at all.

						Also note, that a new OpenGL Texture is generated everytime, we
						paint on the texture.  There could probably be a buffer to improve
						the performance of this call.
		Precondition: 
		Postcondition:
		=============================================== */ 
		void paintTexture(int x, int y, char r, char g, char b);
		
	private:
		// The first texture image
		// This should be a white, black, pink, or other solid image that
		// we can draw on.
		ppm* baseTexture;
		// A second texture image that can be loaded
		// This is a second texture that we can blend onto our sphere
		// This is demonstrating multiple textures or 'multitexturing' as it
		// is called in the graphics world.
		ppm* blendTexture;

		// Create a texture id that uniquly identifies each texture image
		// I believe OpenGL will use these id's when we generate a texture
		// to uniquely identify which array of pixels to map to an image.
		GLuint baseTextureID;
		GLuint blendTextureID;
};

#endif