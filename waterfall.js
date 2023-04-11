class Waterfall {
  constructor() {
    this.blocks = [];
    this.conditions = [];
  }

  addCondition(c) {
    this.conditions.push(c);
  }

  localComputeRates(startBlock, initialRate) {
    startBlock.globalRevenueRate = startBlock.params.localRate * initialRate;
    startBlock.globalTurnoverRate = initialRate;
    let remainRate = initialRate - startBlock.globalRevenueRate;
    console.log(`globalRevenueRate = ${startBlock.globalRevenueRate}`);
    for (const [child, rate] of startBlock.params.children) {
      this.localComputeRates(child, rate * remainRate);
    }
  }

  computeRates(startBlock) {
    for (const block of this.blocks) {
      block.globalRevenueRate = 0;
      block.globalTurnoverRate = 0;
    }
    this.localComputeRates(startBlock, 1);
  }

  findFirstCondition() {
    let MinIncomeNeeded = -1;
    let firstCondition = null;
    for (const condition of this.conditions) {
      if (condition.satisfied) {
        continue;
      }
      let incomeNeeded = condition.IncomeNeeded();
      if (MinIncomeNeeded === -1 || (incomeNeeded < MinIncomeNeeded && incomeNeeded > 0)) {
        MinIncomeNeeded = incomeNeeded;
        firstCondition = condition;
      }
    }
    console.log(`MinIncomeNeeded = ${MinIncomeNeeded}`);
    return firstCondition;
  }

  runIncome(startBlock, initialIncome) {
    let income = initialIncome;
    while (income > 0) {
      this.computeRates(startBlock);
      const firstCondition = this.findFirstCondition();
      if (firstCondition === null) {
        this.flow(startBlock, income);
        this.printValues();
        break;
      }
      let incomeNeeded = firstCondition.needed;
      if (incomeNeeded > income) {
        this.flow(startBlock, income);
        this.printValues();
        break;
      } else {
        this.flow(startBlock, incomeNeeded);
        this.printValues();
        income -= incomeNeeded;
        firstCondition.satisfied = true;
        for (const [block, params] of firstCondition.updates) {
          block.params = params;
        }
      }
    }
  }

  flow(block, income) {
    block.turnover += income;
    let toTake = income * block.params.localRate;
    block.revenue += toTake;
    let remains = income - toTake;
    for (const [child, rate] of block.params.children) {
      this.flow(child, rate * remains);
    }
  }

  printValues() {
    for (const block of this.blocks) {
      console.log(`Revenue: ${block.revenue}   Turnover: ${block.turnover}`);
    }
  }
}


function IncomeNeeded() {
let condRate = 0; // rate of the condition
let amountDone = 0; // what part of the condition is already done
for (let i = 0; i < blocks.length; i++) {
let block = blocks[i];
switch (type) {
case CondType.REVENUE:
condRate += block.globalRevenueRate;
amountDone += block.revenue;
break;
case CondType.TURNOVER:
condRate += block.globalTurnoverRate;
amountDone += block.turnover;
break;
}
}
// income needed = (threshold - amountDone) / condRate
let needed;
if (condRate == 0)
needed = -1; // avoid division by 0, -1 convention for not reachable
else
needed = (threshold - amountDone) / condRate;
return needed;
}
