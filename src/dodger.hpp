#ifndef dodger_hpp
#define dodger_hpp

#include "level.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>

class Dodger
{
public:
    void run();
    int score;

private:
    void init();
    void new_life();
    void new_level();
    void new_game();
    void loop();
    void update();
    void draw();
    void play_sound(const sf::SoundBuffer& sound);

    sf::RenderWindow window;

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

    enum { death_anim_length = 6 };

    struct {
        sf::Texture cell[Level::num_cell_types - 1];
        sf::Texture player[num_directions][num_states];
        sf::Texture death[death_anim_length];
    } textures;

    struct {
        sf::SoundBuffer chomp;
        sf::SoundBuffer dead;
        sf::SoundBuffer gover;
        sf::SoundBuffer wdone;
    } sounds;

    sf::Sprite sprites[Level::num_lines][Level::num_cols];

    std::vector<Level> levels;
    Level level;

    int player_line;
    int player_col;
    int delta_line;
    int delta_col;
    Direction player_direction;
    int player_anim;
    int life_count;
    bool death;
    int death_anim;
    unsigned level_number;
};

void error(const char* format, ...);
void fatal(const char* format, ...);

#endif
