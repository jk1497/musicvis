#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <complex.h>
#include "include/raylib.h"
#define SCREEN_HOR 1200.0
#define SCREEN_VERT 900.0
#define MAX_SHAPES 18
#define ARRAY_LEN(xs)sizeof(xs)/sizeof(xs[0])
#define Float_Complex float complex
#define N (1<<14)
// #define N 256

float in[N];
float complex out[N];
float max_amp;

typedef struct {
    Vector2 startPos;
	Vector2 endPos;
	float thick;
    Color color;
} MyLine;

typedef struct{
    float left;
    float right;
} Frame;

void fft(float in[], size_t stride, Float_Complex out[], size_t n){
    assert(n > 0);

    if (n == 1) {
        out[0] = in[0];
        return;
    }

    fft(in, stride*2, out, n/2);
    fft(in + stride, stride*2,  out + n/2, n/2);

    for (size_t k = 0; k < n/2; ++k) {
        float t = (float)k/n;
        Float_Complex v = cexpf(-2*I*PI*t)*out[k + n/2];
        Float_Complex e = out[k];
        out[k]       = e+v;
        out[k + n/2] = e-v;
    }
}

float amp(float complex z){
	float a = fabsf(crealf(z));
	float b = fabsf(cimagf(z));
	if(a<b) return b;
	return a;
}

void callback(void *bufferData, unsigned int frames){

    // if(frames > N) frames = N;

	Frame *fs = bufferData;

	for (size_t i=0;i<frames;i++){
		memmove(in,in+1,(N-1)*sizeof(in[0]));
		in[N-1] = fs[i].left;
	}

	// for (size_t i = 0; i < frames; i++){
	// 	in[i] = fs[i].left;
	// }

	// fft(in, 1, out, N);

	// max_amp = 0.0f;
	// for (size_t i = 0; i < frames; i++){
	// 	float a = amp(out[i]);
	// 	if (a > max_amp) max_amp = a;
	// }
}

int main(){

	InitAudioDevice();

	SetTargetFPS(60);

	Music myMusic = LoadMusicStream("resources/ffvii_sinscale.mp3");
	assert(myMusic.stream.sampleSize == 32);

	PlayMusicStream(myMusic);

	MyLine lineArray[MAX_SHAPES] = { 0 };
	Color color[3] = {RED,BLUE,YELLOW};

	// Initialize line array
    for (int i = 0; i < MAX_SHAPES; i++){
        lineArray[i].startPos = (Vector2){SCREEN_HOR/(MAX_SHAPES+1)*(i+1),SCREEN_VERT};
		lineArray[i].endPos = (Vector2){SCREEN_HOR/(MAX_SHAPES+1)*(i+1),SCREEN_VERT};
		lineArray[i].thick = 30.0;
        lineArray[i].color = color[i%3];
    }

	InitWindow(SCREEN_HOR,SCREEN_VERT,"musicvis");

	Texture2D overlay = LoadTexture("resources/test.png");  

	//Parameters for modyfying shapes
	float increaseRate = 0.01;
	float decreaseRate = 0.006;

	Vector2 mousePosition = { 0.0f, 0.0f };

	AttachAudioStreamProcessor(myMusic.stream,callback);

	while(!WindowShouldClose()){

		UpdateMusicStream(myMusic);

		if (IsKeyPressed(KEY_SPACE)){
			if(IsMusicStreamPlaying(myMusic)){
				PauseMusicStream(myMusic);
			}
			else{
				ResumeMusicStream(myMusic); 
			}
		}

		if(!IsMusicStreamPlaying(myMusic)){
			DrawText("PAUSED...", 5, 5, 18, DARKGREEN); 
		}
		

		int w = GetRenderWidth();
		int h = GetRenderHeight();

		BeginDrawing();
		ClearBackground((Color){ 240, 237, 228, 255 });

		for (int i = 0; i < MAX_SHAPES; i++){
			DrawLineEx(lineArray[i].startPos,lineArray[i].endPos,lineArray[i].thick,lineArray[i].color);
		}

		float max_h = 0;
		float cur_h = 0;

		// ClearBackground(CLITERAL(Color) {0x18, 0x18, 0x18, 0xFF});
		// float cell_width = (float)w/global_frames_count;

		float playback_status = GetMusicTimePlayed(myMusic)/ GetMusicTimeLength(myMusic);

		float max_line_length = SCREEN_VERT * playback_status;

		Vector2 max_endpos = (Vector2){SCREEN_HOR,SCREEN_VERT-(SCREEN_VERT*playback_status)};
		Vector2 min_endpos = (Vector2){SCREEN_HOR,SCREEN_VERT*playback_status};

		int true_count = 0;
		int false_count = 0;


		float cell_width = 2;
		// for (size_t i = 0; i < N; ++i) {
		// 	float t = amp(out[i])/max_amp;
		// 	printf("\n%f",t);
		// 	DrawRectangle(i*cell_width, SCREEN_VERT/2 - SCREEN_VERT/2*t, cell_width, SCREEN_VERT/2*t, RED);
		// }

		for (size_t i = 0; i < SCREEN_HOR; ++i) {
			float t = amp(out[i])/max_amp;
			// printf("\n%f",t);
			DrawRectangle(i*cell_width, SCREEN_VERT/2 - SCREEN_VERT/2*t, cell_width, SCREEN_VERT/2*t, RED);
		}
		
		//rendering lines
		// for (size_t i = 0; i < global_frames_count; ++i) {

		// 	float t = global_frames[i].left;

		// 	if (h/2*t > max_h){
		// 		max_h = h/2*t;
		// 	}
		
		// 	if (t > 0) {
		// 		// DrawRectangle(i*cell_width, h/2 - h/2*t, 1, h/2*t, RED);

		// 		for (int i = 0; i < MAX_SHAPES; i++){
					
		// 			if (lineArray[i].endPos.y > max_endpos.y)
		// 			{
		// 				lineArray[i].endPos.y -= increaseRate*fabs(t);
		// 			}
					
		// 		}

		// 	true_count++;
		// 	} else {
		// 		// DrawRectangle(i*cell_width, h/2, 1, h/2*t, RED);

		// 		for (int i = 0; i < MAX_SHAPES; i++){

		// 			lineArray[i].endPos.y += decreaseRate*fabs(t);
					
		// 		}
		// 	false_count++;
		// 	}


		// }

		// float ylen = (lineArray[0].endPos.x - lineArray[0].startPos.x)*(lineArray[0].endPos.x - lineArray[0].startPos.x) + (lineArray[0].endPos.y - lineArray[0].startPos.y)*(lineArray[0].endPos.y - lineArray[0].startPos.y);
		// printf("\n%.2f\t%d\t%d\t",playback_status,true_count,false_count);

		EndDrawing();
	}

	UnloadMusicStream(myMusic);   // Unload music stream buffers from RAM

	UnloadTexture(overlay); 

    CloseAudioDevice();         // Close audio device (music streaming is automatically stopped)

	CloseWindow();

	return 0;
}