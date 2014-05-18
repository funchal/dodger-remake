#include "level.hpp"
#include "dodger.hpp"

bool Level::load(std::istream& stream)
{
    stream.exceptions(~std::istream::goodbit);

    // parse positions of enemies
    std::string str;
    std::getline(stream, str);
    if (str.length() < 1) {
        error("Invalid level format: no enemies");
    }

    std::string::iterator pos;
    const std::string enemies_format("nesw");
    for (pos = str.begin(); pos != str.end(); pos++) {
        size_t enemy = enemies_format.find(*pos);
        if (enemy == std::string::npos) {
            error("Invalid level format: unrecognized enemy direction");
        }
        enemies[enemy] = true;
    }

    // parse grid, find player position
    bool found_player = false;
    for (int line = 0; line != num_lines; line++) {
        std::getline(stream, str);
        if (str.length() != num_cols) {
            error("Invalid level format: too many columns");
        }

        for (int col = 0; col != num_cols; col++) {
            int cell = str[col] - '0';
            switch (cell) {
                case blank:
                case grid:
                case wall:
                case skull:
                    break;

                case player:
                    if (found_player) {
                        error("Invalid level format: multiple players");
                    }
                    found_player = true;
                    break;

                case apple:
                case cherry:
                case sberry:
                    food_count++;
                    break;

                default:
                    error("Invalid level format: unrecognized cell type");
            }

            data[line][col] = cell;
        }
    }

    if (!found_player) {
        error("Invalid level format: player not found");
    }

    if (food_count == 0) {
        error("Invalid level format: no food");
    }

    stream.exceptions(std::istream::badbit |
                      std::istream::failbit);

    // check for end of file
    if (stream.peek() == EOF) {
        return false;
    }

    stream.exceptions(~std::istream::goodbit);

    // ignore empty line between levels
    std::getline(stream, str);
    if (str.length() != 0) {
        error("Invalid level format: expected empty line");
    }

    return true;
}

bool Level::import(std::istream& stream)
{
    stream.exceptions(~std::istream::goodbit);

    // parse positions of enemies
    unsigned bitmask = stream.get();
    if ((bitmask & ~0xF) || !(bitmask & 0xF)) {
        error("Invalid level format: unrecognized enemies bitmask");
    }

    enemies[n] = bitmask & (1 << n);
    enemies[e] = bitmask & (1 << e);
    enemies[s] = bitmask & (1 << s);
    enemies[w] = bitmask & (1 << w);

    speed = stream.get();
    if (speed < 0 || speed > 4) {
        error("Invalid level format: speed out of bounds");
    }

    // find player position
    initial_player_col = stream.get();
    initial_player_line = stream.get();
    if (initial_player_line > num_lines || initial_player_col > num_cols) {
        error("Invalid level format: incorrect start position");
    }

    // parse grid
    food_count = 0;
    for (int line = 0; line != num_lines; line++) {
        for (int col = 0; col != num_cols; col++) {
            int cell = stream.get() - '0';
            switch (cell) {
                case blank:
                case grid:
                case wall:
                case skull:
                    break;

                case apple:
                case cherry:
                case sberry:
                    food_count++;
                    break;

                default:
                    error("Invalid level format: unrecognized cell type");
            }

            data[line][col] = cell;
        }
    }

    if (food_count == 0) {
        error("Invalid level format: no food");
    }

    if (data[initial_player_line][initial_player_col] != grid) {
        error("Invalid level format: player must start in grid");
    }

    data[initial_player_line][initial_player_col] = player;

    stream.exceptions(std::istream::badbit |
                      std::istream::failbit);

    // check for end of file
    if (stream.peek() == EOF) {
        return false;
    }

    stream.exceptions(~std::istream::goodbit);

    return true;
}
