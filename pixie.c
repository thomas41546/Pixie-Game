/*
 Game: Pixie
 Author: Thomas Polasek
 Date: October 22nd, 2011
 Licence: None. Use as you wish. However, some credit would be nice.
 */

#include "SDL/SDL.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

const int COLOR_WHITE =   0xFFFFFF;
const int COLOR_BLACK =   0x000000;
const int COLOR_RED =     0xFF0000;
const int COLOR_RED2 =     0xEE0000;
const int COLOR_BLUE =  0x33CCFF;
const int COLOR_GREEN =   0x00FF00;
const int PIXIE_SIZE =   5;
const int PIXIE_COUNT =   100;
const int CELL_WIDTH =   16;
const int CELL_HEIGHT =   12;
const int SCREEN_WIDTH =   640;
const int SCREEN_HEIGHT =   480;
const int MAIN_LOOP_DELAY =   10;

typedef struct {
	unsigned int is_frozen;
	unsigned int pixel_count;
	unsigned int is_herd;
} Cell;

typedef struct {
	unsigned int width,height;
	Cell * array;
} Cells;

typedef struct {
    unsigned int width,height;
	SDL_Surface* surface;
} Screen;

typedef struct {
	float x,y;
	int dx,dy;
	unsigned int is_frozen_px;
} Pixie;

typedef struct {
    unsigned int count;
	Pixie * array;
} Pixies;

static Screen screen;
static Cells cells;
static Pixies pixies;

int calculate_Cell_X(int x){
	return  x/(screen.width/cells.width);
}

int calculate_Cell_Y(int y){
	return y/(screen.height/cells.height);
}

void toggle_Cell(int mouse_x, int mouse_y){
	int i = calculate_Cell_X(mouse_x);
	int j = calculate_Cell_Y(mouse_y);
	(cells.array + j*cells.width + i)->is_frozen ^= 1;
}

int herded_all(){
	int i,j;
	for(j = 0; j < cells.height; j++){
		for(i = 0; i < cells.width; i++){
			if((cells.array + j*cells.width + i)->pixel_count && 
			   !(cells.array + j*cells.width + i)->is_herd){
				return 0;
			}
		}
	}
	return 1;
}

void update_Cells(){
	int i,j,k;
	
	for(j = 0; j < cells.height; j++){
		for(i = 0; i < cells.width; i++){
			(cells.array + j*cells.width + i)->pixel_count = 0;
			for(k = 0; k < pixies.count; k++){
				Pixie * p = (pixies.array + k);
				int cx = calculate_Cell_X( p->x  + ((p->dx+1)*PIXIE_SIZE)/2);
				int cy = calculate_Cell_Y( p->y  + ((p->dy+1)*PIXIE_SIZE)/2);
				if(cx == i && cy == j){
					(cells.array + j*cells.width + i)->pixel_count += 1;
				}
			}
		}
	}
}

void iterate_Pixies(){
	int i;
	for(i = 0; i < pixies.count; i++){
		Pixie * p = (pixies.array + i);
	
		int cx = calculate_Cell_X( p->x  + ((p->dx+1)*PIXIE_SIZE)/2);
		int cy = calculate_Cell_Y( p->y  + ((p->dy+1)*PIXIE_SIZE)/2);
	
		int tx = calculate_Cell_X(p->x + p->dx + ((p->dx+1)*PIXIE_SIZE)/2);
		int ty = calculate_Cell_Y(p->y + p->dy + ((p->dy+1)*PIXIE_SIZE)/2);
		
		p->is_frozen_px = 1;
		if(!(cells.array + ty*cells.width + tx)->is_frozen && 
			tx < cells.width && p->y + p->dy > 0 && 
			ty < cells.height && p->x + p->dx > 0){
			p->x += p->dx;
			p->y += p->dy;
			p->is_frozen_px = 0;
			continue;
		}
		else{
			if(ty >=  cells.height || p->y + p->dy <= 0){
				p->dy *= -1;
				continue;
			}
			if(tx >=  cells.width || p->x + p->dx <= 0){
				p->dx *= -1;
				continue;
			}
			
			if((cells.array + ty*cells.width + tx)->is_frozen){
				if(cy != ty){
					p->dy *= -1;
					continue;
				}
				if(cx != tx){
					p->dx *= -1;
					continue;
				}
			}
		}
		
		
	}
}

void render_Screen(){
	SDL_Rect rect = {0,0,screen.width,screen.height};
	SDL_FillRect(screen.surface, &rect, COLOR_WHITE);
	
	
	int cell_pixel_width = screen.width/cells.width;
	int cell_pixel_height = screen.height/cells.height;
	
	int i,j;
	
	for(j = 0; j < cells.height; j++){
		for(i = 0; i < cells.width; i++){
			
			SDL_Rect rect = {i*cell_pixel_width,j*cell_pixel_height,
							 cell_pixel_width,cell_pixel_height};
			
			if((cells.array + j*cells.width + i)->is_herd && 
			   (cells.array + j*cells.width + i)->is_frozen){
				SDL_FillRect(screen.surface, &rect, COLOR_BLUE);
			}
			else if((cells.array + j*cells.width + i)->is_frozen){
				if((cells.array + j*cells.width + i)->pixel_count){
					SDL_FillRect(screen.surface, &rect, COLOR_RED2);
				}
				else{
				SDL_FillRect(screen.surface, &rect, COLOR_RED);
				}
			}
			else if((cells.array + j*cells.width + i)->is_herd){
				SDL_FillRect(screen.surface, &rect, COLOR_GREEN);
			}
			else{
				SDL_FillRect(screen.surface, &rect, COLOR_WHITE);
			}
					
		}
		
	}
	for(i = 0; i < pixies.count; i++){
		Pixie * p = (pixies.array + i);
		if(p->is_frozen_px){
			SDL_Rect rect = {(int)p->x + (rand()%3 - 1),(int)p->y + (rand()%3 - 1),PIXIE_SIZE,PIXIE_SIZE};
			SDL_FillRect(screen.surface, &rect, COLOR_BLACK);
		}
		else{
			SDL_Rect rect = {(int)p->x,(int)p->y,PIXIE_SIZE,PIXIE_SIZE};
			SDL_FillRect(screen.surface, &rect, COLOR_BLACK);	
		}
	}
	
    SDL_Flip( screen.surface );
}

void exit_Game(){
	free(cells.array);
	free(pixies.array);
	SDL_Quit();
	exit(0);
}

int main( int argc, char* args[] ){
	int i,j;
	
	printf("Pixie - Made by Thomas Polasek (2011)\n");
	printf("Objective: Capture all of the black pixies in the green boundary to win.\n");
	
	srand ( time(NULL) );
	
    SDL_Init( SDL_INIT_EVERYTHING );

	screen.width = SCREEN_WIDTH;
	screen.height = SCREEN_HEIGHT;
    screen.surface = SDL_SetVideoMode( screen.width, screen.height, 24, SDL_SWSURFACE );
	
	cells.width = CELL_WIDTH;
	cells.height = CELL_HEIGHT;
	cells.array = (Cell *)malloc(sizeof(Cell)*cells.width*cells.height);
	
	
	for(j = 0; j < cells.height; j++){
		for(i = 0; i < cells.width; i++){
			(cells.array + j*cells.width + i)->is_frozen = 0;
			(cells.array + j*cells.width + i)->pixel_count = 0;
			(cells.array + j*cells.width + i)->is_herd = 0;
		}
	}
	i = rand() % (cells.width - 1);
	j = rand() % (cells.height - 1);
	(cells.array + j*cells.width + i)->is_herd = 1;
	(cells.array + j*cells.width + (i+1))->is_herd = 1;
	(cells.array + (j+1)*cells.width + i)->is_herd = 1;
	(cells.array + (j+1)*cells.width + (i+1))->is_herd = 1;
	
	pixies.count = PIXIE_COUNT;
	pixies.array = (Pixie *)malloc(sizeof(Pixie)*pixies.count);
	for(i = 0; i < pixies.count; i++){
		Pixie *p = (pixies.array + i);
		p->x = rand() % (screen.width-2) + 1;
		p->y = rand() % (screen.height-2) + 1;
		p->dx = -1;
		p->dy = 1;
		
		if(rand() % 2)
			p->dx *=-1;
		
		if(rand() % 2)
			p->dy *=-1;
		
		p->is_frozen_px = 0;
	}
	
	int is_game = 1;
	
	while(is_game){
		SDL_Event event;
		while ( SDL_PollEvent(&event) ) {
			switch (event.type) {
				case SDL_MOUSEBUTTONDOWN:
					toggle_Cell(event.button.x, event.button.y);
					break;
				
				case SDL_QUIT:
					is_game = 0;
					break;
			}
		}
		iterate_Pixies();
		
		update_Cells();
		
		if(herded_all()){
			printf("You won the game, nice!\n");
			for(i = 0; i < 50; i++){
				SDL_Rect rect = {0,0,screen.width,screen.height};
				SDL_FillRect(screen.surface, &rect, COLOR_BLACK);
				SDL_Flip( screen.surface );
				SDL_Delay( 20 );
				render_Screen();
				SDL_Delay( 20 );
				
			}
			exit_Game();
			return 0;
		}
		
		render_Screen();
		SDL_Delay( MAIN_LOOP_DELAY );
	}
	printf("You lost the game.\nNoob!\n");
    exit_Game();
	return 0;
}
