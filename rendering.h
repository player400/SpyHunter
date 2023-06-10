#pragma once

class Rendering
{
private:
	SDL_Surface* screen;
	SDL_Surface* charset;
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* scrtex;
	int background;

public:
	// draw a text txt on surface screen, starting from the point (x, y)
	// charset is a 128x128 bitmap containing character images
	void DrawString(int x, int y, const char* text)
	{
		int px, py, c;
		SDL_Rect s, d;
		s.w = 8;
		s.h = 8;
		d.w = 8;
		d.h = 8;
		while (*text)
		{
			c = *text & 255;
			px = (c % 16) * 8;
			py = (c / 16) * 8;
			s.x = px;
			s.y = py;
			d.x = x;
			d.y = y;
			SDL_BlitSurface(charset, &s, screen, &d);
			x += 8;
			text++;
		};
	};

	// draw a surface sprite on a surface screen in point (x, y)
	// (x, y) is the center of sprite on screen
	void DrawSurface(SDL_Surface* sprite, int x, int y)
	{
		SDL_Rect dest;
		dest.x = x - sprite->w / 2;
		dest.y = y - sprite->h / 2;
		dest.w = sprite->w;
		dest.h = sprite->h;
		SDL_BlitSurface(sprite, NULL, screen, &dest);
	};

	void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color)
	{
		int bpp = surface->format->BytesPerPixel;
		Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
		*(Uint32*)p = color;
	};

	// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
	void DrawLine(int x, int y, int l, int dx, int dy, Uint32 color)
	{
		for (int i = 0; i < l; i++)
		{
			DrawPixel(screen, x, y, color);
			x += dx;
			y += dy;
		};
	};

	// draw a rectangle of size l by k
	void DrawRectangle(int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor)
	{
		int i;
		DrawLine(x, y, k, 0, 1, outlineColor);
		DrawLine(x + l - 1, y, k, 0, 1, outlineColor);
		DrawLine(x, y, l, 1, 0, outlineColor);
		DrawLine(x, y + k - 1, l, 1, 0, outlineColor);
		for (i = y + 1; i < y + k - 1; i++)
			DrawLine(x + 1, i, l - 2, 1, 0, fillColor);
	};

	SDL_Surface* LoadTexture(char* filename)
	{
		SDL_Surface* result = SDL_LoadBMP(filename);
		if (result == NULL)
		{
			printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
			SDL_FreeSurface(screen);
			SDL_DestroyTexture(scrtex);
			SDL_DestroyWindow(window);
			SDL_DestroyRenderer(renderer);
			SDL_Quit();
			return nullptr;
		};
		return result;
	}

	//Resets the screen to background color and renders objects
	void Refresh()
	{
		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);
		SDL_FillRect(screen, NULL, background);
	}

	void Clear()
	{
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_FreeSurface(charset);
		SDL_Quit();
	}

	SDL_Surface* GetScreen()
	{
		return screen;
	}

	Rendering()
	{
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			printf("SDL_Init error: %s\n", SDL_GetError());
			exit(1);
		}
		int rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
		if (rc != 0)
		{
			SDL_Quit();
			printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
			exit(1);
		};
		SDL_SetWindowResizable(window, SDL_FALSE);
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
		SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_SetWindowTitle(window, "SpyHunter");
		screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
		scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
		SDL_ShowCursor(SDL_DISABLE);
		charset = LoadTexture("./cs8x8.bmp"); //loading font from a file and saving as "charset" surface
		SDL_SetColorKey(charset, true, 0x000000);
		background = SDL_MapRGB(screen->format, ROAD_COLOR);
	}
};
