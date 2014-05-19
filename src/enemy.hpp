#ifndef enemy_hpp
#define enemy_hpp

#include "level.hpp"
#include <SFML/Graphics.hpp>

class Enemy
{
public:
    Enemy(int edge, int speed);
    void update(int player_liine, int player_col);
    void draw(sf::RenderWindow& window, int level_data[Level::num_lines][Level::num_cols]);
    bool is_laser_here(int line, int col);
    void reset();

private:
    enum Orientation { h, v };

    int edge; // one of n e s w

    int line;
    int col;
    int delta_line;
    int delta_col;
    int speed;
    int frame;
    Orientation orientation;
    sf::Texture texture;

    bool firing;
    int shot_line;
    int shot_col;
    int shot_delta_line;
    int shot_delta_col;
    sf::Texture shot_textures[Level::num_cell_types];
};

#endif
