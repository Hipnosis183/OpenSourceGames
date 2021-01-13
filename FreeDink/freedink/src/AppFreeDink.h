#include "app.h"
#include "IOTouchDragAnywhere.h"

class AppFreeDink : public App {
public:
	AppFreeDink();
	~AppFreeDink();
	void init();
	void input(SDL_Event* ev);
	void logic();

	void writeDinksmallwoodIni(bool);
	IOTouchDragAnywhere touchConv;
};
