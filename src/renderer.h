//
//  renderer.h
//  C-Ray
//
//  Created by Valtteri Koskivuori on 19/02/2017.
//  Copyright © 2017 Valtteri Koskivuori. All rights reserved.
//

#pragma once

struct world;
struct outputImage;
struct display;

/**
 Thread information struct to communicate with main thread
 */
struct threadInfo {
#ifdef WINDOWS
	HANDLE thread_handle;
	DWORD thread_id;
#else
	pthread_t thread_id;
#endif
	int thread_num;
	bool threadComplete;
};

/**
 Render tile, contains needed information for the renderer
 */
struct renderTile {
	int width;
	int height;
	struct coord begin;
	struct coord end;
	int completedSamples;
	bool isRendering;
	int tileNum;
	time_t start, stop;
};

enum renderOrder {
	renderOrderTopToBottom = 0,
	renderOrderFromMiddle,
	renderOrderToMiddle,
	renderOrderNormal,
	renderOrderRandom
};

/**
 Main renderer. Stores needed information to keep track of render status,
 as well as information needed for the rendering routines.
 */
struct renderer {
	//Source data
	struct world *scene; //Scene to render
	
	//State data
	struct outputImage *image; //Output image
	struct renderTile *renderTiles; //Array of renderTiles to render
	int tileCount; //Total amount of render tiles
	int renderedTileCount; //Completed render tiles
	double *renderBuffer;  //Double-precision buffer for multisampling
	unsigned char *uiBuffer; //UI element buffer
	int activeThreads; //Amount of threads currently rendering
	bool isRendering;
	bool renderPaused; //SDL listens for P key pressed, which sets this
	bool renderAborted;//SDL listens for X key pressed, which sets this
	time_t avgTileTime;//Used for render duration estimation
	int timeSampleCount;//Used for render duration estimation, amount of time samples captured
	int currentFrame;
	struct threadInfo *renderThreadInfo; //Info about threads
#ifndef WINDOWS
	pthread_attr_t renderThreadAttributes;
#endif
#ifdef UI_ENABLED
	struct display *mainDisplay;
#endif
	
	//Preferences data (Set by user)
	enum fileMode mode;
	enum renderOrder tileOrder;
	char *inputFilePath; //Directory to load input files from
	
	int threadCount; //Amount of threads to render with
	int sampleCount;
	int tileWidth;
	int tileHeight;
	
	bool smoothShading;//Unused
	bool antialiasing;
};

//Renderer
#ifdef WINDOWS
DWORD WINAPI renderThread(LPVOID arg);
#else
void *renderThread(void *arg);
#endif
void quantizeImage();
void reorderTiles(enum renderOrder order);
