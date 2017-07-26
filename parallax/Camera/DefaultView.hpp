/************************************************************************/
/* DefaultView class : No translations, just uses identity view matrix	*/
/*																		*/
/* Needs to be attached to a Camera object to be used.					*/
/*																		*/
/*	Base class from the Resurrection Engine Project						*/
/************************************************************************/

#ifndef RESURRECTIONENGINE_DEFAULTVIEW_HPP
#define RESURRECTIONENGINE_DEFAULTVIEW_HPP

// this header will need redirecting unless each project maintains the base
// directory layout
#include "IWorldView.hpp"
#include "../Maths/mtxlib.h"

namespace Resurrection
{
	namespace CameraSystem
	{
		class DefaultView : public IWorldView
		{
		public:
			DefaultView();
			~DefaultView();
			// Extract the view Matrix (used by Camera class)
			matrix44 getViewMatrix();
		protected:
		private:
		};
	}
}

#endif