#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <complex.h>
#include "include/raylib.h"
#define SCREEN_HOR 1200.0
#define SCREEN_VERT 900.0
#define MAX_SHAPES 18
#define MAX_BARS 100
#define MAX_HERZ 20000.0
#define ARRAY_LEN(xs)sizeof(xs)/sizeof(xs[0])
#define Float_Complex float complex
#define N (1<<13)
// #define N 256

float in1[N];
float in2[N];
float complex out[N];

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
	// float a = fabsf(crealf(z));
	// float b = fabsf(cimagf(z));
	// if(a<b) return b;
	// return a;

	return cabsf(z);
}

void callback(void *bufferData, unsigned int frames){
	Frame *fs = bufferData;

	for (size_t i=0;i<frames;i++){
		memmove(in1,in1+1,(N-1)*sizeof(in1[0]));
		in1[N-1] = fs[i].left;
	}
}

int main(){

	InitAudioDevice();

	SetTargetFPS(60);



	Music myMusic = LoadMusicStream("resources/ffvii_short.mp3");
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
	float increaseRate = 1;
	float decreaseRate = 0.006;

	Vector2 mousePosition = { 0.0f, 0.0f };

	AttachAudioStreamProcessor(myMusic.stream,callback);

	bool istab = true;

	while(!WindowShouldClose()){


		// Use tab to switch views
		if(IsKeyPressed(KEY_TAB)){
			if (istab) istab = false;
			else istab = true;
		}

		UpdateMusicStream(myMusic);

		// Use spacebar to pause music
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
		
		int w = GetScreenWidth();
		int h = GetScreenHeight();
		// int w = GetRenderWidth();
		// int h = GetRenderHeight();

		BeginDrawing();

		float max_h = 0;
		float cur_h = 0;

		float playback_status = GetMusicTimePlayed(myMusic)/ GetMusicTimeLength(myMusic);

		float max_line_length = SCREEN_VERT * playback_status;

		Vector2 max_endpos = (Vector2){SCREEN_HOR,SCREEN_VERT-(SCREEN_VERT*playback_status)};
		Vector2 min_endpos = (Vector2){SCREEN_HOR,SCREEN_VERT*playback_status};

		for (size_t i = 0; i < N; ++i){
			float t = (float)i/N;
			float hann = 0.5-0.5*cosf(2*PI*t);
			in2[i] = in1[i] * hann;
		}
		
		// Perform Fast Fourier Transform on input signal
		fft(in2,1,out,N);

		float max_amp = 0.0f;
		for (size_t i = 0; i < N; i++){
			float a = amp(out[i]);
			if (max_amp < a) max_amp = a;
		}

		float step = 1.06;
		float lowf = 1.0f;
		size_t m = 0;
		for (float f = lowf; (size_t) f < N/2; f *= step){
			m+=1;
		}

		float cell_width = 1;//(float)w/m;

		m=0;


		// View line drawing
		if (istab){
			ClearBackground((Color){ 240, 237, 228, 255 });

			// Bass: frequency < 500Hz
			// Mid: 500 - 2000Hz
			// Highs: 2000Hz+

			for (float f = lowf; (size_t) f < N/2; f = ceilf(f*step)) {
				float f1 = ceilf(f*step);
				float a = 0.0f;
				for (size_t q = (size_t) f; q < N/2 && q < (size_t) f1; ++q) { 
					float b = amp(out[q]);
					if(b>a)a=b;
				} 

				// t represents amplitude, 0 < t < 1
				float t = a/max_amp;


				// Calculate frequency: f = k/N*R (sampling rate)
				// EG 48.83Hz = 1/1024*50kHz
				float frequency = f/N*myMusic.stream.sampleRate;

				for (int i = 0; i < MAX_SHAPES; i++){
					DrawLineEx(lineArray[i].startPos,lineArray[i].endPos,lineArray[i].thick,lineArray[i].color);

					if (frequency < 500 && i%3 == 0 && lineArray[i].endPos.y > max_endpos.y && t == 1){
					lineArray[i].endPos.y -= increaseRate*fabs(t);
					}

					if (frequency > 500 && i%3 == 1 && frequency < 1500 && lineArray[i].endPos.y > max_endpos.y && t == 1){
					lineArray[i].endPos.y -= increaseRate*fabs(t);
					}

					if (frequency > 1500 && i%3 == 2 && lineArray[i].endPos.y > max_endpos.y && t > 0.5){
					lineArray[i].endPos.y -= increaseRate*fabs(t);
					}
				}
				m += 1;
			}
		}

		float xStep = pow(2.0f, (log(w/1) / log(2.0f)) / w);

		// Plot spectrum
		if (!istab){
			ClearBackground((Color){ 240, 237, 228, 255 });

			for (float f = lowf; (size_t) f < N/2; f = ceilf(f+xStep)) {

				float f1 = ceilf(f+xStep);
				float a = 0.0f;
				for (size_t q = (size_t) f; q < N/2 && q < (size_t) f1; ++q) { 
					float b = amp(out[q]);
					if(b>a)a=b;
				} 
				
				// t represents amplitude, 0 < t < 1
				float t = a/max_amp;

				if(t>0){

				float frequency = f/N*myMusic.stream.sampleRate;
				float x_pos = log10(frequency)/log10(MAX_HERZ);

				DrawRectangle(w*x_pos, h/2 - h/2*t+0.25*SCREEN_VERT, cell_width, h/2*t, GREEN);

				} 
				m += 1;
			}
		}
		EndDrawing();
	}

	UnloadMusicStream(myMusic);   // Unload music stream buffers from RAM

	UnloadTexture(overlay); 

    CloseAudioDevice();         // Close audio device (music streaming is automatically stopped)

	CloseWindow();

	return 0;
}