#include "Tile.h"

Tile::Tile()
{
}

void Tile::GetIndex(int & row, int & column)
{
	row = row_;
	column = column_;
}

int Tile::GetScore()
{
	return score_;
}
char Tile::GetLetter()
{
	return this->letter_;
}
void Tile::SetLetter(char c)
{
	this->letter_=c;
	
}
int Tile::GetBonus()
{
	return bonus_;
}
void Tile::SetScore(int score)
{
	score_=score;
}

void Tile::SetParams(char letter,int row,int col,int score,int bonus)
{
	letter_=letter;
	row_=row;
	column_=col;
	score_=score;
	bonus_=bonus;
}

void Tile::SetBonus(int  bonus){
	bonus_=bonus;
}

void Tile::CalculateTotalScore()
{
	this->score_ = this->score_ * this->bonus_;
}



Tile::~Tile()
{
}

void Tile::setPosition(int row, int col){
	this->row_ = row;
	this->column_ = col;
}