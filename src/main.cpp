#include <iostream>
#include <memory>
#include <ostream>
#include <raylib.h>
#include <raymath.h>
#include <string.h>
#include <string>
#include <vector>

const char screenshot_path[] = "screenshots/take_1/";

#define RAYLIB_ENABLED 1
#define SCREEN_W 1920
#define SCREEN_H 1080
#define GRID_CELL_SIZE 4
#define ENABLE_SCREEN_CAPTURE 0
#define LD_RATIO 0.3
#define RANDOM_SEED 1

const int8_t sur_d[] = {0, -1, -1, 0, 1, 1, 1, 0, -1,  0, 0, 1, 1, 1, 0, -1, -1, -1};
const double weight_grid[] =  {0, 1, sqrt(2), 1, sqrt(2), 1, sqrt(2), 1, sqrt(2)};

Color COL_WHITE         = {255, 255, 255, 255};
Color COL_GRAY          = {100, 100, 100, 255};
Color COL_GREEN         = {15,  165, 10,  255};
Color COL_BRIGHT_GREEN  = {15,  255, 10,  255};
Color COL_DARK_GREEN    = {5,    85, 3,   255};
Color COL_RED           = {255, 100, 10,  255};
Color COL_BLUE          = {10,  100, 255, 255};
Color COL_MAGENTA       = {120, 10,  200, 255};
Color COL_BLACK         = {1,   1,   1,   255};

enum col_enum{
    _BLACK,
    _GREEN,
    _RED,
    _BLUE,
    _BGREEN,
    _DGREEN,
    _GRAY,
    _MAGENTA,
    _WHITE
};

Color col_array[] = {COL_BLACK, COL_GREEN, COL_RED, COL_BLUE, COL_BRIGHT_GREEN, COL_DARK_GREEN, COL_GRAY, COL_MAGENTA, COL_WHITE};

void xy_to_pos(int* pos, int x, int y, int columns){
    if (x < 0 || x >= columns || y < 0 || y >= columns){
        *pos = -2;
        return;
    }
    *pos = x + (y * columns);
    return;
}

void pos_to_xy(int* x, int* y, int pos, int columns){
    *x = pos % columns;
    *y = pos / columns;
    return;
}


bool get_random_cell_value(){
    int cutoff = (int) (65536.0 * LD_RATIO);
    int r = rand() % 65536;
    if (r > cutoff){
        return 0;
    }else{
        return 1;
    }
}


bool rule_0_1(bool vi){ return vi & 0; }
bool rule_2__(bool vi){ return vi & 1; }
bool rule_3__(bool vi){ return vi | 1; }
bool rule_4_p(bool vi){ return vi & 0; }

enum w_states{
    _WAIT,
    _RUN, 
    _STOP,
};

class World{
    private:
        int m_state = _WAIT;
        size_t m_screen_h;
        size_t m_screen_w;
        size_t m_field_h;
        size_t m_field_w;
        size_t m_cell_size;
        size_t m_columns;
        size_t m_remainder_w;
        size_t m_rows;
        size_t m_remainder_h;
        size_t m_grit_count;
        size_t m_live_count;
        double m_ld_ratio;
        bool* m_cell_value_a;
        bool* m_cell_value_b;
        bool m_cycle_turn;
        bool* m_cell_values[2];
        bool (*m_rule_array[9])(bool value);
        char* state_text[];

    public:
        World(){}
        World(size_t screen_h, size_t screen_w, size_t cell_size) :
            m_screen_h(screen_h), m_screen_w(screen_w), m_cell_size(cell_size)
        {
            if (RANDOM_SEED){
                srand(time(NULL));
            }
            
            m_field_h = m_screen_h - 40;
            m_field_w = m_screen_w;

            m_columns = m_field_w / m_cell_size;
            m_rows    = m_field_h / m_cell_size;
            m_grit_count = m_columns * m_rows;
            m_remainder_w = m_field_w - (m_cell_size * m_columns);
            m_remainder_h = m_field_h - (m_cell_size * m_rows);
            m_cell_value_a = new bool[m_grit_count];
            m_cell_value_b = new bool[m_grit_count];
            m_cell_values[0] = m_cell_value_a;
            m_cell_values[1] = m_cell_value_b;
            m_cycle_turn = 0;
            m_live_count = 0;
            m_ld_ratio = 0;
            setup_cells();
            m_rule_array[0] = &rule_0_1;
            m_rule_array[1] = &rule_0_1;
            m_rule_array[2] = &rule_2__;
            m_rule_array[3] = &rule_3__;
            m_rule_array[4] = &rule_4_p;
            m_rule_array[5] = &rule_4_p;
            m_rule_array[6] = &rule_4_p;
            m_rule_array[7] = &rule_4_p;
            m_rule_array[8] = &rule_4_p;

        }
        ~World(){ 
            delete[] m_cell_value_a;
            delete[] m_cell_value_b;
        }

        void set_state(int state){
            m_state = state;
        }

        void run(){
            m_state = _RUN;
        }

        void stop(){
            m_state = _STOP;
        }

        int get_state(){
            return m_state;
        }

        void setup_cells(){
            for (size_t c=0; c<m_grit_count; c++){
                bool c_value = get_random_cell_value();
                m_cell_values[m_cycle_turn][c] = c_value;
            }
        }

        int get_neighbour_value_count(size_t index){
            int sum = 0;
            int other_ix;
            int x, y;
            pos_to_xy(&x, &y, index, m_columns);
            for (int i=1; i<9; i++){
                int ox = x + sur_d[i];
                int oy = y + sur_d[i+9];
                if (ox<0){
                    ox = m_columns + ox;
                }
                if (oy<0){
                    oy = m_rows + oy;
                }
                ox = ox % m_columns;
                oy = oy % m_rows;
                    xy_to_pos(&other_ix, ox, oy, m_columns); // Other node's postion.
                    sum = sum + m_cell_values[m_cycle_turn][other_ix];
            }
            return sum;
        }

        void cycle(){
            m_live_count = 0;
            for (size_t c=0; c<m_grit_count; c++){
                int neighbour_count = get_neighbour_value_count(c);
                bool new_val = m_cell_values[m_cycle_turn][c];
                new_val = (*m_rule_array[neighbour_count])(new_val);
                m_live_count = m_live_count + new_val;
                m_cell_values[!m_cycle_turn][c] = new_val;
            }
            m_ld_ratio = (double) m_live_count / m_grit_count;
            m_cycle_turn = !m_cycle_turn;
        }

        size_t get_cell_count(){
            return m_grit_count;
        }

        int get_cell_index_from_pos(int x, int y){
            size_t xp = (x - (m_remainder_w / 2)) / m_cell_size;
            size_t yp = (y - (m_remainder_h / 2)) / m_cell_size;
            int pos;
            xy_to_pos(&pos, xp, yp, m_columns);
            return pos;
        }

        void draw_colour_cell(size_t x, size_t y, int colour){
            DrawRectangle(x+2, y+2, m_cell_size-2, m_cell_size-2, col_array[colour]);   
        }

        void print_cell_grit(){
            for (int c=0; c<m_grit_count; c++){
                if (c%m_columns==0){ printf("\n");}
                printf("%d ", m_cell_values[m_cycle_turn][c]);
            }
            printf("\n");
            printf("live count: %d, ratio: %.4f\n", m_live_count, m_ld_ratio);
        }

        void draw_text(const char* text){
            char text_buffer[255];
            sprintf(text_buffer, "live: %d \t ratio: %1.4f \t", m_live_count, m_ld_ratio);
            DrawText(text_buffer, 0, m_field_h + 2, 16, COL_WHITE);
            sprintf(text_buffer, "%s, press 'q' to quit", text);
            DrawText(text_buffer, m_field_w/2, m_field_h + 2, 16, COL_WHITE);
        }

        void draw_cells(){
            int xp, yp;
            for (size_t i=0; i<m_grit_count; i++){
                pos_to_xy(&xp, &yp, i, m_columns);
                size_t x = (xp * m_cell_size) + (m_remainder_w / 2);
                size_t y = (yp * m_cell_size) + (m_remainder_h / 2);
#if RAYLIB_ENABLED
                if (x < m_field_w - m_remainder_w && y < m_field_h - m_remainder_h){
#if GRID_CELL_SIZE > 20
                    DrawRectangleLines(x, y, m_cell_size, m_cell_size, COL_GRAY);
#endif
                    if (m_cell_values[m_cycle_turn][i]){
                        draw_colour_cell(x, y, _WHITE);
                    }
                }
#endif
            }
        }

        void draw(){
            switch (m_state){
                case _WAIT:
                    draw_text("Press 'r' to run.");
                    break;
                case _RUN:
                    cycle();
                    draw_cells();
                    draw_text("Running.");
                    break;
                case _STOP:
                    draw_cells();
                    draw_text("Stopped.");
                    break;
            }
        }
};


void capture_screen(){
    static size_t capture_count = 0;
    if (capture_count < 500){
        char file_name[255];
        sprintf(file_name, "%sscreen_%ld.png", screenshot_path, capture_count);
        TakeScreenshot(file_name);
        capture_count++;
    }
}
    
World world(SCREEN_H, SCREEN_W, GRID_CELL_SIZE);

int main(){

#if RAYLIB_ENABLED
    //InitWindow(GRID_CELL_SIZE*GRID_CELL_COUNT, GRID_CELL_SIZE*GRID_CELL_COUNT, "window");

    InitWindow(SCREEN_W, SCREEN_H, "Tiles");
    SetTargetFPS(60);
    ToggleBorderlessWindowed();
    while (WindowShouldClose() == false){
        BeginDrawing();
        if (IsMouseButtonPressed(0)){
        }
        if (IsMouseButtonPressed(1)){
        }

        if(IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_Q)){
            break;
        }
        if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_G)){
        }
        ClearBackground(COL_BLACK);

        if (IsKeyPressed(KEY_R)){
            world.run();
        }

        if (IsKeyPressed(KEY_S)){
            world.stop();
        }

        if (world.get_state() == _RUN){
        }
        world.draw();

        EndDrawing();
        if (ENABLE_SCREEN_CAPTURE){
            capture_screen();
        }
    }
    CloseWindow();
#else
    world.print_cell_grit();
    world.cycle();
    world.print_cell_grit();
    world.cycle();
    world.print_cell_grit();
    world.cycle();
    world.print_cell_grit();
    world.cycle();
    world.print_cell_grit();
    world.cycle();
    world.print_cell_grit();
    
    
#endif //RL ENABLED
    return 0;
}

