#ifndef score_panell_hpp
#define score_panell_hpp

#include <SFML/Graphics.hpp>

class ScorePanel
{
public:
    ScorePanel();
    void set_num_lives(int num_lifes);
    void set_level(int level);
    void set_score(int new_score);
    void set_high_score(int new_high_score);
    void draw(sf::RenderWindow& window);

private:
    enum { n0 = 0, n1, n2, n3, n4, n5, n6, n7, n8, n9,
        letterS, letterC, letterL, letterV, letterH, letterI,
        colon, life, blank, num_textures };

    enum { board_length = 28 };

    void update_number(int number, int first_digit_index, int num_digits);

    sf::Texture textures[num_textures];
    sf::Sprite board[board_length];
};

#endif
