// Project Identifier: A8A3A33EF075ACEF9B08F5B9845569ECCB423725

#include <getopt.h>
#include <cstring>
#include <vector>
#include <iostream>
#include <string>
#include <deque>
#include <cctype>
#include <cstdlib>

using namespace std;

struct state
{
    int color;
    int row;
    int col;
};

char Int2Char (const int integer) {
    if (integer == 0) {
        return '^';
    }
    else {
        return static_cast<char>((integer - 1) + 'a');
    }
}

int Char2Int (const char character) {
    if (character == '^' || character == '@') {
        return 0;
    }
    else {
        return character - 'a' + 1;
    }
}

int Char2IntinDis(const char character, const state &current) {
    if (character == '^') {
        return 0;
    }
    else if (character == '.' || character == '@') {
        return current.color;
    }
    else {
        return tolower(character) - 'a' + 1;
    }
}

void Initialization(vector<vector<vector<char>>> &Backtrace, const int numColors, vector<vector<char>> &map, state &start) {
    string line;
    while (getline(cin, line)) {
        if (line[0] == '/') {
            continue;
        }
        vector<char> row(line.begin(), line.end());
        map.push_back(row);
    }
    int countStartEnd = 0;
    for (size_t i = 0; i < map.size(); ++i) {
        for (size_t j = 0; j < map[0].size(); ++j) {
            if (map[i][j] == '@') {
                countStartEnd += 1;
                start.color = 0;
                start.row = static_cast<int>(i);
                start.col = static_cast<int>(j);
                Backtrace[0][i][j] = '@';
            }
            else if (map[i][j] == '?') {
                countStartEnd += 1;
            }
            else if (isalpha(map[i][j])) {
                if (tolower(map[i][j]) - 'a' + 1 > numColors) {
                    if (isupper(map[i][j])) {
                        cerr << "Invalid door in map";
                    }
                    else {
                        cerr << "Invalid wall in map";
                    }
                    exit(1);
                }
            }
            else if (map[i][j] != '.' && map[i][j] != '#' && map[i][j] != '^') {
                cerr << "Invalid character in map";
                exit(1);
            }
        }
    }
    if (countStartEnd > 2) {
        cerr << "Puzzle must have only one start and one target";
        exit(1);
    }
    else if (countStartEnd < 2) {
        cerr << "Puzzle must have exactly one start and one target";
        exit(1);
    }
}

bool validState(const vector<vector<vector<char>>> &Backtrace, const vector<vector<char>> &map, state &next, state &current) {
    if (next.row >= 0 && next.col >= 0) { //search if valid
        if (next.row < static_cast<int>(map.size()) && next.col < static_cast<int>(map[0].size())) {
            next.color = current.color;
            char character = map[next.row][next.col];
            if (character != '#') {
                if (!Backtrace[next.color][next.row][next.col]) {
                    if (isalpha(character)) {
                        if (isupper(character)) {
                            if (tolower(character) - 'a' + 1 != current.color) {
                                return false;
                            }
                            return true;
                        }
                        return true;
                    }
                    return true;
                }
                return false;
            }
            return false;
        }
        return false;
    }
    return false;
}

bool target(const vector<vector<char>> &map, const int row, const int col) {
    if (map[row][col] == '?') {
        return true;
    }
    return false;
}

void investigate(vector<vector<vector<char>>> &Backtrace, state &current, deque<state> &searchContainer, const vector<vector<char>> &map, const bool stackUsage, bool &finish) {
    state next;
    next.row = current.row - 1;
    next.col = current.col;
    if (validState(Backtrace, map, next, current)) {
        Backtrace[next.color][next.row][next.col] = 'S';
        if (target(map, next.row, next.col)) {
            current = next;
            finish = true;
            return;
        }
        if (stackUsage) {
            searchContainer.push_front(next);
        }
        searchContainer.push_back(next);
    }
    next.row = current.row;
    next.col = current.col + 1;
    if (validState(Backtrace, map, next, current)) {
        Backtrace[next.color][next.row][next.col] = 'W';
        if (target(map, next.row, next.col)) {
            current = next;
            finish = true;
            return;
        }
        if (stackUsage) {
            searchContainer.push_front(next);
        }
        searchContainer.push_back(next);
    }
    next.row = current.row + 1;
    next.col = current.col;
    if (validState(Backtrace, map, next, current)) {
        Backtrace[next.color][next.row][next.col] = 'N';
        if (target(map, next.row, next.col)) {
            current = next;
            finish = true;
            return;
        }
        if (stackUsage) {
            searchContainer.push_front(next);
        }
        searchContainer.push_back(next);
    }
    next.row = current.row;
    next.col = current.col - 1;
    if (validState(Backtrace, map, next, current)) {
        Backtrace[next.color][next.row][next.col] = 'E';
        if (target(map, next.row, next.col)) {
            current = next;
            finish = true;
            return;
        }
        if (stackUsage) {
            searchContainer.push_front(next);
        }
        searchContainer.push_back(next);
    }
}


void discover(state &current, const state &start, const vector<vector<char>> &map, vector<vector<vector<char>>> &Backtrace, deque<state> &searchContainer, const bool stackUsage, const bool queueUsage) {
    searchContainer.push_back(start);
    bool finish = false;
    while (!searchContainer.empty() && !finish) {
        current = searchContainer.front();
        searchContainer.pop_front();
        if (Char2IntinDis(map[current.row][current.col], current) == current.color) {
            investigate(Backtrace, current, searchContainer, map, stackUsage, finish);
        }
        else {
            state button;
            button.color = Char2Int(map[current.row][current.col]);
            button.row = current.row;
            button.col = current.col;
            if (!Backtrace[button.color][button.row][button.col]) {
                Backtrace[button.color][button.row][button.col] = Int2Char(current.color);
                if (stackUsage) {
                    searchContainer.push_front(button);
                }
                else if (queueUsage) {
                    searchContainer.push_back(button);
                }
            }
        }
    }
}

void retracePath(state &current, const vector<vector<char>> &map, deque<state> &path, const vector<vector<vector<char>>> &Backtrace) {
    if (target(map, current.row, current.col)) {
        path.push_front(current);
        while (Backtrace[current.color][current.row][current.col] != '@') {
            char character = Backtrace[current.color][current.row][current.col];
            if (character == 'S') {
                current.row += 1;
                path.push_front(current);
            }
            else if (character == 'W') {
                current.col -= 1;
                path.push_front(current);
            }
            else if (character == 'N') {
                current.row -= 1;
                path.push_front(current);
            }
            else if (character == 'E') {
                current.col += 1;
                path.push_front(current);
            }
            else {
                current.color = Char2Int(character);
                path.push_front(current);
            }
        }
    }
}

void fillDiscover(const vector<vector<vector<char>>> &Backtrace, vector<vector<char>> &map, const int numColors) {
    for (size_t i = 0; i < map.size(); i++) {
        for (size_t j = 0; j < map[0].size(); j++) {
            bool justDiscover = false;
            for (int k = 0; k <= numColors; k++) {
                if (Backtrace[k][i][j]) {
                    justDiscover = true;
                    break;
                }
            }
            if (!justDiscover) {
                map[i][j] = '#';
            }
            cout << map[i][j];
        }
        cout << "\n";
    }
}
void printOutput(const string output, deque<state> &path, vector<vector<char>> &map, vector<vector<vector<char>>> &Backtrace, const int numColors, const int height, const int width, const state &start, state &current) {
    if (path.empty()) {
        cout << "No solution.\nDiscovered:\n";
        fillDiscover(Backtrace, map, numColors);
    }
    else {
        if (output == "list") {
            while (!path.empty()) {
                current = path.front();
                path.pop_front();
                cout << "(" << Int2Char(current.color) << ", (" << current.row << ", " << current.col << "))\n";
            }
        }
        else if (output == "map") {
            for (int color = 0; color <= numColors; ++color) {
                for (int i = 0; i < height; ++i) {
                    for (int j = 0; j < width; ++j) {
                        if(color == 0 && map[i][j] == '^') {
                            Backtrace[color][i][j] = '.';
                        }
                        else if (isalpha(map[i][j])) {
                            if (tolower(map[i][j]) - 'a' + 1 == color) {
                                Backtrace[color][i][j] = '.';
                            }
                            else {
                                Backtrace[color][i][j] = map[i][j];
                            }
                        }
                        else {
                            Backtrace[color][i][j] = map[i][j];
                        }
                    }
                }
                if (color != 0) {
                    Backtrace[color][start.row][start.col] = '.';
                }
            }
            state checkPath;
            while(!path.empty()) {
                current = path.front();
                path.pop_front();
                Backtrace[current.color][current.row][current.col] = '@';
                while (!path.empty()) {
                    checkPath = path.front();
                    if(checkPath.color == current.color) {
                        Backtrace[checkPath.color][checkPath.row][checkPath.col] = '+';
                        current = checkPath;
                    }
                    else {
                        break;
                    }
                    path.pop_front();
                }
                Backtrace[current.color][current.row][current.col] = '%';
                if (target(map, current.row, current.col)) {
                    Backtrace[current.color][current.row][current.col] = '?';
                }
            }
            for (int color = 0; color <= numColors; color++) {
                cout << "// color " << Int2Char(color) << "\n";
                for (size_t j = 0; j < map.size(); j++) {
                    for (size_t i = 0; i < map[0].size(); i++) {
                        cout << Backtrace[color][j][i];
                    }
                    cout << "\n";
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    static struct option long_options[] = {
            {"help", 0, nullptr, 'h'},
            {"queue", 0, nullptr, 'q'},
            {"stack", 0, nullptr, 's'},
            {"output", 1, nullptr, 'o'},
            {0, 0, nullptr, 0}
        };
    int option;
    bool stackUsage = false;
    bool queueUsage = false;
    bool outputType = false;
    string output;
    while ((option = getopt_long(argc, argv, "hqso:", long_options, nullptr)) != -1) {
        switch (option)
        {
        case 'h':
            cout << "This is a program to solve your puzzle\n";
            return 0;
        case 'q':
            if (stackUsage) {
                cerr << "Can not have both stack and queue\n";
                exit(1);
            }
            queueUsage = true;
            break;
        case 's':
            if (queueUsage) {
                cerr << "Can not have both stack and queue\n";
                exit(1);
            }
            stackUsage = true;
            break;
        case 'o':
            if (outputType) {
                cerr << "Please  choose only one output type: map or list\n";
                exit(1);
            }
            output = optarg;
            if (strcmp(optarg, "map") != 0 && strcmp(optarg, "list") != 0) {
                cerr << "Invalid output mode\n";
                exit(1);
            }
            else {
                outputType = true;
            }
            break;
        default:
            cerr << "Unknown option\n";
            exit(1);
        }
    }
    if (stackUsage == false && queueUsage == false) {
        cerr << "Must have at least one of stack or queue";
        exit(1);
    }
    if (outputType == false) {
        output = "map";
    }

    int numColors, height, width;
    string junk;
    state start;

    cin >> numColors >> height >> width;
    if (numColors < 0 || numColors > 26) {
        cerr << "Invalid numColor";
        exit(1);
    }
    if (height < 1) {
        cerr << "Invalid height";
        exit(1);
    }
    if (width < 1) {
        cerr << "Invalid width";
        exit(1);
    }
    getline(cin, junk);
    vector<vector<char>> map;
    vector<vector<vector<char>>> Backtrace(numColors + 1, vector<vector<char>>(height, vector<char>(width)));
    deque<state> searchContainer;
    state current;
    Initialization(Backtrace, numColors, map, start);
    discover(current, start, map, Backtrace, searchContainer, stackUsage, queueUsage);
    searchContainer.clear();
    retracePath(current, map, searchContainer, Backtrace);
    printOutput(output, searchContainer, map, Backtrace, numColors, height, width, start, current);
}