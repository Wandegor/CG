#pragma once

enum class PieceType
{
    Pawn,
    Knight,
    Queen,
    King,
    Rook,
    Bishop,
};

enum class PieceColor
{
    Black,
    White,
};

struct Piece
{
    PieceType type = PieceType::Pawn;
    PieceColor color;
    bool isEmpty = true;
};