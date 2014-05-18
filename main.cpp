#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <cstring>
#include <cassert>
#include <iostream>
#include <cstdarg>
#include <exception>

enum { n = 0, e = 1, s = 2, w = 3 };

void error(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

void fatal(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    abort();
}

class Level
{
public:
    Level();

//private:

    enum {
        num_lines = 20,
        num_cols = 25,
    };

    enum {
        blank = 0,
        grid,
        wall,
        apple,
        sberry,
        cherry,
        skull,
        player,
        num_cell_types
    };

    bool enemies[4];
    int data[num_lines][num_cols];
    int food_count;
};

Level::Level()
: enemies(), data(), food_count()
{}

class LevelPack
{
public:
    void load(const char* filename);

//private:
    std::vector<Level> levels;
};

void LevelPack::load(const char* filename)
{
    try {
        std::ifstream stream;
        stream.exceptions(std::ios_base::badbit |
                          std::ios_base::failbit |
                          std::ios_base::eofbit);
        stream.open(filename, std::ios_base::binary);

        std::string str;
        while (true) {
            // new Level
            Level level;
            bool found_player = false;

            // parse positions of enemies
            std::getline(stream, str);
            std::string::iterator pos;
            static const std::string enemies_format("nesw");
            assert(str.length() >= 1);
            for (pos = str.begin(); pos != str.end(); pos++) {
                size_t enemy = enemies_format.find(*pos);
                assert(enemy != std::string::npos);
                level.enemies[enemy] = true;
            }

            // parse grid
            for (int line = 0; line != Level::num_lines; line++) {
                std::getline(stream, str);
                assert(str.length() == Level::num_cols);
                for (int col = 0; col != Level::num_cols; col++) {
                    int cell = str[col] - '0';
                    assert(0 <= cell && cell < Level::num_cell_types);
                    switch (cell) {
                        case Level::player:
                            assert(!found_player);
                            found_player = true;
                            break;
                        case Level::apple:
                        case Level::cherry:
                        case Level::sberry:
                            level.food_count++;
                            break;
                        default:
                            break;
                    }
                    level.data[line][col] = cell;
                }
            }
            assert(found_player);
            assert(level.food_count > 0);

            levels.push_back(level);

            stream.exceptions(std::ios_base::badbit |
                              std::ios_base::failbit);

            // check for end of file
            if (stream.peek() == EOF) {
                break;
            } else {
                stream.exceptions(std::ios_base::badbit |
                                  std::ios_base::failbit |
                                  std::ios_base::eofbit);
                // check for empty line between levels
                std::getline(stream, str);
                assert(str.length() == 0);
            }
        }
    } catch(...) {
        error("Failed to load file '%s'\n", filename);
    }
}

class Dodger
{
public:
    void run();
    int score;

//private:
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
        closed = 0,
        open,
        num_states,
    };

    sf::Texture player_textures[num_directions][num_states];

    int level_data[Level::num_lines][Level::num_cols];
    sf::Sprite sprites[Level::num_lines][Level::num_cols];

    LevelPack level_pack;

    int player_line;
    int player_col;
    Direction player_direction;
    int player_anim;
    int food_count;
};

void Dodger::run()
{
    int width = (Level::num_cols + 3) * 24;
    int height = (Level::num_lines + 4.5) * 24;
    window.create(sf::VideoMode(width, height), "Dodger");
    window.setFramerateLimit(4);
    window.setVerticalSyncEnabled(true);

    for (int line = 0; line != Level::num_lines; line++) {
        for (int col = 0; col != Level::num_cols; col++) {
            sprites[line][col].setPosition(24 * (1.5 + col), 24 * (3 + line));
        }
    }

    cell_textures[Level::blank].loadFromFile("blank.bmp");
    cell_textures[Level::grid].loadFromFile("grid.bmp");
    cell_textures[Level::wall].loadFromFile("wall.bmp");
    cell_textures[Level::apple].loadFromFile("apple.bmp");
    cell_textures[Level::sberry].loadFromFile("sberry.bmp");
    cell_textures[Level::cherry].loadFromFile("cherry.bmp");
    cell_textures[Level::skull].loadFromFile("skull.bmp");

    player_textures[up][open].loadFromFile("gmanuo.bmp");
    player_textures[left][open].loadFromFile("gmanlo.bmp");
    player_textures[down][open].loadFromFile("gmando.bmp");
    player_textures[right][open].loadFromFile("gmanro.bmp");
    player_textures[up][closed].loadFromFile("gmanuc.bmp");
    player_textures[left][closed].loadFromFile("gmanlc.bmp");
    player_textures[down][closed].loadFromFile("gmandc.bmp");
    player_textures[right][closed].loadFromFile("gmanrc.bmp");

    level_pack.load("originallevels.txt");

    for (int line = 0; line != Level::num_lines; line++) {
        for (int col = 0; col != Level::num_cols; col++) {
            int cell = level_pack.levels[0].data[line][col];
            if (cell != Level::player) {
                level_data[line][col] = cell;
                sprites[line][col].setTexture(cell_textures[cell]);
            } else {
                level_data[line][col] = Level::grid;
                sprites[line][col].setTexture(cell_textures[Level::grid]);
                player_line = line;
                player_col = col;
            }
        }
    }

    // one frame closed followed by two frames open and one frame closed
    player_direction = up;
    player_anim = 1;

    loop();

    window.close();
}

void Dodger::loop()
{
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                return;
            }
            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                case sf::Keyboard::Up:
                    player_direction = up;
                    break;
                case sf::Keyboard::Down:
                    player_direction = down;
                    break;
                case sf::Keyboard::Left:
                    player_direction = left;
                    break;
                case sf::Keyboard::Right:
                    player_direction = right;
                    break;
                default:
                    break;
                }
            }
        }

        update();

        draw();
    }
}

void Dodger::update()
{
    int new_player_line = player_line;
    int new_player_col = player_col;
    switch (player_direction) {
        case up:
            new_player_line--;
            break;
        case left:
            new_player_col--;
            break;
        case down:
            new_player_line++;
            break;
        case right:
            new_player_col++;
            break;
        default:
            break;
    }

    int new_cell = level_data[new_player_line][new_player_col];

    // don't move if blocked
    if (new_player_line < 0
            || new_player_line >= Level::num_lines
            || new_player_col < 0
            || new_player_col >= Level::num_cols
            || new_cell == Level::blank
            || new_cell == Level::wall) {
        new_player_line = player_line;
        new_player_col = player_col;
        new_cell = level_data[new_player_line][new_player_col];
    } else {
        player_anim = (player_anim + 1) % 4;
    }

    // check if dead
    // TODO: lasers can kill too
    if (new_cell == Level::skull) {
        // TODO: death animation and lose one life.
        return; // can't eat if dead
    }

    if (new_cell == Level::apple
            || new_cell == Level::sberry
            || new_cell == Level::cherry) {
        food_count--;
        level_data[new_player_line][new_player_col] = Level::grid;

        switch (new_cell) {
        case Level::apple:
            score += 20;
            break;
        case Level::sberry:
            score += 30;
            break;
        case Level::cherry:
            score += 50;
            break;
        }
    }

    if (food_count == 0) {
        // TODO: freeze image, pop-up with password to next level.
    }

    sprites[player_line][player_col].setTexture(cell_textures[Level::grid]);
    player_line = new_player_line;
    player_col = new_player_col;
    AnimState anim_state = (AnimState) (player_anim / 2);
    sprites[player_line][player_col].setTexture(
            player_textures[player_direction][anim_state]);
}

void Dodger::draw()
{
    window.clear();
    for (int line = 0; line != Level::num_lines; line++) {
        for (int col = 0; col != Level::num_cols; col++) {
            window.draw(sprites[line][col]);
        }
    }
    window.display();
}

int main(int, char*[])
{
    Dodger dodger;
    dodger.run();
    return EXIT_SUCCESS;
}
