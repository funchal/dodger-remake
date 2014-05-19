#include "dodger.hpp"
#include "level.hpp"
#include "enemy.hpp"
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

    sf::Image icon;
    icon.loadFromFile("ico/dodger.bmp");

    sf::Vector2u icon_size = icon.getSize();
    window.setIcon(icon_size.x, icon_size.y, icon.getPixelsPtr());

    for (int line = 0; line != Level::num_lines; line++) {
        for (int col = 0; col != Level::num_cols; col++) {
            set_position(sprites[line][col], line, col);
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

void Dodger::new_life()
{
    death = false;
    player_direction = up;

    player_line = level.initial_player_line;
    player_col = level.initial_player_col;

    // no initial movement
    delta_line = 0;
    delta_col = 0;

    // one frame closed followed by two frames open and one frame closed
    player_anim = 1;

    std::vector<Enemy>::iterator enemy;
    for (enemy = enemies.begin(); enemy != enemies.end(); enemy++) {
        enemy->reset();
    }

}

void Dodger::new_level()
{
    level = levels[level_number - 1];

    for (int line = 0; line != Level::num_lines; line++) {
        for (int col = 0; col != Level::num_cols; col++) {
            int cell = level.data[line][col];
            if (cell != Level::player) {
                sprites[line][col].setTexture(textures.cell[cell]);
            } else {
                sprites[line][col].setTexture(textures.cell[Level::grid]);
            }
        }
    }

    enemies.clear();
    for (int i = 0; i < Level::num_enemies; i++) {
        if (level.enemies[i]) {
            enemies.push_back(Enemy(i, level.speed));
        }
    }

    new_life();
}

void Dodger::new_game()
{
    life_count = 3;
    level_number = 1;

    new_level();
}

void Dodger::run()
{
    init();

    new_game();

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
                    delta_line = -1;
                    delta_col = 0;
                    break;
                case sf::Keyboard::Down:
                    player_direction = down;
                    delta_line = 1;
                    delta_col = 0;
                    break;
                case sf::Keyboard::Left:
                    player_direction = left;
                    delta_line = 0;
                    delta_col = -1;
                    break;
                case sf::Keyboard::Right:
                    player_direction = right;
                    delta_line = 0;
                    delta_col = 1;
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
    if (level.food_count == 0) {
        std::cout << "level cleared" << std::endl;
        if (level_number == levels.size()) {
            // TODO: well done screen
            play_sound(sounds.wdone);
            std::cout << "well done" << std::endl;
            new_game();
        } else {
            // TODO: freeze image, pop-up with password to next level.
            level_number++;
            new_level();
        }
        return;
    }

    if (death) {
        death_anim++;
        if (death_anim == death_anim_length) {
            if (life_count > 0) {
                sprites[player_line][player_col].setTexture(
                        textures.cell[level.data[player_line][player_col]]);
                new_life();
            } else {
                // TODO: game over screen
                play_sound(sounds.gover);
                std::cout << "game over" << std::endl;
                new_game();
            }
            return;
        }
        sprites[player_line][player_col].setTexture(textures.death[death_anim]);
        return;
    }

    int new_player_line = player_line + delta_line;
    int new_player_col = player_col + delta_col;

    int new_cell = level.data[new_player_line][new_player_col];

    // don't move if blocked
    if (new_player_line < 0
            || new_player_line >= Level::num_lines
            || new_player_col < 0
            || new_player_col >= Level::num_cols
            || new_cell == Level::blank
            || new_cell == Level::wall) {
        new_cell = level.data[player_line][player_col];
    } else if (delta_line != 0 or delta_col != 0) {
        // draw a grid on the previous position and update the position
        sprites[player_line][player_col].setTexture(textures.cell[Level::grid]);
        player_line = new_player_line;
        player_col = new_player_col;
        player_anim = (player_anim + 1) % 4;
    }

    // check if dead and update laser shots
    // need to check if dodger is hit before AND after updating the laser shots
    // to prevent the dodger and the shot to pass each other with no hit.
    bool hit = false;
    std::vector<Enemy>::iterator enemy;
    for (enemy = enemies.begin(); enemy != enemies.end(); enemy++) {
        if (enemy->is_laser_here(player_line, player_col)) {
            hit = true;
        }
        enemy->update(player_line, player_col);
        if (enemy->is_laser_here(player_line, player_col)) {
            hit = true;
        }
    }
    if (hit || new_cell == Level::skull) {
        play_sound(sounds.dead);
        life_count--;
        death = true;
        death_anim = 0;
        sprites[player_line][player_col].setTexture(textures.death[death_anim]);
        return; // can't eat if dead
    }

    if (new_cell == Level::apple
            || new_cell == Level::sberry
            || new_cell == Level::cherry) {
        level.food_count--;
        level.data[new_player_line][new_player_col] = Level::grid;

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

    if (! death) {
        std::vector<Enemy>::iterator enemy;
        for (enemy = enemies.begin(); enemy != enemies.end(); enemy++) {
            enemy->draw(window, (level.data));
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

void set_position(sf::Sprite& sprite, int line, int col)
{
    sprite.setPosition(24 * (1.5 + col), 24 * (3 + line));
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
