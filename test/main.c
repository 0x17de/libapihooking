#include <signal.h>
#include <SDL2/SDL.h>
#include <GL/gl.h>

#include "libapihooking.h"

static int running = 1;

static void sigint()
{
    running = 0;
}

void myRenderHook()
{
    printf("myhook\n");
    fflush(stdout);
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f, 1.0f, 0.0f);
    glEnd();
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
}

int main()
{
    signal(SIGINT, sigint);
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    ApiHook_init();
    ApiHook_unprotect(myRenderHook);
    ApiHook_unprotect(SDL_RenderPresent);
    ApiHook_hookFunction(myRenderHook, SDL_RenderPresent);
    ApiHook_printFunction(myRenderHook);
    ApiHook_cleanup();

    SDL_Event event;
    while(running)
    {
        SDL_PollEvent(&event);
        if (event.type == SDL_KEYDOWN)
            if (event.key.keysym.sym == SDLK_ESCAPE)
                running = 0;
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3f(1.0f, 0.0f, 1.0f);
        glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(1.0f, 0.0f, 0.0f);
        glEnd();

        printf("RenderLoop\n");
        fflush(stdout);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}
