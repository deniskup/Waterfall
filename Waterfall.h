#include <vector>
using namespace std;

class Block; // forward declaration

class BlockParams
{ // parameters for a block, to be changed via thresholds
public:
    BlockParams();
    BlockParams(float localRate);
    BlockParams(float localRate, vector<pair<Block *, float>> children);
    float localRate;                       // current fraction of local income taken as revenue (e.g. 25%)
    vector<pair<Block *, float>> children; // list of children and their fractions of income (e.g. 50% to child 1, 50% to child 2)
};

class Block
{ // a block in the waterfall
public:
    Block();
    Block(float localRate);
    Block(float localRate, vector<pair<Block *, float>> children);
    Block(float localRate, Block *b); // only one son

    BlockParams params; // parameters for the block

    float revenue;  // current value of the block: money received
    float turnover; // total turnover

    float globalRevenueRate;  // current fraction of original income taken as revenue (e.g. 25%), to be computed depending of income block
    float globalTurnoverRate; // current fraction of original income taken as turnover (e.g. 25%), to be computed depending of income block
};

enum CondType
{
    REVENUE,
    TURNOVER
};

class Condition
{
public:
    Condition(CondType type, float threshold);
    Condition(CondType type, float threshold, vector<Block *> blocks, vector<pair<Block *, BlockParams>> updates);

    // it is either a simple threshold on a block value, or on the sum of block values, or on revenues or turnovers

    // thresholds
    CondType type;
    float threshold;
    
    vector<Block *> blocks;                     // list of blocks to check for their sum
    vector<pair<Block *, BlockParams>> updates; // list of new parameters for some blocks if the condition is met
    bool satisfied;                             // whether the condition is already satisfied

    float IncomeNeeded(); // compute the income needed to satisfy the condition
    float needed; //to be computed by the above function
};

class Waterfall
{
public:
    Waterfall();
    vector<Block *> blocks;
    vector<Condition *> conditions; // all conditions

    void addCondition(Condition *c); // add a condition to the waterfall

    void computeRates(Block *startBlock); // compute the rates for each block, initially called on (startBlock,1).
    Condition *findFirstCondition();                         // find the first condition that will be satisfied
    void runIncome(Block *startBlock, float initialIncome);  // run income through the waterfall, starting at block
    void flow(Block *startBlock, float income);              // basic version of runIncome, without conditions
    void printValues();                                      // print revenues and turnovers of all blocks

    private:
    void localComputeRates(Block *startBlock, float initialRate); // compute the rates for each block, initially called on (startBlock,1).
};