#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

class Piece;

class IMovingStrategy {
public:
    virtual bool isValidMove(int src, int srr, int desc, int desr, vector<vector<Piece*>> &board) = 0;
    virtual Piece* makeMove(int src, int srr, int desc, int desr, vector<vector<Piece*>> &board) = 0;
    virtual ~IMovingStrategy() = default;
};

class Piece {
protected:
    int r, c;
    int color;  
    IMovingStrategy* movingStrategy;
public:
    Piece(int row, int col, int clr, IMovingStrategy* ms)
        : r(row), c(col), color(clr), movingStrategy(ms) {}
    int getColor() { return color; }
    virtual string getType() = 0;
    IMovingStrategy* getMovingStrategy() const { return movingStrategy; }
    virtual ~Piece() = default;
};


class rookMove : public IMovingStrategy {
public:
    bool isValidMove(int src, int srr, int desc, int desr, vector<vector<Piece*>> &board) override {
        if (src - desc != 0 && srr - desr != 0) {
            return false;
        }
        if (src - desc == 0) {
            for (int i = min(srr, desr) + 1; i < max(srr, desr); i++) {
                if (board[i][src] != nullptr) {
                    cout << "Invalid Move: Path is blocked\n";
                    return false;
                }
            }
        }
        if (srr - desr == 0) {
            for (int i = min(src, desc) + 1; i < max(src, desc); i++) {
                if (board[srr][i] != nullptr) {
                    cout << "Invalid Move: Path is blocked\n";
                    return false;
                }
            }
        }
        if (board[desr][desc] && board[desr][desc]->getColor() == board[srr][src]->getColor()) {
            cout << "Invalid Move: Destination occupied by same color piece\n";
            return false;
        }
        return true;
    }
    Piece* makeMove(int src, int srr, int desc, int desr, vector<vector<Piece*>> &board) override {
        if (isValidMove(src, srr, desc, desr, board)) {
            Piece* captured = board[desr][desc];
            board[desr][desc] = board[srr][src];
            board[srr][src] = nullptr;
            return captured;
        } else {
            cout << "Move not executed due to invalid move\n";
            return nullptr;
        }
    }
};

class PawnMove : public IMovingStrategy {
public:
    bool isValidMove(int src, int srr, int desc, int desr, vector<vector<Piece*>> &board) override {
        int direction = (board[srr][src]->getColor() == 0) ? -1 : 1;
        if (desc == src && desr == srr + direction && board[desr][desc] == nullptr) {
            return true;
        }
        if (desc == src && desr == srr + 2 * direction &&
            board[srr + direction][src] == nullptr && board[desr][desc] == nullptr &&
            (srr == 1 || srr == 6)) {
            return true;
        }
        if (abs(desc - src) == 1 && desr == srr + direction &&
            board[desr][desc] != nullptr &&
            board[desr][desc]->getColor() != board[srr][src]->getColor()) {
            return true;
        }
        return false;
    }
    
    Piece* makeMove(int src, int srr, int desc, int desr, vector<vector<Piece*>> &board) override {
        if (isValidMove(src, srr, desc, desr, board)) {
            Piece* captured = board[desr][desc];
            board[desr][desc] = board[srr][src];
            board[srr][src] = nullptr;
            return captured;
        } else {
            cout << "Invalid move for Pawn\n";
            return nullptr;
        }
    }
};

class KnightMove : public IMovingStrategy {
public:
    bool isValidMove(int src, int srr, int desc, int desr, vector<vector<Piece*>> &board) override {
        int dr = abs(desr - srr);
        int dc = abs(desc - src);
        if ((dr == 2 && dc == 1) || (dr == 1 && dc == 2)) {
            if (board[desr][desc] == nullptr || board[desr][desc]->getColor() != board[srr][src]->getColor()) {
                return true;
            }
        }
        return false;
    }
    
    Piece* makeMove(int src, int srr, int desc, int desr, vector<vector<Piece*>> &board) override {
        if (isValidMove(src, srr, desc, desr, board)) {
            Piece* captured = board[desr][desc];
            board[desr][desc] = board[srr][src];
            board[srr][src] = nullptr;
            return captured;
        } else {
            cout << "Invalid move for Knight\n";
            return nullptr;
        }
    }
};

class BishopMove : public IMovingStrategy {
public:
    bool isValidMove(int src, int srr, int desc, int desr, vector<vector<Piece*>> &board) override {
        if (abs(desc - src) != abs(desr - srr)) {
            return false;
        }
        int dr = (desr > srr) ? 1 : -1;
        int dc = (desc > src) ? 1 : -1;
        int r = srr + dr, c = src + dc;
        while (r != desr && c != desc) {
            if (board[r][c] != nullptr) return false;
            r += dr;
            c += dc;
        }
        return (board[desr][desc] == nullptr ||
                board[desr][desc]->getColor() != board[srr][src]->getColor());
    }
    
    Piece* makeMove(int src, int srr, int desc, int desr, vector<vector<Piece*>> &board) override {
        if (isValidMove(src, srr, desc, desr, board)) {
            Piece* captured = board[desr][desc];
            board[desr][desc] = board[srr][src];
            board[srr][src] = nullptr;
            return captured;
        } else {
            cout << "Invalid move for Bishop\n";
            return nullptr;
        }
    }
};

class QueenMove : public IMovingStrategy {
public:
    bool isValidMove(int src, int srr, int desc, int desr, vector<vector<Piece*>> &board) override {
        int dr = abs(desr - srr);
        int dc = abs(desc - src);
        if (dr == dc) {
            int rowDir = (desr > srr) ? 1 : -1;
            int colDir = (desc > src) ? 1 : -1;
            int r = srr + rowDir, c = src + colDir;
            while (r != desr && c != desc) {
                if (board[r][c] != nullptr) return false;
                r += rowDir;
                c += colDir;
            }
            return board[desr][desc] == nullptr ||
                   board[desr][desc]->getColor() != board[srr][src]->getColor();
        }
        if (src == desc || srr == desr) {
            int rowDir = (srr == desr) ? 0 : (desr > srr ? 1 : -1);
            int colDir = (src == desc) ? 0 : (desc > src ? 1 : -1);
            int r = srr + rowDir, c = src + colDir;
            while (r != desr || c != desc) {
                if (board[r][c] != nullptr) return false;
                r += rowDir;
                c += colDir;
            }
            return board[desr][desc] == nullptr ||
                   board[desr][desc]->getColor() != board[srr][src]->getColor();
        }
        return false;
    }
    
    Piece* makeMove(int src, int srr, int desc, int desr, vector<vector<Piece*>> &board) override {
        if (isValidMove(src, srr, desc, desr, board)) {
            Piece* captured = board[desr][desc];
            board[desr][desc] = board[srr][src];
            board[srr][src] = nullptr;
            return captured;
        } else {
            cout << "Invalid move for Queen\n";
            return nullptr;
        }
    }
};

class KingMove : public IMovingStrategy {
public:
    bool isValidMove(int src, int srr, int desc, int desr, vector<vector<Piece*>> &board) override {
        int dr = abs(desr - srr);
        int dc = abs(desc - src);
        if ((dr <= 1 && dc <= 1) &&
            (board[desr][desc] == nullptr ||
             board[desr][desc]->getColor() != board[srr][src]->getColor())) {
            return true;
        }
        return false;
    }
    
    Piece* makeMove(int src, int srr, int desc, int desr, vector<vector<Piece*>> &board) override {
        if (isValidMove(src, srr, desc, desr, board)) {
            Piece* captured = board[desr][desc];
            board[desr][desc] = board[srr][src];
            board[srr][src] = nullptr;
            return captured;
        } else {
            cout << "Invalid move for King\n";
            return nullptr;
        }
    }
};

class Pawn : public Piece {
public:
    Pawn(int row, int col, int clr, IMovingStrategy* ms)
        : Piece(row, col, clr, ms) {}
    string getType() override {
        return "pawn";
    }
};

class Rook : public Piece {
public:
    Rook(int row, int col, int clr, IMovingStrategy* ms)
        : Piece(row, col, clr, ms) {}
    string getType() override {
        return "rook";
    }
};

class Bishop : public Piece {
public:
    Bishop(int row, int col, int clr, IMovingStrategy* ms)
        : Piece(row, col, clr, ms) {}
    string getType() override {
        return "bishop";
    }
};

class Queen : public Piece {
public:
    Queen(int row, int col, int clr, IMovingStrategy* ms)
        : Piece(row, col, clr, ms) {}
    string getType() override {
        return "queen";
    }
};

class King : public Piece {
public:
    King(int row, int col, int clr, IMovingStrategy* ms)
        : Piece(row, col, clr, ms) {}
    string getType() override {
        return "king";
    }
};

class Knight : public Piece {
public:
    Knight(int row, int col, int clr, IMovingStrategy* ms)
        : Piece(row, col, clr, ms) {}
    string getType() override {
        return "knight";
    }
};

class PieceFactory {
public:
    static Piece* createPiece(string type, int row, int col, int color, IMovingStrategy* ms) {
        if (type == "pawn")   return new Pawn(row, col, color, ms);
        if (type == "rook")   return new Rook(row, col, color, ms);
        if (type == "bishop") return new Bishop(row, col, color, ms);
        if (type == "queen")  return new Queen(row, col, color, ms);
        if (type == "king")   return new King(row, col, color, ms);
        if (type == "knight") return new Knight(row, col, color, ms);
        return nullptr;
    }
};

IMovingStrategy* getStrategyForPiece(const string& type) {
    if (type == "pawn")   return new PawnMove();
    if (type == "rook")   return new rookMove();
    if (type == "knight") return new KnightMove();
    if (type == "bishop") return new BishopMove();
    if (type == "queen")  return new QueenMove();
    if (type == "king")   return new KingMove();
    return nullptr;
}

class Board {
private:
    vector<vector<Piece*>> board;
public:
    Board(int n = 8) {
        board.resize(n, vector<Piece*>(n, nullptr));
    }
    
    void initializeBoard() {
        int white = 0, black = 1;
        for (int i = 0; i < 8; i++) {
            board[1][i] = PieceFactory::createPiece("pawn", 1, i, black, getStrategyForPiece("pawn"));
            board[6][i] = PieceFactory::createPiece("pawn", 6, i, white, getStrategyForPiece("pawn"));
        }
        vector<string> pieceOrder = {"rook", "knight", "bishop", "queen", "king", "bishop", "knight", "rook"};
        for (int i = 0; i < 8; i++) {
            board[0][i] = PieceFactory::createPiece(pieceOrder[i], 0, i, black, getStrategyForPiece(pieceOrder[i]));
            board[7][i] = PieceFactory::createPiece(pieceOrder[i], 7, i, white, getStrategyForPiece(pieceOrder[i]));
        }
    }
    
    void displayBoard() {
        for (int i = 0; i < board.size(); i++) {
            for (int j = 0; j < board[i].size(); j++) {
                if (board[i][j])
                    cout << board[i][j]->getType()[0] << " ";
                else
                    cout << ". ";
            }
            cout << endl;
        }
    }
    
    Piece* getPieceAt(int row, int col) {
        if (row >= 0 && row < board.size() && col >= 0 && col < board[0].size())
            return board[row][col];
        return nullptr;
    }
    
    vector<vector<Piece*>> &getBoard() {
        return board;
    }
    
    ~Board() {
        for (int i = 0; i < board.size(); i++) {
            for (int j = 0; j < board[i].size(); j++) {
                delete board[i][j];
            }
        }
    }
};

class Player {
private:
    int color;
    vector<Piece*> pieces;
public:
    Player(int clr) : color(clr) {}
    
    void addPiece(Piece* piece) {
        pieces.push_back(piece);
    }
    
    void removePiece(Piece* piece) {
        auto it = find(pieces.begin(), pieces.end(), piece);
        if (it != pieces.end()) {
            pieces.erase(it);
        }
    }
    
    vector<Piece*> getPieces() const {
        return pieces;
    }
    
    int getColor() const {
        return color;
    }
};


class Game {
private:
    Board board;
    Player whitePlayer;
    Player blackPlayer;
    int currentTurn;
public:
    Game() : whitePlayer(0), blackPlayer(1), currentTurn(0) {
        board.initializeBoard();
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                Piece* piece = board.getPieceAt(i, j);
                if (piece) {
                    if (piece->getColor() == 0)
                        whitePlayer.addPiece(piece);
                    else
                        blackPlayer.addPiece(piece);
                }
            }
        }
    }
    
    bool executeMove(int srcCol, int srcRow, int destCol, int destRow) {
        Piece* movingPiece = board.getPieceAt(srcRow, srcCol);
        if (!movingPiece || movingPiece->getColor() != currentTurn) {
            cout << "Invalid move: Not your turn or no piece at the source.\n";
            return false;
        }
        
        IMovingStrategy* strategy = movingPiece->getMovingStrategy();
        if (!strategy->isValidMove(srcCol, srcRow, destCol, destRow, board.getBoard())) {
            cout << "Invalid move according to piece rules.\n";
            return false;
        }
        
        Piece* captured = strategy->makeMove(srcCol, srcRow, destCol, destRow, board.getBoard());
        if (captured) {
            if (captured->getColor() == 0)
                whitePlayer.removePiece(captured);
            else
                blackPlayer.removePiece(captured);
            delete captured;
        }
        
        currentTurn = 1 - currentTurn;
        return true;
    }
    
    void play() {
        while (true) {
            board.displayBoard();
            int srcCol, srcRow, destCol, destRow;
            cout << (currentTurn == 0 ? "White" : "Black")
                 << "'s turn. Enter move (srcCol srcRow destCol destRow): ";
            cin >> srcCol >> srcRow >> destCol >> destRow;
            if (!cin) break;
            if (!executeMove(srcCol, srcRow, destCol, destRow))
                cout << "Move failed. Try again.\n";
        }
    }
};

int main() {
    Game game;
    game.play();
    return 0;
}
