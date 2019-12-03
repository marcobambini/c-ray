//
//  ui.h
//  C-ray
//
//  Created by Valtteri Koskivuori on 19/02/2017.
//  Copyright © 2015-2019 Valtteri Koskivuori. All rights reserved.
//

#pragma once

struct renderer;

//FIXME: This should be in datatypes
struct display {
#ifdef UI_ENABLED
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
	SDL_Texture *overlayTexture;
#endif
	bool enabled;
	bool isBorderless;
	bool isFullScreen;
	float windowScale;
	
	int height;
	int width;
};

int initSDL(struct display *d);
void freeDisplay(struct display *disp);

void printDuration(float time);
void getKeyboardInput(struct renderer *r);
void drawWindow(struct renderer *r);
