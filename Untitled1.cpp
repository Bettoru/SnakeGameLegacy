#include <iostream> 
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <ctime>
#include <conio.h>
#include <windows.h>
using namespace std;

void SetColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void PrintBox(string title) {
    SetColor(10); // Màu xanh lá
    cout << "##########################################\n";
    cout << "#                                        #\n";
    cout << "#    ";
    SetColor(14); // Màu vàng
    cout << title;
    SetColor(10);
    cout << "        #\n";
    cout << "#                                        #\n";
    cout << "##########################################\n";
    SetColor(7); // Tr? v? màu m?c d?nh
}


// C?u trúc v? trí
struct Position {
    int x, y;
    Position(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

// L?p d?i di?n cho r?n
class Snake {
private:
    vector<Position> body;
    char direction;

public:
    Snake(Position start) : direction(' ') {
        body.push_back(start); // Ð?t d?u r?n
    }

    const vector<Position>& GetBody() const {
        return body;
    }

    void ChangeDirection(char newDirection) {
        if ((direction == 'w' && newDirection == 's') ||
            (direction == 's' && newDirection == 'w') ||
            (direction == 'a' && newDirection == 'd') ||
            (direction == 'd' && newDirection == 'a')) {
            return;
        }
        direction = newDirection;
    }

    void Move() {
        Position head = body[0];
        switch (direction) {
            case 'w': head.y--; break;
            case 's': head.y++; break;
            case 'a': head.x--; break;
            case 'd': head.x++; break;
            default: return;
        }

        body.insert(body.begin(), head);
        body.pop_back();
    }

    void Grow() {
        body.push_back(body.back());
    }

    bool CollidesWith(const Position& pos) const {
        for (const auto& segment : body) {
            if (segment == pos) return true;
        }
        return false;
    }

    bool CollidesWithItself() const {
        const Position& head = body[0];
        for (size_t i = 1; i < body.size(); i++) {
            if (body[i] == head) return true;
        }
        return false;
    }
};

// L?p d?i di?n cho trái cây
class Fruit {
private:
    Position position;

public:
    Fruit(int maxWidth, int maxHeight) {
        GenerateNewPosition(maxWidth, maxHeight);
    }

    const Position& GetPosition() const {
        return position;
    }

    void GenerateNewPosition(int maxWidth, int maxHeight) {
        position.x = rand() % maxWidth;
        position.y = rand() % maxHeight;
    }
};

// L?p d?i di?n cho di?m cao
class HighScore {
public:
    string playerName;
    int score;

    HighScore(string playerName, int score) : playerName(playerName), score(score) {}

    bool operator<(const HighScore& other) const {
        return score > other.score;
    }
};

// L?p qu?n lý danh sách di?m cao
class HighScoreManager {
private:
    const string fileName = "highscores.txt";
    vector<HighScore> highScores;

    void LoadHighScores() {
        highScores.clear();
        ifstream file(fileName);
        if (file.is_open()) {
            string name;
            int score;
            while (file >> name >> score) {
                highScores.emplace_back(name, score);
            }
            file.close();
        }
    }

    void SaveHighScores() {
        ofstream file(fileName);
        if (file.is_open()) {
            for (const auto& hs : highScores) {
                file << hs.playerName << " " << hs.score << endl;
            }
            file.close();
        }
    }

public:
    HighScoreManager() {
        LoadHighScores();
    }

    void AddHighScore(const string& playerName, int score) {
        highScores.emplace_back(playerName, score);
        sort(highScores.begin(), highScores.end());
        if (highScores.size() > 10) {
            highScores.pop_back();
        }
        SaveHighScores();
    }

    void ShowHighScores() const {
        system("cls");
        cout << "High Scores:\n";
        cout << "Rank\tName\t\tScore\n";
        cout << "---------------------------------\n";
        int rank = 1;
        for (const auto& hs : highScores) {
            cout << rank++ << "\t" << hs.playerName << "\t\t" << hs.score << endl;
        }
        cout << "---------------------------------\n";
        cout << "Press any key to return to the menu...";
        _getch();
    }
};

// L?p d?i di?n cho trò choi chính
class Game {
private:
    const int WIDTH, HEIGHT;
    Snake snake;
    Fruit fruit;
    HighScoreManager highScoreManager;
    bool gameOver;
    bool paused;
    int score, speed;

    void Draw() {
    system("cls");

    SetColor(9); // Màu xanh duong
    for (int i = 0; i < WIDTH + 2; i++) cout << "#";
    cout << endl;

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (j == 0) cout << "#";
            if (i == snake.GetBody()[0].y && j == snake.GetBody()[0].x) {
                SetColor(14); // Màu vàng cho d?u r?n
                cout << "O";
                SetColor(9);
            } else if (i == fruit.GetPosition().y && j == fruit.GetPosition().x) {
                SetColor(12); // Màu d? cho trái cây
                cout << "F";
                SetColor(9);
            } else {
                bool isTail = false;
                for (const auto& segment : snake.GetBody()) {
                    if (segment.x == j && segment.y == i) {
                        SetColor(10); // Màu xanh lá cho duôi r?n
                        cout << "o";
                        SetColor(9);
                        isTail = true;
                        break;
                    }
                }
                if (!isTail) cout << " ";
            }
            if (j == WIDTH - 1) cout << "#";
        }
        cout << endl;
    }

    for (int i = 0; i < WIDTH + 2; i++) cout << "#";
    cout << endl;

    SetColor(7); // Tr? v? màu m?c d?nh
    cout << "Score: " << score << endl;
    cout << "Use WASD to move. Press 'P' to pause/resume. Press 'X' to quit." << endl;
}

    void Input() {
        if (_kbhit()) {
            char input = _getch();
            if (input == 'x') {
                gameOver = true;
            } else if (input == 'p') {
                paused = !paused;
            } else if (!paused) {
                snake.ChangeDirection(input);
            }
        }
    }

    void Logic() {
        if (paused) return;

        snake.Move();

        if (snake.GetBody()[0].x < 0 || snake.GetBody()[0].x >= WIDTH ||
            snake.GetBody()[0].y < 0 || snake.GetBody()[0].y >= HEIGHT) {
            gameOver = true;
        }

        if (snake.CollidesWithItself()) {
            gameOver = true;
        }

        if (snake.CollidesWith(fruit.GetPosition())) {
            score += 10;
            snake.Grow();
            fruit.GenerateNewPosition(WIDTH, HEIGHT);
        }
    }

    void ShowInstructions() {
        system("cls");
        cout << "Welcome to Snake Game!\n\n";
        cout << "Instructions:\n";
        cout << "1. Use W, A, S, D to move the snake.\n";
        cout << "2. Collect fruits (F) to grow the snake and increase your score.\n";
        cout << "3. Avoid colliding with walls or yourself.\n";
        cout << "4. Press 'P' to pause/resume the game.\n";
        cout << "5. Press 'X' to exit the game.\n\n";
        cout << "Press any key to return to the menu...";
        _getch();
    }

public:
    Game(int width, int height)
        : WIDTH(width),
          HEIGHT(height), 
          snake(Position(width / 2, height / 2)), 
          fruit(width, height), 
          gameOver(false), 
          paused(false), 
          score(0), 
          speed(200) {}

    void Run() {
        while (true) {
            MainMenu();
            SetSpeed();
            StartGame();
        }
    }

private:
    void MainMenu() {
    while (true) {
        system("cls");
        PrintBox("WELCOME TO SNAKE GAME XD !!!");
        cout << "1. Let's Started Now!\n";
        cout << "2. Instructions\n";
        cout << "3. High Scores - Mark Your Glorious Feats!\n";
        cout << "4. Exit\n";
        cout << "Enter your choice (1-4): ";
        string choice;
        cin >> choice;

        if (choice == "1") {
            break; // Start game
        } else if (choice == "2") {
            ShowInstructions();
        } else if (choice == "3") {
            highScoreManager.ShowHighScores();
        } else if (choice == "4") {
            exit(0);
        } else {
            SetColor(12); // Màu d? cho l?i
            cout << "Invalid choice! Please enter a number between 1 and 4.\n";
            SetColor(7);
            Sleep(1000);
        }
    }
}

    void SetSpeed() {
        while (true) {
            system("cls");
            cout << "Choose game speed:\n";
            cout << "  1. Beyond Lightning & Thunder! (Hard mode)\n";
            cout << "  2. Faster than Wind (Medium mode)\n";
            cout << "  3. Slow and Steady (Easy mode)\n";
            cout << "???. NIGHTMARE REALM (Warning:Very fast OwO)\n";
            cout << "Enter your choice (1-3): ";
            string choice;
            cin >> choice;

            if (choice == "1") {
                speed = 50;
                break;
            } else if (choice == "2") {
                speed = 100;
                break;
            } else if (choice == "3") {
                speed = 300;
                break;
            } else if (choice == "666") {
                speed = 1;
                break;
            } else {
                cout << "Invalid choice! Please enter 1, 2, or 3.\n";
                Sleep(1000);
            }
        }
    }

    void StartGame() {
        gameOver = false;
        score = 0;
        snake = Snake(Position(WIDTH / 2, HEIGHT / 2));
        fruit.GenerateNewPosition(WIDTH, HEIGHT);

        cout << "Enter your name: ";
        string playerName;
        cin >> playerName;

        while (!gameOver) {
            Draw();
            Input();
            Logic();
            Sleep(speed);
        }

        cout << "Game Over! Final Score: " << score << endl;
        highScoreManager.AddHighScore(playerName, score);

        PostGameMenu();
    }

    void PostGameMenu() {
        while (true) {
            cout << "Do you want to return to the menu? (y/n): ";
            char choice;
            cin >> choice;

            if (choice == 'y' || choice == 'Y') {
                return; // Quay l?i menu chính
            } else if (choice == 'n' || choice == 'N') {
                exit(0);
            } else {
                cout << "Invalid choice! Please enter 'y' or 'n'.\n";
            }
        }
    }
};

int main() {
    srand(static_cast<unsigned int>(time(0)));
    Game game(50, 20);
    game.Run();
    return 0;
}

