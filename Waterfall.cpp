#include <iostream>
#include "Waterfall.h"

using namespace std;

int main()
{
    Waterfall *w = new Waterfall();
    Block *b3 = new Block(1.);
    Block *b2 = new Block(0.5, b3);
    Block *b1 = new Block(0.25, b2);
    w->blocks = {b1, b2, b3};

    w->addCondition(new Condition(CondType::REVENUE, 300, {b2}, {{b2, BlockParams(0, b2->params.children)}}));

    w->runIncome(b1, 1000);
    w->printValues();
    return 0;
}

void Waterfall::addCondition(Condition *c)
{
    conditions.push_back(c);
}

void Waterfall::localComputeRates(Block *startBlock, float initialRate)
{
    startBlock->globalRevenueRate = startBlock->params.localRate * initialRate;
    startBlock->globalTurnoverRate = initialRate;
    float remainRate = 1 - startBlock->globalRevenueRate;
    for (auto &child : startBlock->params.children)
    {
        localComputeRates(child.first, child.second * remainRate); // pass the remains to each child
    }
}

// assume steady regime, compute global rates of each block
void Waterfall::computeRates(Block *startBlock)
{
    // initialize to 0, for blocks that are not in the path
    for (auto &block : blocks)
    {
        block->globalRevenueRate = 0;
        block->globalTurnoverRate = 0;
    }
    localComputeRates(startBlock, 1);
}

Condition *Waterfall::findFirstCondition()
{
    float MinIncomeNeeded = -1; // default value
    Condition *firstCondition = nullptr;
    for (auto condition : conditions)
    {
        if (condition->satisfied)
        {
            continue;
        }
        float incomeNeeded = condition->IncomeNeeded();
        if (MinIncomeNeeded == -1 || (incomeNeeded < MinIncomeNeeded && incomeNeeded > 0))
        // we assume for now that two conditions are not reach simultaneously, tests to be added to take care of this case
        {
            MinIncomeNeeded = incomeNeeded;
            firstCondition = condition;
        }
    }
    return firstCondition;
}

void Waterfall::runIncome(Block *startBlock, float initialIncome)
{
    float income = initialIncome;
    while (income > 0)
    {
        computeRates(startBlock);
        Condition *firstCondition = findFirstCondition();
        if (firstCondition == nullptr)
        {
            flow(startBlock, income);
            break;
        }
        float incomeNeeded = firstCondition->needed;
        if (incomeNeeded > income)
        {
            flow(startBlock, income);
            break;
        }
        else
        {
            flow(startBlock, incomeNeeded);
            income -= incomeNeeded;
            firstCondition->satisfied = true;
            for (auto &update : firstCondition->updates)
            {
                update.first->params = update.second;
            }
        }
    }
}

void Waterfall::flow(Block *block, float income)
{
    // make it flow
    block->turnover += income;
    float toTake = income * block->params.localRate;
    block->revenue += toTake;
    float remains = income - toTake;
    for (auto &child : block->params.children)
    {
        flow(child.first, child.second * remains); // pass the remains to each child
    }
}

void Waterfall::printValues()
{
    for (auto block : blocks)
    {
        cout << "Revenue: " << block->revenue << "   Turnover: " << block->turnover << endl;
    }
}

// Constructors
BlockParams::BlockParams()
{
    localRate = 0;
}

BlockParams::BlockParams(float localRate) : localRate(localRate)
{
}

BlockParams::BlockParams(float localRate, vector<pair<Block *, float>> children) : localRate(localRate), children(children)
{
}

Block::Block(float localRate)
{
    params.localRate = localRate;
}

Block::Block() : Block(0)
{
}

Block::Block(float localRate, vector<pair<Block *, float>> children) : Block(localRate)
{
    params.children = children;
}

// only one son
Block::Block(float localRate, Block *b) : Block(localRate, {make_pair(b, 1)})
{
}

Condition::Condition(CondType type, float threshold) : type(type), threshold(threshold), satisfied(false)
{
}

Condition::Condition(CondType type, float threshold, vector<Block *> blocks, vector<pair<Block *, BlockParams>> updates) : Condition(type, threshold)
{
    this->blocks = blocks;
    this->updates = updates;
}

// take the computed global rates into account to compute the income needed
float Condition::IncomeNeeded()
{
    float condRate = 0; // rate of the condition
    for (auto &block : blocks)
    {
        switch (type)
        {
        case CondType::REVENUE:
            condRate += block->globalRevenueRate;
            break;
        case CondType::TURNOVER:
            condRate += block->globalTurnoverRate;
            break;
        }
    }
    // income needed = threshold / condRate
    if (condRate == 0)
        needed = -1; // avoid division by 0 (no condition
    else
        needed = threshold / condRate;
    return needed;
}

Waterfall::Waterfall()
{
}
