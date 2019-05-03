#pragma once
#include"../Models/Bag.h"
#include"../Models/Rack.h"
#include"../Models/Board.h"
#include"../Models/Move.h"
#include"../MoveGenerator/MoveGenerator.h"
#include"Evaluator.h"
#include <algorithm>
#include <random>

class PreendgameEvaluator : public Evaluator {
    Rack enemyrack_;
    Move enemymove_;
    Board* board_;
    MoveGenerator *movegenerator_;
    vector<Move> possiblemoves_;
    int numTilesByOpponent_; //number of tiles played by oponent in his last turn
    vector<char> remainingletters_; //max size=16
    map<string, double> * doubleValued_; //temporary
    double CalculateLeave(string); //temporary
    int GetBonus(int rowIterator, int columnIterator, Board board);
    double CalculatePenalty(Move * move, Board board);
    Move ComputeBestMove(Rack* opponentRack = NULL);
    void Combination(int start, int size, string &rem, string &rackleave, double &MaxScore, string &BestRackLeave);
    void OpponentRackEstimation(); //public for testing only, should be private later
    Move OpponentBestMove();
public:
    
    PreendgameEvaluator();
    PreendgameEvaluator(map<string, double> *,Board* board, MoveGenerator * movGen,vector<Move> possibleMoves, vector <char> remLetters, int numTilesByOpponent);
    Rack GetEnemyRack();
    double Evaluate(Move * move);
    vector<Move>* EvaluateGame(Rack* opponentRack = NULL);
    vector<Move>* Evaluate();
    ~PreendgameEvaluator();    
};