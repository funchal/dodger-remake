#ifndef Dodger_hpp
#define Dodger_hpp

#include <SFML/Graphics.hpp>
#include <vector>

struct Level
{
    enum { 
        n,
        e,
        s,
        w,
    };

    enum {
        num_lines = 20,
        num_cols = 25,
    };

    enum {
        blank,
        grid,
        wall,
        apple,
        sberry,
        cherry,
        skull,
        player,
        num_cell_types,
    };

    bool enemies[4];
    int data[num_lines][num_cols];
    int food_count;
};

class Dodger
{
public:
    void run();
    int score;

private:
    void loop();
    void update();
    void draw();

    void load_levels(const char* filename);

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

    sf::Texture player_textures[num_directions][num_states];

    int level_data[Level::num_lines][Level::num_cols];
    sf::Sprite sprites[Level::num_lines][Level::num_cols];

    std::vector<Level> levels;

    int player_line;
    int player_col;
    Direction player_direction;
    int player_anim;
    int food_count;
};

void error(const char* format, ...);
void fatal(const char* format, ...);

#endif
