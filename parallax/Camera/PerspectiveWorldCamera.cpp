/************************************************************************/
/* PerspectiveWorldCamera; used for a standard perspective view of the  */
/* world.																*/
/*																		*/
/* Inherits from IWorldCamera and requires a view to operate       		*/
/*																		*/
/*	Class from the Resurrection Engine Project						    */
/************************************************************************/

#include "PerspectiveWorldCamera.hpp"
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
		PerspectiveWorldCamera::PerspectiveWorldCamera(float l, float r, float b, float t, float n, float f)
		{
			view_ = NULL;
			projMatrix_ = FrustumMatrix44(l,r,b,t,n,f);
		};

		PerspectiveWorldCamera::PerspectiveWorldCamera(float fieldOfViewY, float aspect, float n, float f)
		{
			view_ = NULL;
			projMatrix_ = PerspectiveMatrix44(fieldOfViewY,aspect,n,f);
		};

		PerspectiveWorldCamera::PerspectiveWorldCamera(matrix44 const& projMatrix) : projMatrix_(projMatrix) 	{view_ = NULL;};
		PerspectiveWorldCamera::PerspectiveWorldCamera(float *projMatrix) : projMatrix_(projMatrix) {view_ = NULL;};
		PerspectiveWorldCamera::~PerspectiveWorldCamera() {};
		
		PerspectiveWorldCamera::PerspectiveWorldCamera(PerspectiveWorldCamera const& rhs)
		{
			projMatrix_ = rhs.projMatrix_;
			mvMatrix_ = rhs.mvMatrix_;
		};

		void PerspectiveWorldCamera::setView(IWorldView *view) { view_ = view;};
		// Updates local cache of view matrix
		void PerspectiveWorldCamera::updateViewMatrix()
		{
			if(view_)
				mvMatrix_ = view_->getViewMatrix();
		};

		// Sets the various matrices to allow for rendering
		void PerspectiveWorldCamera::setMatricies()
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