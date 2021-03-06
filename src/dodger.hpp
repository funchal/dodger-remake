#ifndef dodger_hpp
#define dodger_hpp

#include "level.hpp"
#include "enemy.hpp"
#include "score_panel.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>

class Dodger
{
public:
    void run();
    int score;
    int high_score;

private:
    void init();
    void new_life();
    void new_level();
    void new_game();
    void loop();
    void process_events();
    void update();
    void draw();
    void play_sound(const sf::SoundBuffer& sound);
    void load_screen(int data[Level::num_lines][Level::num_cols]);
    void welcome_screen();
    void play();
    void password_screen();
    void dead();
    void game_over_screen();
    void well_done_screen();
    bool hit_by_laser();
    void update_enemies();
    void update_dodger_position();
    void check_if_eating(int new_cell);


    sf::RenderWindow window;

    enum GameState {
        welcome, playing, password, dying, game_over, well_done
    } game_state;

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
    ScorePanel score_panel;
    std::vector<std::string> passwords;

    int player_line;
    int player_col;
    int delta_line;
    int delta_col;
    Direction player_direction;
    int player_anim;
    int life_count;
    unsigned level_number;
    bool next_screen;
    std::vector<Enemy> enemies;
    bool running;
    sf::RectangleShape pwd_rect;
    sf::Font font;
    sf::Text main_text;
    sf::Text password_text;
    bool skip_level;
};

void set_position(sf::Sprite& sprite, float line, float col);

void error(const char* format, ...);
void fatal(const char* format, ...);

#endif
