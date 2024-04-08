#include <stdio.h>
#include "include/raylib.h"
#define SCREEN_HOR 800.0
#define SCREEN_VERT 600.0
#define MAX_SHAPES 9
#define SHAPE_SIZE 60.0

typedef struct {
    Vector2 position;
	Vector2 size;
    Color color;
} MyShape;

typedef struct {
    Vector2 startPos;
	Vector2 endPos;
	float thick;
    Color color;
} MyLine;

int main(){

	InitAudioDevice();

	Music myMusic = LoadMusicStream("resources/ffvii.mp3");

	PlayMusicStream(myMusic);

    MyShape shapeArray[MAX_SHAPES] = { 0 };
	MyLine lineArray[MAX_SHAPES] = { 0 };
	Color color[3] = {RED,BLUE,YELLOW};

    // Initialize Shapes
    for (int i = 0; i < MAX_SHAPES; i++){
        shapeArray[i].position = (Vector2){SCREEN_HOR/(MAX_SHAPES+1)*(i+1)+30,SCREEN_VERT};
		shapeArray[i].size = (Vector2){ -60.0f, -60.0f };
        shapeArray[i].color = color[i%3];
    }

    for (int i = 0; i < MAX_SHAPES; i++){
        lineArray[i].startPos = (Vector2){SCREEN_HOR/(MAX_SHAPES+1)*(i+1),SCREEN_VERT};
		lineArray[i].endPos = (Vector2){SCREEN_HOR/(MAX_SHAPES+1)*(i+1),SCREEN_VERT};
		lineArray[i].thick = 15.0;
        lineArray[i].color = color[i%3];
    }

	InitWindow(SCREEN_HOR,SCREEN_VERT,"musicvis");

	Texture2D overlay = LoadTexture("resources/scarab.png");  

	//Parameters for modyfying shapes
	float increaseRate = 100;
	float decreaseRate = 25;

	Vector2 mousePosition = { 0.0f, 0.0f };

	while(!WindowShouldClose()){
		mousePosition = GetMousePosition();

		UpdateMusicStream(myMusic);

		//Prints current position of mouse
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
			printf("current position: %f\t%f\t%f\n",mousePosition.x,mousePosition.y,GetMusicTimePlayed(myMusic));
		}
		
		//Modify shapes with arrow keys
		if (IsKeyDown(KEY_LEFT)){
			for (int i = 0; i < MAX_SHAPES; i+=3){
				shapeArray[i].size.y -= GetFrameTime()*increaseRate;
			}
		}
		if (IsKeyDown(KEY_UP)){
			for (int i = 1; i < MAX_SHAPES; i+=3){
				shapeArray[i].size.y -= GetFrameTime()*increaseRate;
			}
		}
		if (IsKeyDown(KEY_RIGHT)){
			for (int i = 2; i < MAX_SHAPES; i+=3){
				shapeArray[i].size.y -= GetFrameTime()*increaseRate;
			}
		}

		//Modify lines with arrow keys
		if (IsKeyDown(KEY_LEFT)){
			for (int i = 0; i < MAX_SHAPES; i+=3){
				lineArray[i].endPos.y -= GetFrameTime()*increaseRate;
			}
		}
		if (IsKeyDown(KEY_UP)){
			for (int i = 1; i < MAX_SHAPES; i+=3){
				lineArray[i].endPos.y -= GetFrameTime()*increaseRate;
			}
		}
		if (IsKeyDown(KEY_RIGHT)){
			for (int i = 2; i < MAX_SHAPES; i+=3){
				lineArray[i].endPos.y -= GetFrameTime()*increaseRate;
			}
		}

		if (IsKeyDown(KEY_DOWN)){
			for (int i = 0; i < MAX_SHAPES; i+=1){
				lineArray[i].thick += 0.01;
				printf("Current THICC: %f\n",lineArray[i].thick);
			}
		}

		//Decrease size of shapes while inactive
    	for (int i = 0; i < MAX_SHAPES; i++){
			if (shapeArray[i].size.y<-60)shapeArray[i].size.y += GetFrameTime()*decreaseRate;
		}

		//Decrease size of lines while inactive
    	for (int i = 0; i < MAX_SHAPES; i++){
			if (lineArray[i].endPos.y<580)lineArray[i].endPos.y += GetFrameTime()*decreaseRate;
		}

		BeginDrawing();
			ClearBackground((Color){ 240, 237, 228, 255 });

			// DrawTexture(overlay, 0, 0, WHITE);

			// for (int i = 0; i < MAX_SHAPES; i++){
			// 	DrawRectangleV(shapeArray[i].position,shapeArray[i].size,shapeArray[i].color);
			// }

			for (int i = 0; i < MAX_SHAPES; i++){
				DrawLineEx(lineArray[i].startPos,lineArray[i].endPos,lineArray[i].thick,lineArray[i].color);
				
			}

			DrawTexture(overlay, 0, 0, WHITE);


		EndDrawing();
	}

	UnloadMusicStream(myMusic);   // Unload music stream buffers from RAM

	UnloadTexture(overlay); 

    CloseAudioDevice();         // Close audio device (music streaming is automatically stopped)

	CloseWindow();

	return 0;
}