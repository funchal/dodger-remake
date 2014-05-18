#ifndef dodger_hpp
#define dodger_hpp

#include "level.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

class Dodger
{
public:
    void run();
    int score;

private:
    void loop();
    void update();
    void draw();

    sf::RenderWindow window;
    sf::Texture cell_textures[Level::num_cell_types - 1];

    enum Direction {
        up,
        left,
        down,
        right,
        num_directions,
    };

    enum AnimState {
        closed,
        open,
        num_states,
    };

    enum { dying_anim_length = 6 };
    sf::Texture player_textures[num_directions][num_states];
    sf::Texture dying_textures[dying_anim_length];

    int level_data[Level::num_lines][Level::num_cols];
    sf::Sprite sprites[Level::num_lines][Level::num_cols];

    std::vector<Level> levels;

    int player_line;
    int player_col;
    Direction player_direction;
    int player_anim;
    int food_count;
    int life_count;
    bool dying;
    int dying_anim;
    int level_number;
};

void error(const char* format, ...);
void fatal(const char* format, ...);

#endif
