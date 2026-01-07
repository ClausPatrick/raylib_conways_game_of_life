#include "world.h"

#define LD_RATIO 0.2

static char log_buffer[1024];

const int8_t sur_d[] = {0, -1, -1, 0, 1, 1, 1, 0, -1,  0, 0, 1, 1, 1, 0, -1, -1, -1};
const double weight_grid[] =  {0, 1, sqrt(2), 1, sqrt(2), 1, sqrt(2), 1, sqrt(2)};

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


uint8_t get_random_cell_value(double ld_ratio){
    int cutoff = (int) (65536.0 * ld_ratio);
    int r = rand() % 65536;
    if (r > cutoff){
        return 0;
    }else{
        return 1;
    }
}


uint8_t rule_perish(uint8_t vi){ return vi & 0; }
uint8_t rule_rest(uint8_t vi){ return vi & 1; }
uint8_t rule_wake(uint8_t vi){ return vi | 1; }
uint8_t rule_flip(uint8_t vi){ return vi ^ 1; } // “birth but not survive” behavior of B/S rules.

char game_mode_names_full[MODES][35] = {"B3_S23_CONWAY", "B1357_S1357_Replicator", "B2_S_Seeds", "B25_S4", "B3_S012345678_Life_without_death",  "B34_S34_34_Life", "B35678_S5678_Diamoeba", "B36_S125_2x2", "B36_S23_HighLife", "B3678_S34678_Day_Night", "B368_S245_Morley", "B4678_S35678_Anneal"};
const char game_mode_names[MODES][20] = {"CONWAY", "Replicator", "Seeds", "B25_S4", "Life_without_death", "34_Life", "Diamoeba", "2x2", "HighLife", "Day_Night", "Morley", "Anneal"};


World::World(size_t screen_h, size_t screen_w, size_t cell_size) :
        m_screen_h(screen_h), m_screen_w(screen_w), m_cell_size(cell_size)
    {
        
        m_field_h = m_screen_h - 40;
        m_field_w = m_screen_w;

        m_columns = m_field_w / m_cell_size;
        m_rows    = m_field_h / m_cell_size;
        m_grit_count = m_columns * m_rows;
        m_remainder_w = m_field_w - (m_cell_size * m_columns);
        m_remainder_h = m_field_h - (m_cell_size * m_rows);
        sprintf(log_buffer, "%s: height: %ld, width: %ld.", __func__, m_field_h, m_field_w);
        logger(log_buffer, 4);
        sprintf(log_buffer, "%s: cols: %ld, rows: %ld, cells: %ld.", __func__, m_columns, m_rows, m_grit_count);
        logger(log_buffer, 4);
        sprintf(log_buffer, "%s: rem_w: %ld, rem_h: %ld.", __func__, m_remainder_w, m_remainder_h);
        logger(log_buffer, 4);
        m_cell_value_a = new uint8_t[m_grit_count];
        m_cell_value_b = new uint8_t[m_grit_count];
        m_cell_ages = new size_t[m_grit_count];
        m_max_age = 0;
        m_cell_values[0] = m_cell_value_a;
        m_cell_values[1] = m_cell_value_b;
        m_cycle_turn = 0;
        m_live_count = 0;
        m_ld_ratio = 0;
        m_mode_ix = _CONWAY;
        m_mode_str = game_mode_names[m_mode_ix];
        m_new_mode_str = game_mode_names[m_mode_ix];
        select_game_mode(m_mode_ix);
        setup_cells(LD_RATIO);
}

World::~World(){ 
        delete[] m_cell_value_a;
        delete[] m_cell_value_b;
        delete[] m_cell_ages;
}

void World::select_game_mode(uint8_t mode){
    switch (mode){
        case _CONWAY:
            get_rules_conway();
            break;
        case _REPLICATOR:
            get_rules_replicator();
            break;

        case _SEEDS:
            get_rules_seeds();
            break;

        case _B25_S4:
            get_rules_b25_s4();
            break;

        case _LIFE_WITHOUT_DEATH:
            get_rules_life_without_death();
            break;

        case _34_LIFE:
            get_rules_34_life();
            break;

        case _DIAMOEBA:
            get_rules_diamoeba();
            break;

        case _2X2:
            get_rules_2x2();
            break;

        case _HIGHLIFE:
            get_rules_highlife();
            break;

        case _DAY_NIGHT:
            get_rules_day_night();
            break;

        case _MORLEY:
            get_rules_morley();
            break;

        case _ANNEAL:
            get_rules_anneal();
            break;
    }
}

void World::get_rules_conway(){
        m_rule_array[0] = &rule_perish;
        m_rule_array[1] = &rule_perish;
        m_rule_array[2] = &rule_rest;
        m_rule_array[3] = &rule_wake;
        m_rule_array[4] = &rule_perish;
        m_rule_array[5] = &rule_perish;
        m_rule_array[6] = &rule_perish;
        m_rule_array[7] = &rule_perish;
        m_rule_array[8] = &rule_perish;
}


void World::get_rules_replicator(){
        m_rule_array[0] = &rule_perish;
        m_rule_array[1] = &rule_wake;
        m_rule_array[2] = &rule_perish;
        m_rule_array[3] = &rule_wake;
        m_rule_array[4] = &rule_perish;
        m_rule_array[5] = &rule_wake;
        m_rule_array[6] = &rule_perish;
        m_rule_array[7] = &rule_wake;
        m_rule_array[8] = &rule_perish;
}
void World::get_rules_seeds(){
        m_rule_array[0] = &rule_perish;
        m_rule_array[1] = &rule_perish;
        m_rule_array[2] = &rule_flip;
        m_rule_array[3] = &rule_perish;
        m_rule_array[4] = &rule_perish;
        m_rule_array[5] = &rule_perish;
        m_rule_array[6] = &rule_perish;
        m_rule_array[7] = &rule_perish;
        m_rule_array[8] = &rule_perish;
}
void World::get_rules_b25_s4(){
        m_rule_array[0] = &rule_perish;
        m_rule_array[1] = &rule_perish;
        m_rule_array[2] = &rule_flip;
        m_rule_array[3] = &rule_perish;
        m_rule_array[4] = &rule_rest;
        m_rule_array[5] = &rule_flip;
        m_rule_array[6] = &rule_perish;
        m_rule_array[7] = &rule_perish;
        m_rule_array[8] = &rule_perish;
}
void World::get_rules_life_without_death(){
        m_rule_array[0] = &rule_rest;
        m_rule_array[1] = &rule_rest;
        m_rule_array[2] = &rule_rest;
        m_rule_array[3] = &rule_wake;
        m_rule_array[4] = &rule_rest;
        m_rule_array[5] = &rule_rest;
        m_rule_array[6] = &rule_rest;
        m_rule_array[7] = &rule_rest;
        m_rule_array[8] = &rule_rest;
}
void World::get_rules_34_life(){
        m_rule_array[0] = &rule_perish;
        m_rule_array[1] = &rule_perish;
        m_rule_array[2] = &rule_wake;
        m_rule_array[3] = &rule_wake;
        m_rule_array[4] = &rule_perish;
        m_rule_array[5] = &rule_perish;
        m_rule_array[6] = &rule_perish;
        m_rule_array[7] = &rule_perish;
        m_rule_array[8] = &rule_perish;
}
void World::get_rules_2x2(){
        m_rule_array[0] = &rule_perish;
        m_rule_array[1] = &rule_flip;
        m_rule_array[2] = &rule_flip;
        m_rule_array[3] = &rule_wake;
        m_rule_array[4] = &rule_perish;
        m_rule_array[5] = &rule_flip;
        m_rule_array[6] = &rule_wake;
        m_rule_array[7] = &rule_perish;
        m_rule_array[8] = &rule_perish;
}
void World::get_rules_highlife(){
        m_rule_array[0] = &rule_perish;
        m_rule_array[1] = &rule_perish;
        m_rule_array[2] = &rule_rest;
        m_rule_array[3] = &rule_wake;
        m_rule_array[4] = &rule_perish;
        m_rule_array[5] = &rule_perish;
        m_rule_array[6] = &rule_flip;
        m_rule_array[7] = &rule_perish;
        m_rule_array[8] = &rule_perish;
}
void World::get_rules_day_night(){
        m_rule_array[0] = &rule_perish;
        m_rule_array[1] = &rule_perish;
        m_rule_array[2] = &rule_perish;
        m_rule_array[3] = &rule_wake;
        m_rule_array[4] = &rule_rest;
        m_rule_array[5] = &rule_perish;
        m_rule_array[6] = &rule_wake;
        m_rule_array[7] = &rule_wake;
        m_rule_array[8] = &rule_wake;
}
void World::get_rules_morley(){
        m_rule_array[0] = &rule_perish;
        m_rule_array[1] = &rule_perish;
        m_rule_array[2] = &rule_rest;
        m_rule_array[3] = &rule_flip;
        m_rule_array[4] = &rule_rest;
        m_rule_array[5] = &rule_rest;
        m_rule_array[6] = &rule_flip;
        m_rule_array[7] = &rule_perish;
        m_rule_array[8] = &rule_flip;
}
void World::get_rules_anneal(){
        m_rule_array[0] = &rule_perish;
        m_rule_array[1] = &rule_perish;
        m_rule_array[2] = &rule_perish;
        m_rule_array[3] = &rule_rest;
        m_rule_array[4] = &rule_flip;
        m_rule_array[5] = &rule_rest;
        m_rule_array[6] = &rule_wake;
        m_rule_array[7] = &rule_wake;
        m_rule_array[8] = &rule_wake;
}

void World::get_rules_diamoeba(){
        m_rule_array[0] = &rule_perish;
        m_rule_array[1] = &rule_perish;
        m_rule_array[2] = &rule_perish;
        m_rule_array[3] = &rule_flip;
        m_rule_array[4] = &rule_perish;
        m_rule_array[5] = &rule_wake;
        m_rule_array[6] = &rule_wake;
        m_rule_array[7] = &rule_wake;
        m_rule_array[8] = &rule_wake;
}

void World::prompt_mode(){
        m_state_sub = _MODE;
}
void World::prompt_digit(){ 
        m_state_sub = _RANDOM; 
}

void World::set_mode(int value){
        if (value == 0){
            m_mode_str = m_new_mode_str;
            select_game_mode(m_mode_ix);
            m_state_sub = _NONE;
        }else{
            if (m_mode_ix == 0 && value == -1){
                m_mode_ix = MODES-1;
            }else{
                m_mode_ix = (m_mode_ix + value) % MODES;
            }
            m_new_mode_str = game_mode_names[m_mode_ix];
        }
        sprintf(log_buffer, "%s: cmix: %d, cms: %s.", __func__, m_mode_ix, m_new_mode_str);
        logger(log_buffer, 4);
}

void World::set_state(int state){ m_state = state; }
void World::run(){ m_state = _RUN; }
void World::stop(){ m_state = _STOP; }

int World::get_state(){ return m_state; }


void World::toggle_rs(){
        if (m_state == _RUN){
            m_state = _STOP;
            return;
        }
        if (m_state == _STOP){
            m_state = _RUN;
            return;
        }
}


void World::clear(){
        for (size_t c=0; c<m_grit_count; c++){
            m_cell_values[m_cycle_turn][c] = 0;
            m_cell_ages[c] = 0;
            m_max_age = 0;
        }
}

void World::gradient(){
        for (size_t c=0; c<m_grit_count; c++){
            double ratio = (double) (c % m_columns) / (double)  (m_columns + 1);
            uint8_t c_value = get_random_cell_value(ratio);
            m_cell_values[m_cycle_turn][c] = c_value;
            m_cell_ages[c] = c_value;
            m_max_age = 1;
        }
}

void World::randomize_cells(uint8_t ld_value){
        double ld_ratio = (double) ld_value / 11;
        sprintf(log_buffer, "%s: ld_value: %d, ld_ratio: %f.", __func__, ld_value, ld_ratio);
        logger(log_buffer, 4);
        setup_cells(ld_ratio);
}

void World::setup_cells(double ld_ratio){
        for (size_t c=0; c<m_grit_count; c++){
            uint8_t c_value = get_random_cell_value(ld_ratio);
            m_cell_values[m_cycle_turn][c] = c_value;
            m_cell_ages[c] = c_value;
            m_max_age = 1;
        }
}

void World::set_cell(size_t index, int value){
        m_cell_values[m_cycle_turn][index] = value;
        m_cell_ages[index] = value;
        m_max_age = 1;
}

int World::get_neighbour_value_count(size_t index){
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
            //if (ox>=0 && oy>=0 && ox<(int)m_columns && oy<(int)m_rows ){
                xy_to_pos(&other_ix, ox, oy, m_columns); // Other node's postion.
                sum = sum + m_cell_values[m_cycle_turn][other_ix];
            //}
        }
        return sum;
}

void World::cycle(){
        for (size_t c=0; c<m_grit_count; c++){
            int neighbour_count = get_neighbour_value_count(c);
            uint8_t new_val = m_cell_values[m_cycle_turn][c];
            new_val = (*m_rule_array[neighbour_count])(new_val);
            m_cell_ages[c] = (m_cell_ages[c] + new_val) * new_val; // Increment if 1, set to 0 if 0;
            m_cell_values[!m_cycle_turn][c] = new_val;
        }
        m_cycle_turn = !m_cycle_turn;
}

size_t World::get_cell_count(){
        return m_grit_count;
}

int World::get_cell_index_from_pos(int x, int y){
        size_t xp = (x - (m_remainder_w / 2)) / m_cell_size;
        size_t yp = (y - (m_remainder_h / 2)) / m_cell_size;
        int pos;
        xy_to_pos(&pos, xp, yp, m_columns);
        return pos;
}

void World::draw_colour_rgb(size_t x, size_t y, int colour){
        int colour_ix = m_cell_ages[colour];
        if (colour_ix >= 361){
            colour_ix = 361;
        }
        Color col = {
            rgb_values[colour_ix][0], 
            rgb_values[colour_ix][1],
            rgb_values[colour_ix][2], 
            255};

        DrawRectangle(x+2, y+2, m_cell_size-2, m_cell_size-2, col);   
}

void World::draw_colour(size_t x, size_t y, int colour){
        DrawRectangle(x+2, y+2, m_cell_size-2, m_cell_size-2, col_array[colour]);   
}

void World::print_age_grit(){
        for (size_t c=0; c<m_grit_count; c++){
            if (c%m_columns==0){ printf("\n");}
            m_max_age = std::max(m_max_age, m_cell_ages[c]);
            printf("%ld ", m_cell_ages[c]);
        }
        printf("\nMax age: %ld.\n", m_max_age);
}


void World::print_cell_grit(){
        for (size_t c=0; c<m_grit_count; c++){
            if (c%m_columns==0){ printf("\n");}
            printf("%d ", m_cell_values[m_cycle_turn][c]);
        }
        printf("\n");
        printf("live count: %ld, ratio: %.4f\n", m_live_count, m_ld_ratio);
}

void World::draw_text(const char* text, Color colour){
        char text_buffer[255];
        //size_t y_pos[] = {0, m_field_w/
        size_t lv = m_live_count;
        double rt = m_ld_ratio;
        sprintf(text_buffer, "Mode: %s, live: %ld \t ratio: %1.4f \t", m_mode_str, lv, rt);
        DrawText(text_buffer, 0, m_field_h + 2, 16, COL_WHITE);

        sprintf(text_buffer, "Press 'q' to quit");
        DrawText(text_buffer, m_field_w/5, m_field_h + 2, 16, COL_WHITE);

        sprintf(text_buffer, "%s", text);
        DrawText(text_buffer, (m_field_w - m_field_w/10), m_field_h + 2, 18, colour);

        if (m_state_sub == _RANDOM){
            sprintf(text_buffer, "%s", "RANDOM: Enter digit (0~9)");
            DrawText(text_buffer, (m_field_w/2), m_field_h + 2, 16, COL_WHITE);
            m_state_sub = _NONE;
        }
        if (m_state_sub == _MODE){
            sprintf(text_buffer, "%s '%s'", "MODE: Press up/down and enter", m_new_mode_str);
            DrawText(text_buffer, (m_field_w/2), m_field_h + 2, 16, COL_WHITE);
            //m_state_sub = _NONE;
        }

}


void World::act_on_mouse(int mouse_button){
    int mouse_x = GetMouseX();
    int mouse_y = GetMouseY();
    int cell_index = get_cell_index_from_pos(mouse_x, mouse_y);
    set_cell(cell_index, 1 - mouse_button);
}


void World::draw_cells(){
        int xp, yp;
        m_live_count = 0;
        for (size_t i=0; i<m_grit_count; i++){
            pos_to_xy(&xp, &yp, i, m_columns);
            size_t x = (xp * m_cell_size) + (m_remainder_w / 2);
            size_t y = (yp * m_cell_size) + (m_remainder_h / 2);
            if (x < m_field_w - m_remainder_w && y < m_field_h - m_remainder_h){
#if GRID_CELL_SIZE > 20
                DrawRectangleLines(x, y, m_cell_size, m_cell_size, COL_GRAY);
#endif
                if (m_cell_values[m_cycle_turn][i]){
                    m_live_count++;
                    draw_colour_rgb(x, y, i);
                }
            }
        }
        if (m_live_count == 0 || m_live_count == m_grit_count){
            stop();
        }
        m_ld_ratio = (double) m_live_count / m_grit_count;
}

void World::draw(){
        switch (m_state){
            case _WAIT:
                draw_cells();
                draw_text("Press 'r' to run.", COL_YELLOW);
                break;
            case _RUN:
                cycle();
                draw_cells();
                draw_text("Running.", COL_GREEN);
                break;
            case _STOP:
                draw_cells();
                draw_text("Stopped.", COL_RED);
                break;
        }
}



