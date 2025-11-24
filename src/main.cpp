#include <deque>
#include <iostream>
#include <memory>
#include <ostream>
#include <raylib.h>
#include <raymath.h>
#include <string.h>
#include <string>
#include <vector>


const char screenshot_path[] = "screenshots/take_2/";


#define RAYLIB_ENABLED 1
#define SCREEN_W 1920
#define SCREEN_H 1040
//#define SCREEN_H 10
//#define SCREEN_W 10
//#define GRID_CELL_SIZE 40
#define GRID_CELL_SIZE 6 
#define ENABLE_SCREEN_CAPTURE 0 
#define LD_RATIO 0.04


//#define SUR_D 20
//const int8_t sur_d[SUR_D*2] = {0, -1, -1, 0, 1, 1, 1, 0, -1,  0, 0, 1, 1, 1, 0, -1, -1, -1};
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
    int cutoff = (int) (1000.0 * LD_RATIO);
    int r = rand() % 1000;
    if (r > cutoff){
        return 0;
    }else{
        return 1;
    }
}

class Cell{
    private:
        size_t m_pos;
        size_t m_columns;
        int m_x;
        int m_y;
        bool m_value;
        Color m_colour;
    public:
        Cell(size_t pos, bool value, size_t column_count) :
            m_pos(pos), m_value(value), m_columns(column_count)
    {
        pos_to_xy(&m_x, &m_y, m_pos, m_columns);
    }
        ~Cell(){}

        int get_x(){ return m_x; }
        int get_y(){ return m_y; }
        size_t get_pos(){ return m_pos; }
        size_t get_value(){ return m_value; }
        Color get_colour(){ return m_colour; }

        void set_x(size_t x){ m_x = x; }
        void set_y(size_t y){ m_y = y; }
        void set_pos(size_t pos){ m_pos = pos; }
        void set_value(size_t value){ m_value = value; }
        void set_colour(Color colour){ m_colour = colour; }
};


bool rule_0_1(bool vi){ return vi & 0; }
bool rule_2__(bool vi){ return vi & 1; }
bool rule_3__(bool vi){ return vi | 1; }
bool rule_4_p(bool vi){ return vi & 0; }


class World{
    private:
        size_t m_screen_h;
        size_t m_screen_w;
        size_t m_cell_size;
        size_t m_columns;
        size_t m_remainder_w;
        size_t m_rows;
        size_t m_remainder_h;
        size_t m_grit_count;
        size_t m_live_count;
        bool* m_cell_value_a;
        bool* m_cell_value_b;
        bool m_cycle_turn;
        bool* m_cell_values[2];
        std::vector<std::unique_ptr<Cell>> m_cells;
        bool (*m_rule_array[9])(bool value);

    public:
        World(){}
        World(size_t screen_h, size_t screen_w, size_t cell_size) :
            m_screen_h(screen_h), m_screen_w(screen_w), m_cell_size(cell_size)
        {
            m_columns = screen_w / m_cell_size;
            m_rows    = screen_h / m_cell_size;
            m_grit_count = m_columns * m_rows;
            m_remainder_w = screen_w - (m_cell_size * m_columns);
            m_remainder_h = screen_h - (m_cell_size * m_rows);
            m_cell_value_a = new bool[m_grit_count];
            m_cell_value_b = new bool[m_grit_count];
            m_cell_values[0] = m_cell_value_a;
            m_cell_values[1] = m_cell_value_b;
            m_cycle_turn = 0;
            m_live_count = 0;
            create_cells();
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

        void create_cells(){
            for (size_t c=0; c<m_grit_count; c++){
                bool c_value = get_random_cell_value();
                size_t index = m_cells.size();
                m_cell_values[m_cycle_turn][index] = c_value;
                m_cells.push_back(std::make_unique<Cell>(index, c_value, m_columns));
                if (c_value){
                    m_live_count++;
                    m_cells.at(index)->set_colour(COL_BLACK);
                }else{
                    m_cells.at(index)->set_colour(COL_WHITE);
                }
            }
        }
        int get_neighbour_value_count(size_t index){
            int sum = 0;
            int other_ix;
            int x = m_cells.at(index)->get_x();
            int y = m_cells.at(index)->get_y();
            for (int i=1; i<9; i++){
                int ox = x + sur_d[i];
                int oy = y + sur_d[i+9];
                if (ox>=0 && oy>=0 && ox<(int)m_columns && oy<(int)m_rows ){
                    xy_to_pos(&other_ix, ox, oy, m_columns); // Other node's postion.
                    sum = sum + m_cells.at(other_ix)->get_value();
                }
            }
            return sum;
        }

        void cycle(){
            m_live_count = 0;
            for (size_t c=0; c<m_grit_count; c++){
                int neighbour_count = get_neighbour_value_count(c);
                //printf("%d,", neighbour_count); 
                bool new_val = m_cell_values[m_cycle_turn][c];
                new_val = (*m_rule_array[neighbour_count])(new_val);
                m_live_count = m_live_count + new_val;
                m_cell_values[!m_cycle_turn][c] = new_val;
                m_cells.at(c)->set_value(new_val);
            }
            m_cycle_turn = !m_cycle_turn;
        }

        size_t get_cell_count(){
            return m_cells.size();
        }

        int get_cell_index_from_pos(int x, int y){
            size_t xp = (x - (m_remainder_w / 2)) / m_cell_size;
            size_t yp = (y - (m_remainder_h / 2)) / m_cell_size;
            int pos;
            xy_to_pos(&pos, xp, yp, m_columns);
            return pos;
        }

        void set_cell_colour(size_t pos, Color colour){
            m_cells.at(pos)->set_colour(colour);
        }

        Color get_cell_colour(size_t pos){
            return m_cells.at(pos)->get_colour();
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
            double ratio = (double) m_live_count / m_grit_count;
            printf("live count: %d, ratio: %.4f\n", m_live_count, ratio);
        }

        void draw_cells(){
            int xp, yp;
            for (size_t i=0; i<m_grit_count; i++){
            //for (size_t i=0; i<4; i++){
                pos_to_xy(&xp, &yp, i, m_columns);
                size_t x = (xp * m_cell_size) + (m_remainder_w / 2);
                size_t y = (yp * m_cell_size) + (m_remainder_h / 2);
#if RAYLIB_ENABLED
                if (x < m_screen_w - m_remainder_w && y < m_screen_h - m_remainder_h){
                    DrawRectangleLines(x, y, m_cell_size, m_cell_size, COL_GRAY);
                    if (m_cells.at(i)->get_value()){
                        draw_colour_cell(x, y, _WHITE);
                    }
                }
#endif
            }
        }
};


void capture_screen(){
    static size_t capture_count = 0;
    char file_name[255];
    sprintf(file_name, "%sscreen_%ld.png", screenshot_path, capture_count);
    TakeScreenshot(file_name);
    capture_count++;
}
    
World world(SCREEN_H, SCREEN_W, GRID_CELL_SIZE);

int main(){
#if RAYLIB_ENABLED
    //InitWindow(GRID_CELL_SIZE*GRID_CELL_COUNT, GRID_CELL_SIZE*GRID_CELL_COUNT, "window");
    InitWindow(SCREEN_W, SCREEN_H, "Tiles");
    //SetTargetFPS(60);
    ToggleBorderlessWindowed();
    while (WindowShouldClose() == false){
        BeginDrawing();

        if(IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_Q)){
            break;
        }

        ClearBackground(COL_BLACK);
        world.cycle();
        world.draw_cells();
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

