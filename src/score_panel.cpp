#include "score_panel.hpp"
#include "dodger.hpp"
#include <math.h>

ScorePanel::ScorePanel()
{
    textures[n0].loadFromFile("img/n0.bmp");
    textures[n1].loadFromFile("img/n1.bmp");
    textures[n2].loadFromFile("img/n2.bmp");
    textures[n3].loadFromFile("img/n3.bmp");
    textures[n4].loadFromFile("img/n4.bmp");
    textures[n5].loadFromFile("img/n5.bmp");
    textures[n6].loadFromFile("img/n6.bmp");
    textures[n7].loadFromFile("img/n7.bmp");
    textures[n8].loadFromFile("img/n8.bmp");
    textures[n9].loadFromFile("img/n9.bmp");
    textures[letterS].loadFromFile("img/s.bmp");
    textures[letterC].loadFromFile("img/c.bmp");
    textures[letterL].loadFromFile("img/l.bmp");
    textures[letterV].loadFromFile("img/v.bmp");
    textures[letterH].loadFromFile("img/h.bmp");
    textures[letterI].loadFromFile("img/i.bmp");
    textures[colon].loadFromFile("img/colon.bmp");
    textures[life].loadFromFile("img/lives.bmp");
    textures[blank].loadFromFile("img/blank.bmp");

    for (int i = 0; i < board_length; i++) {
        set_position(board[i], -3, i - 1.5);
    }

    // score
    board[0].setTexture(textures[letterS]);
    board[1].setTexture(textures[letterC]);
    board[2].setTexture(textures[colon]);
    board[3].setTexture(textures[n0]);
    board[4].setTexture(textures[n0]);
    board[5].setTexture(textures[n0]);
    board[6].setTexture(textures[n0]);
    board[7].setTexture(textures[n0]);

    board[8].setTexture(textures[blank]);

    // lives
    board[9].setTexture(textures[life]);
    board[10].setTexture(textures[life]);
    board[11].setTexture(textures[life]);

    board[12].setTexture(textures[blank]);

    // level
    board[13].setTexture(textures[letterL]);
    board[14].setTexture(textures[letterV]);
    board[15].setTexture(textures[colon]);
    board[16].setTexture(textures[n0]);
    board[17].setTexture(textures[n0]);

    board[18].setTexture(textures[blank]);
    board[19].setTexture(textures[blank]);

    // high score
    board[20].setTexture(textures[letterH]);
    board[21].setTexture(textures[letterI]);
    board[22].setTexture(textures[colon]);
    board[23].setTexture(textures[n0]);
    board[24].setTexture(textures[n0]);
    board[25].setTexture(textures[n0]);
    board[26].setTexture(textures[n0]);
    board[27].setTexture(textures[n0]);
}

void ScorePanel::set_num_lives(int num_lifes)
{
    int max_lives = 3;
    for (int i = 0; i < num_lifes; i++) {
        board[9 + i].setTexture(textures[life], true);
    }
    for (int i = num_lifes; i < max_lives; i++) {
        board[9 + i].setTexture(textures[blank]);
    }
}

void ScorePanel::update_number(int number, int first_digit_index, int num_digits)
{
    if (number >= pow(10, num_digits + 1)) {
        error("Can't display score. Not enough digits.");
    }
    for (int digit_pos = num_digits - 1; digit_pos >= 0; digit_pos--) {
        int power = pow(10, digit_pos);
        int val = number / power;
        int index = first_digit_index + num_digits - digit_pos;
        board[index].setTexture(textures[val]);
        number -= val * power;
    }
}

void ScorePanel::set_level(int level)
{
    update_number(level, 15, 2);
}

void ScorePanel::set_score(int new_score)
{
    update_number(new_score, 2, 5);
}

void ScorePanel::set_high_score(int new_high_score)
{
    update_number(new_high_score, 22, 5);
}

void ScorePanel::draw(sf::RenderWindow& window)
{
    for (int i = 0; i < board_length; i++) {
        window.draw(board[i]);
    }
}
