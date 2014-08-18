#include "stdafx.h"
#include "Viewport.h"


Viewport::Viewport(RECT *r)
{
	POINT p;
	POINT offsetTopLeft, offsetBottomRight;

	screen = *r;

	p.x = (float)(screen.right - screen.left) / 2;
	offsetTopLeft.x = p.x - screen.left;
	offsetBottomRight.x = screen.right - p.x;

	p.y = (float)(screen.bottom - screen.top) / 2;
	offsetTopLeft.y = p.y - screen.top;
	offsetBottomRight.y = screen.bottom - p.y;


}


Viewport::~Viewport()
{
}
