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
    None,
};

struct Piece
{
    PieceType type = PieceType::Pawn;
    PieceColor color = PieceColor::None;
    bool isEmpty = true;
};