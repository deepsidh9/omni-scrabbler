#include <iostream>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include "math.h"

#include "./MonteCarlo.h"
#include "../Models/Bag.h"
#include "../Models/Tile.h"
#include "../Models/Play.h"
#include "../Models/Tile.h"
#include "../MoveGenerator/MoveGenerator.h"

#define INTMAX = 2147483647;

using namespace std;

MonteCarlo::MonteCarlo(Board boardState, vector<Move> Moves, Rack currentRack, Rack oponentRack, Bag bag, MoveGenerator *movGen)
{
    NodeMC *temp = new NodeMC;
    Rack tempRack = currentRack;
    this->movGen = movGen;
    temp->boardState = boardState;
    temp->Rack = tempRack;
    temp->Parent = nullptr;

    temp->nodeState.treeDepth = 0;
    temp->nodeState.possibleActions = Moves;
    temp->nodeState.reward = 0;
    temp->nodeState.nbOfVisits = 0;

    temp->nodeState.UCB = 0;
    temp->currentBag = bag;

    vector<NodeMC *> children;
    temp->children = children;

    this->mainRack = tempRack;
    this->oponentRack = oponentRack;
    //populate the first level of actions.
    // i need the new rack of each state
    // board state
    // parent to be set to the root
    //depth
    //set of moves
    //
    this->Root = temp;
    //should generate the oponent Rack.
    firstLevel();
}

void MonteCarlo::firstLevel()
{
    //temp rack to use until we implement the random function.
    //we should use the bag to generate the random racks for each new state inside the loop.
    Tile RackTiles[7];

    RackTiles[0].SetLetter('f');
    RackTiles[0].SetScore(1);
    RackTiles[1].SetLetter('a');
    RackTiles[1].SetScore(4);
    RackTiles[2].SetLetter('l');
    RackTiles[2].SetScore(3);
    RackTiles[3].SetLetter('e');
    RackTiles[3].SetScore(2);
    RackTiles[4].SetLetter('m');
    RackTiles[4].SetScore(1);
    RackTiles[5].SetLetter('o');
    RackTiles[5].SetScore(4);
    RackTiles[6].SetLetter('r');
    RackTiles[6].SetScore(3);

    vector<Tile> rackTiles(RackTiles, RackTiles + sizeof RackTiles / sizeof RackTiles[0]);
    Rack rack(rackTiles);

    Board tempLevel1Board = this->Root->boardState;

    //loop over the number of possible actions to make in order to get all the possible states in the level.
    for (int i = 0; i < 10; i++)
    {

        tempLevel1Board = this->Root->boardState;
        //generate the first board state after my move.
        tempLevel1Board.SimulateMove(&(this->Root->nodeState.possibleActions[i]));

        double reward = calculateMoveReward(this->Root->nodeState.possibleActions[i]);
        //using the move generator to generate new set of moves for each action.
        vector<Move> nextMoves;

        nextMoves = this->movGen->Generate(&this->oponentRack, tempLevel1Board, tempLevel1Board.GetCount() == 0);
        vector<Move> simVec;
        for (int j = 0; j < 10; j++)
        {
            simVec.push_back(nextMoves.at(j));
        }

        this->Root->children.push_back(newNode(tempLevel1Board, simVec, rack, Root->currentBag, Root, 1, reward));
    }
    cout << "done making first level" << endl;
}

NodeMC *MonteCarlo::newNode(Board boardState, vector<Move> Moves, Rack currentRack, Bag bag, NodeMC *parent, int level, double reward)
{
    NodeMC *temp = new NodeMC;
    Rack tempRack = currentRack;

    temp->boardState = boardState;
    temp->Rack = tempRack;
    temp->Parent = parent;

    vector<NodeMC *> children;
    temp->children = children;

    temp->nodeState.treeDepth = level;
    temp->nodeState.possibleActions = Moves;

    //the reward should be the score of the move.
    temp->nodeState.reward = reward;
    temp->nodeState.nbOfVisits = 0;
    temp->nodeState.UCB = 100000000;

    return temp;
}

void MonteCarlo::LevelOrderTraversal(NodeMC *root)
{
    if (root == NULL)
        return;

    queue<NodeMC *> q;
    q.push(root);
    while (!q.empty())
    {
        int n = q.size();
        while (n > 0)
        {
            NodeMC *p = q.front();
            q.pop();
            Tile *tiles[15][15];
            p->boardState.GetTiles(tiles);

            cout << "child begin ... " << endl;
            for (int i = 0; i < 15; i++)
            {
                for (int j = 0; j < 15; j++)
                {
                    cout << tiles[i][j]->GetLetter() << " ";
                }
                cout << endl;
            }
            cout << "child end ..." << endl;
            for (int i = 0; i < p->children.size(); i++)
                q.push(p->children[i]);
            n--;
        }

        cout << endl;
    }
}

void MonteCarlo::calculateUCB(NodeMC *node)
{
    //calculate UCB
    //Assume C = root(2)
    //http://mcts.ai/about/
    //Warning don't calculate the parent's UCB
    node->nodeState.UCB = node->nodeState.reward + sqrt(2) * sqrt(log(node->Parent->nodeState.nbOfVisits) / node->nodeState.nbOfVisits);
}

double MonteCarlo::calculateMoveReward(Move move)
{
    return move.GetScore(); //+ move.GetRackLeave() + move.GetPenalty();
}

NodeMC *MonteCarlo::promisingNode(NodeMC *root)
{
    //Promising node's reward is calculated here
    //UCB is calculated on rollout
    NodeMC *Max = root->children.at(0);
    for (int i = 0; i < root->children.size(); i++)
    {
        if (root->children.at(i)->nodeState.UCB > Max->nodeState.UCB)
        {
            Max = root->children.at(i);
        }
    }
    return Max;
}

void MonteCarlo::Rollout(NodeMC *node)
{
    // //E7na hanrollout el node direct ahan kan el level
    // //Bs msh hano3od ntl3
    // //y3ni lw kona @node fi level=3 han3ml rollout(elNodeDeh) w bas msh han3ml rollout(elNodeDeh); rollout(elNodeDehwElFo2eha); y3ni msh recusrive
    // switch (node->nodeState.treeDepth)
    // {
    // case 0:
    // 	//Current state no Simulations
    // 	node->nodeState.nbOfVisits++;
    // 	break;
    // case 1:
    // 	//State after my move
    // 	//node->nodeState.nbOfVisits++; already done while calculating ucb.
    // 	node->Parent->nodeState.nbOfVisits++;
    // 	calculateUCB(node);
    // 	break;
    // case 2:
    // 	//State after my move + opponenet move
    // 	node->Parent->nodeState.reward -= node->nodeState.reward;
    // 	node->nodeState.nbOfVisits++;
    // 	node->Parent->nodeState.nbOfVisits++;
    // 	node->Parent->Parent->nodeState.nbOfVisits++;
    // 	calculateUCB(node);
    // 	calculateUCB(node->Parent);
    // 	break;
    // case 3:
    // 	//State after my move -> opp -> my mmove
    // 	node->Parent->nodeState.reward -= node->nodeState.reward;
    // 	node->nodeState.nbOfVisits++;
    // 	node->Parent->nodeState.nbOfVisits++;
    // 	node->Parent->Parent->nodeState.nbOfVisits++;
    // 	node->Parent->Parent->Parent->nodeState.nbOfVisits++;
    // 	calculateUCB(node);
    // 	calculateUCB(node->Parent);
    // 	calculateUCB(node->Parent->Parent);
    // 	break;
    // default:
    // 	break;
    // }

    NodeMC *tempNode = node;
    double childUCB = 0;
    while (tempNode != nullptr)
    {
        if (tempNode->nodeState.treeDepth == 0)
        {
            tempNode->nodeState.UCB += childUCB;
        }
        else if (tempNode->nodeState.treeDepth == 1 || tempNode->nodeState.treeDepth == 2)
        {
            tempNode->nodeState.UCB -= childUCB;
        }
        childUCB = tempNode->nodeState.UCB;
        tempNode = tempNode->Parent;
        if(tempNode != nullptr) tempNode->nodeState.nbOfVisits++;
    }
}

void MonteCarlo::Expand(NodeMC *node)
{

    // vector<NodeMC *> newstates;
    Board tempBoard = node->boardState;
    //MoveGenerator movGen(tempBoard);

    for (int i = 0; i < 10; i++)
    {

        tempBoard = node->boardState;

        if (node->nodeState.treeDepth == 1)
        {
            //generate the first board state after my move.
            tempBoard.SimulateMove(&(node->nodeState.possibleActions[i]));

            double reward = calculateMoveReward(node->nodeState.possibleActions[i]);

            //using the move generator to generate new set of moves for each action.
            vector<Move> nextMoves;

            Rack myRackRem;
            nextMoves = this->movGen->Generate(&myRackRem, tempBoard, tempBoard.GetCount() == 0);
            vector<Move> simVec;
            for (int i = 0; i < 10; i++)
            {
                simVec.push_back(nextMoves.at(i));
            }

            //generate a random rack for me.

            //keeping track of the bag
            Bag bagRem;

            node->children.push_back(newNode(tempBoard, simVec, myRackRem, bagRem, node, node->nodeState.treeDepth + 1, reward));
        }
        else if (node->nodeState.treeDepth == 2)
        {
            //generate the first board state after my move.
            tempBoard.SimulateMove(&(node->nodeState.possibleActions[i]));

            double reward = calculateMoveReward(node->nodeState.possibleActions[i]);

            //using the move generator to generate new set of moves for each action.
            vector<Move> nextMoves;

            Rack myRackRem;
            nextMoves = this->movGen->Generate(&myRackRem, tempBoard, tempBoard.GetCount() == 0);
            vector<Move> simVec;
            for (int i = 0; i < 10; i++)
            {
                simVec.push_back(nextMoves.at(i));
            }

            //generate a random rack for me.

            //keeping track of the bag
            Bag bagRem;

            node->children.push_back(newNode(tempBoard, simVec, myRackRem, bagRem, node, node->nodeState.treeDepth + 1, reward));
        }
    }
}

NodeMC *MonteCarlo::Simulation()
{
    while (true)
    {
        NodeMC *node = promisingNode(this->Root);

        while (node->children.size() != 0)
        {
            node = promisingNode(node);
        }

        if (node->nodeState.nbOfVisits <= 0)
        {
            node->nodeState.nbOfVisits++;
            calculateUCB(node);
            Rollout(node);
        }
        else
        {
            if (node->nodeState.treeDepth < 3)
            {
                Expand(node);
                node = node->children.at(rand() % 30);
                calculateUCB(node);
                Rollout(node);
            }
            else
            {
                calculateUCB(node);
                Rollout(node);
            }
        }
    }

    return promisingNode(this->Root);
}