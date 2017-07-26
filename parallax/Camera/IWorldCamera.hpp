/************************************************************************/
/* IWorldCamera interface for cameras used to view the world            */
/*																		*/
/* Base interface class for the view system, all other classes must		*/
/* inherit from this one.												*/
/*																		*/
/*	Base class from the Resurrection Engine Project						*/
/************************************************************************/
#ifndef RESURRECTIONENGINE_IWORLDCAMERA_HPP
#define RESURRECTIONENGINE_IWORLDCAMERA_HPP

#include "IWorldView.hpp"

namespace Resurrection
{
	namespace CameraSystem
	{
		class IWorldCamera
		{
		public:
			virtual void setView(IWorldView *view) = 0;
			virtual void updateViewMatrix() = 0;
			virtual void setMatricies() = 0;
		protected:
		private:
		};
	}
}

#endif