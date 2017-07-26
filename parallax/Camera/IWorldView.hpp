/************************************************************************/
/* IWorldView interface for views into the world                        */
/*																		*/
/* Base interface class for the view system, all other classes must		*/
/* inherit from this one.												*/
/*																		*/
/*	Base class from the Resurrection Engine Project						*/
/************************************************************************/

#ifndef RESURRECTIONENGINE_IWORLDVIEW_HPP
#define RESURRECTIONENGINE_IWORLDVIEW_HPP

// this header will need redirecting unless each project maintains the base
// directory layout
#include "../Maths/mtxlib.h"

namespace Resurrection
{
	namespace CameraSystem
	{
		class IWorldView
		{
		public:
			virtual ~IWorldView() {};
			virtual matrix44 getViewMatrix() = 0;
		protected:
		private:
		};
	}
}


#endif