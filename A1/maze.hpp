// Termm--Fall 2020

#pragma once

class Maze
{
public:
enum class Direction {
	Down = 0,
	Left = 1,
	Up = 2,
	Right = 3
};

enum class MoveState {
	Normal = 0,
	Dashing
};

public:
	Maze( size_t dim );
	~Maze();

	void reset();

	size_t getDim() const;

	int getValue( int x, int y ) const;

	void setValue( int x, int y, int h );

	void digMaze();

	void move(Direction d);

	int getAvatarRow();

	int getAvatarCol();

	void setMoveState(MoveState s);

	void printMaze(); // for debugging

private:
	size_t m_dim;
	int *m_values;
	int m_loc_avatar[2];
	MoveState m_move_state;

	void recDigMaze(int r, int c);
	int numNeighbors(int r, int c);
};
