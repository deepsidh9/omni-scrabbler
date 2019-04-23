#include "Move.h"
#include <vector>


Move::Move()
{
}

void Move::SetPlay(Play* p){
	this->play_ = p;

}


void Move::SetRack(Rack rack){
	this->rack_=rack;
}


void Move::SetRack(string newRack)
{
	for (int i = 0; i < (int)newRack.size(); i++) {
		this->rack_.SetTile(newRack[i],i);
	}
}

Play* Move::GetPlay()
{
	return play_;
}

string Move::GetRack()
{
	string s ="";
	for (int j = 0; j < 7; j++)
	{
		s+= rack_.GetLetter(j);
	}
	return s;
}

double Move::GetScore()
{
	return heuristicscore_;
}


void Move::SetScore(double score)
{
	heuristicscore_ = score;
}

void Move::CalculateScore()
{
	this->heuristicscore_ = this->play_->GetScore()-this->penalty_;

}

void Move::SetPenalty(double penalty){
	this->penalty_ = penalty;
}

Move::~Move()
{
}
