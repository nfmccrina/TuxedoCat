/*
* The MIT License (MIT)
*
* Copyright (c) 2016 Nathan McCrina
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation 
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/

#include "../include/Piece.hpp"
#include <cctype>

using namespace TuxedoCat;

Piece::Piece()
    : color(Color::NONE), rank(Rank::NONE), square(0x00ULL)
{
}

Piece::Piece(Color c, Rank r, uint64_t s)
    : color(c), rank(r), square(s)
{
}

Piece::Piece(const Piece& p)
    : color(p.color), rank(p.rank), square(p.square)
{
}

bool Piece::isValid() const
{
    return color != Color::NONE &&
        rank != Rank::NONE &&
        square != 0;
}

std::string Piece::toString() const
{
    std::string chRank = rankToString(rank);

    if (color == Color::WHITE)
    {
        chRank[0] = std::toupper(chRank[0]);
    }

    return chRank;
}
