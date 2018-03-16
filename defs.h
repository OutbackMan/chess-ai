#ifndef __defs_h__
#define __defs_h__

#include <stdint.h>

#define BRD_SQ_NUM 120

typedef uint64_t u64;

enum { EMPTY, WP, WN, WB, WR, WQ, WK, BP, BN, BB, BR, BQ, BK };
enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };
enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };
enum { WHITE, BLACK, BOTH };

enum {
	A1 = 21, A2, A3, A4, A5, A6, A7, A8,
	B1 = 31, B2, B3, B4, B5, B6, B7, B8,
	C1 = 41, C2, C3, C4, C5, C6, C7, C8,
	D1 = 51, D2, D3, D4, D5, D6, D7, D8,
	E1 = 61, E2, E3, E4, E5, E6, E7, E8,
	F1 = 71, F2, F3, F4, F5, F6, F7, F8,
	G1 = 81, G2, G3, G4, G5, G6, G7, G8,
	H1 = 91, H2, H3, H4, H5, H6, H7, H8, NO_SQ
};

typedef struct {
	int pieces[BRD_SQ_NUM];		

	// each array for white, black and both pawn colours respectively
	// bit will be 1 if square is owned by particular colour
	u64 pawns[3]; 

	// kings
	int king_sq[2];

	int side;
	int en_pas;
	int fifty_move;

	int ply;
	int his_ply;

	u64 pos_key;

	int piece_num[13];
	int big_piece[3];
	int major_piece[3];
	int minor_piece[3];

} Board;

#endif
