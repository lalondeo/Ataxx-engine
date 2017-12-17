#include "board.h"

board::board(BoardType _board_type) : board_type(_board_type) 
{
	board_representation = new char[PRODUCTDIMENSIONS];
}

board::board() : board_type(NORMAL)
{
	board_representation = new char[PRODUCTDIMENSIONS];
}


board::~board()
{
	delete[] board_representation;
}


void board::init_board()
{
	for(char pos = 0; pos < PRODUCTDIMENSIONS; pos++)
	{
		if(pos % WIDTH <= 1 || pos % WIDTH >= WIDTH - 2 || pos < 2 * HEIGHT || pos > PRODUCTDIMENSIONS - 2 * HEIGHT)
			board_representation[pos] = BLOCKED_SQUARE;
		else
			board_representation[pos] = EMPTY_SQUARE;
	}
	set_type(BLUE_STONE, 0, 6);
	set_type(BLUE_STONE, 6, 0);
	set_type(RED_STONE, 0, 0);
	set_type(RED_STONE, 6, 6);
	side_to_move = BLUE;
	
}

inline int abs(int x)
{
	return (x < 0 ? -x : x);
}

void board::print_board()
{
	for(char y = 6; y >= 0; y--)
	{
		for(char x = 0; x < 7; x++)
			printf("%c", get_square_repr(get_type(x, y)));
		
		printf("\n");
	}
}

char board::get_type(char x, char y)
{
	return board_representation[(y + 2) * WIDTH + x + 2];
}

void board::set_type(char type, char x, char y)
{
	board_representation[(y + 2) * WIDTH + x + 2] = type;
}

void board::generate_moves(short int * moves)
{
	const char friendly_stone = side_to_move == BLUE ? BLUE_STONE : RED_STONE;
	const char ennemy_stone = side_to_move == BLUE ? RED_STONE : BLUE_STONE;
	const char duplication_origin_square = PRODUCTDIMENSIONS + (side_to_move == BLUE ? 0 : 1);
	unsigned int iMoves = 0;
	int move;
	char type;
	for(char x = 0; x < 7; x++)
		for(char y = 0; y < 7; y++)
		{
			type = get_type(x, y);
			if(type != BLOCKED_SQUARE && type != ennemy_stone)
			{
				if(type == EMPTY_SQUARE)
				{
					bool has_neighbor = false;
					// Check all directly neighboring squares
					for(char deltax = -1; deltax <= 1; deltax++)
						for(char deltay = -1; deltay <= 1; deltay++)
						{
							if(get_type(x + deltax, y + deltay) == friendly_stone)
							{
								has_neighbor = true;
								goto end;
							}
						}
					end:
					if(has_neighbor)
					{
						move = (((short int)duplication_origin_square) << 8) + (y + 2) * WIDTH + x + 2;
						moves[iMoves] = move;
						iMoves++;
					}
				}
				else
				{
					for(char deltax = -2; deltax <= 2; deltax++)
					{
						for(char deltay = -2; deltay <= 2; deltay++)
						{
							if(abs(deltax) < 2 && abs(deltay) < 2) continue;
							
							if(get_type(x + deltax, y + deltay) == EMPTY_SQUARE)
							{
								move = (((y + 2) * WIDTH + x + 2) << 8) + (y + 2 + deltay) * WIDTH + x + deltax + 2;
								moves[iMoves] = move;
								iMoves++;
							}
						}
					}
				}
			}
		}
	
	moves[iMoves] = NULLMOVE;
	
		
}

void board::play_move(short int move)
{
	char origin_square = move >> 8;
	char destination_square = (move << 8) >> 8;
	if(origin_square < PRODUCTDIMENSIONS) {board_representation[origin_square] = EMPTY_SQUARE; }
	
	const char friendly_stone = side_to_move == BLUE ? BLUE_STONE : RED_STONE;
	const char ennemy_stone = side_to_move == BLUE ? RED_STONE : BLUE_STONE;
	
	board_representation[destination_square] = friendly_stone;
	for(char delta1 = -1; delta1 <= 1; delta1++)
		for(char delta2 = -WIDTH; delta2 <= WIDTH; delta2+= WIDTH)
			if(board_representation[destination_square + delta1 + delta2] == ennemy_stone)
				board_representation[destination_square + delta1 + delta2] = friendly_stone;
	
	if(side_to_move == BLUE) side_to_move = RED;
	else side_to_move = BLUE;
}

void board::load_board(const char * board_representation, Sides side_to_move)
{
	this->side_to_move = side_to_move;
	for(int pos = 0; pos < PRODUCTDIMENSIONS; pos++)
		this->board_representation[pos] = board_representation[pos];
}

void board::load_board(board & _board)
{
	load_board(_board.board_representation, _board.side_to_move);
}

bool board::compare(board & _board)
{
	bool is_identical = true;
	is_identical &= _board.side_to_move == side_to_move;
	for(int i = 0; i < PRODUCTDIMENSIONS && is_identical; i++)
		is_identical &= _board.board_representation[i] == board_representation[i];
	
	return is_identical;
}

uint64_t _perft(board boards[20], short int all_moves[20][100], int depth_to_go);

uint64_t perft(int depth)
{

	short int all_moves[20][100];
	board boards[20];

	boards[depth - 1].init_board();
	uint64_t res = _perft(boards, all_moves, depth - 1);
	
	return res;
	
}

uint64_t _perft(board boards[20], short int all_moves[20][100], int depth_to_go)
{
	uint64_t tot = 0;
	
	board & currentboard = boards[depth_to_go];
	
	Sides side_to_move = currentboard.side_to_move;
	const char * board_representation = currentboard.board_representation;
	currentboard.generate_moves(all_moves[depth_to_go]);
	
	if(depth_to_go == 0)
		for(unsigned int iMoves = 0; all_moves[depth_to_go][iMoves] != 0; iMoves++) tot++;

	else
	{
		
		board & nextboard = boards[depth_to_go - 1]; 
		for(unsigned int iMoves = 0; all_moves[depth_to_go][iMoves] != 0; iMoves++)
		{
			nextboard.load_board(board_representation, side_to_move);
			nextboard.play_move(all_moves[depth_to_go][iMoves]);
			tot += _perft(boards, all_moves, depth_to_go - 1);
		}
	}
	
	return tot;
	
	
	
}
