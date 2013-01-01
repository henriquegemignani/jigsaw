// Code wrote by Henrique Gemignani Passos Lima

#include <cmath>
#include <iostream>
#include <algorithm>
#include "GL/glew.h"
#define NO_SDL_GLEXT
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_opengl.h"
#ifdef _WIN32
#include <Windows.h>
#include <Commdlg.h>
#endif

#include "tools.h"
#include "logic.h"
#include "Timer.h"
#include "Piece.h"
#include "PieceSet.h"

//Screen attributes
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000
#define SCREEN_BPP 32
#define MAX_SCREEN_SCALE 0.9

int RESOLUTION_WIDTH = -1, RESOLUTION_HEIGHT = -1;

// The frame rate
#define FRAMES_PER_SECOND 60

GLuint loadTex ( char *filename, int *image_width, int *image_height ) {
    GLuint retval;
    SDL_Surface *sdlimage;
    int w, h, i, j, bpp;
    Uint8 *srcPixel, *dstPixel, *raw;
    Uint32 truePixel;

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

    CHECK_GL_ERROR();

    glGenTextures( 1, &retval );
    glBindTexture( GL_TEXTURE_2D, retval );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glBlendFunc(GL_SRC_ALPHA,GL_ONE);

    if(CHECK_GL_ERROR()) {
        free(raw);
        return 0;
    }

    gluBuild2DMipmaps( GL_TEXTURE_2D, 4, w, h, GL_RGBA, GL_UNSIGNED_BYTE, raw);

    free(raw);

    if(CHECK_GL_ERROR()) return 0;
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

char FileName[260];

bool init() {
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) < 0 )
        return false;

	const SDL_VideoInfo *videoinfo = SDL_GetVideoInfo();
	RESOLUTION_WIDTH = videoinfo->current_w;
	RESOLUTION_HEIGHT = videoinfo->current_h;

    //Set caption
    char title[260 + sizeof("Jigsaw: ")];
    strcpy(title, "Jigsaw: ");
    strcpy(title + sizeof("Jigsaw: ") - 1, FileName);
    SDL_WM_SetCaption( title, NULL );

    //Initialize OpenGL
    changeResolution(SCREEN_WIDTH, SCREEN_HEIGHT);

    //Set clear color
    glClearColor( 0, 0, 0, 0 );

    glEnable(GL_TEXTURE_2D);            // Enable Texture Mapping
    //glEnable(GL_BLEND);

    //If there was any errors
    if( glGetError() != GL_NO_ERROR )
        return false;


    return true;
}

bool InitOpenGLExtensions(void) {
    static bool extensions_init = false;
    if (extensions_init) return true;
    extensions_init = true;

    GLenum err = glewInit();

    if (GLEW_OK != err) {
        std::cout << "Error:" << glewGetErrorString(err) << std::endl;
        extensions_init = false;
        return false;
    }

    std::cout << "OpenGL Vendor: " << (char*) glGetString(GL_VENDOR) << std::endl;
    std::cout << "OpenGL Renderer: " << (char*) glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL Version: " << (char*) glGetString(GL_VERSION) << std::endl;
    std::cout << "OpenGL Extensions:\n" << (char*) glGetString(GL_EXTENSIONS) << std::endl << std::endl;

    return true;
}

void clean_up() {
    //Quit SDL
    SDL_Quit();
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

#ifdef _WIN32
static bool OpenFileDialog() {
    OPENFILENAME  ofn;        
    memset(&ofn,0,sizeof(ofn));
    ofn.lStructSize     = sizeof(ofn);
    ofn.hwndOwner       = NULL;
    ofn.hInstance       = NULL;
    ofn.lpstrFilter     = "Image Files (*.png,gif,jpg,jpeg,bmp,tga)\0*.png;*.gif;*.jpg;*.jpeg;*.bmp;*.tga\0\0";    
    ofn.lpstrFile       = FileName;
    ofn.nMaxFile        = MAX_PATH;
    ofn.lpstrTitle      = "Please Select A File To Open";
    ofn.Flags           = OFN_NONETWORKBUTTON |
                          OFN_FILEMUSTEXIST |
                          OFN_HIDEREADONLY;
    return GetOpenFileName(&ofn) != 0;
}
#endif

int main(int argc, char *argv[]) {
    //Quit flag
    bool quit = false;
	if(argc == 1) {
#ifdef _WIN32
        if(!OpenFileDialog())
            exit(1);
#else
		fprintf(stderr, "No file given.\n");
		exit(1);
#endif
	} else {
        strncpy(FileName, argv[1], sizeof(FileName));
    }

    //Initialize
    if( init() == false )
        return 1;
    InitOpenGLExtensions();

    int image_width, image_height;
    GLuint thetexture = loadTex(FileName, &image_width, &image_height);
    if(thetexture == 0) {
#ifdef _WIN32
        MessageBox(HWND_DESKTOP, "Invalid file.", "Fatal Error", MB_OK | MB_ICONERROR);
#else
		fprintf(stderr, "Invalid file.\n");
#endif
        exit(2);
    }
    glBindTexture(GL_TEXTURE_2D, thetexture);

    int nx = 8, ny = 8;
    if(argc >= 3)
        nx = ny = atoi(argv[2]);

    int screenwidth = image_width, screenheight = image_height;

    double xmul = (double)(screenwidth) / SCREEN_WIDTH,
          ymul = (double)(screenheight) / SCREEN_HEIGHT;

    nx = (int)(nx * xmul);
    ny = (int)(ny * ymul);


	double scale = 1.0;
    if(argc >= 4) {
        scale = (double) atof(argv[3]);
    } else {
		if(RESOLUTION_WIDTH * MAX_SCREEN_SCALE < screenwidth)
			scale = RESOLUTION_WIDTH * MAX_SCREEN_SCALE / screenwidth;
		if(RESOLUTION_HEIGHT * MAX_SCREEN_SCALE < screenheight)
			scale = std::min(scale, RESOLUTION_HEIGHT * MAX_SCREEN_SCALE / screenheight);
	}
    screenwidth = (int)(screenwidth * scale);
    screenheight = (int)(screenheight * scale);

    changeResolution(screenwidth, screenheight);
    
    nx = (int)(nx * scale);
    ny = (int)(ny * scale);
    int total = nx*ny;

    //The frame rate regulator
    Timer fps, timer;

    //Wait for user exit

    timer.start();

    Layout layout(nx, ny, screenwidth, screenheight);
    Logic logic(layout);
    logic.Start();
    while( quit == false ) {
        // Start the frame timer
        fps.start();

        // While there are events to handle
        SDL_Event event;
        while( SDL_PollEvent( &event ) ) {
            //Handle key presses
            if( (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) || (event.type == SDL_QUIT) ) {
                quit = true;
            } else {
                logic.HandleEvent(event);
            }
        }

        // Draw the simple pieces
        for(int i = 0; i < layout.num_x(); i++) {
            for(int j = 0; j < layout.num_y(); j++) {
                const bool in_dragg = logic.dragged().IsSelected(i, j);
                if(in_dragg) continue;

                Piece* p = logic.pieces().get_current(i,j);
                if(logic.selection().IsSelected(i, j))
                    glColor3f(0.75, 0.75, 1.0);
                else {
                    glColor3f(1.0, 1.0, 1.0);
                }
                p->Render();
            }
        }

        glColor3f(1.0, 1.0, 1.0);

        // Draw the pieces we are dragging
        for(auto it : logic.dragged()) {
            logic.pieces().get_current(it)->CustomRender(logic.cursor(), logic.dragged().top_left());
        }

        if(logic.selecting_origin()) {
            glEnable(GL_BLEND);
            glColor4d(1.0, 1.0, 1.0, 1.0);
            Vector2D scale = Vector2D(1.0 / layout.num_x(), 1.0 / layout.num_y());
            Vector2D start = logic.selecting_origin()->Scale(scale), end = logic.cursor().position.Scale(scale);
            glRectd(start.x, start.y, end.x, end.y);
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
    fprintf(out, "Puzzle \"%s\" - %d x %d: ", FileName, layout.num_x(), layout.num_y());
    if((logic.pieces().get_matches()) == total) {
		fprintf(out, "Success with %d moves in ", logic.moves());
		printTime(out, timer.get_ticks());
	} else {
		fprintf(out, "Quit after ");
		printTime(out, timer.get_ticks());
		fprintf(out, " with %d moves", logic.moves());
	}
    fprintf(out, ".\n");
    fclose(out);

    //Clean up
    clean_up();

    return 0;
}
