#include "app.h"

int SDL_main(int argc, char* argv[])
{
    App app;
    if (!app_init(&app)) return -1;

    app_run(&app);

    app_cleanup(&app);
}