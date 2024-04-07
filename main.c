#include <stdio.h>
#include "include/raylib.h"
#define SCREEN_HOR 800.0
#define SCREEN_VERT 600.0
#define MAX_SHAPES 9

typedef struct {
    Vector2 position;
	Vector2 size;
    Color color;
} MyShape;

int main(){

    MyShape shapeArray[MAX_SHAPES] = { 0 };
	Color color[3] = {RED,BLUE,YELLOW};

    // Initialize Shapes
    for (int i = 0; i < MAX_SHAPES; i++){
        shapeArray[i].position = (Vector2){SCREEN_HOR/10*(i+1)+30,SCREEN_VERT};
		shapeArray[i].size = (Vector2){ -60.0f, -60.0f };
        shapeArray[i].color = color[i%3];
    }

	InitWindow(SCREEN_HOR,SCREEN_VERT,"basic window");

	//Parameters for modyfying shapes
	float increaseRate = 100;
	float decreaseRate = 25;

	Vector2 mousePosition = { 0.0f, 0.0f };

	while(!WindowShouldClose()){

		mousePosition = GetMousePosition();

		//Prints current position of mouse
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
			printf("current position: %f\t%f\t%f\n",mousePosition.x,mousePosition.y,GetTime());
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

		//Decrease size of shapes while inactive
    	for (int i = 0; i < MAX_SHAPES; i++){
			if (shapeArray[i].size.y<-60)shapeArray[i].size.y += GetFrameTime()*decreaseRate;
		}

		BeginDrawing();
			// ClearBackground(BEIGE);
			// ClearBackground(GetColor(16764108));
			ClearBackground((Color){ 240, 237, 228, 255 });

			//Color){ 200, 200, 200, 255 }

			for (int i = 0; i < MAX_SHAPES; i++){
				DrawRectangleV(shapeArray[i].position,shapeArray[i].size,shapeArray[i].color);
			}

		EndDrawing();
	}

	CloseWindow();

	return 0;
}
