/************************************************************************/
/* PolarOrbitView class : targeted orbiting view around the Y axis      */
/*																		*/
/* Requires target, distance from target and angle to view.				*/
/* Needs to be attached to a Camera object to be used.					*/
/*																		*/
/*	Base class from the Resurrection Engine Project						*/
/************************************************************************/

#ifndef RESURRECTIONENGINE_POLARORBITVIEW_HPP
#define RESURRECTIONENGINE_POLARORBITVIEW_HPP

// this header will need redirecting unless each project maintains the base
// directory layout
#include "IWorldView.hpp"
#include "../Maths/mtxlib.h"

namespace Resurrection
{
	namespace CameraSystem
	{
		class PolarOrbitView : public IWorldView
		{
		public:
			PolarOrbitView(vector3 const &target, vector3 const &distance, float const angle);
			~PolarOrbitView();
			PolarOrbitView(const PolarOrbitView &rhs);
			// Set Distance from viewer to target position
			void setDistance(vector3 const &distance);
			// Set view angle around Y axis
			void setAngle(float const angle);
			// Set position to target
			void setTarget(vector3 const &target);
			// Extract the view Matrix (used by Camera class)
			matrix44 getViewMatrix();

		protected:
		private:
			vector3 distance_;	// How far we want to be from the target
			vector3 target_;	// What we want to look at
			float angle_;		// Angle of rotation
		};
	}
}

#endif