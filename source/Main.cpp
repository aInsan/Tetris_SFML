#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

// Using constexpr because they are declared variables are evaluated at compile time and can be used to declare things like arrays that need const expressions
constexpr int BOARD_WIDTH = 10; 
constexpr int BOARD_HEIGHT = 20;
constexpr int BLOCK_SIZE = 30;
constexpr int WINDOW_WIDTH = BOARD_WIDTH * BLOCK_SIZE + 200;
constexpr int WINDOW_HEIGHT = BOARD_HEIGHT * BLOCK_SIZE;
const sf::Color BACKGROUND_COLOR = sf::Color::Black;

// 3D vector for the teraminos
const std::vector<std::vector<std::vector<std::vector<int>>>> SHAPES = {
    // I Shape
    {
        {
            {1, 1, 1, 1}
        },
        {
            {1},
            {1},
            {1},
            {1}
        }
    },
    // J Shape
    {
        {
            {1, 0, 0},
            {1, 1, 1}
        },
        {
            {1, 1},
            {1, 0},
            {1, 0}
        },
        {
            {1, 1, 1},
            {0, 0, 1}
        },
        {
            {0, 1},
            {0, 1},
            {1, 1}
        }
    },
    // L Shape
    {
        {
            {0, 0, 1},
            {1, 1, 1}
        },
        {
            {1, 0},
            {1, 0},
            {1, 1}
        },
        {
            {1, 1, 1},
            {1, 0, 0}
        },
        {
            {1, 1},
            {0, 1},
            {0, 1}
        }
    },
    // O Shape
    {
        {
            {1, 1},
            {1, 1}
        }
    },
    // S Shape
    {
        {
            {0, 1, 1},
            {1, 1, 0}
        },
        {
            {1, 0},
            {1, 1},
            {0, 1}
        }
    },
    // T Shape
    {
        {
            {0, 1, 0},
            {1, 1, 1}
        },
        {
            {1, 0},
            {1, 1},
            {1, 0}
        },
        {
            {1, 1, 1},
            {0, 1, 0}
        },
        {
            {0, 1},
            {1, 1},
            {0, 1}
        }
    },
    // Z Shape
    {
        {
            {1, 1, 0},
            {0, 1, 1}
        },
        {
            {0, 1},
            {1, 1},
            {1, 0}
        }
    }
};

const sf::Color PIECE_COLORS[] = {
    sf::Color(160, 32, 240), //Purple
    sf::Color::Yellow,
    sf::Color(255, 165, 0), // Orange (there is no Orange in SFML by default)
    sf::Color::Blue,
    sf::Color::Magenta,
    sf::Color::Green,
    sf::Color::Red
};

class TetrisGame {
public:
    TetrisGame()
        : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Tetris"),
          block(sf::Vector2f(BLOCK_SIZE, BLOCK_SIZE)),
          nextBlock(sf::Vector2f(BLOCK_SIZE, BLOCK_SIZE)),
          font(),
          scoreText(),
          linesClearedText(),
          nextPieceText(),
          titleText(),
          gameOverText(),
          backgroundMusic()
    {
        if (!font.loadFromFile("resources/coolvetica.otf")) {
            throw std::runtime_error("Failed to load font file.");
        }
        if (!backgroundMusic.openFromFile("resources/music.flac")) {
            throw std::runtime_error("Failed to load music file.");
        }


        // Whole lot of text adjustments and stuff
        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(BOARD_WIDTH * BLOCK_SIZE + 60, 400);
        scoreText.setOrigin(scoreText.getLocalBounds().width / 2.f, 0.f);
        linesClearedText.setFont(font);
        linesClearedText.setCharacterSize(24);
        linesClearedText.setFillColor(sf::Color::White);
        linesClearedText.setPosition(BOARD_WIDTH * BLOCK_SIZE + 20, 320);
        linesClearedText.setOrigin(linesClearedText.getLocalBounds().width / 2.f, 0.f);
        nextPieceText.setFont(font);
        nextPieceText.setString("Next Piece: \n\n\n\n\n R to reset");
        nextPieceText.setCharacterSize(24);
        nextPieceText.setFillColor(sf::Color::White);
        nextPieceText.setPosition(BOARD_WIDTH * BLOCK_SIZE + 40, 120);
        titleText.setFont(font);
        titleText.setCharacterSize(36);
        titleText.setFillColor(sf::Color::White);
        titleText.setString("Tetris");
        titleText.setPosition(WINDOW_WIDTH - 150, 20);
        gameOverText.setFont(font);
        gameOverText.setCharacterSize(36);
        gameOverText.setFillColor(sf::Color::White);
        gameOverText.setString("Game Over");
        gameOverText.setPosition(BOARD_WIDTH * BLOCK_SIZE / 2 - 90, BOARD_HEIGHT * BLOCK_SIZE / 2 - 50);

        resetGame();
    }

    void run() {
        // The clock runs the game at a constant speed at any fps.
        sf::Clock clock;
        sf::Time timeSinceLastUpdate = sf::Time::Zero;
        const sf::Time timePerFrame = sf::seconds(1.0f / 2.0f);

        while (window.isOpen()) {
            processEvents();
            //get the time between frames
            timeSinceLastUpdate += clock.restart(); 
            while (timeSinceLastUpdate > timePerFrame) {
                timeSinceLastUpdate -= timePerFrame;
                //Do the math between frames
                processEvents();
                update();
            }
            //But only render at constant times
            render();
        }
    }

private:
    sf::RenderWindow window;
    sf::RectangleShape block;
    sf::RectangleShape nextBlock;
    sf::Font font;
    sf::Text scoreText;
    sf::Text linesClearedText;
    sf::Text titleText;
    sf::Text nextPieceText;
    sf::Text gameOverText;
    sf::Music backgroundMusic;

    std::vector<std::vector<int>> board;
    int currentPieceIndex;
    int currentPieceRotation;
    int currentPieceX;
    int currentPieceY;
    int nextPieceIndex;
    int score;
    int linesCleared;
    bool isGameOver;


    // Resets the Game and all it's values
    void resetGame() {
        board = std::vector<std::vector<int>>(BOARD_HEIGHT, std::vector<int>(BOARD_WIDTH, 0));
        currentPieceIndex = getRandomPieceIndex();
        currentPieceRotation = 0;
        currentPieceX = BOARD_WIDTH / 2 - 1;
        currentPieceY = 0;
        nextPieceIndex = getRandomPieceIndex();
        score = 0;
        linesCleared = 0;
        isGameOver = false;

        backgroundMusic.play();
    }

    // For random blocks
    int getRandomPieceIndex() {
        return rand() % SHAPES.size(); // random number between 0 and number of blocks
    }

    // Function that checks for functions
    bool isCollision(const std::vector<std::vector<int>>& shape, int x, int y) {
        for (int i = 0; i < shape.size(); i++) {
            for (int j = 0; j < shape[i].size(); j++) {
                // If the pieces surrounding the piece are empty and it is not on an edge, return true
                if (shape[i][j] != 0 && (x + j < 0 || x + j >= BOARD_WIDTH || y + i >= BOARD_HEIGHT || board[y + i][x + j] != 0)) {
                    return true;
                }
            }
        }
        //else false
        return false;
    }

    // If a piece reached the bottom, permanently place it.
    void placePiece(const std::vector<std::vector<int>>& shape, int x, int y) {
        for (int i = 0; i < shape.size(); i++) {
            for (int j = 0; j < shape[i].size(); j++) {
                if (shape[i][j] != 0) {
                    board[y + i][x + j] = shape[i][j];
                }
            }
        }
    }

    // If a row if full clear it
    bool removeFullRows() {
        bool rowRemoved = false;
        for (int i = BOARD_HEIGHT - 1; i >= 0; i--) {
            bool isFullRow = true;
            for (int j = 0; j < BOARD_WIDTH; j++) {
                if (board[i][j] == 0) {
                    isFullRow = false;
                    break;
                }
            }
            if (isFullRow) {
                board.erase(board.begin() + i);
                board.insert(board.begin(), std::vector<int>(BOARD_WIDTH, 0));
                score += 10;
                linesCleared++;
                rowRemoved = true;
            }
        }
        return rowRemoved;
    }

    // Process SFML events like keypresses or hitting the x on the app
    void processEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                } else if (event.key.code == sf::Keyboard::Left) {
                    if (!isCollision(SHAPES[currentPieceIndex][currentPieceRotation], currentPieceX - 1, currentPieceY)) {
                        --currentPieceX;
                    }
                } else if (event.key.code == sf::Keyboard::Right) {
                    if (!isCollision(SHAPES[currentPieceIndex][currentPieceRotation], currentPieceX + 1, currentPieceY)) {
                        ++currentPieceX;
                    }
                } else if (event.key.code == sf::Keyboard::Down) {
                    if (!isCollision(SHAPES[currentPieceIndex][currentPieceRotation], currentPieceX, currentPieceY + 1)) {
                        ++currentPieceY;
                    }
                } else if (event.key.code == sf::Keyboard::Space) {
                    while (!isCollision(SHAPES[currentPieceIndex][currentPieceRotation], currentPieceX, currentPieceY + 1)) {
                        ++currentPieceY;
                    }
                } else if (event.key.code == sf::Keyboard::Up) {
                    int nextRotation = (currentPieceRotation + 1) % SHAPES[currentPieceIndex].size();
                    if (!isCollision(SHAPES[currentPieceIndex][nextRotation], currentPieceX, currentPieceY)) {
                        currentPieceRotation = nextRotation;
                    }
                }
                else if (event.key.code == sf::Keyboard::R) {
                    resetGame();
                }
            }
        }
    }

    // Update the Position of the pieces and check for game over
    void update() {
        if (!isGameOver) {
            if (!isCollision(SHAPES[currentPieceIndex][currentPieceRotation], currentPieceX, currentPieceY + 1)) {
                ++currentPieceY;
            } else {
                placePiece(SHAPES[currentPieceIndex][currentPieceRotation], currentPieceX, currentPieceY);
                if (isCollision(SHAPES[nextPieceIndex][0], BOARD_WIDTH / 2 - 1, 0)) {
                    isGameOver = true;
                    backgroundMusic.stop();
                } else {
                    currentPieceIndex = nextPieceIndex;
                    currentPieceRotation = 0;
                    currentPieceX = BOARD_WIDTH / 2 - 1;
                    currentPieceY = 0;
                    nextPieceIndex = getRandomPieceIndex();
                }
            }
            removeFullRows();
        }
    }

    //Draw all the pieces and text
    void render() {
        // Clear the window
    window.clear(sf::Color::Black);

    // Draw the background
    sf::RectangleShape background(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    background.setFillColor(sf::Color(64, 64, 64)); // Dark gray color
    window.draw(background);

    // Calculate the dimensions of the play area
    float playAreaWidth = BLOCK_SIZE * 10 - 6;
    float playAreaHeight = BLOCK_SIZE * 20 - 6;

    // Draw the play area with a darker gray color and gold border
    sf::RectangleShape playArea(sf::Vector2f(playAreaWidth, playAreaHeight));
    playArea.setPosition(3.0f, 3.0f);
    playArea.setFillColor(sf::Color(96, 96, 96)); // Darker gray color
    playArea.setOutlineThickness(3.0f);
    playArea.setOutlineColor(sf::Color(255,215,0) /* Gold */);
    window.draw(playArea);

    sf::RectangleShape block(sf::Vector2f(BLOCK_SIZE, BLOCK_SIZE));
    block.setOutlineThickness(1.f);

    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (board[i][j] != 0) {
                block.setPosition(j * BLOCK_SIZE, i * BLOCK_SIZE);
                block.setFillColor(PIECE_COLORS[board[i][j] - 1]); // Assign color based on the piece type
                window.draw(block);
            }
        }
    }

    for (int i = 0; i < SHAPES[currentPieceIndex][currentPieceRotation].size(); i++) {
        for (int j = 0; j < SHAPES[currentPieceIndex][currentPieceRotation][i].size(); j++) {
            if (SHAPES[currentPieceIndex][currentPieceRotation][i][j] != 0) {
                block.setPosition((currentPieceX + j) * BLOCK_SIZE, (currentPieceY + i) * BLOCK_SIZE);
                block.setFillColor(PIECE_COLORS[currentPieceIndex]); // Assign color based on the current piece type
                window.draw(block);
            }
        }
    }

    for (int i = 0; i < SHAPES[nextPieceIndex][0].size(); i++) {
        for (int j = 0; j < SHAPES[nextPieceIndex][0][i].size(); j++) {
            if (SHAPES[nextPieceIndex][0][i][j] != 0) {
                block.setPosition((BOARD_WIDTH + 2 + j) * BLOCK_SIZE, (6 + i) * BLOCK_SIZE);
                block.setFillColor(PIECE_COLORS[nextPieceIndex]); // Assign color based on the next piece type
                window.draw(block);
            }
        }
    }

    scoreText.setString("Score: " + std::to_string(score));
    linesClearedText.setString("Lines Cleared: " + std::to_string(linesCleared));
    window.draw(scoreText);
    window.draw(linesClearedText);
    window.draw(nextPieceText);

    if (isGameOver) {
        window.draw(gameOverText);
    } else {
        window.draw(titleText);
    }

    window.display();
    }
};

int main() {
    srand(static_cast<unsigned>(time(0)));

    TetrisGame game;
    game.run();

    return 0;
}
