#include "dodger.hpp"
#include "level.hpp"
#include <cstdlib>
#include <fstream>
#include <cstdarg>
#include <exception>

#include <iostream>

void Dodger::init()
{
    int width = (Level::num_cols + 3) * 24;
    int height = (Level::num_lines + 4.5) * 24;
    window.create(sf::VideoMode(width, height), "dodger-remake");
    window.setFramerateLimit(7);
    window.setVerticalSyncEnabled(true);

    for (int line = 0; line != Level::num_lines; line++) {
        for (int col = 0; col != Level::num_cols; col++) {
            sprites[line][col].setPosition(24 * (1.5 + col), 24 * (3 + line));
        }
    }

    textures.cell[Level::blank].loadFromFile("img/blank.bmp");
    textures.cell[Level::grid].loadFromFile("img/grid.bmp");
    textures.cell[Level::wall].loadFromFile("img/wall.bmp");
    textures.cell[Level::apple].loadFromFile("img/apple.bmp");
    textures.cell[Level::sberry].loadFromFile("img/sberry.bmp");
    textures.cell[Level::cherry].loadFromFile("img/cherry.bmp");
    textures.cell[Level::skull].loadFromFile("img/skull.bmp");

    textures.player[up][open].loadFromFile("img/gmanuo.bmp");
    textures.player[left][open].loadFromFile("img/gmanlo.bmp");
    textures.player[down][open].loadFromFile("img/gmando.bmp");
    textures.player[right][open].loadFromFile("img/gmanro.bmp");
    textures.player[up][closed].loadFromFile("img/gmanuc.bmp");
    textures.player[left][closed].loadFromFile("img/gmanlc.bmp");
    textures.player[down][closed].loadFromFile("img/gmandc.bmp");
    textures.player[right][closed].loadFromFile("img/gmanrc.bmp");

    textures.death[0] = textures.player[up][closed];
    textures.death[1] = textures.player[up][open];
    textures.death[2].loadFromFile("img/dead3.bmp");
    textures.death[3].loadFromFile("img/dead4.bmp");
    textures.death[4].loadFromFile("img/dead5.bmp");
    textures.death[5].loadFromFile("img/dead6.bmp");

    sounds.chomp.loadFromFile("snd/chomp.wav");
    sounds.dead.loadFromFile("snd/dead.wav");
    sounds.gover.loadFromFile("snd/gover.wav");
    sounds.wdone.loadFromFile("snd/wdone.wav");

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
}

void Dodger::load_level()
{
    Level level = levels[level_number - 1];
    for (int line = 0; line != Level::num_lines; line++) {
        for (int col = 0; col != Level::num_cols; col++) {
            int cell = level.data[line][col];
            if (cell != Level::player) {
                level_data[line][col] = cell;
                sprites[line][col].setTexture(textures.cell[cell]);
            } else {
                level_data[line][col] = Level::grid;
                sprites[line][col].setTexture(textures.cell[Level::grid]);
                initial_player_line = line;
                initial_player_col = col;
                player_line = line;
                player_col = col;
            }
        }
    }

    // one frame closed followed by two frames open and one frame closed
    player_direction = up;
    player_anim = 1;
    death = false;
    food_count = level.food_count;
}

void Dodger::run()
{
    init();

    life_count = 3;
    level_number = 1;

    load_level();

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
    if (food_count == 0) {
        if (level_number == levels.size()) {
            // TODO: well done screen
        } else {
            // TODO: freeze image, pop-up with password to next level.
            level_number++;
            load_level();
        }
        return;
    }

    if (death) {
        death_anim++;
        if (death_anim == death_anim_length) {
            if (life_count > 0) {
                sprites[player_line][player_col].setTexture(
                        textures.cell[level_data[player_line][player_col]]);
                player_line = initial_player_line;
                player_col = initial_player_col;
                death = false;
            } else {
                // TODO: game over screen
                std::cout << "game over" << std::endl;
            }
            return;
        }
        sprites[player_line][player_col].setTexture(textures.death[death_anim]);
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
        sprites[player_line][player_col].setTexture(textures.cell[Level::grid]);
        player_line = new_player_line;
        player_col = new_player_col;
        player_anim = (player_anim + 1) % 4;
    }

    // check if dead
    // TODO: lasers can kill too
    if (new_cell == Level::skull) {
        life_count--;
        death = true;
        death_anim = 0;
        sprites[player_line][player_col].setTexture(textures.death[death_anim]);
        return; // can't eat if dead
    }

    if (new_cell == Level::apple
            || new_cell == Level::sberry
            || new_cell == Level::cherry) {
        food_count--;
        level_data[new_player_line][new_player_col] = Level::grid;

        play_sound(sounds.chomp);

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

    AnimState anim_state = (AnimState) (player_anim / 2);
    sprites[player_line][player_col].setTexture(
            textures.player[player_direction][anim_state]);
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

static float random_float(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

void Dodger::play_sound(const sf::SoundBuffer& sound)
{
    static sf::Sound s[50];
    static unsigned next = 0;
    next = (next + 1) % 50;

    s[next].setPitch(random_float(0.95f, 1.1f));
    s[next].setBuffer(sound);
    s[next].play();
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
