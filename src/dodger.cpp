#include "dodger.hpp"
#include "level.hpp"
#include <cstdlib>
#include <fstream>
#include <cstdarg>
#include <exception>

void Dodger::run()
{
    int width = (Level::num_cols + 3) * 24;
    int height = (Level::num_lines + 4.5) * 24;
    window.create(sf::VideoMode(width, height), "Dodger");
    window.setFramerateLimit(7);
    window.setVerticalSyncEnabled(true);

    for (int line = 0; line != Level::num_lines; line++) {
        for (int col = 0; col != Level::num_cols; col++) {
            sprites[line][col].setPosition(24 * (1.5 + col), 24 * (3 + line));
        }
    }

    cell_textures[Level::blank].loadFromFile("img/blank.bmp");
    cell_textures[Level::grid].loadFromFile("img/grid.bmp");
    cell_textures[Level::wall].loadFromFile("img/wall.bmp");
    cell_textures[Level::apple].loadFromFile("img/apple.bmp");
    cell_textures[Level::sberry].loadFromFile("img/sberry.bmp");
    cell_textures[Level::cherry].loadFromFile("img/cherry.bmp");
    cell_textures[Level::skull].loadFromFile("img/skull.bmp");

    player_textures[up][open].loadFromFile("img/gmanuo.bmp");
    player_textures[left][open].loadFromFile("img/gmanlo.bmp");
    player_textures[down][open].loadFromFile("img/gmando.bmp");
    player_textures[right][open].loadFromFile("img/gmanro.bmp");
    player_textures[up][closed].loadFromFile("img/gmanuc.bmp");
    player_textures[left][closed].loadFromFile("img/gmanlc.bmp");
    player_textures[down][closed].loadFromFile("img/gmandc.bmp");
    player_textures[right][closed].loadFromFile("img/gmanrc.bmp");

    dying_textures[0] = player_textures[up][closed];
    dying_textures[1] = player_textures[up][open];
    dying_textures[2].loadFromFile("img/dead3.bmp");
    dying_textures[3].loadFromFile("img/dead4.bmp");
    dying_textures[4].loadFromFile("img/dead5.bmp");
    dying_textures[5].loadFromFile("img/dead6.bmp");

    std::ifstream stream("dodger-1.0/dodger.dat");

    Level level;
    //try {
        stream.ignore(1);
        while (level.import(stream)) {
            levels.push_back(level);
        }
    //} catch(...) {
    //}

    stream.close();

    level_number = 1;
    for (int line = 0; line != Level::num_lines; line++) {
        for (int col = 0; col != Level::num_cols; col++) {
            int cell = levels[level_number - 1].data[line][col];
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
    life_count = 3;
    dying = false;

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
    if (dying) {
        dying_anim++;
        if (dying_anim == dying_anim_length) {
            if (life_count > 0) {
                // TODO: reset level
            } else {
                // TODO: game over screen
            }
            return;
        }
        sprites[player_line][player_col].setTexture(dying_textures[dying_anim]);
        return;
    }

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
        new_cell = level_data[new_player_line][new_player_col];
    } else {
        // draw a grid on the previous position and update the position
        sprites[player_line][player_col].setTexture(cell_textures[Level::grid]);
        player_line = new_player_line;
        player_col = new_player_col;
        player_anim = (player_anim + 1) % 4;
    }

    // check if dead
    // TODO: lasers can kill too
    if (new_cell == Level::skull) {
        life_count--;
        dying = true;
        dying_anim = 0;
        sprites[player_line][player_col].setTexture(dying_textures[dying_anim]);
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
        if (level_number == levels.size()) {
            // TODO: well done screen
        } else {
            // TODO: freeze image, pop-up with password to next level.
        }
    }

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

void error(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    throw std::exception();
}

void fatal(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    abort();
}

int main(int, char*[])
{
    Dodger dodger;
    dodger.run();
    return EXIT_SUCCESS;
}
