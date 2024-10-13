#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <string>
#include <chrono>
#include <windows.h>
#include <stack> 
#include <cmath> 

using namespace std;
using namespace std::chrono;

// Структура, представляющая доску и фишки
struct Board {
    vector<vector<int>> grid;
    int num_pieces;

    Board()
    {}

    Board(const vector<vector<int>>& initial_grid) : grid(initial_grid) {
        num_pieces = count_pieces();
    }

    // Подсчёт фишек на доске
    int count_pieces() {
        int count = 0;
        for (const auto& row : grid) {
            for (int cell : row) {
                if (cell == 1) count++;
            }
        }
        return count;
    }

    // Проверка окончания игры (осталась одна фишка)
    bool is_goal() const {
        return num_pieces == 1;
    }

    // Вывод доски
    void print() const {
        for (const auto& row : grid) {
            for (int cell : row) {
                cout << (cell == 1 ? 'O' : '.');
            }
            cout << endl;
        }
        cout << endl;
    }

    // Генерация новых состояний (ходы)
    vector<pair<Board, string>> get_neighbors() const {
        vector<pair<Board, string>> neighbors;
        int n = grid.size();

        // Проверяем все возможные ходы (вверх, вниз, влево, вправо)
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (grid[i][j] == 1) {
                    // Вверх
                    if (i > 1 && grid[i - 1][j] == 1 && grid[i - 2][j] == 0) {
                        Board new_board = *this;
                        new_board.grid[i][j] = 0;
                        new_board.grid[i - 1][j] = 0;
                        new_board.grid[i - 2][j] = 1;
                        new_board.num_pieces--;
                        neighbors.push_back({ new_board, "Move (" + to_string(i) + ", " + to_string(j) + ") to (" + to_string(i - 2) + ", " + to_string(j) + ")" });
                    }
                    // Вниз
                    if (i < n - 2 && grid[i + 1][j] == 1 && grid[i + 2][j] == 0) {
                        Board new_board = *this;
                        new_board.grid[i][j] = 0;
                        new_board.grid[i + 1][j] = 0;
                        new_board.grid[i + 2][j] = 1;
                        new_board.num_pieces--;
                        neighbors.push_back({ new_board, "Move (" + to_string(i) + ", " + to_string(j) + ") to (" + to_string(i + 2) + ", " + to_string(j) + ")" });
                    }
                    // Влево
                    if (j > 1 && grid[i][j - 1] == 1 && grid[i][j - 2] == 0) {
                        Board new_board = *this;
                        new_board.grid[i][j] = 0;
                        new_board.grid[i][j - 1] = 0;
                        new_board.grid[i][j - 2] = 1;
                        new_board.num_pieces--;
                        neighbors.push_back({ new_board, "Move (" + to_string(i) + ", " + to_string(j) + ") to (" + to_string(i) + ", " + to_string(j - 2) + ")" });
                    }
                    // Вправо
                    if (j < n - 2 && grid[i][j + 1] == 1 && grid[i][j + 2] == 0) {
                        Board new_board = *this;
                        new_board.grid[i][j] = 0;
                        new_board.grid[i][j + 1] = 0;
                        new_board.grid[i][j + 2] = 1;
                        new_board.num_pieces--;
                        neighbors.push_back({ new_board, "Move (" + to_string(i) + ", " + to_string(j) + ") to (" + to_string(i) + ", " + to_string(j + 2) + ")" });
                    }
                }
            }
        }
        return neighbors;
    }

    // Оператор сравнения для хранения в unordered_set (поиск состояний)
    bool operator==(const Board& other) const {
        return grid == other.grid;
    }

    // Функция хэширования для unordered_set
    struct Hash {
        size_t operator()(const Board& b) const {
            size_t hash_value = 0;
            for (const auto& row : b.grid) {
                for (int cell : row) {
                    hash_value = hash_value * 31 + cell;
                }
            }
            return hash_value;
        }
    };
};

struct Node {
    Board board;
    int g;
    int h;
    int f;
    Node* parent;
    string move;

    Node(Board board, int g, int h, Node* parent = nullptr, string move = "") 
    {
        parent = parent;

        board = board;

        g = g;
        h = h;
        f = g + h;
        move = move;
    }

    Node(){}

    bool operator<(const Node& other) const {
        return f > other.f; // Для min-heap
    }
};

// Создание нового узла
Node* newNode(Board board, int g, int h, Node* parent = nullptr, string move = "") {

    Node* temp = new Node();
    temp->parent = parent;

    temp->board = board;

    temp->g = g;
    temp->h = h;
    temp->f = g + h;
    temp->move = move;

    return temp;

}

// Эвристика
int heuristic(const Board& board, int g){

    int total_distance = 0;
    //Эвристика для глубины > 15 (манхэттенские растояния)
    if (g > 15)
    {
        vector<pair<int, int>> pieces;

        // Собираем координаты всех фишек
        for (int i = 0; i < board.grid.size(); i++) {
            for (int j = 0; j < board.grid[i].size(); j++) {
                if (board.grid[i][j] == 1) {
                    pieces.push_back({ i, j });
                }
            }
        }

        // Считаем манхэттенские расстояния между всеми фишками
        for (size_t i = 0; i < pieces.size(); i++) {
            for (size_t j = i + 1; j < pieces.size(); j++) {
                total_distance += abs(pieces[i].first - pieces[j].first) + abs(pieces[i].second - pieces[j].second);
            }
        }
    }
    //до глубины 15 эвристика по количеству соседей
    else
    {
        int n = board.grid.size();

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (board.grid[i][j] == 1) {
                    int neighbors = 0;
                    if (i > 0 && board.grid[i - 1][j] == 1) neighbors++;
                    if (i < n - 1 && board.grid[i + 1][j] == 1) neighbors++;
                    if (j > 0 && board.grid[i][j - 1] == 1) neighbors++;
                    if (j < n - 1 && board.grid[i][j + 1] == 1) neighbors++;
                    total_distance += neighbors;
                }
            }
        }
    }

    return total_distance;
}

// Восстановление и вывод последовательности ходов
void print_solution(Node* node) {
    stack<Node*> moves;
    while (node->parent != nullptr) {
        moves.push(node);
        node = node->parent;
    }

    int i = 0;
    cout << "Последовательность ходов:" << endl;
    while (!moves.empty()) {
        cout << "\n Ход " << ++i << ": " << moves.top()->move << "\n";
        moves.top()->board.print();
        moves.pop();
    }
}

// A* поиск
bool astar(const Board& start_board) {
    auto start = high_resolution_clock::now();
    priority_queue<Node*> open_list;
    unordered_set<Board, Board::Hash> closed_list;

    open_list.push(newNode(start_board, 0, heuristic(start_board, 0), NULL));

    while (!open_list.empty()) {
        Node* current = open_list.top();
        open_list.pop();

        // Если достигли цели (осталась одна фишка)
        if (current->board.is_goal()) {
            auto finish = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(finish - start);
            cout << "Время: " << duration.count() / 1000.0 << " мсек.\n";
            cout << "Игра завершена за " << current->g << " ходов." << endl;
            current->board.print();
            print_solution(current);
            return true;
        }

        // Добавляем текущее состояние в закрытый список
        closed_list.insert(current->board);

        // Генерируем соседей (возможные ходы)
        vector<pair<Board, string>> neighbors = current->board.get_neighbors();
        for (const auto& neighbor_pair : neighbors) {
            const Board& neighbor = neighbor_pair.first;
            const string& move = neighbor_pair.second;
            if (closed_list.find(neighbor) == closed_list.end()) {
                int new_g = current->g + 1;
                int new_h = heuristic(neighbor, current->g);
                open_list.push(newNode(neighbor, new_g, new_h, current, move));
            }
        }
    }

    return false; // Если решение не найдено
}

int main() {
    setlocale(LC_ALL, "RU");

    // Начальная конфигурация доски (1 - фишка, 0 - пусто)
    vector<vector<int>> initial_grid = {
        {0, 0, 1, 1, 1, 0, 0},
        {0, 0, 1, 1, 1, 0, 0},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 0, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {0, 0, 1, 1, 1, 0, 0},
        {0, 0, 1, 1, 1, 0, 0}
    };

    Board start_board(initial_grid);

    if (!astar(start_board)) {
        cout << "Решение не найдено." << endl;
    }

    return 0;
}
