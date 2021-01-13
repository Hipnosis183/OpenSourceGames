#ifdef __ANDROID__
/* GUI apps are compiled with main->SDL_main (and ld -shared) */
/* For a CLI app, re-introduce main (and disable -shared) */
extern "C" int SDL_main(int argc, char *argv[]);
int main(int argc, char* argv[]) {
	SDL_main(argc, argv);
}
#endif
