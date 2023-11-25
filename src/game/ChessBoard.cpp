/**
 * @file ChessBoard.cpp
 * @brief Implementation of the ChessBoard class.
 * 
 * This file contains the implementation of the methods declared in ChessBoard.h.
 * The ChessBoard class represents the chess game board and provides functionalities to 
 * interact with the chess pieces positioned on it. It allows querying and modification 
 * of the board state, including placing, moving, and removing pieces. The ChessBoard 
 * also initializes the standard chess layout when constructed and ensures pieces' 
 * correct positioning throughout the game. This class works closely with individual 
 * chess piece classes to facilitate moves and interactions specific to each piece type.
 * 
 * @author Bradley McGlynn
 * @date September 24th, 2023
 */

#include <string>
#include <stdexcept>
#include <memory>
#include <fstream>
#include <algorithm>
#include "PieceFactory.h"
#include "ChessBoard.h"
#include "Pawn.h" 
#include "Rook.h"
#include "Knight.h"
#include "Bishop.h"
#include "Queen.h"
#include "King.h"
#include "Piece.h"

/**
 * @brief Constructs a ChessBoard object and initializes the board.
 */
ChessBoard::ChessBoard() {
    initializeBoard(); // Initialize the board when a ChessBoard object is created 
}

/**
 * @brief Destructor for the ChessBoard class.
 * Cleans up memory by deleting all piece objects and clearing the board vector.
 */
ChessBoard::~ChessBoard() {}

/**
 * @brief Returns the piece at the given position on the board.
 * @param position The position on the board.
 * @return Pointer to the Piece object at the given position.
 */
Piece* ChessBoard::getPieceAtPosition(const Position& position) const {
    return board[position.row][position.col].get();
}

/**
 * @brief Moves the piece from the start position to the end position on the board if the move is valid.
 * @param start The start position of the piece on the board.
 * @param end The end position to move the piece to on the board.
 * @param override Overrides move validation if true, for test mode.
 * @return true if move was made, false otherwise.
 */
bool ChessBoard::movePiece(const Position& start, const Position& end, bool override) {
    Piece* piece = board[start.row][start.col].get();

    // Check if there is a piece at the start position 
    if (!piece) return false;
    
    // Check if the move is valid 
    if (override || piece->isValidMove(start, end)) {
        
        // Update the board and the piece's position and valid moves 
        board[end.row][end.col] = std::move(board[start.row][start.col]);
        piece->setPosition(end);
        // Keep track of the number of moves a pawn has made 
        if (piece->getPieceType() == PieceType::PAWN) {
            Pawn* pawn = dynamic_cast<Pawn*>(piece);
            pawn->incrementMoveCounter();
            pawn->setEnPassant();
        }
        if (!piece->getHasMoved()) piece->setHasMoved();

        // update valid moves for all other pieces based on the new board state 
        for(auto& inner : board) {
            for(auto& piece : inner) {
                if (piece != nullptr) {
                    piece->updateValidMoves(*this);
                }
            }
        }

        return true;
    }
    return false; 
}

/**
 * @brief Checks if a proposed move is valid.
 * @param start The start position of the piece on the board.
 * @param end The end position to move the piece to on the board.
 * @return true if move was valid, false otherwise.
 */
bool ChessBoard::isValidMove(const Position& start, const Position& end) const {
    Piece* piece = board[start.row][start.col].get();

    // Check if there is a piece at the start position 
    if (!piece) return false;
    
    // Check if the move is valid 
    if (piece->isValidMove(start, end)) {
        return true;
    }
    return false; 
}

/**
 * @brief Checks if the space at the given position on the board is empty.
 * @param position The position on the board to check.
 * @return True if the space is empty, false otherwise.
 */
bool ChessBoard::isSpaceEmpty(const Position& position) const {
    return board[position.row][position.col].get() == nullptr;
}

/**
 * @brief Checks if the space at the given position on the board has a friendly piece of the given colour.
 * @param position The position on the board to check.
 * @param colour The colour to check for.
 * @return True if the space has a friendly piece, false otherwise.
 */
bool ChessBoard::isSpaceFriendly(const Position& position, Colour colour) const {
    Piece* piece = board[position.row][position.col].get();
    return piece && piece->getColour() == colour;
}

/**
 * @brief Checks if the space at the given position on the board has an enemy piece of the opposite colour.
 * @param position The position on the board to check.
 * @param colour The colour to check against.
 * @return True if the space has an enemy piece, false otherwise.
 */
bool ChessBoard::isSpaceEnemy(const Position& position, Colour colour) const {
    Piece* piece = board[position.row][position.col].get();
    return piece && piece->getColour() != colour;
}

/**
 * @brief Determines if a potential spot would put a King in check.
 * @param pos The potential position a King seeks to move to.
 * @param colour The colour of the King trying to make the move.
 * @return true if the move would not result in check, false otherwise.
 */
bool ChessBoard::isSquareSafe(const Position& pos, Colour colour) const {
    Colour opponentColour = (colour == Colour::WHITE) ? Colour::BLACK : Colour::WHITE;
    for (const auto& piece : getPiecesOfColour(opponentColour)) {
        /** Check if the square is a valid move (i.e. is being attacked by) 
         * an enemy piece that is not a pawn. Pawns are handled separately
        */
        if (piece->getPieceType() != PieceType::PAWN && piece->isValidMove(piece->getPosition(), pos)) return false;

        /** If the piece type is a pawn, ensure the column numbers are different (i.e. ensure it would be a diagonal capture)
         * as pawns can't capture pieces directly in front of them.
        */
        else if (piece->getPieceType() == PieceType::PAWN && pos.col != piece->getPosition().col && piece->isValidMove(piece->getPosition(), pos)) return false;
    }
    return true;
}

/**
 * @brief Finds and returns the position of the king of a specific colour on the board.
 * @param colour The colour of the king whose position is to be determined.
 * @return The position of the king on the board.
 * @throws runtime_error if the king of the specified colour is not found.
 */
Position ChessBoard::getKingPosition(Colour colour) const {
    for (const auto& row : board) {
        for (const auto& piece : row) {
            if (piece && piece->getColour() == colour && piece->getPieceType() == PieceType::KING) {
                return piece->getPosition();
            }
        }
    }
    throw std::runtime_error("King not found");
}

/**
 * @brief Retrieves all the pieces of a specific colour present on the board.
 * @param colour The colour of the pieces to be retrieved.
 * @return A vector of pointers to the pieces of the specified colour.
 */
std::vector<Piece*> ChessBoard::getPiecesOfColour(Colour colour) const {
    std::vector<Piece*> pieces;
    for (const auto& row : board) {
        for (const auto& piece : row) {
            if (piece && piece->getColour() == colour) {
                pieces.emplace_back(piece.get()); 
            }
        }
    }
    return pieces;
}

/**
 * @brief Initializes the board with Piece objects in their correct initial positions.
 * @param testMode If test mode is on, initialize the board with only kings. Defaults to false.
 */
void ChessBoard::initializeBoard(bool testMode) {
    clearBoard(); // Clear the board of any existing pieces 

    if(!testMode) {
        // Initialize pawns 
        for (int i = 0; i < 8; ++i) {
            board[1][i] = PieceFactory::createPiece(Colour::BLACK, Position{1, i}, PieceType::PAWN);
            board[6][i] = PieceFactory::createPiece(Colour::WHITE, Position{6, i}, PieceType::PAWN);
        }

        // Initialize rooks 
        board[0][0] = PieceFactory::createPiece(Colour::BLACK, Position{0, 0}, PieceType::ROOK);
        board[0][7] = PieceFactory::createPiece(Colour::BLACK, Position{0, 7}, PieceType::ROOK);
        board[7][0] = PieceFactory::createPiece(Colour::WHITE, Position{7, 0}, PieceType::ROOK);
        board[7][7] = PieceFactory::createPiece(Colour::WHITE, Position{7, 7}, PieceType::ROOK);

        // Initialize knights 
        board[0][1] = PieceFactory::createPiece(Colour::BLACK, Position{0, 1}, PieceType::KNIGHT);
        board[0][6] = PieceFactory::createPiece(Colour::BLACK, Position{0, 6}, PieceType::KNIGHT);
        board[7][1] = PieceFactory::createPiece(Colour::WHITE, Position{7, 1}, PieceType::KNIGHT);
        board[7][6] = PieceFactory::createPiece(Colour::WHITE, Position{7, 6}, PieceType::KNIGHT);

        // Initialize bishops 
        board[0][2] = PieceFactory::createPiece(Colour::BLACK, Position{0, 2}, PieceType::BISHOP);
        board[0][5] = PieceFactory::createPiece(Colour::BLACK, Position{0, 5}, PieceType::BISHOP);
        board[7][2] = PieceFactory::createPiece(Colour::WHITE, Position{7, 2}, PieceType::BISHOP);
        board[7][5] = PieceFactory::createPiece(Colour::WHITE, Position{7, 5}, PieceType::BISHOP);

        // Initialize queens 
        board[0][3] = PieceFactory::createPiece(Colour::BLACK, Position{0, 3}, PieceType::QUEEN);
        board[7][3] = PieceFactory::createPiece(Colour::WHITE, Position{7, 3}, PieceType::QUEEN);
    }

    // Initialize kings 
    board[0][4] = PieceFactory::createPiece(Colour::BLACK, Position{0, 4}, PieceType::KING);
    board[7][4] = PieceFactory::createPiece(Colour::WHITE, Position{7, 4}, PieceType::KING); 

    for(auto& inner : board) {
        for(auto& piece : inner) {
            if (piece != nullptr) {
                piece->updateValidMoves(*this); // Update the valid moves for each piece 
            }
        }
    }
}

/**
 * @brief Returns the current board state as a string to make board state referencing easier at higher levels.
 * @return A string representing the current state of the chess board.
 */
std::string ChessBoard::getBoardStateAsString() const {
    std::string boardStr;
    for (const auto& row : board) {
        for (const auto& piece : row) {
            if (piece) {
                PieceType type = piece->getPieceType();
                Colour colour = piece->getColour();
                std::string typeStr;
                switch(type) {
                    case PieceType::PAWN: typeStr = "P"; break;
                    case PieceType::ROOK: typeStr = "R"; break;
                    case PieceType::KNIGHT: typeStr = "N"; break;
                    case PieceType::BISHOP: typeStr = "B"; break;
                    case PieceType::QUEEN: typeStr = "Q"; break;
                    case PieceType::KING: typeStr = "K"; break;
                    case PieceType::NONE: typeStr = "?"; break;
                    default: typeStr = "Unknown"; break;
                }
                switch (colour) {
                    case Colour::BLACK: std::transform(typeStr.begin(), typeStr.end(), typeStr.begin(), ::tolower);; break; // make black pieces lower case 
                    case Colour::WHITE: break;
                    default: typeStr = "N"; break;
                }
                boardStr += typeStr;
                boardStr += "-";
            } else {
                boardStr += '.'; // Use '.' to represent an empty square. 
            }
        }
        boardStr += '\n'; // Add a newline character at the end of each row. 
    }
    return boardStr;
}

/**
 * @brief Determines if the current board position is a dead position.
 * @return true if the board is in a dead position, false otherwise.
 */
bool ChessBoard::isDeadPosition() const {
    int whitePieceCount = 0, blackPieceCount = 0;
    bool whiteBishop = false, blackBishop = false;
    bool whiteKnight = false, blackKnight = false;
    
    for (const auto& row : board) {
        for (const auto& piece : row) {
            if (piece) {
                if (piece->getColour() == Colour::WHITE) {
                    whitePieceCount++;
                    if (piece->getPieceType() == PieceType::BISHOP) whiteBishop = true;
                    if (piece->getPieceType() == PieceType::KNIGHT) whiteKnight = true;
                } else {
                    blackPieceCount++;
                    if (piece->getPieceType() == PieceType::BISHOP) blackBishop = true;
                    if (piece->getPieceType() == PieceType::KNIGHT) blackKnight = true;
                }
            }
        }
    }
    
    // King against King 
    if (whitePieceCount == 1 && blackPieceCount == 1) return true;
    
    // King and Bishop against King 
    if ((whitePieceCount == 1 && blackPieceCount == 2 && blackBishop) ||
        (blackPieceCount == 1 && whitePieceCount == 2 && whiteBishop)) return true;
    
    // King and Knight against King 
    if ((whitePieceCount == 1 && blackPieceCount == 2 && blackKnight) ||
        (blackPieceCount == 1 && whitePieceCount == 2 && whiteKnight)) return true;
    
    // Other scenarios can be added as needed 
    
    return false;
}

/**
 * @brief Checks if a given position is on the board.
 * @param position The position to check.
 * @return true if the position is on the board, false otherwise.
 */
bool ChessBoard::isPositionOnBoard(const Position& position) const {
    return position.row >= 0 && position.row < 8 && position.col >= 0 && position.col < 8;
}

/**
 * @brief Checks if a pawn is eligible for en passant.
 * @param position The position of the pawn to check.
 * @param colour The colour of the pawn to check.
 * @return true if the pawn is eligible for en passant, false otherwise.
 */
bool ChessBoard::isPawnEligibleForEnPassant(const Position& position, Colour colour) const {
    // Check if the pawn is on the correct row 
    if ((colour == Colour::WHITE && position.row != 4) || (colour == Colour::BLACK && position.row != 3)) return false;
    // Check if the pawn is on the correct column 
    if (position.col < 0 || position.col > 7) return false;
    // check if the enemy pawn has moved once but has moved two spaces 
    Piece* piece = getPieceAtPosition(position);
    if (!piece || piece->getPieceType() != PieceType::PAWN) return false;
    Pawn* pawn = dynamic_cast<Pawn*>(piece);
    if (!pawn || !pawn->getEnPassant()) return false;
    return true;
}

/**
 * @brief Clears the board of all pieces.
 */
void ChessBoard::clearBoard() {
    board.resize(8); // Ensure the outer vector has the correct size 
    for(auto& inner : board) {
        inner.resize(8); // Ensure each inner vector has the correct size 
        for(auto& piece : inner) {
            piece = nullptr; // Clear each position on the board 
        }
    }
}

/**
 * @brief Places a piece on the board at the given position. Mainly for testing.
 * @param pos The position to place the piece at.
 * @param piece The piece to place on the board.
 * @return true if the piece was placed, false otherwise.
 */
bool ChessBoard::placePiece(const Position& pos, std::unique_ptr<Piece> piece) {
    if(isPositionOnBoard(pos)) {
        board[pos.row][pos.col] = std::move(piece);
        // update valid moves for all other pieces based on the new board state 
        for(auto& inner : board) {
            for(auto& piece : inner) {
                if (piece != nullptr) {
                    piece->updateValidMoves(*this);
                }
            }
        }
        return true;
    } else {
        return false;
    }
}

/**
 * @brief Initializes a chess board state from a string.
 * This method reads through each character in the input string and initializes 
 * the board accordingly. It looks for the "-" character to determine where 
 * spaces without pieces are located, and newline characters to determine 
 * new rows on the board. 
 * @param boardState The string representing the board state.
 * @throw std::out_of_range If the board initialization goes out of bounds.
 */
void ChessBoard::initializeBoardFromString(const std::string& boardState) {
    clearBoard(); // Clear the board of any existing pieces 
    int row = 0, col = 0;
    for (const auto& c : boardState) {
        if (c == '-' || c == '.') {
            col++;
        } else if (c == '\n') {
            row++;
            col = 0;
        } else {
            PieceType type;
            Colour colour;
            switch(c) {
                case 'P': type = PieceType::PAWN; colour = Colour::WHITE; break;
                case 'R': type = PieceType::ROOK; colour = Colour::WHITE; break;
                case 'N': type = PieceType::KNIGHT; colour = Colour::WHITE; break;
                case 'B': type = PieceType::BISHOP; colour = Colour::WHITE; break;
                case 'Q': type = PieceType::QUEEN; colour = Colour::WHITE; break;
                case 'K': type = PieceType::KING; colour = Colour::WHITE; break;
                case 'p': type = PieceType::PAWN; colour = Colour::BLACK; break;
                case 'r': type = PieceType::ROOK; colour = Colour::BLACK; break;
                case 'n': type = PieceType::KNIGHT; colour = Colour::BLACK; break;
                case 'b': type = PieceType::BISHOP; colour = Colour::BLACK; break;
                case 'q': type = PieceType::QUEEN; colour = Colour::BLACK; break;
                case 'k': type = PieceType::KING; colour = Colour::BLACK; break;
                case '.': type = PieceType::NONE; break;
                default: type = PieceType::NONE; break;
            }
            if (type != PieceType::NONE) {
                board[row][col] = PieceFactory::createPiece(colour, Position{row, col}, type);
            } else {
                board[row][col] = nullptr;
            }
        }
        // Bounds checking 
        if (row > 8 || col > 8) {
            throw std::out_of_range("Board initialization went out of bounds");
        }
    }

    for(auto& inner : board) {
        for(auto& piece : inner) {
            if (piece != nullptr) {
                piece->updateValidMoves(*this); // Update the valid moves for each piece 
            }
        }
    }
}

/**
 * @brief Overloaded operator[] to return a reference to a row.
 * @param row The row to return.
 * @return A reference to the row.
 */
std::vector<std::unique_ptr<Piece>>& ChessBoard::operator[](int row) {
    return board[row];
}

/**
 * @brief Overloaded operator[] with const-qualifier to return a const reference to a row.
 * @param row The row to return.
 * @return A const reference to the row.
 */
const std::vector<std::unique_ptr<Piece>>& ChessBoard::operator[](int row) const {
    return board[row];
}
