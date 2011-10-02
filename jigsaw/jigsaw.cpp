// Code wrote by Henrique Gemignani Passos Lima

#include "SDL/SDL.h"
#include <SDL/SDL_image.h>
#include <SDL/SDL_opengl.h>
#include "Timer.h"
#include "Piece.h"
#include "PieceSet.h"
#include "Drag.h"
#include <iostream>
#include <algorithm>
#include <ctime>
#include <cmath>

//Screen attributes
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000
#define SCREEN_BPP 32
#define MAX_SCREEN_SCALE 0.9f

int RESOLUTION_WIDTH = -1, RESOLUTION_HEIGHT = -1;

// The frame rate
#define FRAMES_PER_SECOND 60

GLuint loadTex ( char *filename, int *image_width, int *image_height ) {
    GLuint retval;
    SDL_Surface *sdlimage;
    int w, h, i, j, bpp;
    Uint8 *srcPixel, *dstPixel, *raw;
    Uint32 truePixel;
    GLenum errorCode;

    sdlimage = IMG_Load(filename);

    if ( !sdlimage ) {
        fprintf(stderr, "SDL_Image load error: %s\n", IMG_GetError());
        return 0;
    }
    if ( sdlimage->format->BytesPerPixel < 2 ) {
        fprintf(stderr, "Bad image -- not true color!\n");
        return 0;
    }

    *image_width = w = sdlimage->w;
    *image_height = h = sdlimage->h;

    raw = (Uint8 *)malloc( w * h * 4 );
    dstPixel = raw;

    SDL_LockSurface( sdlimage );

    bpp = sdlimage->format->BytesPerPixel;

    for ( i = 0; i < h ; i++ ) {
        for ( j = 0 ; j < w ; j++ ) {
            srcPixel = (Uint8 *)sdlimage->pixels + i * sdlimage->pitch + j * bpp;
            switch (bpp) {
            case 1:
                truePixel = srcPixel[0];
                break;

            case 2:
                truePixel = srcPixel[0] | srcPixel[1] << 8;
                break;

            case 3:
                if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                    truePixel = srcPixel[0] << 16 | srcPixel[1] << 8 | srcPixel[2];
                } else {
                    truePixel = srcPixel[0] | srcPixel[1] << 8 | srcPixel[2] << 16;
                }
                break;

            case 4:
                //truePixel = *(Uint32 *)srcPixel;
                truePixel = srcPixel[0] | srcPixel[1] << 8 | srcPixel[2] << 16 | srcPixel[3] << 24;
                break;

            default:
                printf("Image bpp of %d unusable\n", bpp);
                free(raw);
                return 0;
                break;
            }
            SDL_GetRGBA( truePixel, sdlimage->format, &(dstPixel[0]), &(dstPixel[1]), &(dstPixel[2]), &(dstPixel[3]));
            dstPixel++;
            dstPixel++;
            dstPixel++;
            dstPixel++;
        }
    }
    SDL_UnlockSurface( sdlimage );
    SDL_FreeSurface( sdlimage );

    while ( glGetError() ) { ; }

    glGenTextures( 1, &retval );
    glBindTexture( GL_TEXTURE_2D, retval );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glBlendFunc(GL_SRC_ALPHA,GL_ONE);

    errorCode = glGetError();
    if ( errorCode != 0 ) {
        if ( errorCode == GL_OUT_OF_MEMORY ) {
            printf("Out of texture memory!\n");
        }
        free(raw);
        return 0;
    }
    gluBuild2DMipmaps( GL_TEXTURE_2D, 4, w, h, GL_RGBA, GL_UNSIGNED_BYTE, raw);

    errorCode = glGetError();
    if ( errorCode != 0 ) {
        if ( errorCode == GL_OUT_OF_MEMORY ) {
            printf("Out of texture memory!\n");
        }
        free(raw);
        return 0;
    }
    free(raw);
    return retval;
}

bool changeResolution(int width, int height) {
    //Create Window
    if( SDL_SetVideoMode( width, height, SCREEN_BPP, SDL_OPENGL ) == NULL )
        return false;

    glViewport(0, 0, width, height);
    glMatrixMode( GL_PROJECTION );

    glLoadIdentity();
    glOrtho( 0, 1, 1, 0, -1, 1 );

    //Initialize modelview matrix
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    return true;
}


bool init() {
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) < 0 )
        return false;

	const SDL_VideoInfo *videoinfo = SDL_GetVideoInfo();
	RESOLUTION_WIDTH = videoinfo->current_w;
	RESOLUTION_HEIGHT = videoinfo->current_h;

    //Initialize OpenGL
    changeResolution(SCREEN_WIDTH, SCREEN_HEIGHT);

    //Set clear color
    glClearColor( 0, 0, 0, 0 );

    glEnable(GL_TEXTURE_2D);            // Enable Texture Mapping
    //glEnable(GL_BLEND);

    //If there was any errors
    if( glGetError() != GL_NO_ERROR )
        return false;
    //Set caption
    SDL_WM_SetCaption( "Jigsaw", NULL );

    return true;
}

void clean_up() {
    //Quit SDL
    SDL_Quit();
}

void minmax(float *a, float *b) {
    float aux = *a;
    *a = std::min(*a, *b);
    *b = std::max(aux, *b);
}

void printTime(FILE *out, int time) {
    time /= 1000; // Time now in seconds.
    int seconds = time % 60;
    time /= 60;
    int minutes = time % 60;
    time /= 60;
    if(time > 0)
        fprintf(out, "%dh ", time);
    if(minutes > 0)
        fprintf(out, "%dmin ", minutes);
    fprintf(out, "%ds", seconds);
}

int main(int argc, char *argv[]) {
    //Quit flag
    bool quit = false;
	if(argc == 1) {
		fprintf(stderr, "No file given.\n");
		exit(1);
	}

    //Initialize
    if( init() == false )
        return 1;

    int image_width, image_height;
    GLuint thetexture = loadTex(argv[1], &image_width, &image_height);
    glBindTexture(GL_TEXTURE_2D, thetexture);

    int nx = 8, ny = 8;
    if(argc >= 3)
        nx = ny = atoi(argv[2]);

    int screenwidth = image_width, screenheight = image_height;

    float xmul = (float)(screenwidth) / SCREEN_WIDTH,
          ymul = (float)(screenheight) / SCREEN_HEIGHT;

    nx = (int)(nx * xmul);
    ny = (int)(ny * ymul);

    int total = nx*ny;

	float scale = 1.0f;
    if(argc >= 4) {
        scale = (float) atof(argv[3]);
    } else {
		if(RESOLUTION_WIDTH * MAX_SCREEN_SCALE < screenwidth)
			scale = RESOLUTION_WIDTH * MAX_SCREEN_SCALE / screenwidth;
		if(RESOLUTION_HEIGHT * MAX_SCREEN_SCALE < screenheight)
			scale = std::min(scale, RESOLUTION_HEIGHT * MAX_SCREEN_SCALE / screenheight);
	}
    screenwidth = (int)(screenwidth * scale);
    screenheight = (int)(screenheight * scale);
    srand((unsigned int) time(NULL));

    changeResolution(screenwidth, screenheight);
	PieceSet *pieces = new PieceSet(nx, ny);
	pieces->Scramble();

    //The frame rate regulator
    Timer fps, timer;

    int moves = 0;

    int selected_x = -1, selected_y = -1;
    float offset_x, offset_y;
    float position_x, position_y;

    float drag_offset_x, drag_offset_y;
    float drag_end_x, drag_end_y;
	drag_offset_x = drag_offset_y = drag_end_x = drag_end_y = -1;
    int rectsizex = -1, rectsizey = -1;
    bool leftdrag = false, rightdrag = false, rect_up = false;
    //Wait for user exit

    timer.start();

	SDL_Event event;
    while( quit == false ) {
        //Start the frame timer
        fps.start();

        //While there are events to handle
        while( SDL_PollEvent( &event ) ) {
            //Handle key presses
            if( event.type == SDL_KEYDOWN ) {
                switch( event.key.keysym.sym ) {
                    case SDLK_ESCAPE: quit = true;
                    default: ;
                }
            }
			else if( pieces->get_matches() >= total) {
                // do nothing
            }
            else if( event.type == SDL_MOUSEBUTTONDOWN ) {
                if(event.button.button == SDL_BUTTON_LEFT && !rightdrag) {
                    offset_x = (float)(nx) * event.motion.x / screenwidth;
                    offset_y = (float)(ny) * event.motion.y / screenheight;

					if(rect_up) {
						int startx = (int)(drag_offset_x);
						int starty = (int)(drag_offset_y);
						for(int i = 0; i < rectsizex; i++)
							for(int j = 0; j < rectsizey; j++)
								pieces->get_current(startx + i, starty + j)->set_color(Color(1.0f, 1.0f, 1.0f));
					}


                    rect_up = !(offset_x < drag_offset_x || drag_end_x + 1 < offset_x ||
						offset_y < drag_offset_y || drag_end_y + 1 < offset_y);
                    if(!rect_up) {
                        drag_end_x = drag_offset_x = (float)(nx) * event.motion.x / screenwidth;
                        drag_end_y = drag_offset_y = (float)(ny) * event.motion.y / screenheight;
                        rectsizex = rectsizey = 1;
                        rect_up = true;
                    }
					leftdrag = true;
					// Store the piece we're dragging.
					selected_x = (int)(offset_x);
					selected_y = (int)(offset_y);

					// Store the position of the piece relative to the mouse
					offset_x = (selected_x - offset_x) / nx;
					offset_y = (selected_y - offset_y) / ny;
                } else if(event.button.button == SDL_BUTTON_RIGHT) {
                    rightdrag = true;

					if(rect_up) {
						int startx = (int)(drag_offset_x);
						int starty = (int)(drag_offset_y);
						for(int i = 0; i < rectsizex; i++)
							for(int j = 0; j < rectsizey; j++)
								pieces->get_current(startx + i, starty + j)->set_color(Color(1.0f, 1.0f, 1.0f));
					}

                    rect_up = false;

                    // Store the rect origin.
                    drag_offset_x = (float)(nx) * event.motion.x / screenwidth;
                    drag_offset_y = (float)(ny) * event.motion.y / screenheight;
                }

            } else if( event.type == SDL_MOUSEBUTTONUP ) {
                if(event.button.button == SDL_BUTTON_LEFT && rect_up) {
                    int finalx = nx * event.motion.x / screenwidth;
                    int finaly = ny * event.motion.y / screenheight;
                    int startx = (int)(drag_offset_x);
                    int starty = (int)(drag_offset_y);
                    finalx += (startx - selected_x);
                    finaly += (starty - selected_y);
                    if(finalx >= 0 && finaly >= 0 && finalx + rectsizex - 1 < nx && finaly + rectsizey - 1 < ny) {
						{
							for(int i = 0; i < rectsizex; i++)
								for(int j = 0; j < rectsizey; j++)
									pieces->get_current(startx + i, starty + j)->set_alpha(1.0f);
						}
                        if(startx >= finalx) {
                            for(int i = 0; i < rectsizex; i++) {
                                if(starty >= finaly)
                                    for(int j = 0; j < rectsizey; j++)
										pieces->Swap(startx + i, starty + j, finalx + i, finaly + j);
                                else
                                    for(int j = rectsizey - 1; j >= 0; j--)
                                        pieces->Swap(startx + i, starty + j, finalx + i, finaly + j);
                            }
                        } else {
                            for(int i = rectsizex - 1; i >= 0; i--) {
                                if(starty >= finaly)
                                    for(int j = 0; j < rectsizey; j++)
                                        pieces->Swap(startx + i, starty + j, finalx + i, finaly + j);
                                else
                                    for(int j = rectsizey - 1; j >= 0; j--)
                                        pieces->Swap(startx + i, starty + j, finalx + i, finaly + j);
                            }
                        }
                    }
                    selected_x = selected_y = -1;
                    moves++;
                    rect_up = false;
                    leftdrag = false;
                    drag_offset_x = drag_offset_y = drag_end_x = drag_end_y = -1;

                } else if(event.button.button == SDL_BUTTON_RIGHT) {
                    // Store the rect end
                    drag_end_x = (float)(nx) * event.motion.x / screenwidth;
                    drag_end_y = (float)(ny) * event.motion.y / screenheight;

                    // It's handy to have the end be to the bottom-right of the origin.
                    minmax(&drag_offset_x, &drag_end_x);
                    minmax(&drag_offset_y, &drag_end_y);

                    // Store the rect size
                    rectsizex = (int)(drag_end_x) - (int)(drag_offset_x) + 1;
                    rectsizey = (int)(drag_end_y) - (int)(drag_offset_y) + 1;

                    drag_offset_x = floor(drag_offset_x);
                    drag_offset_y = floor(drag_offset_y);
                    drag_end_x = floor(drag_end_x);
                    drag_end_y = floor(drag_end_y);

                    rect_up = true;
                    rightdrag = false;

					{
						int startx = (int)(drag_offset_x);
						int starty = (int)(drag_offset_y);
						// Mouse pointer does not necessarily hold the origin, so fix it
						for(int i = 0; i < rectsizex; i++)
							for(int j = 0; j < rectsizey; j++)
								pieces->get_current(startx + i, starty + j)->set_color(Color(0.5f, 0.5f, 1.0f));
					}
                }
            }
            else if( event.type == SDL_MOUSEMOTION ) {
                // Keep track of the mouse position in a handy format.
                position_x = (float)(event.motion.x) / screenwidth;
                position_y = (float)(event.motion.y) / screenheight;
            }
            if( event.type == SDL_QUIT ) {
                quit = true;
            }
        }
        for(int i = 0; i < nx; i++)
            for(int j = 0; j < ny; j++) {
                // Dont't render the dragged rect, if we're dragging one.
                if((int)(drag_offset_x) <= i && i <= (int)(drag_end_x) &&
                   (int)(drag_offset_y) <= j && j <= (int)(drag_end_y)) {
                    if(leftdrag)
                        continue;
                    else if(rect_up)
                        glColor3f(0.50f, 0.50f, 1.0f);
                } else
                    glColor3f(1.0f, 1.0f, 1.0f);
                pieces->get_current(i,j)->Render();
            }

        // Dragging one piece, render it on mouse. Also, keep the position of the mouse relative to the piece constant.
        if(leftdrag && !rect_up && selected_x != -1 && selected_y != -1)
            pieces->get_current(selected_x,selected_y)->CustomRender(position_x + offset_x, position_y + offset_y);

        if(leftdrag && rect_up && selected_x != -1 && selected_y != -1) {
            int startx = (int)(drag_offset_x);
            int starty = (int)(drag_offset_y);

            // Mouse pointer does not necessarily hold the origin, so fix it
            float custom_offx = position_x + offset_x + (startx - selected_x) / (float)(nx);
            for(int i = 0; i < rectsizex; i++) {
                float custom_offy = position_y + offset_y + (starty - selected_y) / (float)(ny);
                for(int j = 0; j < rectsizey; j++) {
					pieces->get_current(startx + i, starty + j)->CustomRender(custom_offx, custom_offy);
                    custom_offy += 1.0f / ny;
                }
                custom_offx += 1.0f / nx;
            }
        }
        if(rightdrag) {
            glEnable(GL_BLEND);
            glRectf( drag_offset_x / nx, drag_offset_y / ny, position_x, position_y );
            glDisable(GL_BLEND);
        }
        //Update screen
        SDL_GL_SwapBuffers();

        //Clear the screen
        glClear( GL_COLOR_BUFFER_BIT );

        //Cap the frame rate
        if( fps.get_ticks() < 1000 / FRAMES_PER_SECOND )
            SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - fps.get_ticks() );
    }
    FILE *out = fopen("out.txt", "a");
	fprintf(out, "Puzzle \"%s\" - ", argv[1]);
	if((pieces->get_matches()) == total) {
		fprintf(out, "Success with %d moves in ", moves);
		printTime(out, timer.get_ticks());
	} else {
		fprintf(out, "Quit after ");
		printTime(out, timer.get_ticks());
		fprintf(out, " with %d moves", moves);
	}
    printTime(out, timer.get_ticks());
    fprintf(out, ".\n");
    fclose(out);

    //Clean up
    clean_up();

    return 0;
}
