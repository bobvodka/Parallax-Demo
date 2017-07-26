// parallax.cpp : Defines the entry point for the console application.
//
#define NOMINMAX
#include "stdafx.h"
#include <Windows.h>
#include <mmsystem.h>

#include <oglwfw/WindowMgr.hpp>
#include "Camera/OrthographicWorldCamera.hpp"
#include "Camera/DefaultView.hpp"

#include <gtl/GameTextureLoader.hpp>

#include <cmath>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm> 
#include <vector>

#define BASSDEF(f) (WINAPI *f)	// define the functions as pointers
#include "bass.h"	// for a basic sound system

#pragma comment(lib,"Winmm.lib")

/* AngelCode's BMFGen font support code */
/* Passer written by Promit */
/* http://www.gamedev.net/community/forums/topic.asp?topic_id=330742 */
struct CharDescriptor
{
	//clean 16 bytes
	unsigned short x, y;
	unsigned short Width, Height;
	unsigned short XOffset, YOffset;
	unsigned short XAdvance;
	unsigned short Page;

	CharDescriptor() : x( 0 ), y( 0 ), Width( 0 ), Height( 0 ), XOffset( 0 ), YOffset( 0 ),
		XAdvance( 0 ), Page( 0 )
	{ }
};

struct Charset
{
	unsigned short LineHeight;
	unsigned short Base;
	unsigned short Width, Height;
	unsigned short Pages;
	CharDescriptor Chars[256];
};

bool ParseFont( std::istream& Stream, Charset& CharsetDesc )
{	
	std::string Line;
	std::string Read, Key, Value;
	std::size_t i;
	while( !Stream.eof() )
	{
		std::stringstream LineStream;
		std::getline( Stream, Line );
		LineStream << Line;

		//read the line's type
		LineStream >> Read;
		if( Read == "common" )
		{
			//this holds common data
			while( !LineStream.eof() )
			{
				std::stringstream Converter;
				LineStream >> Read;
				i = Read.find( '=' );
				Key = Read.substr( 0, i );
				Value = Read.substr( i + 1 );

				//assign the correct value
				Converter << Value;
				if( Key == "lineHeight" )
					Converter >> CharsetDesc.LineHeight;
				else if( Key == "base" )
					Converter >> CharsetDesc.Base;
				else if( Key == "scaleW" )
					Converter >> CharsetDesc.Width;
				else if( Key == "scaleH" )
					Converter >> CharsetDesc.Height;
				else if( Key == "pages" )
					Converter >> CharsetDesc.Pages;
			}
		}
		else if( Read == "char" )
		{
			//this is data for a specific char
			unsigned short CharID = 0;

			while( !LineStream.eof() )
			{
				std::stringstream Converter;
				LineStream >> Read;
				i = Read.find( '=' );
				Key = Read.substr( 0, i );
				Value = Read.substr( i + 1 );

				//assign the correct value
				Converter << Value;
				if( Key == "id" )
					Converter >> CharID;
				else if( Key == "x" )
					Converter >> CharsetDesc.Chars[CharID].x;
				else if( Key == "y" )
					Converter >> CharsetDesc.Chars[CharID].y;
				else if( Key == "width" )
					Converter >> CharsetDesc.Chars[CharID].Width;
				else if( Key == "height" )
					Converter >> CharsetDesc.Chars[CharID].Height;
				else if( Key == "xoffset" )
					Converter >> CharsetDesc.Chars[CharID].XOffset;
				else if( Key == "yoffset" )
					Converter >> CharsetDesc.Chars[CharID].YOffset;
				else if( Key == "xadvance" )
					Converter >> CharsetDesc.Chars[CharID].XAdvance;
				else if( Key == "page" )
					Converter >> CharsetDesc.Chars[CharID].Page;
			}
		}
	}

	return true;
}

/* End BMFGen support code */

GLuint loadTexture(std::string const& name)
{	
	// Load a texture to place over the rasterbars for a fade effect
	GameTextureLoader::ImagePtr tex = GameTextureLoader::LoadTextureSafe(name);
	GLuint texture;
	glGenTextures(1,&texture);

	glBindTexture(GL_TEXTURE_2D,texture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

	if(tex->getFormat() == GameTextureLoader::FORMAT_BGR)
	{
		// normal BGR texture with no mipmaps
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,tex->getWidth(),tex->getHeight(),0,GL_BGR,GL_UNSIGNED_BYTE,tex->getDataPtr());
	}
	else if(tex->getFormat() == GameTextureLoader::FORMAT_RGB)
	{
		// normal RGB texture with no mipmaps
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,tex->getWidth(),tex->getHeight(),0,GL_RGB,GL_UNSIGNED_BYTE,tex->getDataPtr());
	}
	else if (tex->getFormat() == GameTextureLoader::FORMAT_RGBA)
	{
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,tex->getWidth(),tex->getHeight(),0,GL_RGBA,GL_UNSIGNED_BYTE,tex->getDataPtr());
	}

	return texture;
}

class IDemoScene
{
public:
	virtual ~IDemoScene(){};
	virtual void start(int time) { this->stime = time;}
	virtual void Update(int time) = 0;
	virtual void Render() = 0;
protected:
	int stime;
private:
};

// Various bits of the scene
class ScrollText : public IDemoScene
{
public:
	ScrollText(int screenwidth, int screenheight, std::string const &filename, std::string const &fntname) 
		: screenwidth_(screenwidth), screenheight_(screenheight), currentchar(0), 
		baseoffset(float(screenwidth)), scrolltextpos(0.0f),camera(0.0f, float(screenwidth), float(screenheight), 0.0f, 1.0f, -1.0f)
	{
		texture = loadTexture(filename);
		
		std::ifstream scroller(fntname.c_str());
		ParseFont(scroller, scrolltextChars);
		unsigned short maxHeight = 0;
		for(int x = 0; x < 256; x++)
		{
			maxHeight = std::max(maxHeight, scrolltextChars.Chars[x].Height);
		}
		scrolltextpos = (float(screenheight_)/2.0f) - (float(maxHeight) / 2.0f);

		camera.setView(&view);
		camera.updateViewMatrix();

		//text = "This will be the scroller and this is a test of a scroller with a longer length than before... ";
		text =  "Hello, good evening and welcome... So what's this then? This is old skool babeh! Parallax scrolls, raster bars and scroll text... ";
		text += "Yeah, haven't seen this since the 90s... Why this? Well I was bored and I figured I'd write something and I've been meaning to do this ";
		text += "for some time now.. and well, it was more amusing than assignments or project work... and shockingly loads of cats used... how rare! ;) ";
		text += "Anyways, the traditional way of finishing one of these off is to perform some kind of shout out to people.. but I'm lazy so I'll just say ";
		text += "Word to the #gamedev'ers, the various monkeys who read my gamedev.net journal, the various myspace monkeys I know and those on MSN... and Onions ";
		text += "who is a dirty dirty ginger!  bwaf... Yeah... I'm done.............                 ";

	};
	~ScrollText()
	{
		glDeleteTextures(1,&texture);
	};
	void Update(int currenttime)
	{
		// Character position testing
		// Current character has scrolled off the screen so reset the position
		baseoffset -= 2.0f;
		if(baseoffset  + scrolltextChars.Chars[text[currentchar]].XAdvance < 0.0f)
		{
			currentchar++;
			baseoffset = 0.0f;
			if(currentchar == text.length())
				currentchar = 0;
		}
	}

	void Render()
	{
		glEnable(GL_BLEND);
		// Change blending mode
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_ALPHA_TEST);
		
		camera.setMatricies();

		glTranslatef(0.0f, scrolltextpos, 0.0f);
		glBindTexture(GL_TEXTURE_2D,texture);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

		float endpos = baseoffset /*- 1.0f*/;	// set the endpos marker to the start of the scroller
		int renderchar = currentchar;

		glBegin(GL_QUADS);
		while (endpos < float(screenwidth_))
		{
			int CharX =		scrolltextChars.Chars[text[renderchar]].x;
			int CharY =		scrolltextChars.Chars[text[renderchar]].y;
			int Width =		scrolltextChars.Chars[text[renderchar]].Width;
			int Height =	scrolltextChars.Chars[text[renderchar]].Height;
			int OffsetX =	scrolltextChars.Chars[text[renderchar]].XOffset;
			int OffsetY =	scrolltextChars.Chars[text[renderchar]].YOffset;


			glTexCoord2f(float(CharX) / float(scrolltextChars.Width), float(CharY) / float(scrolltextChars.Height) ); 
			glVertex2f(endpos + float(OffsetX), float (OffsetY));

			glTexCoord2f(float(CharX + Width) / float(scrolltextChars.Width), float(CharY) / float(scrolltextChars.Height) ); 
			glVertex2f(endpos + float(OffsetX + Width), float (OffsetY));

			glTexCoord2f(float(CharX + Width) / float(scrolltextChars.Width), float(CharY+Height) / float(scrolltextChars.Height) ); 
			glVertex2f(endpos + float(OffsetX + Width), float (Height + OffsetY));

			glTexCoord2f(float(CharX) / float(scrolltextChars.Width), float(CharY+Height) / float(scrolltextChars.Height) ); 
			glVertex2f(endpos + float(OffsetX), float (Height + OffsetY));

			endpos += float(scrolltextChars.Chars[text[renderchar]].XAdvance);
			renderchar++;
			if (renderchar > int(text.length()))
			{
				renderchar = 0;
			}
		}
		glEnd();

	}
protected:
private:
	int screenwidth_, screenheight_;
	GLuint texture;
	int currentchar;	// current offset into the scroller
	float baseoffset;
	std::string text;
	float scrolltextpos;
	Charset scrolltextChars;

	// Camera used for this bit of the scene
	Resurrection::CameraSystem::DefaultView view;
	Resurrection::CameraSystem::OrthographicWorldCamera camera;

};


class CopperBars : public IDemoScene
{
public:
	CopperBars(int count, std::string const& filename, const GLfloat colors[][4]) 
		: NUMBER_OF_BARS(count), camera(0.0f, 1.0f, 1.0f, 0.0f, 1.0f, -1.0f),barsangles(new float[count]),rastercol(new float*[count])
	{
		
		texture = loadTexture(filename);
		camera.setView(&view);
		camera.updateViewMatrix();

		for (int x = 0; x < NUMBER_OF_BARS; x++)
		{
			barsangles[x] = x*10.0f;
		}
		
		for(int x = 0; x < NUMBER_OF_BARS; x++)
		{
			rastercol[x] = new float[4];
			for(int y = 0; y < 4; y++)
				rastercol[x][y] = colors[x][y];
			
		}

	}
	~CopperBars()
	{
		delete[] barsangles;
		for(int x = 0; x < NUMBER_OF_BARS; x++)
		{
			delete[] rastercol[x];
		}
		delete[] rastercol;

		glDeleteTextures(1,&texture);
	};
	void Update(int currenttime)
	{
		for (int x = 0; x < NUMBER_OF_BARS; x++)
		{
			barsangles[x] += 1.0f;
			if(barsangles[x] > 360.0f )
				barsangles[x] = 0;
		}
	}

	void Render()
	{
		camera.setMatricies();
		glBindTexture(GL_TEXTURE_2D,texture);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
		glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
		glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
		glEnable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);	// set the required blending mode
		for(int x = 0; x < NUMBER_OF_BARS; x++)
		{
			glPushMatrix();
			glTranslatef(0.0f,0.5f - (std::sin(DegToRad(barsangles[x]))/3.0f), 0.0f);
			glColor4fv(rastercol[x]);
			glBegin(GL_QUADS);
			{
				glTexCoord2f(0.0f, 0.0f);	glVertex2f(0.0f,-0.05f);	// top left
				glTexCoord2f(0.0f, 1.0f);	glVertex2f(0.0f,0.05f);
				glTexCoord2f(1.0f, 1.0f);	glVertex2f(1.0f,0.05f);
				glTexCoord2f(1.0f, 0.0f);	glVertex2f(1.0f,-0.05f);
			}
			glEnd();

			glPopMatrix();
		}
	}
protected:
private:
	const int NUMBER_OF_BARS;
	float * barsangles;
	GLuint texture;

	GLfloat **rastercol;

	// View for this bit of scene
	Resurrection::CameraSystem::DefaultView view;
	Resurrection::CameraSystem::OrthographicWorldCamera camera;
};

class ParallaxScroller : public IDemoScene
{
public:
	ParallaxScroller(int layers, std::string const &filename) 
		: NUMBER_OF_LAYERS(layers), layerscroll(new float[layers]),camera(0.0f, 1.0f, 1.0f, 0.0f, 1.0f, -1.0f)
	{
		texture = loadTexture(filename);
		camera.setView(&view);
		camera.updateViewMatrix();

		for (int x = 0; x < NUMBER_OF_LAYERS; x++)
		{
			layerscroll[x] = 0.0f;
		}

		glAlphaFunc(GL_GREATER, 0.9f);				// Alpha test func for parallax scroller texture	

	}
	~ParallaxScroller()
	{
		delete[] layerscroll;
		glDeleteTextures(1,&texture);
	}

	void Update(int currenttime)
	{
		for(int x = 0; x < NUMBER_OF_LAYERS; x++)
		{
			int invLayer = x;
			layerscroll[x] += float(invLayer+1) * 0.01f;
			if(layerscroll[x] > 1.0f)
				layerscroll[x] = -1.0f;
		}
	}

	void Render()
	{
		camera.setMatricies();
		glBindTexture(GL_TEXTURE_2D,texture);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_REPLACE);
		glColor4f(1.0f,1.0f,1.0f,1.0f);

		glDisable(GL_BLEND);
		glEnable(GL_ALPHA_TEST);
		glBegin(GL_QUADS);
		{
			float layeroffset = 0.0f;
			for(int layer = 0; layer < NUMBER_OF_LAYERS; layer++)
			{
				// two test quads for top and bottom
				glTexCoord2f(-layerscroll[layer] , 0.0f);			glVertex2f(0.0f,0.0f + layeroffset);
				glTexCoord2f(-layerscroll[layer] , 1.0f);			glVertex2f(0.0f,0.1f + layeroffset);
				glTexCoord2f(-layerscroll[layer] + 5.0f, 1.0f);		glVertex2f(1.0f,0.1f + layeroffset);
				glTexCoord2f(-layerscroll[layer] + 5.0f, 0.0f);		glVertex2f(1.0f,0.0f + layeroffset);


				glTexCoord2f(-layerscroll[layer] , 0.0f);			glVertex2f(0.0f,0.9f - layeroffset);
				glTexCoord2f(-layerscroll[layer] , -1.0f);			glVertex2f(0.0f,1.0f - layeroffset);
				glTexCoord2f(-layerscroll[layer] + 5.0f, -1.0f);	glVertex2f(1.0f,1.0f - layeroffset);
				glTexCoord2f(-layerscroll[layer] + 5.0f, 0.0f);		glVertex2f(1.0f,0.9f - layeroffset);

				layeroffset += 0.02f;
			}

		}
		glEnd();
	}

protected:
private:
	const int NUMBER_OF_LAYERS;
	GLuint texture;
	float * layerscroll;

	// View for this bit of scene
	Resurrection::CameraSystem::DefaultView view;
	Resurrection::CameraSystem::OrthographicWorldCamera camera;
};

class ScrollingBackGround : public IDemoScene
{
public:
	ScrollingBackGround(std::vector<std::string> const &filenames, int delay) : camera(0.0f, 1.0f, 1.0f, 0.0f, 1.0f, -1.0f), delay(delay),currentTexture(0),
		nextTexture(0), xpos(0.0f), ypos(0.0f), currentAngle(0.0f), blendend(0.0f), blending(false)
	{
		for (int x = 0; x < int(filenames.size()); x++)
		{
			GLuint texture = loadTexture(filenames[x]);
			textures.push_back(texture);
		}

		camera.setView(&view);
		camera.updateViewMatrix();
	}

	void Update(int currenttime)
	{
		if(blending)
		{
			blendfactor -= 0.02f;

			if(currenttime > blendend)
			{
				blending = false;

				currentTexture = nextTexture;
			}
		}
		
		if(stime + delay < currenttime)
		{
			// swap textures
			nextTexture = currentTexture + 1;
			if (nextTexture == textures.size())
			{
				nextTexture = 0;
			}
			stime = currenttime;
			blendend = currenttime + 2000;	// blend over 2 seconds
			blending = true;
			blendfactor = 1.0f;
		}

		
		currentAngle += 0.5f;
		xpos = -1.0f - (sin(DegToRad(currentAngle)) + cos(DegToRad(2*currentAngle))) / 2.0f;
		ypos = -1.0f - (cos(DegToRad(currentAngle)) * sin(DegToRad(currentAngle)))/ 2.0f;
	}

	void Render()
	{
		camera.setMatricies();
		glBindTexture(GL_TEXTURE_2D,textures[currentTexture]);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE);
		glDisable(GL_ALPHA_TEST);

		if (blending)
		{
			glEnable(GL_BLEND);
			glColor4f(1.0f, 1.0f, 1.0f, blendfactor);
		}
		else
		{
			glDisable(GL_BLEND);
			glColor4f(1.0f, 1.0f,1.0f, 1.0f);
		}
			
		
		glBegin(GL_QUADS);
		{
			glTexCoord2f(0.0f, 0.0f);	glVertex2f(xpos, ypos);	// top left corner
			glTexCoord2f(0.0f, 10.0f);	glVertex2f(xpos, ypos + 3.0f);
			glTexCoord2f(10.0f, 10.0f);	glVertex2f(xpos + 3.0f, ypos + 3.0f);
			glTexCoord2f(10.0f, 0.0f);	glVertex2f(xpos + 3.0f, ypos);
		}
		glEnd();

		if(blending)
		{
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f - blendfactor);
			glBindTexture(GL_TEXTURE_2D,textures[nextTexture]);
			glBegin(GL_QUADS);
			{
				glTexCoord2f(0.0f, 0.0f);	glVertex2f(xpos, ypos);	// top left corner
				glTexCoord2f(0.0f, 10.0f);	glVertex2f(xpos, ypos + 3.0f);
				glTexCoord2f(10.0f, 10.0f);	glVertex2f(xpos + 3.0f, ypos + 3.0f);
				glTexCoord2f(10.0f, 0.0f);	glVertex2f(xpos + 3.0f, ypos);
			}
			glEnd();
		}

	}

	~ScrollingBackGround()
	{
		glDeleteTextures(GLsizei(textures.size()), &textures[0]);
		
	}
protected:
	using IDemoScene::stime;
private:
	
	std::vector<GLuint> textures;
	int delay;	// delay between image swaps
	int currentTexture;
	int nextTexture;
	float currentAngle;
	float xpos, ypos;

	int blendend;	// time cross image blending ends
	bool blending;
	float blendfactor;

	// View for this bit of scene
	Resurrection::CameraSystem::DefaultView view;
	Resurrection::CameraSystem::OrthographicWorldCamera camera;
};

#define LOADBASSFUNCTION(f) *((void**)&f)=GetProcAddress(bass,#f)

class BassSoundSystem
{
public:
	BassSoundSystem(std::string const &filename) : loaded(false)
	{
		if(!(bass = LoadLibrary(L"bass.dll")))
		{
			return;
		}
		
		
		LOADBASSFUNCTION(BASS_Init);
		LOADBASSFUNCTION(BASS_Free);
		LOADBASSFUNCTION(BASS_MusicLoad);
		LOADBASSFUNCTION(BASS_ChannelPlay);
		LOADBASSFUNCTION(BASS_ChannelStop);

		if (!BASS_Init(-1,44100,BASS_DEVICE_MONO,0,NULL))
		{
			if(!BASS_Init(-1,22000,BASS_DEVICE_MONO,0,NULL))		
			{
				if(!BASS_Init(-1,22000,0,0,NULL))
					return;
			}
		}
		
		if(!(channel = BASS_MusicLoad(FALSE, filename.c_str(), 0,0,BASS_SAMPLE_LOOP|BASS_MUSIC_RAMPS|BASS_MUSIC_PRESCAN|BASS_SAMPLE_MONO,0)))
			return;

		loaded = true;
	}
	~BassSoundSystem()
	{
		

	}
	void play()
	{
		if(loaded)
			BASS_ChannelPlay(channel,FALSE);
	}
	void stop()
	{
		if(loaded)
		{
			BASS_ChannelStop(channel);
			BASS_Free();
			FreeLibrary(bass);
		}
	}
protected:
private:
	bool loaded;
	HINSTANCE bass;
	int channel;
};

int _tmain(int argc, _TCHAR* argv[])
{
	
	const int WIN_WIDTH = 800;
	const int WIN_HEIGHT = 600;
	
	OpenGLWFW::WindowManager WinMgr;
	try
	{
	
		if(!WinMgr.FindCompatibleOGLMode())
			return -1;
		if(!WinMgr.FindCompatibleDisplayMode(WIN_WIDTH,WIN_HEIGHT))
			return -2;
		//WinMgr.SetFullScreen(OpenGLWFW::winprops::fullscreen);
		WinMgr.CreateWin();
		WinMgr.Show();
	}
	catch (std::exception &e) 
	{
		return -3;
	}

	// Standard OpenGL setup stuff
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);		// This Will Clear The Background Color To black
	glClearDepth(1.0);							// Enables Clearing Of The Depth Buffer
	glDepthFunc(GL_LEQUAL);						// The Type Of Depth Test To Do
	glEnable(GL_DEPTH_TEST);					// Turn Depth Testing On
	glShadeModel(GL_SMOOTH);					// Enables Smooth Color Shading
	
	glColor4f(1.0f, 1.0f, 1.0f, 0.0f);					// Full Brightness.  0% Alpha
	glViewport(0,0,WIN_WIDTH,WIN_HEIGHT);

	const int NUMBER_OF_BARS = 8;
	GLfloat rastercol[NUMBER_OF_BARS][4]=				// Array For Top Colors
	{
		// Dark:  Red, Orange, Yellow, Green, Blue
		{.5f,0.0f,0.0f,1.0f},{0.5f,0.25f,0.0f,1.0f},{0.5f,0.5f,0.0f,1.0f},{0.0f,0.5f,0.0f,1.0f},
		{0.0f,0.5f,0.5f,1.0f},{0.0,0.25,0.5,1.0f},{0.0,0.25,0.25,1.0f},{0.0,0.0,0.25,1.0f}
	};
	

	const int NUMBER_OF_LAYERS = 4;	// number of parallax scroll layers

	
	ParallaxScroller pscroller(NUMBER_OF_LAYERS,"data\\ghost.png");
	ScrollText textScroller(WIN_WIDTH,WIN_HEIGHT,"data\\scrolltext_00.png","data\\scrolltext.fnt");
	CopperBars bars(NUMBER_OF_BARS,"data\\raster2.png",rastercol);

	std::vector<std::string> filenames;
	filenames.push_back("data\\bg1.jpg");
	filenames.push_back("data\\bg2.jpg");
	filenames.push_back("data\\bg3.jpg");
	filenames.push_back("data\\bg4.jpg");
	filenames.push_back("data\\bg5.jpg");
	filenames.push_back("data\\bg6.jpg");
	filenames.push_back("data\\bg7.jpg");
	filenames.push_back("data\\bg8.jpg");
	filenames.push_back("data\\bg9.jpg");
	filenames.push_back("data\\bg10.jpg");

	ScrollingBackGround bgscroller(filenames,10000);	// delay in ms, 1000 = 1sec

	BassSoundSystem sound("data\\BJ-SMELL.S3M");

	timeBeginPeriod(1);	// set timer res to 1ms -- WIN32 ONLY!!
	float time_delta = 20.0f;
	int now = timeGetTime();

	bars.start(now);
	pscroller.start(now);
	textScroller.start(now);
	bgscroller.start(now);
	sound.play();

	glEnable(GL_TEXTURE_2D);

	while (WinMgr.Dispatch())
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		int currenttime = timeGetTime();
		if(currenttime - now >= time_delta)
		{
				
			bars.Update(currenttime);
			pscroller.Update(currenttime);
			textScroller.Update(currenttime);
			bgscroller.Update(currenttime);

			now = timeGetTime();	// reset 'now' point
		}

		bgscroller.Render();
		bars.Render();
		pscroller.Render();
		textScroller.Render();
	
		WinMgr.SwapBuffers();

		if(GetAsyncKeyState(VK_ESCAPE) & 0x8000)
			break;
	}
	sound.stop();
	timeEndPeriod(1);	// undo timer res. setup

	return 0;
}

