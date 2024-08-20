// PROGRAMMING FUNDAMENTAL'S PROJECT FOR FALL 2022 BS(CS)

#include "utils.h"

int* shadow_block() {
    // Array to hold the maximum Y positions for each column
    static int max_y[10];

    // Initialize max_y with the maximum possible row (M - 1)
    for (int i = 0; i < 10; i++) {
        max_y[i] = M - 1;
    }

    // Iterate over the columns
    for (int x = 0; x < 10; x++) {
        // Start checking from the bottom row
        for (int y = M - 1; y >= 0; y--) {
            if (gameGrid[y][x]) {
                max_y[x] = y - 1; // Set max_y to the row above the occupied cell
                break;            // Break once the first occupied cell is found
            }
        }
    }

    return max_y;
}

int min_drop_value() {
    int *max_y = shadow_block();
    int minDrop = 20; // Start with a value higher than the possible grid height

    // Find the maximum drop distance for the entire piece
    for (int i = 0; i < 4; i++) {
        int x = point_1[i][0];
        int y = point_1[i][1];

        // Calculate the potential drop distance for the current block
        int drop = max_y[x] - y;

        // Ensure drop distance does not cause the piece to go below the grid
        if (drop < minDrop) {
            minDrop = drop;
        }

        // Check how far the piece can drop
        while (gameGrid[y + minDrop][x] != 0 || gameGrid[y + minDrop - 1][x] != 0) {
            minDrop--;
        }
    }
    return minDrop;
}

void drawing_blocks(RenderWindow &window, Sprite &tile, Sprite &shadow, Sprite &bomb, int &colorNum, int &bomb_color, bool &bomb_fall) {
    // Draw the pieces on the grid
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            if (gameGrid[i][j] == 0)
                continue;
            tile.setTextureRect(IntRect(gameGrid[i][j] * 18, 0, 18, 18));
            tile.setPosition(j * 18, i * 18);
            tile.move(28, 31); //offset
            window.draw(tile);
        }
    }
    for (int i = 0; i < 4; i++) {
        tile.setTextureRect(IntRect(colorNum * 18, 0, 18, 18));
        tile.setPosition(point_1[i][0] * 18, point_1[i][1] * 18);
        tile.move(28, 31);
        window.draw(tile);
    }

    // Draw the shadow piece
    int minDrop = min_drop_value();
    // Draw the shadow piece at the calculated drop distance
    for (int i = 0; i < 4; i++) {
        int x = point_1[i][0];
        int y = point_1[i][1];

        // Calculate the shadow position for the entire piece
        int shadowY = y + minDrop;

        shadow.setTextureRect(IntRect(colorNum * 18, 0, 18, 18));
        shadow.setPosition(x * 18, shadowY * 18);
        shadow.move(28, 31);
        window.draw(shadow);
    }

    // Draw the bomb piece
    if (bomb_fall) {
        bomb.setTextureRect(IntRect(bomb_color * 18, 0, 18, 18));
        bomb.setPosition(bomb_point[1] * 18, bomb_point[0] * 18);
        bomb.move(28, 31);
        window.draw(bomb);
    }
}

void start_new_game(RenderWindow &window, GameData &gameData) {
    // Resetting game grid
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            gameGrid[i][j] = 0;
        }
    }
    // Resetting coordinates
    gameData.n = rand() % 7;
    gameData.color_num = rand() % 7 + 1;
    for (int i = 0; i < 4; i++) {
        point_1[i][0] = BLOCKS[gameData.n][i] % 2;
        point_1[i][1] = BLOCKS[gameData.n][i] / 2;
    }
    // Resetting bomb coordinates
    bomb_point[0] = 0;
    bomb_point[1] = 0;
    // Resetting all variables
    gameData.delta_x = 0, gameData.color_num = rand() % 7 + 1, gameData.score = 0, gameData.lines_cleared = 0, gameData.bomb_color = rand() % 7;
    gameData.timer = 0, gameData.prime_delay = 0.3, gameData.current_delay = 0.3, gameData.time = 0, gameData.bomb_timer = 10, gameData.bomb_countdown = 10;
    gameData.rotate = 0, gameData.fall = true, gameData.is_paused = true, gameData.line_cleared = false, gameData.bomb_fall = false;
}

bool display_high_scores(RenderWindow &window, Sprite &background, Font &font) {
    Text title;

    // Get the window size
    Vector2u windowSize = window.getSize();

    title.setFont(font);
    title.setStyle(Text::Bold);
    title.setFillColor(Color(38, 56, 88));
    title.setString("High Scores");
    title.setCharacterSize(40);
    FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.left + titleBounds.width / 2.0f, titleBounds.top + titleBounds.height / 2.0f);
    title.setPosition(windowSize.x / 2.0f, 60);

    // Arrays to store scores
    const int maxScores = 100;
    std::string names[maxScores];
    int scores[maxScores];
    int scoreCount = 0;

    // Read scores from scores.txt
    std::ifstream file("scores.txt");
    if (file.is_open()) {
        std::string name;
        int score;
        while (file >> name >> score && scoreCount < maxScores) {
            names[scoreCount] = name;
            scores[scoreCount] = score;
            scoreCount++;
        }
        file.close();
    } else {
        // The file cannot be opened
        return false;
    }

    // Sort scores in descending order using bubble sort algorithm
    /* Other sorting algorithms can be used for better performance
       but since the number of scores is small, bubble sort is sufficient */
    for (int i = 0; i < scoreCount - 1; ++i) {
        for (int j = 0; j < scoreCount - i - 1; ++j) {
            if (scores[j] < scores[j + 1]) {
                swap(scores[j], scores[j + 1]);
                swap(names[j], names[j + 1]);
            }
        }
    }

    Text exit;
    exit.setFont(font);
    exit.setStyle(Text::Bold);
    exit.setString("ESC to Exit");
    exit.setCharacterSize(30);
    FloatRect exitBounds = exit.getLocalBounds();
    exit.setOrigin(exitBounds.left + exitBounds.width / 2.0f, exitBounds.top + exitBounds.height / 2.0f);
    exit.setPosition(window.getSize().x / 2.0f, window.getSize().y - 60);

    while (window.isOpen()) {
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) {
                return false;
            }
            if (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape) {
                return true;
            }
        }

        window.clear(Color::Black);
        window.draw(background);
        window.draw(title);

        // Displaying each score
        Text text;
        text.setFont(font);
        text.setFillColor(Color::White);
        text.setCharacterSize(20);

        float y = 100.0f;
        for (int i = 0; i < (scoreCount < 10 ? scoreCount : 10); ++i) {
            std::string scoreString = names[i] + " " + std::to_string(scores[i]);
            text.setString(scoreString);
            FloatRect textBounds = text.getLocalBounds();
            text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
            text.setPosition(window.getSize().x / 2.0f, y);
            window.draw(text);
            y += 30; // Move down for the next score
        }

        window.draw(exit);
        window.display();
    }
}

bool display_help(RenderWindow &window, Sprite &background, Font &font) {
    Text title;

    // Get the window size
    Vector2u windowSize = window.getSize();

    title.setFont(font);
    title.setStyle(Text::Bold);
    title.setFillColor(Color(38, 56, 88));
    title.setString("Help");
    title.setCharacterSize(40);
    FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.left + titleBounds.width / 2.0f, titleBounds.top + titleBounds.height / 2.0f);
    title.setPosition(windowSize.x / 2.0f, 60);

    // Create the text
    Text text;
    text.setFont(font);
    text.setStyle(Text::Bold);
    text.setFillColor(Color::White);
    text.setCharacterSize(20);
    text.setString( " Arrow keys move the pieces  \n\n"
                    "   Spacebar drops the pieces\n\n"
                    "        'P' pauses the game  \n\n"
                    "       'ESC' exits the game     ");
    FloatRect textBounds = text.getLocalBounds();
    text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
    text.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f);

    // Create the exit button
    Text exit;
    exit.setFont(font);
    exit.setStyle(Text::Bold);
    exit.setString("ESC to Exit");
    exit.setCharacterSize(30);
    FloatRect exitBounds = exit.getLocalBounds();
    exit.setOrigin(exitBounds.left + exitBounds.width / 2.0f, exitBounds.top + exitBounds.height / 2.0f);
    exit.setPosition(window.getSize().x / 2.0f, window.getSize().y - 60);

    while (window.isOpen()) {
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) {
                return false;
            }
            if (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape) {
                return true;
            }
        }

        window.clear(Color::Black);
        window.draw(background);
        window.draw(title);
        window.draw(text);
        window.draw(exit);
        window.display();
    }
}

void display_main_menu(RenderWindow &window, GameData &gameData) {
    // Menu items
    Text title, start, highScores, help, exit;

    // Get the window size
    Vector2u windowSize = window.getSize();

    title.setFont(gameData.font);
    title.setStyle(Text::Bold);
    title.setFillColor(Color(38, 56, 88));
    title.setString("Tetris");
    title.setCharacterSize(50);
    FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.left + titleBounds.width / 2.0f, titleBounds.top + titleBounds.height / 2.0f);
    title.setPosition(windowSize.x / 2.0f, 60);

    start.setFont(gameData.font);
    start.setStyle(Text::Bold);
    start.setString("Start Game");
    start.setCharacterSize(30);
    FloatRect startBounds = start.getLocalBounds();
    start.setOrigin(startBounds.left + startBounds.width / 2.0f, startBounds.top + startBounds.height / 2.0f);
    start.setPosition(windowSize.x / 2.0f, 160);

    highScores.setFont(gameData.font);
    highScores.setStyle(Text::Bold);
    highScores.setString("High Scores");
    highScores.setCharacterSize(30);
    FloatRect highScoresBounds = highScores.getLocalBounds();
    highScores.setOrigin(highScoresBounds.left + highScoresBounds.width / 2.0f, highScoresBounds.top + highScoresBounds.height / 2.0f);
    highScores.setPosition(windowSize.x / 2.0f, 220);

    help.setFont(gameData.font);
    help.setStyle(Text::Bold);
    help.setString("Help");
    help.setCharacterSize(30);
    FloatRect helpBounds = help.getLocalBounds();
    help.setOrigin(helpBounds.left + helpBounds.width / 2.0f, helpBounds.top + helpBounds.height / 2.0f);
    help.setPosition(windowSize.x / 2.0f, 280);

    exit.setFont(gameData.font);
    exit.setStyle(Text::Bold);
    exit.setString("Exit");
    exit.setCharacterSize(30);
    FloatRect exitBounds = exit.getLocalBounds();
    exit.setOrigin(exitBounds.left + exitBounds.width / 2.0f, exitBounds.top + exitBounds.height / 2.0f);
    exit.setPosition(windowSize.x / 2.0f, 340);

    int option = 1;
    bool flagMenu = false;

    while (window.isOpen()) {
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed)
                window.close();

            if (e.type == Event::KeyPressed) {
                switch (e.key.code) {
                    case Keyboard::Down:
                        option++;
                        if (option == 5) option = 1;
                        flagMenu = false;
                        break;
                    case Keyboard::Up:
                        option--;
                        if (option == 0) option = 4;
                        flagMenu = false;
                        break;
                    case Keyboard::Enter:
                        flagMenu = true;
                        break;
                    default:
                        break;
                }
            }
        }

        if (flagMenu) {
            switch(option) {
                case 1:
                    // Start a new game
                    start_new_game(window, gameData);
                    return;
                case 2:
                    // See high scores
                    if (!display_high_scores(window, gameData.background, gameData.font)) window.close();
                    break;
                case 3:
                    // Help
                    if (!display_help(window, gameData.background, gameData.font)) window.close();
                    break;
                case 4:
                    // Exit
                    window.close();
                    return;
                default:
                    break;
            }
            flagMenu = false;
        }

        // Reset all menu items color to white
        start.setFillColor(Color::White);
        highScores.setFillColor(Color::White);
        help.setFillColor(Color::White);
        exit.setFillColor(Color::White);

        // Highlight the selected option
        switch (option) {
            case 1:
                start.setFillColor(Color(180, 20, 20));
                break;
            case 2:
                highScores.setFillColor(Color(180, 20, 20));
                break;
            case 3:
                help.setFillColor(Color(180, 20, 20));
                break;
            case 4:
                exit.setFillColor(Color(180, 20, 20));
                break;
        }

        window.clear();
        window.draw(gameData.background);
        window.draw(title);
        window.draw(start);
        window.draw(highScores);
        window.draw(help);
        window.draw(exit);
        window.display();
    }
}

char display_pause_menu(RenderWindow &window, GameData &gameData) {
    // Create semi-transparent background
    RectangleShape overlay(Vector2f(window.getSize().x, window.getSize().y));
    overlay.setFillColor(Color(0, 0, 0, 150)); // Black with 150 alpha for semi-transparency

    // Pause menu items
    Text pauseText, resume, mainMenu;

    pauseText.setFont(gameData.font);
    pauseText.setStyle(Text::Bold);
    pauseText.setFillColor(Color::White);
    pauseText.setString("Paused");
    pauseText.setCharacterSize(50);
    FloatRect pauseTextBounds = pauseText.getLocalBounds();
    pauseText.setOrigin(pauseTextBounds.left + pauseTextBounds.width / 2.0f, pauseTextBounds.top + pauseTextBounds.height / 2.0f);
    pauseText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f - 100);

    resume.setFont(gameData.font);
    resume.setStyle(Text::Bold);
    resume.setFillColor(Color::White);
    resume.setString("Resume");
    resume.setCharacterSize(30);
    FloatRect resumeBounds = resume.getLocalBounds();
    resume.setOrigin(resumeBounds.left + resumeBounds.width / 2.0f, resumeBounds.top + resumeBounds.height / 2.0f);
    resume.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);

    mainMenu.setFont(gameData.font);
    mainMenu.setStyle(Text::Bold);
    mainMenu.setFillColor(Color::White);
    mainMenu.setString("Main Menu");
    mainMenu.setCharacterSize(30);
    FloatRect mainMenuBounds = mainMenu.getLocalBounds();
    mainMenu.setOrigin(mainMenuBounds.left + mainMenuBounds.width / 2.0f, mainMenuBounds.top + mainMenuBounds.height / 2.0f);
    mainMenu.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f + 60);

    int option = 1;

    while (window.isOpen()) {
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) {
                window.close();
                return 'E';
            }
            if (e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::Down) {
                    option++;
                    if (option == 3) option = 1;
                }
                if (e.key.code == Keyboard::Up) {
                    option--;
                    if (option == 0) option = 2;
                }
                if (e.key.code == Keyboard::Enter) {
                    if (option == 1) {
                        return 'R';
                    } else if (option == 2) {
                        return 'M';
                    }
                }
            }
        }

        // Reset all menu items color to white
        resume.setFillColor(Color::White);
        mainMenu.setFillColor(Color::White);

        // Highlight the selected option
        if (option == 1) {
            resume.setFillColor(Color(180, 20, 20));
        } else if (option == 2) {
            mainMenu.setFillColor(Color(180, 20, 20));
        }

        window.clear();
        window.draw(gameData.background);
        window.draw(gameData.frame);
        drawing_blocks(window, gameData.tile, gameData.shadow, gameData.bomb, gameData.color_num, gameData.bomb_color, gameData.bomb_fall);
        window.draw(overlay);
        window.draw(pauseText);
        window.draw(resume);
        window.draw(mainMenu);
        window.display();
    }
}

string display_gameover(RenderWindow &window, GameData &gameData) {
    // Create semi-transparent background
    RectangleShape overlay(Vector2f(window.getSize().x, window.getSize().y));
    overlay.setFillColor(Color(0, 0, 0, 150)); // Black with 150 alpha for semi-transparency

    Text gameoverText, enter_name, enter;
    gameoverText.setFont(gameData.font);
    gameoverText.setStyle(Text::Bold);
    gameoverText.setFillColor(Color::White);
    gameoverText.setString("Game Over");
    gameoverText.setCharacterSize(50);
    FloatRect gameoverTextBounds = gameoverText.getLocalBounds();
    gameoverText.setOrigin(gameoverTextBounds.left + gameoverTextBounds.width / 2.0f,
                           gameoverTextBounds.top + gameoverTextBounds.height / 2.0f);
    gameoverText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f - 100);

    enter_name.setFont(gameData.font);
    enter_name.setStyle(Text::Bold);
    enter_name.setFillColor(Color::White);
    enter_name.setString("Enter your name:");
    enter_name.setCharacterSize(25);
    FloatRect enter_nameBounds = enter_name.getLocalBounds();
    enter_name.setOrigin(enter_nameBounds.left + enter_nameBounds.width / 2.0f,
                         enter_nameBounds.top + enter_nameBounds.height / 2.0f);
    enter_name.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);

    enter.setFont(gameData.font);
    enter.setStyle(Text::Bold);
    enter.setFillColor(Color::White);
    enter.setString("");
    enter.setCharacterSize(30);
    FloatRect enterBounds = enter.getLocalBounds();
    enter.setOrigin(enterBounds.left + enterBounds.width / 2.0f, enterBounds.top + enterBounds.height / 2.0f);
    enter.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f + 50);

    string name = "";
    bool flag = false;

    while (window.isOpen()) {
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) {
                window.close();
                return "";
            }
            if (e.type == Event::TextEntered) {
                if (e.text.unicode == 13) {
                    flag = true;
                } else if (e.text.unicode == 8) {
                    if (name.size() > 0) {
                        name.pop_back();
                    }
                } else if (e.text.unicode < 128) {
                    name += static_cast<char>(e.text.unicode);
                }
            } else if (e.type == Event::KeyPressed && e.key.code == Keyboard::Enter) {
                flag = true;
            }
        }

        if (flag) {
            return name;
        }

        enter.setString(name);

        // Center the text
        enterBounds = enter.getLocalBounds();
        enter.setOrigin(enterBounds.left + enterBounds.width / 2.0f, enterBounds.top + enterBounds.height / 2.0f);
        enter.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f + 50);


        window.clear();
        window.draw(gameData.background);
        window.draw(gameData.frame);
        drawing_blocks(window, gameData.tile, gameData.shadow, gameData.bomb, gameData.color_num, gameData.bomb_color, gameData.bomb_fall);
        window.draw(overlay);
        window.draw(gameoverText);
        window.draw(enter_name);
        window.draw(enter);
        window.display();
    }

    return "";
}

void falling_piece(float &timer, float &delay, int &colorNum, int n) {
    if (timer > delay) {
        for (int i = 0; i < 4; i++) {
            point_2[i][0] = point_1[i][0];
            point_2[i][1] = point_1[i][1];
            point_1[i][1] += 1;
        }
        if (!anomaly()) {
            for (auto &i: point_2) {
                gameGrid[i[1]][i[0]] = colorNum;
            }
            colorNum = rand() % 7 + 1; // Random color
            // Coordinates of according to the shape
            for (int i = 0; i < 4; i++) {
                point_1[i][0] = BLOCKS[n][i] % 2;
                point_1[i][1] = BLOCKS[n][i] / 2;
            }
        }
        timer = 0;
    }
}

void falling_bomb(float &timer, float &delay, int &bomb_color, bool &bomb_fall, float &bomb_countdown) {
    if (timer > delay) {
        bomb_point[0] += 1;
        if (bomb_point[0] >= M || gameGrid[bomb_point[0]][bomb_point[1]]) {
            if(gameGrid[bomb_point[0]][bomb_point[1]] == bomb_color){
                for(int i = 0; i < M; i++){
                    for(int j = 0; j < N; j++) {
                        gameGrid[i][j] = 0;
                    }
                }
            } else{
                gameGrid[bomb_point[0]][bomb_point[1]] = 0;
                gameGrid[(bomb_point[0] + 1 < M) ? bomb_point[0] + 1 : bomb_point[0] - 1][bomb_point[1]] = 0;
                gameGrid[(bomb_point[0] + 1 < M) ? bomb_point[0] + 1 : bomb_point[0] - 1][(bomb_point[1] + 1 < N) ? bomb_point[1] + 1 : bomb_point[1] - 1] = 0;
                gameGrid[(bomb_point[0] + 2 < M) ? bomb_point[0] + 2 : bomb_point[0] - 2][bomb_point[1]] = 0;
                gameGrid[(bomb_point[0] + 2 < M) ? bomb_point[0] + 2 : bomb_point[0] - 2][(bomb_point[1] + 1 < N) ? bomb_point[1] + 1 : bomb_point[1] - 1] = 0;
            }
            bomb_point[0] = 0;
            bomb_point[1] = rand() % N;
            bomb_color = rand() % 7;
            bomb_fall = false;
            bomb_countdown = 7 + rand() % 7;
        }
        timer = 0;
    }
}

/////////////////////////////
void moving_right() {
    bool canMove = true;
    for (auto &i : point_1) {
        int x = i[0];
        int y = i[1];
        if (x >= 9 || gameGrid[y][x + 1]) {  // Check right boundary and collision
            canMove = false;
            break;
        }
    }
    if (canMove) {
        for (auto &i : point_1) {
            i[0] += 1;
        }
    }
}

void moving_left() {
    bool canMove = true;
    for (auto &i : point_1) {
        int x = i[0];
        int y = i[1];
        if (x <= 0 || gameGrid[y][x - 1]) {  // Check left boundary and collision
            canMove = false;
            break;
        }
    }
    if (canMove) {
        for (auto &i : point_1) {
            i[0] -= 1;
        }
    }
}

void rotating() {
    int centerX = point_1[1][0];
    int centerY = point_1[1][1];
    bool check = true;
    //CHECKS IF SHAPE GOES OUTSIDE GRID
    for (auto &i: point_1) {
        if ((i[1] - centerY + centerX < 0) || (i[1] - centerY + centerX > 9)) {
            check = false;
        }
        if ((centerX - point_1[0][0] + centerY < 0) || (centerX - point_1[3][0] + centerY > 19)) {
            check = false;
        }
    }
    //ROTATION
    if (check) {
        for (auto &i: point_1) {
            int tempx = i[0];
            int tempy = i[1];
            i[0] = tempy - centerY + centerX;
            i[1] = centerX - tempx + centerY;
        }
    }
}

bool clear_line(int &score, int &tLines) {
    int a = 19, line = 0;
    for (int i = a; i > 0; i--) {
        int count = 0;
        for (int j = 0; j < 10; j++) {
            if (gameGrid[i][j]) {
                count++;
            }
            gameGrid[a][j] = gameGrid[i][j]; //Clearing the line
        }
        if (count == 10) {//ALL X-COORDINATES != 0
            line++;
        }
        if (count < 10) {
            a--;
        }
    }
    // Score calculation
    if (line == 1) {
        score += 10;
        tLines += 1;
        return true;
    } else if (line == 2) {
        score += 30;
        tLines += 2;
        return true;
    } else if (line == 3) {
        score += 60;
        tLines += 3;
        return true;
    } else if (line == 4) {
        score += 100;
        tLines += 4;
        return true;
    }
    return false;
}

void file_handling(string name, int &score) {
    // Writing the score to the file
    fstream file("scores.txt", ios::app);
    file << name << " " << score << endl;

    file.close();

    // Reading the scores from the file
    int size = 0;
    ifstream infile1("scores.txt");
    while (infile1 >> name >> score) {
        size++;
    }
    infile1.close();

    int score1[size];
    string name1[size];

    ifstream infile("scores.txt");
    int index = 0;
    while (infile >> name1[index] >> score1[index]) {
        infile >> name >> score;
        index++;
    }
    infile.close();


}

void spacebar(int &colorNum) {
    int minDrop = min_drop_value();
    // Draw the shadow piece at the calculated drop distance
    for (int i = 0; i < 4; i++) {
        int x = point_1[i][0];
        int y = point_1[i][1];

        // Calculate the shadow position for the entire piece
        int shadowY = y + minDrop;

        // Draw the shadow block at the calculated position
        gameGrid[shadowY][x] = colorNum;
        point_1[i][1] = shadowY;
    }

}

void event_handling(RenderWindow &window, GameData &gameData) {
    Event e;
    while (window.pollEvent(e)) {                       //Event is occurring - until the game is in running state
        if (e.type == Event::Closed) {                      //If cross/close is clicked/pressed
            window.close();                                         //Opened window disposes
        } else if (e.type == Event::KeyPressed) {           //If any other key (not cross) is pressed
            if (e.key.code == Keyboard::Up) {                   //Check if the other key pressed is UP key
                gameData.rotate = true;                                      //Rotation gets on
            } else if (e.key.code == Keyboard::Left) {          //Check if the other key pressed is LEFT key
                gameData.delta_x = -1;                                       //Change in X-Axis - Negative
                moving_left();
            } else if (e.key.code == Keyboard::Right) {         //Check if the other key pressed is RIGHT key
                gameData.delta_x = 1;                                        //Change in X-Axis - Positive
                moving_right();
            } else if (e.key.code == Keyboard::Space) {         //Check if the other key pressed is RIGHT key
                spacebar(gameData.color_num);
            } else if (e.key.code == Keyboard::Down) {          //Check if the other key pressed is DOWN key
                gameData.current_delay = 0.05;                               //Speed of the falling piece increases
            } else if (e.key.code == Keyboard::Escape) {        //Check if the other key pressed is ESCAPE key
                window.close();                                     //Opened window disposes
            } else if (e.key.code == Keyboard::P) {             //Check if the other key pressed is P key
                char result = display_pause_menu(window, gameData);
                if (result == 'P') {
                    gameData.is_paused = false;
                } else if (result == 'M') {
                    display_main_menu(window, gameData);
                }
            }
        }
    }
}

void drawing_score(RenderWindow &window, int &score, Font &font) {
    // Score text
    ostringstream score_text;
    score_text << score;
    string scoreF = score_text.str();
    font.loadFromFile("GameResources/Skia.otf");
    Text text;
    text.setFont(font);
    text.setStyle(Text::Bold);
    text.setFillColor(Color::White);
    text.setCharacterSize(25);
    text.setString(scoreF);
    text.move(240.f, 210.f);
    window.draw(text);
    Text text2;
    text2.setFont(font);
    text2.setStyle(Text::Bold);
    text2.setFillColor(Color::White);
    text2.setCharacterSize(20);
    text2.setString("SCORE");
    text2.move(230.f, 180.f);
    window.draw(text2);
}


