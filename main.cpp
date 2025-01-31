/*****************************************************************************
 *                             T e ⊥ r i s
 *  **************************************************************************
 * sept 24 - OE3KUW / tetris24
 * oct 24  - drawSquare colors added           (Samuel Kirakosyan)
 * nov 16  - drawLine colors added             (Bernhard Lintner) :)
 * nov 16  - drawStageFrame added              (Reinhard Daxböck)
 * nov 16  - drawText now with colors          (Paul Kronawitter)
 * nov 16  - generate figures started
 * nov 30  - first figures are falling
 * dec 11  - merge first change requests (Feichtinger, Iusufi,
 *           Hintermeier, Kerschbaumer, Landrichter, Haindl, Moosbauer)
 * jan 10  - over next figure window
 * jan 29  - store nick name and highscore - management
 *****************************************************************************
 * install:
 * 1) get clone tetris24 store everything in your own directory
 * for e.g. prepare a c:/TechnischeInformatik
 * change to this directory and write there:
 * git clone https://github.com/OE3KUW/tetris
 * if you prefer zip / unzip -  use that instead.
 * you should now have a main.cpp and a CMakeLists.txt in your dir
 * 2) download https://www.sfml-dev.org/download.php SFML 2.6.2
 * no higher versions!!!
 * extract it, store it: for e.g. in C:/TechnischeInformatik/Cpp/SFML
 * if you like, you can use any other (self prepared) directory
 * be only sure: the path is stored then in CMakeLists -> Step 3
 * 3) open CMakeLists.txt find there the line:
 * set(SFML_DIR "C://TechnischeInformatik/CPP/SFML/lib/cmake/SFML")
 * be sure - this is exactly the path, where you have stored your smfl-files
 * 4) compile it - this creates a cmake-build-debug directory
 * in this directory your executeable file will be stored - named firstSFML.exe
 * Now copy from there SFML/bin/ all *.dll in your cmake-build-debug
 * Its also possible to set an enviroment path for this dll - files
 * 5) compile again!  :-)
 ****************************************************************************/
#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>
#include <thread> // Für std::this_thread::sleep_for
#include <chrono> // Für std::chrono::milliseconds
#include <cstring>

using namespace sf;


#define TRUE                 1
#define FALSE                0

#define ROWS                30 // 5 hidden + 25 visible
#define COLS                20
#define HIDDEN_LINES         5 // line 0,1,2,3,4 are hidden!
#define WIDTH              600
#define HEIGHT             800
#define TLX                100
#define TLY                 50
#define BRX                500
#define BRY                550


#define TLPX               560  // Top Left Preparation window X
#define TLPY               TLY
#define BRPX               700
#define BRPY               190

#define TLOX               560  // Top Left over next window X
#define TLOY               (TLY + 200)
#define BROX               700
#define BROY               (190 + 200)


#define BOX_X              ((BRX - TLX)/COLS)
#define BOX_Y              ((BRY - TLY)/(ROWS - HIDDEN_LINES))
#define BOX_X_SIZE         ((float)(BRX - TLX)/COLS)
#define BOX_Y_SIZE         ((float)(BRY - TLY)/(ROWS - HIDDEN_LINES))

#define TEXT_LENGTH         30
#define N                    7
#define FRAME_WIDTH          3 // the frame will be drawn with more than one line ...

#define TEXT_X            TLPX
#define TEXT_LEVEL        (BRY - 24)

#define MAX_FIGURE          16
#define LEFT                1
#define RIGHT               2
#define DOWN                3

#define MAX_USERNAME_LEN 20
#define MAX_LINE_LEN 100
#define HIGHSCORE_PATH "../gameData.txt"

#define HIGHSCORE_Y         (BRY - 48 - 10)
#define USERNAME_Y          (BRY - 72 - 20)

int stage [ROWS][COLS] = {{0}};
int frozen[ROWS][COLS] = {{0}};
int shadow[ROWS][COLS] = {{0}};
/**/int sPattern[N][N] = {{0}};

enum COLORS {
    BLACK = 0,
    WHITE,
    RED,
    GREEN,
    BLUE,
    YELLOW,
    MAGENTA,
    CYAN,
    TRANSPARENT
};

#define MAX_COLOR TRANSPARENT

// ****************************** globals: *********************************
/// pointer on the window
RenderWindow * w;

// ****************************** prototypes: *********************************
void drawSquare(int y, int x, int color);
void drawLine(int x1, int y1, int x2, int y2, int color); // Lintner
void drawText(char * string, int x, int y, int color); // Kranewitter
void drawStageFrame(void); // Daxböck
void drawPatternFrame(void);
void drawPattern(int p[][N]);
void drawStage(void);
void drawFrozen(void);
void drawOverNextPatternFrame(void);    //#1
void drawOverNextPattern(int onp[][N]); //#1
void drawGrid(void);  // Fabian Berger
void drawPlus(int x, int y); //helps draw Grid // Fabian Berger

void setNextFigure(int p[][N], int onp[][N]);
void setNewCenterPoint(int *y, int *x);

void exchange2Boxes(int p[][N],int y1, int x1, int y2, int x2);
void exchangePatternBoxes(int p[][N],
                          int y1, int x1, int y2, int x2,
                          int y3, int x3, int y4, int x4,
                          int y5, int x5, int y6, int x6);
void rotatePattern(int p[][N]);
void storePattern(int p[][N]);
void reStorePattern(int p[][N]);
int rotationIsNotPossible(int cx, int cy, int s[][COLS], int f[][COLS], int p[][N]);

int isMovePossibleD(int cx, int cy, int s[][COLS], int f[][COLS], int p[][N]);

int isMovePossible(int cx, int cy, int dir, int s[][COLS], int f[][COLS], int p[][N]);
void move(int cx, int cy, int s[][COLS], int p[][N]);
void freeze(int cx, int cy, int s[][COLS], int f[][COLS], int p[][N]);
void eliminateCompletedLine(int f[ROWS][COLS],int *eliminated_lines);
void getUserData(char username[MAX_USERNAME_LEN], int *userLine, int *entireLines, int *userHighscore);
void updateHighscore(char username[MAX_USERNAME_LEN], int userLine, int entireLines, int userHighscore, int count);


// ******************************   m a i n:  *********************************
int main()
{
    char username[MAX_USERNAME_LEN];
    int userLine = 0;
    int entireLines = 0;
    int userHighscore = 0;


    int x, y, color, tick = 0;
    char text[TEXT_LENGTH];
    int count = 0;
    int pattern[N][N] = {{0}};
    int overNextPattern[N][N] = {{0}};   //#1
    int refresh = TRUE;
    int work = TRUE;
    int doItOnlyOnce = TRUE;
    int eliminated_lines = 0;
    Event event;
    int cy, cx; // center point of falling objects

//### Jan 25
    getUserData(username, &userLine, &entireLines, &userHighscore);

    // initialization phase:
    RenderWindow window(sf::VideoMode(HEIGHT, WIDTH), "TETRIS 2CHELS 2024");
    w = &window;

    // inits:
    setNextFigure(pattern, overNextPattern);
    setNextFigure(pattern, overNextPattern); // twice!

    setNewCenterPoint(&cy,&cx);
    printf("ok!\n");

    while (window.isOpen() && work)
    {
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();

            if (Keyboard::isKeyPressed(Keyboard::Space))
            {
                // diese Funktion wird jeweils zweimal angesprochen...
                if (doItOnlyOnce) {
                    storePattern(pattern); // bisherige pattern speichern
                    rotatePattern(pattern);
                    drawPattern(pattern);
                    refresh = TRUE;
                    doItOnlyOnce = FALSE;

                    if (rotationIsNotPossible(cx, cy, stage, frozen, pattern))
                    {
                        reStorePattern(pattern);
                    }
                    // falls sich die neue Figur nicht übertragen lässt -> zurück zur gespeicherten

                } else doItOnlyOnce = TRUE;

            } else if (Keyboard::isKeyPressed(Keyboard::Left))
            {
                if (isMovePossible(cx, cy, LEFT, stage, frozen, pattern))
                {
                    move(--cx, cy, stage, pattern);
                }
            } else if (Keyboard::isKeyPressed(Keyboard::Right))
            {
                if (isMovePossible(cx, cy, RIGHT, stage, frozen, pattern))
                {
                    move(++cx, cy, stage, pattern);
                }
            }
            else if (Keyboard::isKeyPressed(Keyboard::Down))
            {
                while (isMovePossibleD(cx, cy, stage, frozen, pattern))
                {
                    move(cx, ++cy, stage, pattern);
                }
            }
            else if (Keyboard::isKeyPressed(Keyboard::Enter))
            {
                count = -1;
            }
            else if (Keyboard::isKeyPressed(Keyboard::Escape))
            {
                work = FALSE;
            }
        }

        window.clear();
        drawStageFrame();
        drawPatternFrame();
        drawPattern(pattern);

        drawOverNextPatternFrame();
        drawOverNextPattern(overNextPattern);

        tick++;

        if (tick > 20)
        {
            tick = 0;
            refresh = TRUE;

            if (isMovePossibleD(cx, cy, stage, frozen, pattern))
            {
                    move(cx, ++cy, stage, pattern);
            }
            else
            {
                freeze(cx, cy, stage, frozen, pattern);

                // neues Element einbauen
                setNextFigure(pattern, overNextPattern);
                setNewCenterPoint(&cy,&cx);
                // überprüfe alle Zeilen in frozen auf Völlständigkeit
                // falls Vollständig -> Zeile löschen

                eliminateCompletedLine(frozen,&eliminated_lines); //# Kranewitter

            }
            count++;
        }

        drawStage();
        drawFrozen();
        drawGrid();
        sprintf(text, "count: %d", count);
        drawText(text, TEXT_X, TEXT_LEVEL, GREEN);
        sprintf(text, "user: %s", username);
        drawText(text, TEXT_X, USERNAME_Y, GREEN);
        sprintf(text, "highscore: %d", userHighscore);
        drawText(text, TEXT_X, HIGHSCORE_Y, GREEN);
        sprintf(text,"line score : %d",eliminated_lines);
        drawText(text, TEXT_X, USERNAME_Y - 34, GREEN);


        if (refresh) { window.display(); refresh = FALSE;}
        std::  this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    window.close();

    updateHighscore(username, userLine, entireLines, userHighscore, count);

    printf("--- created by 2AFELC 24/25 ! ---\n");

    return 0;
}


// ****************************** functions: **********************************


void drawSquare(int y, int x, int color)
//  paint square; stage position, color
{
    // if (shadow[y][x] != color) {


        RectangleShape Rect(sf::Vector2f(BOX_X_SIZE, BOX_Y_SIZE));
        switch (color) {
            case BLACK:
                Rect.setFillColor(sf::Color::Black);
                break;
            case WHITE:
                Rect.setFillColor(sf::Color::White);
                break;
            case RED:
                Rect.setFillColor(sf::Color::Red);
                break;
            case GREEN:
                Rect.setFillColor(sf::Color::Green);
                break;
            case BLUE:
                Rect.setFillColor(sf::Color::Blue);
                break;
            case YELLOW:
                Rect.setFillColor(sf::Color::Yellow);
                break;
            case MAGENTA:
                Rect.setFillColor(sf::Color::Magenta);
                break;
            case CYAN:
                Rect.setFillColor(sf::Color::Cyan);
                break;
            case TRANSPARENT:
                Rect.setFillColor(sf::Color::Transparent);
                break;
            default:
                std::cerr << "unknown color used in drawSquare" << std::endl;
        }
        Rect.setPosition((int) (x * BOX_X) + TLX, (int) ((y - HIDDEN_LINES) * BOX_Y) + TLY);
        w->draw(Rect);

       //  shadow[y][x] != color; }
}
void drawGrid(void)
{
    for (int i = 0; i < ROWS - HIDDEN_LINES; i += 1) {
        for (int j = 0; j < COLS; j++) {
            drawPlus(TLX + BOX_X * j, TLY + BOX_Y * i);
        }
    }
}


void drawPlus(const int x, const int y)
{
    //### Klasse fragen
    //    drawLine(x + BOX_X / 3, y + BOX_Y / 2, x + BOX_X - 1 - BOX_X / 3, y + BOX_Y / 2, WHITE);
//    drawLine(x + BOX_X / 2, y + BOX_Y / 3, x + BOX_X / 2, y + BOX_Y - BOX_Y / 3, WHITE);
    drawLine(x , y , x , y + BOX_Y , BLUE);
}


///  from x1,y1 to x2,y2, color
void drawLine(int x1, int y1, int x2, int y2, int color)
{
    sf::VertexArray line(sf::Lines, 2);
    // Startpunkt der Linie
    line[0].position = sf::Vector2f(x1, y1);
    //line[0].color = sf::Color::White;

    line[1].position = sf::Vector2f(x2, y2);
    //line[1].color = sf::Color::White;

    switch (color)
    {
        case BLACK:         line[0].color = sf::Color::Black;
            line[1].color = sf::Color::Black;       break;
        case WHITE:         line[0].color = sf::Color::White;
            line[1].color = sf::Color::White;       break;
        case RED:           line[0].color = sf::Color::Red;
            line[1].color = sf::Color::Red;         break;
        case GREEN:         line[0].color = sf::Color::Green;
            line[1].color = sf::Color::Green;       break;
        case BLUE:          line[0].color = sf::Color::Blue;
            line[1].color = sf::Color::Blue;        break;
        case YELLOW:        line[0].color = sf::Color::Yellow;
            line[1].color = sf::Color::Yellow;      break;
        case MAGENTA:       line[0].color = sf::Color::Magenta;
            line[1].color = sf::Color::Magenta;     break;
        case CYAN:          line[0].color = sf::Color::Cyan;
            line[1].color = sf::Color::Cyan;        break;
        case TRANSPARENT:   line[0].color = sf::Color::Transparent;
            line[1].color = sf::Color::Transparent; break;
        default: std::cerr << "this color does not exist in drawLine" << std::endl;
    }

    w->draw(line);

}
///  string x,y left top corner, color
void drawText(char * string, int x, int y, int color)
{
    sf::Font font;

    RectangleShape Rect(sf::Vector2f(150, 24 ));
    Rect.setFillColor(sf::Color::Black);
    Rect.setPosition(x, y);
    w->draw(Rect);


    if (!font.loadFromFile("c:/Windows/Fonts/arial.ttf")) {
        // Fehlerbehandlung, falls die Schriftart nicht geladen werden kann
        // ?
    }

    // Text erstellen
    sf::Text text;
    text.setFont(font); // Schriftart setzen
    text.setString(string); // Textinhalt
    text.setCharacterSize(24); // Schriftgröße in Pixeln

    // integrate here same colors as in drawSquare instead of white....

    switch (color)
    {
        case BLACK:     text.setFillColor(sf::Color::Black);    break;
        case WHITE:     text.setFillColor(sf::Color::White);    break;
        case RED:       text.setFillColor(sf::Color::Red);      break;
        case GREEN:     text.setFillColor(sf::Color::Green);    break;
        case BLUE:      text.setFillColor(sf::Color::Blue);     break;
        case YELLOW:    text.setFillColor(sf::Color::Yellow);   break;
        case MAGENTA:   text.setFillColor(sf::Color::Magenta);  break;
        case CYAN:      text.setFillColor(sf::Color::Cyan);     break;
        case TRANSPARENT:
            text.setFillColor(sf::Color::Transparent);    break;
        default: std::cerr << "unknown color used in drawText" << std::endl;
    }

    text.setStyle(sf::Text::Bold); // Stil (optional)
    text.setPosition(x, y); // x = 200, y = 150
    w->draw(text);

}

void drawStageFrame(void)
{

    for (int i = 0; i < FRAME_WIDTH; i++)
    {
        drawLine(TLX - i, TLY - i, TLX - i, BRY + i, WHITE);
        drawLine(TLX - i, BRY + i, BRX + i, BRY + i, WHITE);
        drawLine(BRX + i, TLY - i, BRX + i, BRY + i, WHITE);
        //drawLine(TLX - i, TLY - i, BRX + i, TLY - i, WHITE);
    }

}

void drawPatternFrame(void)
{
    drawLine(TLPX - 1, TLPY - 1, TLPX - 1, BRPY + 1, WHITE);
    drawLine(TLPX - 1, BRPY + 1, BRPX + 1, BRPY + 1, WHITE);
    drawLine(BRPX + 1, TLPY - 1, BRPX + 1, BRPY + 1, WHITE);
    drawLine(TLPX - 1, TLPY - 1, BRPX + 1, TLPY - 1, WHITE);
}

void drawOverNextPatternFrame(void)
{
    drawLine(TLOX - 1, TLOY - 1, TLOX - 1, BROY + 1, WHITE);
    drawLine(TLOX - 1, BROY + 1, BROX + 1, BROY + 1, WHITE);
    drawLine(BROX + 1, TLOY - 1, BROX + 1, BROY + 1, WHITE);
    drawLine(TLOX - 1, TLOY - 1, BROX + 1, TLOY - 1, WHITE);
}

void drawPattern(int p[][N])
/// pattern - array
{
    int x, y;
    RectangleShape Rect(sf::Vector2f(BOX_X_SIZE, BOX_Y_SIZE ));

    for (y = 0; y < N; y++) for (x = 0; x < N; x++)
    {
        switch (p[y][x])
        {
            case BLACK:         Rect.setFillColor(sf::Color::Black);       break;
            case WHITE:         Rect.setFillColor(sf::Color::White);       break;
            case RED:           Rect.setFillColor(sf::Color::Red);         break;
            case GREEN:         Rect.setFillColor(sf::Color::Green);       break;
            case BLUE:          Rect.setFillColor(sf::Color::Blue);        break;
            case YELLOW:        Rect.setFillColor(sf::Color::Yellow);      break;
            case MAGENTA:       Rect.setFillColor(sf::Color::Magenta);     break;
            case CYAN:          Rect.setFillColor(sf::Color::Cyan);        break;
            case TRANSPARENT:   Rect.setFillColor(sf::Color::Transparent); break;
            default: std::cerr << "unknown color used in drawSquare" << std::endl;
        }
        Rect.setPosition((x * BOX_X) + TLPX,  (y * BOX_Y) + TLPY);
        w->draw(Rect);
    }

}
void drawOverNextPattern(int onp[][N])
/// pattern - array
{
    int x, y;
    RectangleShape Rect(sf::Vector2f(BOX_X_SIZE, BOX_Y_SIZE ));

    for (y = 0; y < N; y++) for (x = 0; x < N; x++)
    {
            switch (onp[y][x]) {
                case BLACK:
                    Rect.setFillColor(sf::Color::Black);
                    break;
                case WHITE:
                    Rect.setFillColor(sf::Color::White);
                    break;
                case RED:
                    Rect.setFillColor(sf::Color::Red);
                    break;
                case GREEN:
                    Rect.setFillColor(sf::Color::Green);
                    break;
                case BLUE:
                    Rect.setFillColor(sf::Color::Blue);
                    break;
                case YELLOW:
                    Rect.setFillColor(sf::Color::Yellow);
                    break;
                case MAGENTA:
                    Rect.setFillColor(sf::Color::Magenta);
                    break;
                case CYAN:
                    Rect.setFillColor(sf::Color::Cyan);
                    break;
                case TRANSPARENT:
                    Rect.setFillColor(sf::Color::Transparent);
                    break;
                default:
                    std::cerr << "unknown color used in drawSquare" << std::endl;
            }
            Rect.setPosition((x * BOX_X) + TLOX, (y * BOX_Y) + TLOY);
            w->draw(Rect);

    }
}


void setNextFigure(int p[][N], int onp[][N])
{
    int x, y;
    int figure;

    // pattern löschen:
    //for (y = 0; (y) < N; ++y) for (x = 0; x < N; ++x) p[y][x] = 0;
    for (y = 0; (y) < N; ++y) for (x = 0; x < N; ++x) p[y][x] = onp[y][x];
    for (y = 0; (y) < N; ++y) for (x = 0; x < N; ++x) onp[y][x] = 0;

    figure = rand()% MAX_FIGURE;


    switch (figure) {

        case  0: onp[3][3] = CYAN; break;
        case  1: onp[3][3] = BLUE; onp[3][4] = BLUE; onp[4][3] = BLUE; onp[4][4] = BLUE; break;
        case  2: onp[2][2] = RED; onp[2][3] = RED; onp[2][4] = RED; onp[3][2] = RED; onp[3][3] = RED; onp[3][4] = RED; onp[4][2] = RED; onp[4][3] = RED; onp[4][4] = RED; break; // feichtinger
        case  3: onp[3][3] = GREEN; onp[2][3] = GREEN; onp[4][3] = GREEN; onp[3][4] = GREEN;  onp[3][2] = GREEN;break; //isufi
        case  4: onp[3][3] = WHITE; onp[3][2] = WHITE; break;// heindl
        case  5: onp[3][3] = MAGENTA; onp[3][2] = MAGENTA; onp[3][4] = MAGENTA; break; // bruno
        case  6: onp[3][3] = RED; onp[3][2] = RED; onp[4][3] = RED; break; //jakob
        case  7: onp[3][3] = CYAN; onp[3][2] = CYAN; onp[3][4] = CYAN; onp[4][3] = CYAN; break;
        case  8: //zeiringer
        case  9: onp[3][3] = RED; onp[3][2] = RED; onp[2][2] = RED; onp[3][4] = RED; break;// gregor Weber
        case 10: onp[3][3] = RED; onp[3][4] = RED; onp[3][1] = RED; onp[3][2] = RED; onp[4][4] = RED; break;// kerschbaumer
        case 11: onp[3][3] = YELLOW; onp[2][3] = YELLOW; onp[3][2] = YELLOW; onp[2][4] = YELLOW; break;
        case 12: onp[3][3] = YELLOW; onp[1][1] = YELLOW; onp[2][2] = YELLOW; onp[2][3] = YELLOW; break; //Arnold
        case 13: onp[1][1] = GREEN; onp[2][2] = GREEN; onp[3][3] = GREEN; break; // tobias.m
        case 14: onp[1][1] = MAGENTA; onp[2][1] = MAGENTA; onp[3][1] = MAGENTA; onp[3][2] = MAGENTA; break; // tobias.m
        case 15: onp[2][2] = BLUE; onp[2][3] = BLUE; onp[2][4] = BLUE; onp[4][2] = BLUE; onp[4][3] = BLUE; onp[4][4] = BLUE; onp[3][4] = BLUE; break;

    }

}
void rotatePattern(int p[][N])
{
    exchangePatternBoxes(p, 1,1,1,5,5,1,1,1,5,5,5,1);
    exchangePatternBoxes(p, 1,2,2,5,4,1,1,2,5,4,4,1);
    exchangePatternBoxes(p, 1,3,3,5,3,1,1,3,5,3,3,1);
    exchangePatternBoxes(p, 1,4,4,5,2,1,1,4,5,2,2,1);
    exchangePatternBoxes(p, 2,2,2,4,4,2,2,2,4,4,4,2);
    exchangePatternBoxes(p, 2,3,3,4,3,2,2,3,4,3,3,2);
}

void exchangePatternBoxes(int p[][N],
                          int y1, int x1, int y2, int x2,
                          int y3, int x3, int y4, int x4,
                          int y5, int x5, int y6, int x6)
{
    exchange2Boxes(p, y1, x1, y2, x2);
    exchange2Boxes(p, y3, x3, y4, x4);
    exchange2Boxes(p, y5, x5, y6, x6);
}

void exchange2Boxes(int p[][N],int y1, int x1, int y2, int x2)
{
    p[y1][x1] ^= p[y2][x2];
    p[y2][x2] ^= p[y1][x1];
    p[y1][x1] ^= p[y2][x2];
}

void setNewCenterPoint(int *y, int *x)
{
    *y = 2;
    *x = rand()% (COLS - 4) + 2;
    stage[*y][*x] = GREEN;
}

void drawStage(void)
{
    int y, x;

    RectangleShape Rect(sf::Vector2f((BRX - TLX), (BRY - TLY - HIDDEN_LINES) ));

    Rect.setFillColor(sf::Color::Black);
    Rect.setPosition(TLX, TLY);
    w->draw(Rect);

    for (y = 5; y < ROWS; ++y)
        for (x = 0; x < COLS; ++x)
        {
            if (stage[y][x]) drawSquare(y, x, stage[y][x]);
        }
}
void drawFrozen(void)
{
    int y, x;

    for (y = 5; y < ROWS; ++y)
        for (x = 0; x < COLS; ++x)
        {
            if (frozen[y][x]) drawSquare(y, x, frozen[y][x]);  // only frozen stones!
            //n if (frozen[y][x]) drawSquare(y, x, RED/*frozen[y][x]*/);  // only frozen stones!
        }
}


void storePattern(int p[][N])
{
    int i, j;
    for (j = 0; j < N; j++)
        for (i = 0; i < N; i++)
            sPattern[j][i] = p[j]   [i];
}

void reStorePattern(int p[][N])
{
    int i, j;
    for (j = 0; j < N; j++)
        for (i = 0; i < N; i++)
            p[j][i] = sPattern[j][i];
}

int rotationIsNotPossible(int cx, int cy, int s[][COLS], int f[][COLS], int p[][N])
{
    int ret = FALSE; //Is Not Possible ...
    int i, j;

    for (j = 0; (j < N) && (ret == FALSE); j++)
        for (i = 0; (i < N) && (ret == FALSE); i++)
        {
            if (p[j][i])
            {
                // check border
                if ((j - 3 + cy) >= ROWS)      ret = TRUE;
                else if ((j - 3 + cy) < 0)     ret = TRUE;
                else if ((i - 3 + cx) >= COLS) ret = TRUE;
                else if ((i - 3 + cx) < 0)     ret = TRUE;

                    // check frozen elements

                else if (f[j - 3 + cy][i - 3 + cx]) ret = TRUE;

            }
        }




    return ret;
}


int isMovePossibleD(int cx, int cy, int s[][COLS], int f[][COLS], int p[][N])
// check: is a move (cy++) possible?: if not: freeze it, attention check boarders
{
    int ret = TRUE, i, j;

    // damit die eigene Figur nicht sich selbst im Weg steht wird sie hier zunächst entfernt.
    // dabei gehen wir davon aus, dass sie sich auf einer erlaubten Position befindet

    for (j = 0; j < N; j++)
        for (i = 0; i < N; i++)
            if (p[j][i])
                s[cy - 3 + j][cx - 3 + i] = BLACK;

    // nun wird ein Move des Zentrums versucht: cy++

    cy++;

    // für jedes verschobene Element wird nun überprüft, ob der Platz frei ist
    // also ob kein frozen element überschrieben werden würde und
    // ob der Platz noch im Rahmen liegt.
    // sobald das auch nur ein einziges Mal nicht der Fall ist: ret = FALSE

    for (j = 0; (j < N) && (ret); j++)
        for (i = 0; (i < N) && (ret); i++)
        {
            if (p[j][i])
            {
                // check border
                if ((j - 3 + cy) >= ROWS)      ret = FALSE;
                else if ((j - 3 + cy) < 0)     ret = FALSE;
                else if ((i - 3 + cx) >= COLS) ret = FALSE;
                else if ((i - 3 + cx) < 0)     ret = FALSE;

                // check frozen elements

                else if (f[j - 3 + cy][i - 3 + cx]) ret = FALSE;

            }
        }


    // Falls FALSE:
    // Vor dem Return wird die Figur an die ursprüngliche Position wieder zurück eingetragen
    // falls TRUE - nicht mehr.

    if (ret == FALSE)
    {
        cy--; // zurück!
        for (j = 0; j < N; j++)
            for (i = 0; i < N; i++)
                if (p[j][i]) s[cy - 3 + j][cx - 3 + i] = p[j][i];
    }

    return ret;
}


void move(int cx, int cy, int s[][COLS], int p[][N])
{
    int i, j;
    for (j = 0; j < N; j++)
        for (i = 0; i < N; i++)
            s[cy - 3 + j][cx - 3 + i] = p[j][i];
}


int isMovePossible(int cx, int cy, int dir, int s[][COLS], int f[][COLS], int p[][N])
{
    int ret = TRUE, i, j;

    // damit die eigene Figur nicht sich selbst im Weg steht wird sie hier zunächst entfernt.
    // dabei gehen wir davon aus, dass sie sich auf einer erlaubten Position befindet

    for (j = 0; j < N; j++)
       for (i = 0; i < N; i++)
            if (p[j][i])
                s[cy - 3 + j][cx - 3 + i] = BLACK;

    // nun wird ein Move des Zentrums nach links ! versucht:

    switch (dir) {
        case LEFT:   cx--; break;
        case RIGHT:  cx++; break;
        case DOWN:   cy++; break;
    }

    // für jedes verschobene Element wird nun überprüft, ob der Platz frei ist
    // also ob kein frozen element überschrieben werden würde und
    // ob der Platz noch im Rahmen liegt.
    // sobald das auch nur ein einziges Mal nicht der Fall ist: ret = FALSE

    for (j = 0; (j < N) && (ret); j++)
        for (i = 0; (i < N) && (ret); i++)
        {
            if (p[j][i])
            {
                // check border
                     if ((j - 3 + cy) >= ROWS) ret = FALSE;
                else if ((j - 3 + cy) < 0)     ret = FALSE;
                else if ((i - 3 + cx) >= COLS) ret = FALSE;
                else if ((i - 3 + cx) < 0)     ret = FALSE;


                // check frozen elements

                else if (f[j - 3 + cy][i - 3 + cx]) ret = FALSE;

            }
        }


    // Falls FALSE:
    // Vor dem Return wird die Figur an die ursprüngliche Position wieder zurück eingetragen
    // falls TRUE - nicht mehr.

    if (ret == FALSE)
    {
        cx++; // zurück!
        for (j = 0; j < N; j++)
            for (i = 0; i < N; i++)
                if (p[j][i]) s[cy - 3 + j][cx - 3 + i] = p[j][i];
    }

    return ret;
}

void freeze(int cx, int cy, int s[][COLS], int f[][COLS], int p[][N])
{
    int i, j;

    for (j = 0; j < N; j++)
        for (i = 0; i < N; i++)
            if (p[j][i])
            {
                f[j-3+cy][i-3+cx] = s[j-3+cy][i-3+cx];
                s[j-3+cy][i-3+cx] = BLACK;
            }
}

void eliminateCompletedLine(int f[ROWS][COLS],int *eliminated_lines)
{
    int eli = TRUE;

    for(int j = ROWS - 1; j >= 0; j--)
    {
        eli = TRUE;
        for(int i = 0; i < COLS; i++)
        {
            if(f[j][i] == FALSE)
            {
                i = COLS;
                eli = FALSE;
            }
        }

        if(eli == TRUE)
        {
            for(int l = j; l >= 1; l--)
            {
                for(int i = 0; i < COLS; i++)
                {
                    f[l][i] = f[l - 1][i];
                }
            }
//##
            (*eliminated_lines)++;
            j++; // maybe this line is now again! completed
        }
    }
}


void getUserData(char username[MAX_USERNAME_LEN], int *userLine, int *entireLines, int *userHighscore)
{
    printf("Gib Deinen Username ein: ");
    fgets(username, MAX_USERNAME_LEN, stdin);
    *strchr(username, '\n') = '\0';
    FILE * fp = fopen(HIGHSCORE_PATH, "r");
    /*
### Jan 25:
    if (fp == NULL)
    {
        // gameData.txt konnte noch nicht geöffnet werden -> ein neues erstellen!
        fp = fopen(HIGHSCORE_PATH, "w");
        fclose(fp);
        fp = fopen(HIGHSCORE_PATH, "r");
    }
    */
    if (fp == NULL)
    {
        // gameData.txt konnte noch immer nicht geöffnet werden...
        printf("bitte lege zuerst ein gameData.txt File an\n");
        printf("und starte das Programm dann erneut\n");
        exit(1);
    }

    bool userInList = false;

    char line[MAX_LINE_LEN];
    while (fgets(line, MAX_LINE_LEN, fp) != NULL) {
        (*entireLines)++;

        const char *pos = strchr(line, ':');
        if (strncmp(line, username, pos - line) == 0) {
            *userHighscore = (int) strtol(pos + 1, NULL, 10);
            *userLine = *entireLines - 1;
            userInList = true;
        }
    }

    fclose(fp);

    if (!userInList) {
        fp = fopen(HIGHSCORE_PATH, "a");
        if (fp == NULL)
            exit(1);

        fprintf(fp, "\n%s: 0", username);

        *userLine = *entireLines;
        (*entireLines)++;
        fclose(fp);
    }
}


void updateHighscore(char username[MAX_USERNAME_LEN], const int userLine, const int entireLines, const int userHighscore, const int count) {
    if (count > userHighscore) {
        FILE *fp = fopen(HIGHSCORE_PATH, "r");
        char fpContent[entireLines][MAX_LINE_LEN];

        for (int i = 0; i < entireLines; i++) {
            fgets(fpContent[i], MAX_LINE_LEN, fp);
        }
        sprintf(fpContent[userLine], "%s: %d\n", username, count);

        fclose(fp);

        fp = fopen(HIGHSCORE_PATH, "w");

        for (int i = 0; i < entireLines; i++) {
            fprintf(fp, "%s", fpContent[i]);
        }
        fclose(fp);
    }
}
