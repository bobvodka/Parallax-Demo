/************************************************************************/
/* PolarOrbitView class : targeted orbiting view around the Y axis      */
/*																		*/
/* Requires target, distance from target and angle to view.				*/
/* Needs to be attached to a Camera object to be used.					*/
/*																		*/
/*	Base class from the Resurrection Engine Project						*/
/************************************************************************/

// this header will need redirecting unless each project maintains the base
// directory layout
#include "IWorldView.hpp"
#include "PolarOrbitView.hpp"
#define _USE_MATH_DEFINES
#include <math.h>
#include "../Maths/mtxlib.h"

namespace Resurrection
{
	namespace CameraSystem
	{
		PolarOrbitView::PolarOrbitView(vector3 const &target, vector3 const &distance, float const angle) : target_(target), distance_(distance), angle_(angle)
		{
		};
		PolarOrbitView::~PolarOrbitView() {};
		PolarOrbitView::PolarOrbitView(const PolarOrbitView &rhs)
		{
			distance_ = rhs.distance_;
			target_ = rhs.target_;
			angle_ = rhs.angle_;
		}
		void PolarOrbitView::setDistance(vector3 const &distance) { distance_ = distance; };
		void PolarOrbitView::setAngle(float const angle) { angle_ = angle; };
		void PolarOrbitView::setTarget(vector3 const &target) { target_ = target; };

		//BUGBUG: This doesn't take into account a non-(0,0,0) target point
		// We should probably fix this ;)
		matrix44 PolarOrbitView::getViewMatrix()
		{
			// Camera's virtual position
			vector4 position(-(cos(angle_) * distance_.x), -(distance_.y), -(sin(angle_)*distance_.z), 1.0f);
			vector3 forward = target_ - vector3(position.x, position.y, position.z);	// Vector from our position to the target

			forward.normalize();
			vector3 upVec(0.0f,1.0f,0.0f);

			vector3 side = CrossProduct(forward,upVec);
			side.normalize();
			upVec = CrossProduct(side,forward);

			float m[] =
			{
				side.x, upVec.x, -forward.x, 0.0f,
				side.y, upVec.y, -forward.y, 0.0f,
				side.z, upVec.z, -forward.z, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};

			matrix44 rotMatrix(m);	// rotation matrix

			matrix44 transMatrix = TranslateMatrix44(-position.x, -position.y, -position.z);

			return rotMatrix*transMatrix;

		};
	};
};