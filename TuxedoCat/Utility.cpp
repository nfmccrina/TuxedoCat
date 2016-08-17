/*
* The MIT License (MIT)
*
* Copyright (c) 2016 Nathan McCrina
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*/

 /*
  * LSB, MSB and PopCount code is from the chess programming wiki (chessprogramming.wikispaces.com)
  */

#include "TuxedoCat.h"
#include <sstream>
#include <cctype>
#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>
#include <iomanip>

#ifdef _WIN32
#include <time.h>
#endif

using namespace TuxedoCat;

/*
 wiki constants
 */

static const uint64_t k1 = 0x5555555555555555ULL;
static const uint64_t k2 = 0x3333333333333333ULL;
static const uint64_t k4 = 0x0f0f0f0f0f0f0f0fULL;
static const uint64_t kf = 0x0101010101010101ULL;

static int index64[64] = {
	0,  1, 48,  2, 57, 49, 28,  3,
	61, 58, 50, 42, 38, 29, 17,  4,
	62, 55, 59, 36, 53, 51, 43, 22,
	45, 39, 33, 30, 24, 18, 12,  5,
	63, 47, 56, 27, 60, 41, 37, 16,
	54, 35, 52, 21, 44, 32, 23, 11,
	46, 26, 40, 15, 34, 20, 31, 10,
	25, 14, 19,  9, 13,  8,  7,  6
};

static int index64Reverse[64] = {
	0, 47,  1, 56, 48, 27,  2, 60,
	57, 49, 41, 37, 28, 16,  3, 61,
	54, 58, 35, 52, 50, 42, 21, 44,
	38, 32, 29, 23, 17, 11,  4, 62,
	46, 55, 26, 59, 40, 36, 15, 53,
	34, 51, 20, 43, 31, 22, 10, 45,
	25, 39, 14, 33, 19, 30,  9, 24,
	13, 18,  8, 12,  7,  6,  5, 63
};

/*
 end wiki constants
*/

std::vector<std::string> Utility::split(std::string str, std::string delimiter)
{
	size_t tmpIndex;
	std::string tmpStr;
	std::vector<std::string> result;

	tmpStr = str.substr();

	do
	{
		tmpIndex = tmpStr.find(delimiter);

		result.push_back(tmpStr.substr(0, tmpIndex));

		if (tmpIndex != std::string::npos)
		{
			tmpIndex++;
			tmpStr = tmpStr.substr(tmpIndex);
		}
		else
		{
			tmpStr.clear();
		}

	} while (!tmpStr.empty());

	return result;
}

uint64_t Utility::GetSquareFromAlgebraic(std::string algebraic)
{
	int rank = 0;
	int file = static_cast<int>(algebraic[0]);

	file -= 97;

	if (std::isdigit(algebraic[1]))
	{
		rank = std::stoi(algebraic.substr(1)) - 1;
	}

	return 0x0000000000000001ULL << ((rank * 8) + file);
}

int Utility::GetLSB(uint64_t bitmask)
{
	/*
	* begin wiki code
	*/

	if (bitmask == 0x0000000000000000ULL)
	{
		return -1;
	}

	uint64_t debruijn64 = 0x03F79D71B4CB0A89ULL;
	return index64[((bitmask & ((~bitmask) + 1)) * debruijn64) >> 58];

	/*
	* end wiki code
	*/
}

int Utility::GetMSB(uint64_t bitmask)
{
	/*
	* begin wiki code
	*/

	if (bitmask == 0x0000000000000000ULL)
	{
		return -1;
	}

	uint64_t debruijn64 = 0x03F79D71B4CB0A89ULL;
	bitmask |= bitmask >> 1;
	bitmask |= bitmask >> 2;
	bitmask |= bitmask >> 4;
	bitmask |= bitmask >> 8;
	bitmask |= bitmask >> 16;
	bitmask |= bitmask >> 32;
	return index64Reverse[(bitmask * debruijn64) >> 58];

	/*
	* end wiki code
	*/
}

int Utility::PopCount(uint64_t bitmask)
{
	/*
	* begin wiki code
	*/

	uint64_t x = bitmask;

	x = x - ((x >> 1) & k1);
	x = (x & k2) + ((x >> 2) & k2);
	x = (x + (x >> 4)) & k4;
	x = (x * kf) >> 56;

	return (int)x;

	/*
	* end wiki code
	*/
}

uint64_t Utility::GetMaskFromRankOrFile(char rankorfile)
{
	uint64_t result = 0;

	std::string files = "abcdefgh";
	std::string ranks = "12345678";

	if (files.find(rankorfile) != std::string::npos)
	{
		result = 0x0101010101010101ULL;

		result = result << (rankorfile - 97);
	}
	else if (ranks.find(rankorfile) != std::string::npos)
	{
		result = result << ((rankorfile - 49) * 8);
	}

	return result;
}

TuxedoCat::Move Utility::GetMoveFromXBoardNotation(Board& position, std::string moveString)
{
	Move move;
	std::vector<Move> legalMoves;
	bool isLegalMove = false;

	move.SourceLocation = GetSquareFromAlgebraic(moveString.substr(0, 2));
	move.TargetLocation = GetSquareFromAlgebraic(moveString.substr(2, 2));
	move.MoveColor = position.ColorToMove;
	move.CastlingStatus = position.CastlingStatus;
	move.CurrentEnPassant = position.EnPassantTarget;
	move.CurrentHalfMoves = position.HalfMoveCounter;
	move.MovingPiece = Position::GetRankAt(position, move.SourceLocation);
	
	if (moveString.length() == 5)
	{
		if (moveString[4] == 'q')
		{
			move.PromotedRank = PieceRank::QUEEN;
		}
		else if (moveString[4] == 'r')
		{
			move.PromotedRank = PieceRank::ROOK;
		}
		else if (moveString[4] == 'b')
		{
			move.PromotedRank = PieceRank::BISHOP;
		}
		else if (moveString[4] == 'n')
		{
			move.PromotedRank = PieceRank::KNIGHT;
		}
	}
	else
	{
		move.PromotedRank = PieceRank::NONE;
	}

	if (position.ColorToMove == PieceColor::WHITE)
	{
		if (move.MovingPiece == PieceRank::PAWN && move.TargetLocation == position.EnPassantTarget)
		{
			move.CapturedPiece = Position::GetRankAt(position, ((move.TargetLocation >> 8) & position.BlackPawns));
		}
		else
		{
			move.CapturedPiece = Position::GetRankAt(position, (move.TargetLocation & position.BlackPieces));
		}
	}
	else if (position.ColorToMove == PieceColor::BLACK)
	{
		if (move.MovingPiece == PieceRank::PAWN && move.TargetLocation == position.EnPassantTarget)
		{
			move.CapturedPiece = Position::GetRankAt(position, ((move.TargetLocation << 8) & position.WhitePawns));
		}
		else
		{
			move.CapturedPiece = Position::GetRankAt(position, (move.TargetLocation & position.WhitePieces));
		}
	}

	legalMoves = MoveGenerator::GenerateMoves(position);

	for (auto it = legalMoves.begin(); it != legalMoves.end(); it++)
	{
		if (move == *it)
		{
			isLegalMove = true;
			break;
		}
	}

	if (!isLegalMove)
	{
		move.TargetLocation = 0;
	}

	return move;
}

std::string Utility::GenerateXBoardNotation(Move move)
{
	std::stringstream ss;
	std::string result = "";

	ss << GetFileFromLocation(move.SourceLocation) << GetRankFromLocation(move.SourceLocation)
		<< GetFileFromLocation(move.TargetLocation) << GetRankFromLocation(move.TargetLocation);

	if (move.PromotedRank != PieceRank::NONE)
	{
		if (move.PromotedRank == PieceRank::KNIGHT)
		{
			ss << 'n';
		}
		else if (move.PromotedRank == PieceRank::BISHOP)
		{
			ss << 'b';
		}
		else if (move.PromotedRank == PieceRank::ROOK)
		{
			ss << 'r';
		}
		else if (move.PromotedRank == PieceRank::QUEEN)
		{
			ss << 'q';
		}
	}
	
	result = ss.str();

	return result;
}

std::string Utility::RankToString(PieceRank rank)
{
	std::string result = "";

	if (rank == PieceRank::PAWN)
	{
		result = "pawn";
	}
	else if (rank == PieceRank::KNIGHT)
	{
		result = "knight";
	}
	else if (rank == PieceRank::BISHOP)
	{
		result = "bishop";
	}
	else if (rank == PieceRank::ROOK)
	{
		result = "rook";
	}
	else if (rank == PieceRank::QUEEN)
	{
		result = "queen";
	}
	else if (rank == PieceRank::KING)
	{
		result = "king";
	}
	else
	{
		result = "none";
	}

	return result;
}

std::string Utility::ColorToString(PieceColor color)
{
	std::string result;

	if (color == PieceColor::WHITE)
	{
		result = "white";
	}
	else if (color == PieceColor::BLACK)
	{
		result = "black";
	}
	else
	{
		result = "none";
	}

	return result;
}

std::string Utility::CastlingStatusToString(int flags)
{
	std::stringstream result;

	if (flags & CastlingFlags::WHITE_SHORT)
	{
		result << "K";
	}

	if (flags & CastlingFlags::WHITE_LONG)
	{
		result << "Q";
	}

	if (flags & CastlingFlags::BLACK_SHORT)
	{
		result << "k";
	}

	if (flags & CastlingFlags::BLACK_LONG)
	{
		result << "q";
	}

	return result.str();
}

std::string Utility::PrintMove(Move move)
{
	std::stringstream moveString;

	moveString << std::endl << "********" << std::endl;
	moveString << "Source: " << GetFileFromLocation(move.SourceLocation) << GetRankFromLocation(move.SourceLocation) << std::endl;
	moveString << "Target: " << GetFileFromLocation(move.TargetLocation) << GetRankFromLocation(move.TargetLocation) << std::endl;
	moveString << "Piece: " << RankToString(move.MovingPiece) << std::endl;
	moveString << "Color: " << ColorToString(move.MoveColor) << std::endl;
	moveString << "Captured Piece: " << RankToString(move.CapturedPiece) << std::endl;
	moveString << "Promoted Rank: " << RankToString(move.PromotedRank) << std::endl;
	moveString << "Castling Status: " << CastlingStatusToString(move.CastlingStatus) << std::endl;

	if (move.CurrentEnPassant != 0)
	{
		moveString << "En Passant: " << GetFileFromLocation(move.CurrentEnPassant) << GetRankFromLocation(move.CurrentEnPassant) << std::endl;
	}
	else
	{
		moveString << "En Passant: none" << std::endl;
	}

	moveString << "Half Move Counter: " << move.CurrentHalfMoves << std::endl;
	moveString << "********" << std::endl;

	return moveString.str();
}

std::string Utility::GenerateSAN(Board& position, Move move, std::vector<Move> allMoves)
{
	std::stringstream san;
	bool isCastle = false;

	if (move.MovingPiece != PieceRank::PAWN)
	{
		if (move.MovingPiece == PieceRank::KNIGHT)
		{
			san << "N";
		}
		else if (move.MovingPiece == PieceRank::BISHOP)
		{
			san << "B";
		}
		else if (move.MovingPiece == PieceRank::ROOK)
		{
			san << "R";
		}
		else if (move.MovingPiece == PieceRank::QUEEN)
		{
			san << "Q";
		}
		else if (move.MovingPiece == PieceRank::KING)
		{
			if ((move.SourceLocation == 0x0000000000000010UL && move.TargetLocation == 0x0000000000000004UL)
				|| (move.SourceLocation == 0x0000000000000010UL && move.TargetLocation == 0x0000000000000040UL)
				|| (move.SourceLocation == 0x1000000000000000UL && move.TargetLocation == 0x0400000000000000UL)
				|| (move.SourceLocation == 0x1000000000000000UL && move.TargetLocation == 0x4000000000000000UL))
			{
				isCastle = true;
			}
			else
			{
				san << "K";
			}
		}

		if (!isCastle)
		{
			uint64_t pieces = 0x0000000000000000UL;

			if (move.MovingPiece == PieceRank::KNIGHT)
			{
				pieces =
					move.MoveColor == PieceColor::WHITE ? position.WhiteKnights : position.BlackKnights;
			}
			else if (move.MovingPiece == PieceRank::BISHOP)
			{
				pieces =
					move.MoveColor == PieceColor::WHITE ? position.WhiteBishops : position.BlackBishops;
			}
			else if (move.MovingPiece == PieceRank::ROOK)
			{
				pieces =
					move.MoveColor == PieceColor::WHITE ? position.WhiteRooks : position.BlackRooks;
			}
			else if (move.MovingPiece == PieceRank::QUEEN)
			{
				pieces =
					move.MoveColor == PieceColor::WHITE ? position.WhiteQueens : position.BlackQueens;
			}


			if (PopCount(pieces) > 1)
			{
				std::vector<Move> possibleMoves;
				
				for (auto it = allMoves.cbegin(); it != allMoves.cend(); it++)
				{
					if (it->MovingPiece == move.MovingPiece
						&& it->MoveColor == move.MoveColor
						&& it->TargetLocation == move.TargetLocation
						&& it->SourceLocation != move.SourceLocation)
					{
						possibleMoves.push_back(*it);
					}
				}

				if (possibleMoves.size() != 0)
				{
					bool conflictFile = false;
					bool conflictRank = false;

					for (auto it = possibleMoves.cbegin(); it != possibleMoves.cend(); it++)
					{
						if (GetFileFromLocation(it->SourceLocation) == GetFileFromLocation(move.SourceLocation))
						{
							conflictFile = true;
						}

						if (GetRankFromLocation(it->SourceLocation) == GetRankFromLocation(move.SourceLocation))
						{
							conflictRank = true;
						}
					}

					if (!conflictFile)
					{
						san << GetFileFromLocation(move.SourceLocation);
					}
					else if (!conflictRank)
					{
						san << GetRankFromLocation(move.SourceLocation);
					}
					else
					{
						san << GetFileFromLocation(move.SourceLocation);
						san << GetRankFromLocation(move.SourceLocation);
					}
				}
			}

			if (move.CapturedPiece != PieceRank::NONE)
			{
				san << "x";
			}

			san << GetFileFromLocation(move.TargetLocation);
			san << GetRankFromLocation(move.TargetLocation);
		}
		else
		{
			if (move.TargetLocation == 0x0000000000000040UL || move.TargetLocation == 0x4000000000000000UL)
			{
				san << "0-0";
			}
			else
			{
				san << "0-0-0";
			}
		}
	}
	else
	{
		if (move.CapturedPiece != PieceRank::NONE)
		{
			san << GetFileFromLocation(move.SourceLocation);
			san << "x";
		}

		san << GetFileFromLocation(move.TargetLocation);
		san << GetRankFromLocation(move.TargetLocation);

		if (move.TargetLocation == position.EnPassantTarget
			&& move.CapturedPiece != PieceRank::NONE)
		{
			san << "e.p";
		}

		if (move.PromotedRank != PieceRank::NONE)
		{
			san << "=";

			if (move.PromotedRank == PieceRank::KNIGHT)
			{
				san << "N";
			}
			else if (move.PromotedRank == PieceRank::BISHOP)
			{
				san << "B";
			}
			else if (move.PromotedRank == PieceRank::ROOK)
			{
				san << "R";
			}
			else if (move.PromotedRank == PieceRank::QUEEN)
			{
				san << "Q";
			}
		}
	}

	Position::Make(position, move);

	if ((move.MoveColor == PieceColor::WHITE && MoveGenerator::IsSquareAttacked(position.BlackKing, position))
		|| (move.MoveColor == PieceColor::BLACK && MoveGenerator::IsSquareAttacked(position.WhiteKing, position)))
	{
		san << "+";
	}

	Position::Unmake(position, move);

	return san.str();
}

std::string Utility::GetFileFromLocation(uint64_t location)
{
	uint64_t mask = 0x0101010101010101UL;
	char file = ' ';
	std::stringstream ss;

	for (char index = 0; index < 8; index++)
	{
		if ((mask & location) != 0x0000000000000000UL)
		{
			file = index + 97;
			break;
		}

		mask = mask << 1;
	}

	ss << file;
	return ss.str();
}

int Utility::GetRankFromLocation(uint64_t location)
{
	uint64_t mask = 0x00000000000000FFUL;
	int index = 0;

	for (index = 1; index < 9; index++)
	{
		if ((mask & location) != 0x0000000000000000UL)
		{
			break;
		}

		mask = mask << 8;
	}

	return index;
}

void Utility::WriteLog(std::string msg)
{
	std::ofstream fout;
	std::time_t currentTime;
    
    #ifdef _WIN32
	std::tm tm_struct;
    #endif

	currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	fout.open("log.txt", std::ios_base::app);

    #ifdef _WIN32
	localtime_s(&tm_struct, &currentTime);
	fout << std::put_time(&tm_struct, "%c") << ": " << msg << std::endl;
    #else
    fout << std::put_time(localtime(&currentTime), "%c") << ": " << msg << std::endl;
    #endif

	fout.close();
}

bool Utility::ComparePieces(PieceRank pr1, PieceRank pr2)
{
	bool result = false;

	if (pr1 == PieceRank::PAWN && pr2 != PieceRank::PAWN)
	{
		result = true;
	}
	else if (pr1 == PieceRank::KNIGHT && (pr2 != PieceRank::PAWN && pr2 != PieceRank::KNIGHT))
	{
		result = true;
	}
	else if (pr1 == PieceRank::BISHOP && (pr2 != PieceRank::PAWN
		&& pr2 != PieceRank::KNIGHT && pr2 != PieceRank::BISHOP))
	{
		result = true;
	}
	else if (pr1 == PieceRank::ROOK && (pr2 != PieceRank::PAWN
		&& pr2 != PieceRank::KNIGHT && pr2 != PieceRank::BISHOP
		&& pr2 != PieceRank::ROOK))
	{
		result = true;
	}
	else if (pr1 == PieceRank::QUEEN && (pr2 != PieceRank::PAWN
		&& pr2 != PieceRank::KNIGHT && pr2 != PieceRank::BISHOP
		&& pr2 != PieceRank::ROOK && pr2 != PieceRank::QUEEN))
	{
		result = true;
	}

	return result;
}