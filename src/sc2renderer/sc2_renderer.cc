#include "sc2renderer/sc2_renderer.h"

#include "SDL.h"

#include <cassert>
#include <iostream>

namespace {
    SDL_Window* window_;
    SDL_Renderer* renderer_;

    SDL_Rect CreateRect(int x, int y, int w, int h) {
        SDL_Rect r;
        r.x = x;
        r.y = y;
        r.w = w;
        r.h = h;
        return r;
    }
}

namespace sc2 {

namespace renderer {

void Initialize(const char* title, int x, int y, int w, int h, unsigned int flags) {
    int init_result = SDL_Init(SDL_INIT_VIDEO);
    assert(!init_result);

    window_ = SDL_CreateWindow(title, x, y, w, h, flags == 0 ? SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE : flags);
    assert(window_);

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    assert(renderer_);

    // Clear window to black.
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);
}

void Shutdown() {
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
}

void Matrix1BPP(const char* bytes, int w_mat, int h_mat, int off_x, int off_y, int px_w, int px_h) {
    assert(renderer_);
    assert(window_);
    
    SDL_Rect rect = CreateRect(0, 0, px_w, px_h);
    for (size_t y = 0; y < h_mat; ++y) {
        for (size_t x = 0; x < w_mat; ++x) {
            rect.x = off_x + (int(x) * rect.w);
            rect.y = off_y + (int(y) * rect.h);

            size_t index = x + y * w_mat;
            unsigned char mask = 1 << (7 - (index % 8));
            unsigned char data = bytes[index / 8];
            bool value = (data & mask) != 0;

            if (value)
                SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
            else
                SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);

            SDL_RenderFillRect(renderer_, &rect);
        }
    }
}

void Matrix8BPPHeightMap(const char* bytes, int w_mat, int h_mat, int off_x, int off_y, int px_w, int px_h) {
    assert(renderer_);
    assert(window_);

    SDL_Rect rect = CreateRect(0, 0, px_w, px_h);
    for (size_t y = 0; y < h_mat; ++y) {
        for (size_t x = 0; x < w_mat; ++x) {
            rect.x = off_x + (int(x) * rect.w);
            rect.y = off_y + (int(y) * rect.h);

            // Renders the height map in grayscale [0-255]
            size_t index = x + y * w_mat;
            SDL_SetRenderDrawColor(renderer_, bytes[index], bytes[index], bytes[index], 255);
            SDL_RenderFillRect(renderer_, &rect);
        }
    }
}

void Matrix8BPPPlayers(const char* bytes, int w_mat, int h_mat, int off_x, int off_y, int px_w, int px_h) {
    assert(renderer_);
    assert(window_);

    SDL_Rect rect = CreateRect(0, 0, px_w, px_h);
    for (size_t y = 0; y < h_mat; ++y) {
        for (size_t x = 0; x < w_mat; ++x) {
            rect.x = off_x + (int(x) * rect.w);
            rect.y = off_y + (int(y) * rect.h);

            size_t index = x + y * w_mat;
            switch (bytes[index]) {
            case 0:
                SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
                break;
            case 1:
                // Self.
                SDL_SetRenderDrawColor(renderer_, 0, 255, 0, 255);
                break;
            case 2:
                // Enemy.
                SDL_SetRenderDrawColor(renderer_, 255, 0, 0, 255);
                break;
            case 3:
                // Neutral.
                SDL_SetRenderDrawColor(renderer_, 0, 0, 255, 255);
                break;
            case 4:
                SDL_SetRenderDrawColor(renderer_, 255, 255, 0, 255);
                break;
            case 5:
                SDL_SetRenderDrawColor(renderer_, 0, 255, 255, 255);
                break;
            default:
                SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
            }
            SDL_RenderFillRect(renderer_, &rect);
        }
    }
}

void ImageRGB(const char* bytes, int width, int height, int off_x, int off_y) {
    assert(renderer_);
    assert(window_);

    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom((void*)bytes, width, height, 24, 3 * width, 0xFF0000, 0x00FF00, 0x0000FF, 0x000000);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
    SDL_FreeSurface(surface);

    SDL_Rect dstRect = CreateRect(off_x, off_y, width, height);
    SDL_RenderCopy(renderer_, texture, NULL, &dstRect);

    SDL_DestroyTexture(texture);
}

void Render() {
    assert(renderer_);
    assert(window_);
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            Shutdown();
            return;
        }
    }

    SDL_RenderPresent(renderer_);
    SDL_RenderClear(renderer_);
}

}

}