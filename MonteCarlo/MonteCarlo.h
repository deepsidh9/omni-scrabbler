#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include "../MoveGenerator/MoveGenerator.h"

#include "../Models/Board.h"
#include "../Models/Rack.h"
#include "../Models/Move.h"
#include "../Models/Bag.h"
#include <chrono>


struct timer
{
    typedef std::chrono::steady_clock clock ;
    typedef std::chrono::seconds seconds ;

    void reset() { start = clock::now() ; }

    unsigned long long seconds_elapsed() const
    { return std::chrono::duration_cast<seconds>( clock::now() - start ).count() ; }

    private: clock::time_point start = clock::now() ;
};

//NodeState struct will contain meta data
struct NodeState
{
    int treeDepth;                //depth 0 and 2 represent my turn and depth 1 is the oponent's turn.
    vector<Move> possibleActions; //possible moves to play from the current state of the board.
	
    //paramaters used to calculate the UCT of the node.
    long double reward, UCB; //currently eqauls to the score of the move only.
    int nbOfVisits;     //number of visits of the current node.

};

//The nodes of the tree will contain the current state of the game board.
struct NodeMC
{
    Board boardState; //current game state.
    Bag currentBag;
    Rack rack;
  	Rack oldRack;
    NodeMC *Parent;
    NodeState nodeState;
    vector<NodeMC *> children;
};

class MonteCarlo
{

private:
    int numTilesByOpponent_;
    //function to populate the first level.
    void firstLevel();
	  MoveGenerator* movGen;
    //Use heuristics to caluclate the reward of a certain move.
    double calculateMoveReward(Move move);

    //Utility function to calculate the UCB.
    void calculateUCB(NodeMC *node);

    //get the node with the best UCB.
    NodeMC *promisingNode(NodeMC *root);

    //backpropagation function.
    void Rollout(NodeMC *node);

    //adds children to the node of choice.
    void Expand(NodeMC *node);

public:
    //Root of the tree containing the current and main state of the game.
    NodeMC *Root;
    Rack mainRack;
    Rack oponentRack; //oponent RAck will be generated in the constructor.
	map<string, double>* syn2;
	map<char, double>* worth;
    bool MidGame;
    //constructor.
    MonteCarlo(Board boardState, vector<Move> Moves, Rack currentRack, Rack oponentRack, Bag bag, MoveGenerator *movGen, map<string, double>* syn2, map<char, double>* worth,bool MidGame, int numTilesByOpponent);
	Rack GenerateRack(Rack r, NodeMC* node);
    //adding new node to the tree.
    NodeMC *newNode(Board boardState, vector<Move> Moves, Rack currentRack , Rack oldRack, Bag bag, NodeMC *parent, int level,double reward);

    //traverse the tree.
    void LevelOrderTraversal(NodeMC *root);

    //Function to start the simulation.
	int Simulation();

};