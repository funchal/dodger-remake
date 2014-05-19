#include "enemy.hpp"
#include "level.hpp"
#include "dodger.hpp"

Enemy::Enemy(int edge, int speed):
        edge(edge), speed(speed), frame(), firing(false),
        shot_line(), shot_col()
{
    switch (edge) {
    case Level::n:
        line = -1;
        col = 0;
        delta_line = 0;
        delta_col = 1;
        shot_delta_line = 1;
        shot_delta_col = 0;
        orientation = v;
        texture.loadFromFile("img/laserd.bmp");
        break;
    case Level::s:
        line = Level::num_lines;
        col = Level::num_cols - 1;
        delta_line = 0;
        delta_col = -1;
        shot_delta_line = -1;
        shot_delta_col = 0;
        orientation = v;
        texture.loadFromFile("img/laseru.bmp");
        break;
    case Level::w:
        line = Level::num_lines - 1;
        col = -1;
        delta_line = -1;
        delta_col = 0;
        shot_delta_line = 0;
        shot_delta_col = 1;
        orientation = h;
        texture.loadFromFile("img/laserl.bmp");
        break;
    case Level::e:
        line = 0;
        col = Level::num_cols;
        delta_line = 1;
        delta_col = 0;
        shot_delta_line = 0;
        shot_delta_col = -1;
        orientation = h;
        texture.loadFromFile("img/laserr.bmp");
        break;
    }

    if (orientation == v) {
        shot_textures[Level::blank].loadFromFile("img/shotbv.bmp");
        shot_textures[Level::grid].loadFromFile("img/shotgv.bmp");
        shot_textures[Level::wall].loadFromFile("img/shotwv.bmp");
        shot_textures[Level::apple].loadFromFile("img/shotav.bmp");
        shot_textures[Level::sberry].loadFromFile("img/shotsv.bmp");
        shot_textures[Level::cherry].loadFromFile("img/shotcv.bmp");
        shot_textures[Level::skull].loadFromFile("img/shotdv.bmp");
    } else {
        shot_textures[Level::blank].loadFromFile("img/shotbh.bmp");
        shot_textures[Level::grid].loadFromFile("img/shotgh.bmp");
        shot_textures[Level::wall].loadFromFile("img/shotwh.bmp");
        shot_textures[Level::apple].loadFromFile("img/shotah.bmp");
        shot_textures[Level::sberry].loadFromFile("img/shotsh.bmp");
        shot_textures[Level::cherry].loadFromFile("img/shotch.bmp");
        shot_textures[Level::skull].loadFromFile("img/shotdh.bmp");
    }
}

void Enemy::reset() {
    firing = false;

    switch (edge) {
    case Level::n:
        line = -1;
        col = 0;
        delta_col = 1;
        break;
    case Level::s:
        line = Level::num_lines;
        col = Level::num_cols - 1;
        delta_col = -1;
        break;
    case Level::w:
        line = Level::num_lines - 1;
        col = -1;
        delta_line = -1;
        break;
    case Level::e:
        line = 0;
        col = Level::num_cols;
        delta_line = 1;
        break;
    }
}

void Enemy::update(int player_line, int player_col)
{
    if (! firing) {
        if ((orientation == h && line == player_line)
                || (orientation == v && col == player_col)) {
            firing = true;
            shot_line = line;
            shot_col = col;
        }
    }
    if (firing) {
        shot_line += shot_delta_line;
        shot_col += shot_delta_col;
        if (shot_line < 0 || shot_line >= Level::num_lines
                || shot_col < 0 || shot_col >= Level::num_cols) {
            firing = false;
        }
    } else {
        if (frame == 0) {
            line += delta_line;
            col += delta_col;
            if ((orientation == h && (line <= 0 || line >= Level::num_lines - 1))
                    || (orientation == v && (col <= 0 || col >= Level::num_cols - 1))) {
                delta_line = -delta_line;
                delta_col = -delta_col;
            }
        }
        frame = (frame + 1) % speed;
    }
}

void Enemy::draw(sf::RenderWindow& window, int level_data[Level::num_lines][Level::num_cols])
{
    sf::Sprite sprite;
    set_position(sprite, line, col);
    sprite.setTexture(texture);
    window.draw(sprite);

    if (firing) {
        sf::Sprite shot_sprite;
        set_position(shot_sprite, shot_line, shot_col);
        shot_sprite.setTexture(shot_textures[level_data[shot_line][shot_col]]);
        window.draw(shot_sprite);
    }
}

bool Enemy::is_laser_here(int line, int col)
{
    return firing && shot_line == line && shot_col == col;
}
