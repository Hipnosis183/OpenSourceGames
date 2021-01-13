#include "app.h"

class AppFreeDinkedit : public App {
public:
    AppFreeDinkedit();
	void init();
	void input(SDL_Event* ev);
	void logic();

	void input_global_shortcuts();
};
