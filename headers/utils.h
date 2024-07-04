#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <math.h>
#include <vector>

float getRandomFromZeroToOne() {
    float r = ((double) rand() / (RAND_MAX));
    return r;
}

float getRandomLightOffset() {
    return getRandomFromZeroToOne() - 0.5;
}

std::vector<std::string> split(std::string line, char delimiter = ' ') {
    std::vector<std::string> tokens;
    int startIndex = 0;
    int current = 0;

    while (current < line.length()) {
        if (line[current] == delimiter) {
            if (current != startIndex) {
                tokens.push_back(line.substr(startIndex, current-startIndex));
            }
            else {
                tokens.push_back("");
            }
            startIndex = current + 1;
        }
        else if (current == line.length() - 1) {
            tokens.push_back(line.substr(startIndex, current-startIndex+1));
        }
        current++;
    }
    return tokens;
}

float getRandom(int min, int max) {
    float rand = std::rand() % (max-min+1) + min - 1; /* min-1 -> max-1*/
    rand += getRandomFromZeroToOne(); /* min -> max */
    return rand;
}

#endif // UTILS_H_INCLUDED