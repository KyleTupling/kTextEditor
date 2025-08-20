/*
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define WINDOW_WIDTH    1000
#define WINDOW_HEIGHT   600
#define LINE_HEIGHT     20
#define LEFT_MARGIN     50
#define MAX_LINES       1024
#define MAX_LINE_LENGTH 1024

static SDL_Window*   window = NULL;
static SDL_Renderer* renderer = NULL;
static TTF_Font*     font = NULL;

static char lines[MAX_LINES][MAX_LINE_LENGTH];
static int num_lines = 1;

static int cursor_line = 0;
static int cursor_col = 0;

static void DrawText(const char* text, int x, int y, SDL_Color color) 
{
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dest = { x, y, surface->w, surface->h };
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, &dest);
    SDL_DestroyTexture(texture);
}

// Render the background of the current line
static void RenderLineHeight(void)
{
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);

    SDL_Rect line_rect = { LEFT_MARGIN, cursor_line * LINE_HEIGHT, WINDOW_WIDTH - LEFT_MARGIN, LINE_HEIGHT };

    SDL_RenderFillRect(renderer, &line_rect);
}

static void RenderEditor(void) 
{
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    SDL_Color textColor = { 230, 230, 230, 255 };
    SDL_Color lineNumberColor = { 100, 180, 100, 255 };

    RenderLineHeight();

    for (int i = 0; i < num_lines; i++) {
        char lineNumber[16];
        snprintf(lineNumber, sizeof(lineNumber), "%4d", i + 1);
        DrawText(lineNumber, 5, i * LINE_HEIGHT, lineNumberColor);
        DrawText(lines[i], LEFT_MARGIN, i * LINE_HEIGHT, textColor);
    }

    // Draw cursor
    int cursor_x = LEFT_MARGIN + cursor_col * 10; // 10px width per char (monospace assumption)
    int cursor_y = cursor_line * LINE_HEIGHT;
    SDL_Rect cursor_rect = { cursor_x, cursor_y, 2, LINE_HEIGHT };
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(renderer, &cursor_rect);

    SDL_RenderPresent(renderer);
}

void InsertCharacter(char c) {
    char* line = lines[cursor_line];
    size_t len = strlen(line);
    if (len + 1 < MAX_LINE_LENGTH) {
        memmove(&line[cursor_col + 1], &line[cursor_col], len - cursor_col + 1);
        line[cursor_col] = c;
        cursor_col++;
    }
}

static void HandleKey(SDL_KeyCode key) 
{
    char* line = lines[cursor_line];
    switch (key) 
    {
        case SDLK_BACKSPACE:
            if (cursor_col > 0) 
            {
                memmove(&line[cursor_col - 1], &line[cursor_col], strlen(&line[cursor_col]) + 1);
                cursor_col--;
            } 
            else if (cursor_line > 0) 
            {
                int prev_len = strlen(lines[cursor_line - 1]);
                if (prev_len + strlen(line) < MAX_LINE_LENGTH) 
                {
                    strcat(lines[cursor_line - 1], line);
                    for (int i = cursor_line; i < num_lines - 1; i++) 
                    {
                        strcpy(lines[i], lines[i + 1]);
                    }
                    num_lines--;
                    cursor_line--;
                    cursor_col = prev_len;
                }
            }
            break;
        case SDLK_RETURN:
            if (num_lines < MAX_LINES) 
            {
                for (int i = num_lines; i > cursor_line + 1; i--) 
                {
                    strcpy(lines[i], lines[i - 1]);
                }
                strcpy(lines[cursor_line + 1], &line[cursor_col]);
                line[cursor_col] = '\0';
                cursor_line++;
                cursor_col = 0;
                num_lines++;
            }
            break;
        case SDLK_LEFT:
            if (cursor_col > 0) cursor_col--;
            else if (cursor_line > 0) 
            {
                cursor_line--;
                cursor_col = strlen(lines[cursor_line]);
            }
            break;
        case SDLK_RIGHT:
            if (cursor_col < strlen(line)) cursor_col++;
            else if (cursor_line + 1 < num_lines) 
            {
                cursor_line++;
                cursor_col = 0;
            }
            break;
        case SDLK_UP:
            if (cursor_line > 0) 
            {
                cursor_line--;
                if (cursor_col > strlen(lines[cursor_line]))
                    cursor_col = strlen(lines[cursor_line]);
            }
            break;
        case SDLK_DOWN:
            if (cursor_line + 1 < num_lines) 
            {
                cursor_line++;
                if (cursor_col > strlen(lines[cursor_line]))
                    cursor_col = strlen(lines[cursor_line]);
            }
            break;
        default:
            break;
    }
}

static bool InitApp(void) 
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) return false;
    if (TTF_Init() != 0) return false;

    window = SDL_CreateWindow("kTextEditor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;

    font = TTF_OpenFont("resources/fonts/SourceCodePro-Bold.ttf", 16);
    if (!font) return false;

    SDL_StartTextInput();

    strcpy(lines[0], "");
    return true;
}

static void CleanupApp(void) {
    if (font) TTF_CloseFont(font);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_StopTextInput();
    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    if (!InitApp()) return 1;

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            else if (event.type == SDL_KEYDOWN) HandleKey(event.key.keysym.sym);
            else if (event.type == SDL_TEXTINPUT) InsertCharacter(event.text.text[0]);
        }

        RenderEditor();
        SDL_Delay(16); // ~60 FPS
    }

    CleanupApp();
    return 0;
}
*/