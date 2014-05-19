#ifndef level_hpp
#define level_hpp

#include <fstream>

class Level
{
public:
    bool load(std::istream& stream);
    bool import(std::istream& stream);

//private:
    enum {
        s,
        w,
        n,
        e,
        num_enemies,
    };

    enum {
        num_lines = 20,
        num_cols = 25,
    };

    enum {
        blank,
        grid,
        wall,
        apple,
        sberry,
        cherry,
        skull,
        player,
        num_cell_types,
    };

    bool enemies[num_enemies];
    int data[num_lines][num_cols];
    int speed;
    int food_count;
    unsigned initial_player_line;
    unsigned initial_player_col;

    Level(): speed(), food_count(), initial_player_line(), initial_player_col() {}

};

#endif
