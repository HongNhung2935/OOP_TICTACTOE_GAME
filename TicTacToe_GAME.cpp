// I/O stream
#include <iostream>
// calculate
#include <cmath>
#include <algorithm>
// data structure
#include <vector>
#include <string>
// WINAPI
#include <windows.h>
// for random number
#include <ctime>
#include <cstdlib> 
// for exception
#include <stdexcept>

using std::vector;
using std::string;

// constant in game
const int screenWIDTH = 56;
const int screenHEIGHT = 24;
const int buttonWIDTH = 36;
const int buttonHEIGHT = 3;
const int commandColor = 11;
const int buttonColor = 189;
const string command = "input your choice: ";
const string commandMove = "input turn [1-9]: ";
const string commandInput1 = "input your name, player 1: ";
const string commandInput2 = "input your name, player 2: ";
const string commandPlayingOrder = "who first, player1 (1) or player 2 (2): ";
const string announcementTie = "NO ONE WIN";
const string announcementWin = "THE WINNER IS: ";
const string GameTitle = "TIC-TAC-TOE GAME";

#define DefaultColor 250
#define DefaultTime 2000

// using for game, not changing
std::vector <std::vector <std::string>> SYMBOL = {
    {"  ", "  ", "  "},
    {"  ", "  ", "  "},
    {"  ", "  ", "  "}
};

// the coordinate struct
struct COORDINATE {
    int abscissa;
    int ordinate;
};

struct BOX {
    int width;
    int height;
};

// manage the cursor's position in the console window
void setPosition (COORDINATE coord) {
    COORD position = {(short) coord.abscissa,(short) coord.ordinate};
    SetConsoleCursorPosition (GetStdHandle (STD_OUTPUT_HANDLE), position);
}

// show the appearance of cursor in the screen
void disableCursor(BOOL isDisable = false, DWORD dwSize = 25){
    HANDLE consoleOutput = GetStdHandle (STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO ConsoleCursorInfo;
	ConsoleCursorInfo.bVisible = isDisable;
	ConsoleCursorInfo.dwSize = dwSize;
	SetConsoleCursorInfo(consoleOutput, &ConsoleCursorInfo);
}

// set color
void setColor(WORD color){
	HANDLE consoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
	GetConsoleScreenBufferInfo(consoleOutput, &screen_buffer_info);
	SetConsoleTextAttribute(consoleOutput,color);
}

// clearing the text in an area
void clearing (int width, int height, int color, COORDINATE root){
    setColor (color);
    int x = root.abscissa;
    int y = root.ordinate;
    for (int i = 0; i < height; ++i) {
        setPosition ({x, y});
        for (int j = 0; j < width; ++j) std::cout << " ";
        ++y;
    }
}

void printSYMBOL (COORDINATE coord) {
    int x = coord.abscissa;
    int y = coord.ordinate;
    for (auto i : SYMBOL){
        setPosition ({x,y++});
        for (auto j : i) std::cout << j;
        std::cout << '\n';
    }
}

// base class: BOARD
class BOARD {
    protected:
        BOX size;
    public:
        // constructor
        BOARD (): size ({screenWIDTH, screenHEIGHT}) {}
        BOARD (int newWidth, int newHeight): size ({newWidth, newHeight}) {}
        BOARD (BOARD& newBoard): size (newBoard.size) {}

        // methodes
        // drawing the main board in all screen
        void drawGameFrame (int Color = DefaultColor) {
            setColor (Color);
            for (int ordinate = 0; ordinate < size.height; ++ordinate){
                for (int abscissa = 0; abscissa < size.width; ++abscissa){
                    std::cout << " ";
                }
                std::cout << std::endl;
            }
            setPosition ({(int)(size.width/2 - GameTitle.size()) + 8, 1});
            std::cout << GameTitle;
            clearing (screenWIDTH, 1, commandColor, {0, size.height});
            setPosition ({0, size.height});
            std::cout << "input your choice: " ;
        }

        // destructor
        ~BOARD() {
            size.width = 0;
            size.height = 0;
        }
};

// child class: base BOARD
class MenuBOARD : public BOARD {
    private:
        std::vector <BOX> buttons;
        std::vector <string> choices;
    public:
        // constructor
        MenuBOARD () {
            BOARD ();
            // four buttons and four choices
            buttons.resize (4);
            choices.resize (4);
        }

        MenuBOARD (int width, int height, std::vector <BOX> buttonSize, std::vector <string> listChoices):
            BOARD (width, height), buttons (buttonSize), choices (listChoices) {}

        MenuBOARD (MenuBOARD& otherMenu): BOARD (otherMenu), buttons (otherMenu.buttons), choices (otherMenu.choices) {}
        
        // methods
        // set size of buttons
        void setSizeOfButtons (int button_width, int button_height){
            for (int i = 0; i < buttons.size(); ++i) {
                buttons[i] = {button_width, button_height};
            }
        }
        // draw menu board
        void drawMainMenu (int rootX = 10, int rootY = 4) {
            BOARD::drawGameFrame ();
            int x = rootX;
            int y = rootY;
            setColor (buttonColor);
            for (int i = 0; i < buttons.size(); ++i){
                for (int k = 0; k < buttons[i].height; ++k) {
                    setPosition ({x, y++});
                    if (k == buttons[i].height / 2){
                        std::cout << choices[i];
                        for (int m = 0; m < buttons[i].width - choices[i].size(); ++m)
                            std::cout << " ";
                    }
                    else for (int h = 0; h < buttons[i].width; ++h) std::cout << " ";
                }
                setPosition ({x, y += 1});
            }
        }

        // destructor;
        ~MenuBOARD () {
            buttons.clear();
        }

};

// expressing the game
class PlayingBOARD : public BOARD{
    private:
        std::vector<COORDINATE> coordCOMPONENT;
        std::vector <int> Movements ;
    public:
        // constructor
        PlayingBOARD () {}
        PlayingBOARD ( std::vector <COORDINATE> coor, std::vector <int> move):
            BOARD (), coordCOMPONENT (coor), Movements (move) {}
        PlayingBOARD (PlayingBOARD& board):
            BOARD (board), coordCOMPONENT (board.coordCOMPONENT), Movements (board.Movements) {}
        
        // methods
        void initTheMovements () {Movements.resize(9); Movements.assign(9, -1);}
        void modifyTheMovement (int move, int val){ Movements[move-1] = val;}
        int getValueMovement (int move) {return Movements[move-1];}
        COORDINATE getThePosOfCOMPONENT (int postion) {return coordCOMPONENT[postion];}

        void initCoordCOMPONENT (int rootX, int rootY) {
            int x = rootX;
            int y = rootY;
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j){
                    coordCOMPONENT.push_back ({x, y});
                    x += 8;
                }
                x = rootX;
                y += 4;
            }
        }

        void drawGameBOARD () {
            BOARD::drawGameFrame();
            initCoordCOMPONENT (17, 6);
            for (auto coord : coordCOMPONENT){
                printSYMBOL (coord);
            }
        }

        bool isWinTheGame () {
            // check row
            for (int i = 0; i < 9; i += 3)
                if (Movements[i] != -1 && Movements[i] == Movements[i+1] && Movements[i] == Movements[i+2]) return true;

            // check colunm
            for (int i = 0; i < 3; ++i)
                if (Movements[i] != -1 && Movements[i] == Movements[i+3] && Movements[i] == Movements[i+6]) return true;

            if (Movements[0] != -1 && Movements[0] == Movements[4] && Movements[0] == Movements[8]) return true;
            if (Movements[2] != -1 && Movements[2] == Movements[4] && Movements[2] == Movements[6]) return true;
            
            return false;
        }

        bool isFull (){
            for (int move : Movements)
                if (move == -1) return false;
            return true;
        }

        // destructor
        ~PlayingBOARD () {
            coordCOMPONENT.clear ();
            Movements.clear ();
        }

};

class PLAYER {
    private:
        string name;
        int choice;
    public:
        // constructor
        PLAYER () {}
        PLAYER (string newName): name (newName), choice (0) {}
        PLAYER (PLAYER& player): name (player.name), choice (player.choice) {}

        // method
        virtual void setName (string newName){name = newName;}
        virtual int makeChoice () {return choice;}

        void play (PlayingBOARD* &gameBoard ,int currentColor, int val, bool& back, bool& outGame) {
            clearing (screenWIDTH, 1, DefaultColor, {0, screenHEIGHT-3});
            setPosition ({17, screenHEIGHT - 3});
            setColor (currentColor);
            std::cout << "your turn: "; for (char i : name) std::cout << i;
            setPosition ({0, screenHEIGHT});
            for (int i = 0; i < screenWIDTH; ++i) std::cout << " ";
            setPosition ({0, screenHEIGHT});
            std::cout << commandMove;
            setPosition ({(int)commandMove.size() + 1, screenHEIGHT});
            int choice = makeChoice ();
            try {
                if (choice < 1 || choice > 9 || gameBoard -> getValueMovement (choice) != -1) {
                    throw choice;
                }
                setColor (currentColor);
                printSYMBOL (gameBoard -> getThePosOfCOMPONENT (choice - 1));
                gameBoard ->modifyTheMovement(choice, val);
                back = false;
                if (gameBoard -> isWinTheGame()){
                    clearing (screenWIDTH, 1, DefaultColor, {0, screenHEIGHT-3});
                    setPosition ({17, screenHEIGHT - 3});
                    setColor (233);
                    std::cout << announcementWin;
                    setPosition ({(int)announcementWin.size() + 17, screenHEIGHT - 3});
                    std::cout << name;
                    setColor(0);
                    setPosition ({0, screenHEIGHT + 1});
                    system("pause");
                    outGame = true;
                }else if (gameBoard -> isFull()){
                    clearing (screenWIDTH, 1, DefaultColor, {0, screenHEIGHT-3});
                    setPosition ({17, screenHEIGHT - 3});
                    setColor (177);
                    std::cout << announcementTie;
                    setColor(0);
                    setPosition ({0, screenHEIGHT + 1});
                    system("pause");
                    outGame = true;
                }
            }catch (int choice){
                back = true;
            }
        }

        // destructor
        ~PLAYER () {
            name = "";
            choice = -1;
        }
};

class HumanPLAYER : public PLAYER {
    public:
        // constructor
        HumanPLAYER () {}
        HumanPLAYER (string newName): PLAYER (newName) {}
        HumanPLAYER (HumanPLAYER& otherHuman): PLAYER (otherHuman) {}

        // method
        int makeChoice () {
            int Choice;
            std::cin >> Choice;
            return Choice;
        }

        // destructor
        ~HumanPLAYER (){}
};

class ComputerPLAYER : public PLAYER {
    public:
        // constructor
        ComputerPLAYER () {}
        ComputerPLAYER (string newName): PLAYER (newName) {}
        ComputerPLAYER (ComputerPLAYER& otherComputer): PLAYER (otherComputer) {}

        //method
        int makeChoice () {
            int choice = std::rand () % 9 + 1;
            return choice;
        }

        // destructor
        ~ComputerPLAYER () {}
};

class MainGAME {
    private:
        MenuBOARD* mainMenu;
        PlayingBOARD* gameBoard;
        HumanPLAYER* seft;
    public:
        // constructror:
        MainGAME () {
            mainMenu = new MenuBOARD;
            gameBoard = new PlayingBOARD;
            seft = new HumanPLAYER;
        }
        MainGAME ( MenuBOARD menu, PlayingBOARD gameboard): mainMenu (&menu), gameBoard (&gameboard) {}
        MainGAME (MainGAME& otherGame): mainMenu (otherGame.mainMenu), gameBoard (otherGame.gameBoard) {}

        // methods
        // overloading
        void settingBeforeGame (HumanPLAYER* firstPlayer, HumanPLAYER* secondPlayer){
            setColor (commandColor);
            string name;
            string partnerName;

            setPosition ({0, screenHEIGHT});
            std::cout << commandInput1;
            setPosition ({(int)commandInput1.size() + 1, screenHEIGHT});
            std::cin >> name;

            setPosition ({0, screenHEIGHT});
            std::cout << commandInput2; for (int i = 0; i < name.size(); ++i) std::cout << "   ";
            setPosition ({(int)commandInput2.size() + 1, screenHEIGHT});
            std::cin >> partnerName;
            firstPlayer -> setName (name);
            secondPlayer -> setName (partnerName);

            clearing (screenWIDTH, 1, commandColor, {0, screenHEIGHT});
                   
            setPosition ({0, screenHEIGHT});
            std::cout << commandPlayingOrder;
            setPosition ({(int)commandPlayingOrder.size() + 1, screenHEIGHT});
        }

        void settingBeforeGame (HumanPLAYER* firstPlayer, ComputerPLAYER* secondPlayer){
            setColor (commandColor);
            string name;
            setPosition ({0, screenHEIGHT});
            std::cout << commandInput1;
            setPosition ({(int)commandInput1.size() + 1, screenHEIGHT});
            std::cin >> name;
            firstPlayer -> setName (name);
            secondPlayer -> setName ("COMPUTER");
            clearing (screenWIDTH, 1, commandColor, {0, screenHEIGHT});
            setPosition ({0, screenHEIGHT});
            std::cout << commandPlayingOrder;
            setPosition ({(int)commandPlayingOrder.size() + 1, screenHEIGHT});
        }

        void settingBeforeGame (ComputerPLAYER* firstPlayer, ComputerPLAYER* secondPlayer){
            firstPlayer -> setName ("COMPUTER 1");
            secondPlayer -> setName ("COMPUTER 2");
            clearing (screenWIDTH, 1, commandColor, {0, screenHEIGHT});
            setPosition ({0, screenHEIGHT});
            std::cout << commandPlayingOrder;
            setPosition ({(int)commandPlayingOrder.size() + 1, screenHEIGHT});
        }


        // main game
        void startGame () {
            std::vector <BOX> menuList;
            for (int i = 0; i < 4; ++i)
                menuList.push_back ({buttonWIDTH, buttonHEIGHT});
            
            std::vector <string> choiceDetails = {
                "1: PLAY WITH HUMAN",
                "2: PLAY WITH COMPUTER",
                "3: COMPUTER WITH COMPUTER",
                "4: QUIT THE GAME"
            };
            mainMenu = new MenuBOARD (screenWIDTH, screenHEIGHT, menuList, choiceDetails);
            gameBoard = new PlayingBOARD;            
            while (true) {
                disableCursor ();
                setPosition({0, 0});
                mainMenu -> drawMainMenu ();
                gameBoard -> initCoordCOMPONENT (17, 6);
                gameBoard -> initTheMovements();

                int CHOICE;
                setPosition ({(int)command.size() + 1, screenHEIGHT});
                setColor (commandColor);
                std::cin >> CHOICE; // depend on ChoiceDetails list
                
                // playing with human
                if (CHOICE == 1) {
                    setPosition ({0, 0});
                    gameBoard ->drawGameBOARD ();
                    setColor (122);
                    setPosition ({(int)(screenWIDTH/2 - GameTitle.size()) + 5, 3});
                    std::cout << "MODE: HUMAN WITH HUMAN";
                    HumanPLAYER* other = new HumanPLAYER;
                    settingBeforeGame (seft, other);
                    PLAYER* player1 = seft;
                    PLAYER* player2 = other;
                    int whofirst;
                    std::cin >> whofirst;
                    if (whofirst == 2) {
                        player1 = other;
                        player2 = seft;
                    }
                    PLAYER* currentPlayer = player1;
                    int currentColor = 191;
                    int val = 1;
                    bool back = false;
                    bool outGame = false;
                    while (true) {
                        //player action
                        currentPlayer -> play (gameBoard, currentColor, val, back, outGame);
                        if (back) continue;
                        if (outGame) break;
                        currentPlayer = (currentPlayer == player1) ? player2 : player1;
                        currentColor = (currentPlayer == player2) ? 234 : 191;
                        val = (currentPlayer == player2) ? 0 : 1;
                    }
                }
                // playing with computer
                else if (CHOICE == 2){
                    setPosition ({0, 0});
                    gameBoard ->drawGameBOARD ();
                    setColor (151);
                    setPosition ({(int)(screenWIDTH/2 - GameTitle.size()) + 4, 3});
                    std::cout << "MODE: HUMAN WITH COMPUTER";
                    ComputerPLAYER* other = new ComputerPLAYER;
                    settingBeforeGame (seft, other);
                    PLAYER* player1 = seft;
                    PLAYER* player2 = other;
                    int whofirst;
                    std::cin >> whofirst;
                    if (whofirst == 2) {
                        player1 = other;
                        player2 = seft;
                    }
                    PLAYER* currentPlayer = player1;
                    int currentColor = 32;
                    int val = 1;
                    bool back = false;
                    bool outGame = false;
                    while (true) {
                        //player action
                        currentPlayer -> play (gameBoard, currentColor, val, back, outGame);
                        if (back) continue;
                        if (outGame) break;
                        currentPlayer = (currentPlayer == player1) ? player2 : player1;
                        currentColor = (currentPlayer == player2) ? 23 : 32;
                        val = (currentPlayer == player2) ? 0 : 1;
                    }
                }
                // computer beat to each another
                else if (CHOICE == 3) {
                    setPosition ({0, 0});
                    gameBoard ->drawGameBOARD ();
                    setColor (96);
                    setPosition ({(int)(screenWIDTH/2 - GameTitle.size()) + 2, 3});
                    std::cout << "MODE: COMPUTER WITH COMPUTER";
                    ComputerPLAYER* cmp1 = new ComputerPLAYER;
                    ComputerPLAYER* cmp2 = new ComputerPLAYER;
                    settingBeforeGame (cmp1, cmp2);
                    PLAYER* player1 = cmp1;
                    PLAYER* player2 = cmp2;
                    int whofirst;
                    std::cin >> whofirst;
                    if (whofirst == 2) {
                        player1 = cmp2;
                        player2 = cmp1;
                    }
                    PLAYER* currentPlayer = player1;
                    int currentColor = 222;
                    int val = 1;
                    bool back = false;
                    bool outGame = false;
                    while (true) {
                        //player action
                        currentPlayer -> play (gameBoard, currentColor, val, back, outGame);
                        if (back) continue;
                        if (outGame) break;
                        Sleep (DefaultTime);
                        currentPlayer = (currentPlayer == player1) ? player2 : player1;
                        currentColor = (currentPlayer == player2) ? 111 : 222;
                        val = (currentPlayer == player2) ? 0 : 1;
                    }
                }
                // quit the game
                else if (CHOICE == 4){
                    break;
                }
            }       
        }
        // destructror
        ~MainGAME () {
            mainMenu = NULL;
            gameBoard = NULL;
        }
};

int main(){
    MainGAME* game = new MainGAME;
    game -> startGame();
    return 0;
}
