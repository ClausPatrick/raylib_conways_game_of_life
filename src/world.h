#pragma once

#include <iostream>
#include <memory>
#include <ostream>
#include <raylib.h>
#include <raymath.h>
#include <string.h>
#include <string>
#include <vector>
#include "c_logger.h"
#include "colours.h"
#include "rgb_table.h"


void xy_to_pos(int* pos, int x, int y, int columns);
void pos_to_xy(int* x, int* y, int pos, int columns);
uint8_t get_random_cell_value(double ld_ratio);
uint8_t rule_perish(uint8_t vi);
uint8_t rule_rest(uint8_t vi);
uint8_t rule_wake(uint8_t vi);
uint8_t rule_flip(uint8_t vi); // “birth but not survive” behavior of B/S rules.


enum w_states{
    _WAIT,
    _RUN, 
    _STOP,
};

enum w_states_sub{
    _NONE,
    _RANDOM,
    _MODE
};


#define MODES 12
enum game_modes{
    _CONWAY,
    _REPLICATOR,
    _SEEDS,
    _B25_S4,
    _LIFE_WITHOUT_DEATH,
    _34_LIFE,
    _DIAMOEBA,
    _2X2,
    _HIGHLIFE,
    _DAY_NIGHT,
    _MORLEY,
    _ANNEAL
};
enum game_modes_full{
    _B3_S23_CONWAY,
    _B1357_S1357_REPLICATOR,
    _B2_S_SEEDS,
    _B25_S4_,
    _B3_S012345678_LIFE_WITHOUT_DEATH,
    _B34_S34_34_LIFE,
    _B35678_S5678_DIAMOEBA,
    _B36_S125_2X2,
    _B36_S23_HIGHLIFE,
    _B3678_S34678_DAY_NIGHT,
    _B368_S245_MORLEY,
    _B4678_S35678_ANNEAL
};




class World{
    private:
        int m_state = _WAIT;
        int m_state_sub = _NONE;
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
        uint8_t* m_cell_value_a;
        uint8_t* m_cell_value_b;
        uint8_t m_cycle_turn;
        uint8_t* m_cell_values[2];
        size_t* m_cell_ages;
        size_t m_max_age;
        uint8_t (*m_rule_array[9])(uint8_t value);
        //char* state_text[];
        uint8_t m_mode_ix;
        const char* m_mode_str;
        const char* m_new_mode_str;

    public:
        World();
        World(size_t screen_h, size_t screen_w, size_t cell_size);
        ~World();
        void select_game_mode(uint8_t mode);
        void get_rules_conway();
        void get_rules_replicator();
        void get_rules_seeds();
        void get_rules_b25_s4();
        void get_rules_life_without_death();
        void get_rules_34_life();
        void get_rules_2x2();
        void get_rules_highlife();
        void get_rules_day_night();
        void get_rules_morley();
        void get_rules_anneal();
        void get_rules_diamoeba();
        void prompt_mode();
        void prompt_digit();
        void set_mode(int value);
        void set_state(int state);
        void run();
        void stop();
        int get_state();
        void toggle_rs();
        void clear();
        void gradient();
        void randomize_cells(uint8_t ld_value);
        void setup_cells(double ld_ratio);
        void set_cell(size_t index, int value);
        int get_neighbour_value_count(size_t index);
        void cycle();
        size_t get_cell_count();
        int get_cell_index_from_pos(int x, int y);
        void act_on_mouse(int mouse_button);
        void draw_colour_rgb(size_t x, size_t y, int colour);
        void draw_colour(size_t x, size_t y, int colour);
        void print_age_grit();
        void print_cell_grit();
        void draw();
        void draw_cells();
        void draw_text(const char* text, Color colour);
};



