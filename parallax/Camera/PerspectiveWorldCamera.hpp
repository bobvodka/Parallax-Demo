/************************************************************************/
/* PerspectiveWorldCamera; used for a standard perspective view of the  */
/* world.																*/
/*																		*/
/* Inherits from IWorldCamera and requires a view to operate       		*/
/*																		*/
/*	Class from the Resurrection Engine Project						    */
/************************************************************************/
#ifndef RESURRECTIONENGINE_PERSPECTIVEWORLDCAMERA_HPP
#define RESURRECTIONENGINE_PERSPECTIVEWORLDCAMERA_HPP

#include "IWorldCamera.hpp"
#include "../Maths/mtxlib.h"

namespace Resurrection
{
	namespace CameraSystem
	{
		class PerspectiveWorldCamera : public IWorldCamera
		{
		public:
			// Constructors
			PerspectiveWorldCamera(float l, float r, float b, float t, float n, float f);
			PerspectiveWorldCamera(float fieldOfViewY, float aspect, float n, float f);
			PerspectiveWorldCamera(matrix44 const& projMatrix);
			PerspectiveWorldCamera(float *projMatrix);
			~PerspectiveWorldCamera();

			PerspectiveWorldCamera(PerspectiveWorldCamera const& rhs);

			// View Setting method
			void setView(IWorldView *view);
			// Update local matrix cache of view matrix
			void updateViewMatrix();
			// Sets these matrices as the current perspective and model-view matrices
			void setMatricies();

		private:
			matrix44 projMatrix_;
			matrix44 mvMatrix_;

			IWorldView *view_;
		};
	}
}

#endif