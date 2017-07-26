/************************************************************************/
/* DefaultView class : No translations, just uses identity view matrix	*/
/*																		*/
/* Needs to be attached to a Camera object to be used.					*/
/*																		*/
/*	Base class from the Resurrection Engine Project						*/
/************************************************************************/

// this header will need redirecting unless each project maintains the base
// directory layout
#include "IWorldView.hpp"
#include "DefaultView.hpp"
#define _USE_MATH_DEFINES
#include <math.h>
#include "../Maths/mtxlib.h"

namespace Resurrection
{
	namespace CameraSystem
	{
		DefaultView::DefaultView() {};
		DefaultView::~DefaultView() {};
		matrix44 DefaultView::getViewMatrix()
		{
			return IdentityMatrix44();

		};
	};
};