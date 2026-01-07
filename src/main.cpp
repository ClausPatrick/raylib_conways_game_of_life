#include <iostream>
#include <memory>
#include <ostream>
#include <raylib.h>
#include <raymath.h>
#include <string.h>
#include <string>
#include <vector>
#include "c_logger.h"
#include "rgb_table.h"
#include "world.h"

static char log_buffer[1024];

const char screenshot_path[] = "screenshots/take_6/";

#define RAYLIB_ENABLED 1
#define SCREEN_W 1920
#define SCREEN_H 1080
#define GRID_CELL_SIZE 12
#define ENABLE_SCREEN_CAPTURE 0
#define RANDOM_SEED 1

void capture_screen(){
    static size_t capture_count = 0;
    if (capture_count < 1000){
        char file_name[255];
        sprintf(file_name, "%sscreen_%ld.png", screenshot_path, capture_count);
        TakeScreenshot(file_name);
        sprintf(log_buffer, "%s: '%s'.", __func__, file_name);
        capture_count++;
        logger(log_buffer, 4);
    }
}

    
World world(SCREEN_H, SCREEN_W, GRID_CELL_SIZE);

int main(){

    if (RANDOM_SEED){
        srand(time(NULL));
    }
    InitWindow(SCREEN_W, SCREEN_H, "Tiles");
    SetTargetFPS(60);
    ToggleBorderlessWindowed();
    bool wait_for_r_digit = 0;
    bool wait_for_mode = 0;
    while (WindowShouldClose() == false){
        BeginDrawing();
        if (IsMouseButtonPressed(0)){
            world.act_on_mouse(0);
        }
        if (IsMouseButtonPressed(1)){
            world.act_on_mouse(1);
        }

        if(IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_Q)){
            break;
        }
        
        if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_R)){
            wait_for_r_digit = 1;
        }
        if (wait_for_r_digit == 1){
            int c;
            world.prompt_digit();
            while ((c = GetCharPressed()) != 0){
                if (c>='0' && c <= '9'){
                    world.randomize_cells(c-'0');
                    wait_for_r_digit = 0;
                    break;
                }
            }
        }

        if (wait_for_mode == 1){
            //int c;
            world.prompt_mode();
            //while ((c = GetCharPressed()) != 0){
                if (IsKeyPressed(KEY_UP)){
                    sprintf(log_buffer, "%s: key up.", __func__ );
                    logger(log_buffer, 4);
                    world.set_mode(1);
                }
                if (IsKeyPressed(KEY_DOWN)){
                    sprintf(log_buffer, "%s: key down.", __func__ );
                    logger(log_buffer, 4);
                    world.set_mode(-1);
                }
                if (IsKeyPressed(KEY_ENTER)){
                    sprintf(log_buffer, "%s: enter.", __func__ );
                    logger(log_buffer, 4);
                    world.set_mode(0);
                    wait_for_mode = 0;
                }
            //}
        }

        ClearBackground(COL_BLACK);

        if (!IsKeyDown(KEY_LEFT_SHIFT) &&  IsKeyPressed(KEY_R)){
            world.run();
        }
        if (IsKeyPressed(KEY_C)){
            world.clear();
        }

        if (IsKeyPressed(KEY_G)){
            world.gradient();
        }

        if (IsKeyPressed(KEY_M)){
            wait_for_mode = 1;
        }

        if (IsKeyPressed(KEY_S)){
            world.stop();
        }
        
        if (IsKeyPressed(KEY_SPACE)){
            world.toggle_rs();
        }

        world.draw();

        EndDrawing();
        if (ENABLE_SCREEN_CAPTURE){
            if (world.get_state() == _RUN){
                capture_screen();
            }
        }
    }
    CloseWindow();
    sprintf(log_buffer, "Main is done.");
    logger(log_buffer, 4);
    return 0;
}
