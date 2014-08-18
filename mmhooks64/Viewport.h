#pragma once

class Viewport
{
private:
	RECT screen;
	RECT view;

public:
	Viewport(RECT *r);
	~Viewport();
};

