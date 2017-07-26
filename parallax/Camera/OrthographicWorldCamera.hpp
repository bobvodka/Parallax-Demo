/************************************************************************/
/* OrthographicWorldCamera; used for an orthographic view of the world  */
/*																		*/
/* Inherits from IWorldCamera and requires a view to operate       		*/
/*																		*/
/*	Class from the Resurrection Engine Project						    */
/************************************************************************/
#ifndef RESURRECTIONENGINE_ORTHOGRAPHICWORLDCAMERA_HPP
#define RESURRECTIONENGINE_ORTHOGRAPHICWORLDCAMERA_HPP

#include "IWorldCamera.hpp"
#include "../Maths/mtxlib.h"

namespace Resurrection
{
	namespace CameraSystem
	{
		class OrthographicWorldCamera : public IWorldCamera
		{
		public:
			OrthographicWorldCamera(float l, float r, float b, float t, float n, float f);
			OrthographicWorldCamera(matrix44 const& projMatrix);
			OrthographicWorldCamera(float *projMatrix);
			virtual ~OrthographicWorldCamera();
			OrthographicWorldCamera(OrthographicWorldCamera const& rhs);

			void setView(IWorldView *view);
			// Updates local cache of view matrix
			void updateViewMatrix();
			// Sets the various matrices to allow for rendering
			void setMatricies();
		protected:
		private:
			matrix44 projMatrix_;
			matrix44 mvMatrix_;

			IWorldView *view_;
		};
	}
}

#endif