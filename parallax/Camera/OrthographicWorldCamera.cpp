/************************************************************************/
/* OrthographicWorldCamera; used for an orthographic view of the world  */
/*																		*/
/* Inherits from IWorldCamera and requires a view to operate       		*/
/*																		*/
/*	Class from the Resurrection Engine Project						    */
/************************************************************************/

#include "OrthographicWorldCamera.hpp"
#include "../Maths/mtxlib.h"
#include "../Maths/Quaternion.hpp"

// This is bad!
// But as we lack a 'renderer' module to act as a 
// go between for GL stuffs it'll do for now
#include <GLee.h>

namespace Resurrection
{
	namespace CameraSystem
	{
		OrthographicWorldCamera::OrthographicWorldCamera(float l, float r, float b, float t, float n, float f)
		{
			view_ = NULL;
			projMatrix_ = OrthoMatrix44(l,r,b,t,n,f);
		};
		OrthographicWorldCamera::OrthographicWorldCamera(matrix44 const& projMatrix) : projMatrix_(projMatrix) 	{view_ = NULL;};
		OrthographicWorldCamera::OrthographicWorldCamera(float *projMatrix) : projMatrix_(projMatrix) {view_ = NULL;};
		OrthographicWorldCamera::~OrthographicWorldCamera() {};
		OrthographicWorldCamera::OrthographicWorldCamera(OrthographicWorldCamera const& rhs)
		{
			projMatrix_ = rhs.projMatrix_;
			mvMatrix_ = rhs.mvMatrix_;
		};

		void OrthographicWorldCamera::setView(IWorldView *view) { view_ = view;};
		// Updates local cache of view matrix
		void OrthographicWorldCamera::updateViewMatrix()
		{
			if(view_)
				mvMatrix_ = view_->getViewMatrix();
		};

		// Sets the various matrices to allow for rendering
		void OrthographicWorldCamera::setMatricies()
		{
			float m[16];
			// hard coded to OpenGL
			glMatrixMode(GL_PROJECTION);
			//glLoadMatrixf(&projMatrix_.col[0].x);	// set the matrix to our projection matrix
			projMatrix_.getMatrix(m);
			glLoadMatrixf(m);
			glMatrixMode(GL_MODELVIEW);
			//glLoadMatrixf(&mvMatrix_.col[0].x);		// set the matrix to our model-view matrix
			mvMatrix_.getMatrix(m);
			glLoadMatrixf(m);
		};
	}
}