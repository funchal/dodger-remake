#include "dodger.hpp"
#include "level.hpp"
#include "enemy.hpp"
#include "score_panel.hpp"
#include "screens.hpp"
#include <cstdlib>
#include <fstream>
#include <cstdarg>
#include <exception>
#include <sstream>
#include <string>
#include <iostream>

void Dodger::init()
{
    int width = (Level::num_cols + 4) * 24;
    int height = (Level::num_lines + 5) * 24;
    window.create(sf::VideoMode(width, height), "dodger-remake");
    window.setFramerateLimit(9);
    window.setVerticalSyncEnabled(true);

    sf::Image icon;
    icon.loadFromFile("ico/dodger.bmp");

    // http://www.1001fonts.com/tempofont-font.html
    if (!font.loadFromFile("font/TempoFont.ttf"))
    {
        fatal("Loading font failed");
    }
    main_text.setFont(font);
    main_text.setCharacterSize(18); // in pixels, not points!
    main_text.setColor(sf::Color::Black);
    password_text.setFont(font);
    password_text.setCharacterSize(18);
    password_text.setColor(sf::Color::Black);

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

    pwd_rect = sf::RectangleShape(sf::Vector2f(200, 60));
    pwd_rect.setPosition(248, 270);
    pwd_rect.setFillColor(sf::Color(196,195,208));
    pwd_rect.setOutlineThickness(5);
    pwd_rect.setOutlineColor(sf::Color(168,166,186));

    // password to level 2 and following
    passwords.push_back("LEVEL");
    passwords.push_back("THREE");
    passwords.push_back("APPLE");
    passwords.push_back("FAILS");
    passwords.push_back("ERROR");
    passwords.push_back("SEVEN");
    passwords.push_back("QUEUE");
    passwords.push_back("TIMER");
    passwords.push_back("STORE");
    passwords.push_back("UTILS");

    std::ifstream stream("dodger-1.1/dodger.dat");

    Level level;
    //try {
        stream.ignore(1);
        while (level.import(stream)) {
            levels.push_back(level);
        }
    //} catch(...) {
    //}

    stream.close();

    running = true;
    high_score = 500; // TODO;
    score_panel.set_high_score(high_score);
    next_screen = false;
}

void Dodger::new_life()
{
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

void Dodger::load_screen(int data[Level::num_lines][Level::num_cols])
{
    for (int line = 0; line != Level::num_lines; line++) {
        for (int col = 0; col != Level::num_cols; col++) {
            int cell = data[line][col];
            sprites[line][col].setTexture(textures.cell[cell]);
        }
    }
}

void Dodger::new_level()
{
    level = levels[level_number - 1];

    score_panel.set_level(level_number);

    load_screen(level.data);

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
    score = 0;

    score_panel.set_num_lives(life_count);
    score_panel.set_level(level_number);
    score_panel.set_score(score);

    new_level();
}

void Dodger::run()
{
    init();

    game_state = welcome;

    loop();

    // TODO: save high_score
    window.close();
}

void Dodger::loop()
{
    while (window.isOpen() && running) {
        process_events();
        update();
        draw();
    }
}

void Dodger::process_events()
{
    next_screen = false;
    skip_level = false;
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            running = false;
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
            case sf::Keyboard::Return:
                next_screen = true;
                break;
            case sf::Keyboard::N:
                skip_level = true; // to make testing easier
                break;
            default:
                break;
            }
        }
    }
}

bool Dodger::hit_by_laser()
{
    std::vector<Enemy>::iterator enemy;
    for (enemy = enemies.begin(); enemy != enemies.end(); enemy++) {
        if (enemy->is_laser_here(player_line, player_col)) {
            return true;
        }
    }
    return false;
}

void Dodger::update_enemies()
{
    std::vector<Enemy>::iterator enemy;
    for (enemy = enemies.begin(); enemy != enemies.end(); enemy++) {
        enemy->update(player_line, player_col);
    }
}

void Dodger::update_dodger_position()
{
    int new_player_line = player_line + delta_line;
    int new_player_col = player_col + delta_col;
    int new_cell = level.data[new_player_line][new_player_col];

    bool moving = (delta_line != 0 || delta_col != 0);

    bool inside_level_grid = (new_player_line >= 0
            && new_player_line < Level::num_lines
            && new_player_col >= 0
            && new_player_col < Level::num_cols);

    bool blocked = (new_cell == Level::blank || new_cell == Level::wall);

    // only update the position if the user requested a move and the new cell is valid
    if (moving && inside_level_grid && ! blocked) {
        // draw a grid on the previous position and update the position
        sprites[player_line][player_col].setTexture(textures.cell[Level::grid]);
        player_line = new_player_line;
        player_col = new_player_col;
        player_anim = (player_anim + 1) % 4;
    }

    AnimState anim_state = (AnimState) (player_anim / 2);
    sprites[player_line][player_col].setTexture(
            textures.player[player_direction][anim_state]);

}

void Dodger::check_if_eating(int new_cell)
{
    if (new_cell == Level::apple
            || new_cell == Level::sberry
            || new_cell == Level::cherry) {
        level.food_count--;
        level.data[player_line][player_col] = Level::grid;

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

        score_panel.set_score(score);
        if (score > high_score) {
            high_score = score;
            score_panel.set_high_score(high_score);
        }
    }
}

void Dodger::welcome_screen()
{
    static bool first_call = true;
    if (first_call) {
        first_call = false;
        load_screen(screens::welcome_screen);
    }
    if (next_screen) {
        first_call = true;
        new_game();
        game_state = playing;
    }
}

void Dodger::play()
{
    if (skip_level) {
        // cheat to ease testing
        game_state = password;
    }

    update_dodger_position();

    int new_cell = level.data[player_line][player_col];

    // check if dead and update laser shots
    // need to check if dodger is hit before AND after updating the laser shots
    // to prevent the dodger and the shot to pass each other with no hit.
    bool hit = hit_by_laser();
    update_enemies();
    hit |= hit_by_laser();

    if (hit || new_cell == Level::skull) {
        // Note: can't eat if dead
        game_state = dying;

    } else {

        check_if_eating(new_cell);

        if (level.food_count == 0) {
            std::cout << "level cleared" << std::endl;
            if (level_number == levels.size()) {
                game_state = well_done;
            } else {
                game_state = password;
            }
        }
    }
}

void Dodger::password_screen()
{
    static bool first_call = true;
    if (first_call) {
        first_call = false;

        std::ostringstream main_msg;
        main_msg << "Password for level " << level_number + 1 << " is:";
        main_text.setString(main_msg.str());

        std::ostringstream pwd_msg;
        pwd_msg << passwords[level_number - 1];
        password_text.setString(pwd_msg.str());

        //center text
        sf::Vector2u dim = window.getSize();
        sf::FloatRect textRect = main_text.getLocalBounds();
        main_text.setOrigin(textRect.left + textRect.width/2.0f,
                       textRect.top  + textRect.height/2.0f);
        main_text.setPosition(sf::Vector2f(dim.x / 2.0, dim.y / 2.0 - 10));

        textRect = password_text.getLocalBounds();
        password_text.setOrigin(textRect.left + textRect.width/2.0f,
                textRect.top  + textRect.height/2.0f);
        password_text.setPosition(sf::Vector2f(dim.x / 2.0, dim.y / 2.0 + 10));
    }
    if (next_screen) {
        password_text.setString("");
        level_number++;
        new_level();
        game_state = playing;
        first_call = true;
    }
}

void Dodger::dead()
{
    static int death_anim = 0;

    if (death_anim == 0) {
        play_sound(sounds.dead);
        life_count--;
        score_panel.set_num_lives(life_count);
    }

    if (death_anim == death_anim_length) {
        death_anim = 0;
        if (life_count > 0) {
            // update the texture where the dodger died with whatever is on the grid
            sprites[player_line][player_col].setTexture(
                    textures.cell[level.data[player_line][player_col]]);
            new_life();
            game_state = playing;
        } else {
            game_state = game_over;
        }
    } else {
        sprites[player_line][player_col].setTexture(textures.death[death_anim]);
        death_anim++;
    }
}

void Dodger::game_over_screen()
{
    static bool first_call = true;
    if (first_call) {
        first_call = false;
        load_screen(screens::game_over_screen);
        play_sound(sounds.gover);
        std::cout << "game over" << std::endl;
    }
    if (next_screen) {
        game_state = welcome;
        first_call = true;
    }
}

void Dodger::well_done_screen()
{
    static bool first_call = true;
    if (first_call) {
        first_call = false;
        load_screen(screens::well_done_screen);
        play_sound(sounds.wdone);
        std::cout << "well done" << std::endl;
    }
    if (next_screen) {
        game_state = welcome;
        first_call = true;
    }
}

void Dodger::update()
{
    switch (game_state) {
    case welcome:
        welcome_screen();
        break;
    case playing:
        play();
        break;
    case password:
        password_screen();
        break;
    case dying:
        dead();
        break;
    case game_over:
        game_over_screen();
        break;
    case well_done:
        well_done_screen();
        break;
    }
}

void Dodger::draw()
{
    window.clear();

    score_panel.draw(window);

    for (int line = 0; line != Level::num_lines; line++) {
        for (int col = 0; col != Level::num_cols; col++) {
            window.draw(sprites[line][col]);
        }
    }

    if (game_state == playing || game_state == password) {
        std::vector<Enemy>::iterator enemy;
        for (enemy = enemies.begin(); enemy != enemies.end(); enemy++) {
            enemy->draw(window, (level.data));
        }
    }

    if (password_text.getString() != "") {
        window.draw(pwd_rect);
        window.draw(main_text);
        window.draw(password_text);
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

void set_position(sf::Sprite& sprite, float line, float col)
{
    sprite.setPosition(24 * (2 + col), 24 * (3 + line));
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
