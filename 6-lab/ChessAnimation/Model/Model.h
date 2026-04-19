#pragma once
#include <array>
#include <vector>

#include "../IModelListener.h"
#include "Piece.h"

struct Pos
{
    int row;
    int col;
};

struct Move
{
    Pos from;
    Pos to;
    bool isCapture; // Рубим?
};

class Model
{
private:
    std::array<std::array<Piece, 8>, 8> m_grid;

    std::array<Move, 8> m_moves;
    int m_moveIndex;

    std::vector<IModelListener *> m_listeners;

public:
    Model()
    {
        Reset();
        InitMoves();
    }

    void Reset()
    {
        for (int r = 0; r < 8; r++) {
            for (int c = 0; c < 8; c++) {
                m_grid[r][c].isEmpty = true;
            }
        }

        PieceType backLine[] = {
            PieceType::Rook, PieceType::Knight, PieceType::Bishop,
            PieceType::Queen, PieceType::King,
            PieceType::Bishop, PieceType::Knight, PieceType::Rook
        };

        for (int c = 0; c < 8; c++) {
            // белые
            m_grid[0][c] = { backLine[c], PieceColor::White, false };
            m_grid[1][c] = { PieceType::Pawn, PieceColor::White, false };

            // черные
            m_grid[7][c] = { backLine[c], PieceColor::Black, false };
            m_grid[6][c] = { PieceType::Pawn, PieceColor::Black, false };
        }
    }

    void InitMoves()
    {
        m_moveIndex = 0;
        m_moves = {
            Move{{1, 4}, {3, 4}, false}, // 1. e2-e4 (Белая пешка)
            Move{{6, 4}, {4, 4}, false}, // 1. ... e7-e5 (Черная пешка)
            Move{{0, 5}, {3, 2}, false}, // 2. Bc1-c4 (Белый слон)
            Move{{7, 1}, {5, 2}, false}, // 2. ... Nb8-c6 (Черный конь)
            Move{{0, 3}, {4, 7}, false}, // 3. Qd1-h5 (Белый ферзь)
            Move{{7, 6}, {5, 5}, false}, // 3. ... Ng8-f6 (Черный конь)
            Move{{4, 7}, {6, 5}, true}
        };
    }

    Move GetNextMove()
    {
        return m_moves.at(m_moveIndex++);
    }

    void ApplyMove(Move move)
    {
        // переместить фигуру
        m_grid[move.to.row][move.to.col] = m_grid[move.from.row][move.from.col];

        // убрать фигуру с клетки
        m_grid[move.from.row][move.from.col].isEmpty = true;

        NotifyListeners();
    }

    void AddListener(IModelListener* listener)
    {
        m_listeners.push_back(listener);
    }

private:
    void NotifyListeners()
    {
        for (auto listener: m_listeners)
        {
            listener->OnModelChanged();
        }
    }
};
