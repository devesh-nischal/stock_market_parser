#include <iostream>
#include <string>
#include <vector>

#define NUM_ACTIONS 8
#define NUM_STOCK_ACTIONS 4

using namespace std;

//struct to store stock portfolio information
typedef struct stockInfo {
    string ticker;
    double sharePrice;
    int numShares;
}stockInfo;

//global vector used to store stock portfolio information
vector<stockInfo> stocks;

//global variable used to store dividend income
double dividendIncome = 0.0;

//class to hold input actions information
class actions {
public:
    //data
    string date;
    string action;
    double price;
    string ticker;
    int shares;
    
    //constructor (used in initialization)
    actions(string date_, string action_, double price_, string ticker_, int shares_);
    
    //destructor
    ~actions();
    
    //member functions
    void print();       //prints the transaction associated with this class
};

actions::actions(string date_, string action_, double price_, string ticker_, int shares_) {
    this->date = date_;
    this->action = action_;
    this->price = price_;
    this->ticker = ticker_;
    this->shares = shares_;
}

actions::~actions() {
    //no need to do anything here
}

void actions::print() {
    cout.setf(ios::fixed, ios::floatfield);
    cout.precision(2);
    if (this->action == "BUY") {
        cout << "You bought " << this->shares << " shares of " << this->ticker << 
                " at a price of $" << this->price << " per share" << endl;
    }
    else if (this->action == "SELL") {
        double actualPrice;
        for (int i = 0; i < stocks.size(); i++) {
            if (stocks[i].ticker.compare(this->ticker) == 0) {
                actualPrice = stocks[i].sharePrice;  //used in profit calculation
                break;  //no need to keep looping
            }   
        }
        double profit = (this->shares * this->price) - (this->shares * actualPrice);
        if (profit > 0) {
            cout << "You sold " << this->shares << " shares of " << this->ticker << 
                    " at a price of $" << this->price << " per share for a profit of $" << profit << endl;
        }
        else {
            cout << "You sold " << this->shares << " shares of " << this->ticker << 
                    " at a price of $" << this->price << " per share for a loss of $" << profit << endl;
        }
    }
}

//class to store stock actions information
class stock_actions {
public:
    //data
    string date;
    double dividend;
    int split;          //assume this is always an integer
    string stock;
    
    //constructor (used in initialization)
    stock_actions(string date_, double dividend_, int split_, string stock_);
    
    //destructor
    ~stock_actions();
    
    //member functions
    void print();           //prints the transaction associated with this class
};

stock_actions::stock_actions(string date_, double dividend_, int split_, string stock_) {
    this->date = date_;
    this->dividend = dividend_;
    this->split = split_;
    this->stock = stock_;
}

stock_actions::~stock_actions() {
    //no need to do anything here
}

void stock_actions::print() {
    cout.setf(ios::fixed, ios::floatfield);
    cout.precision(2);
    int shares;
    for (int i = 0; i < stocks.size(); i++) {
        if (stocks[i].ticker.compare(this->stock) == 0) {
            shares = stocks[i].numShares;   //find amount of shares owned for this ticker
            break;  //no need to keep looping
        }   
    }
    if (this->split > 1) {
        cout << this->stock << " split " << this->split << " to 1, and you have " 
                << shares << " shares" << endl;
    }
    else {
        cout << this->stock << " paid out $" << this->dividend << 
                " dividend per share, and you have " << shares << " shares" << endl;
    }
}

//helper functions:

//updates the global array and dividend income based on new transactions passed 
//in as pointers to classes
//returns 1 normally, -1 if it could not find the stock in the array, 2 if only
//stock action needs to print, 3 if only action needs to print
int addOrUpdateStock(actions* A, stock_actions* B) {
    if (B == NULL) {    //if only action transaction
        for (int i = 0; i < stocks.size(); i++) {
            if (stocks[i].ticker.compare(A->ticker) == 0) {
                if (A->action.compare("BUY") == 0) {
                    double totalShares = (double)stocks[i].numShares + (double)A->shares;
                    double ratio1 = (double)(stocks[i].numShares)/totalShares;
                    double ratio2 = (double)(A->shares)/totalShares;
                    stocks[i].sharePrice = (ratio1*stocks[i].sharePrice) + (ratio2*A->price);
                    stocks[i].numShares += A->shares;
                }
                else {
                    stocks[i].numShares -= A->shares;
                }
                return 1;
            }
        }
        //did not find the stock
        if (A->action.compare("BUY") == 0) {
            stockInfo newStock;
            newStock.ticker = A->ticker;
            newStock.numShares = A->shares;
            newStock.sharePrice = A->price;
            stocks.push_back(newStock);
            return 1;
        }
        return -1;
    }
    else if (A == NULL){    //if only stock action transaction
        for (int i = 0; i < stocks.size(); i++) {
            if (stocks[i].ticker.compare(B->stock) == 0) {
                if (B->dividend != 0) {
                    dividendIncome += B->dividend*stocks[i].numShares;
                }
                else if (B->split != 0) {
                    stocks[i].numShares *= B->split;
                    stocks[i].sharePrice /= B->split;
                }
                return 1;
            }
        }
        //did not find the stock
        return -1;
    }
    else {  //if both transactions happen at the same date
        bool doneAction = false;
        bool doneStockAction = false;
        for (int i = 0; i < stocks.size(); i++) {
            if (stocks[i].ticker.compare(A->ticker) == 0) {
                if (A->action.compare("BUY") == 0) {
                    double totalShares = (double)stocks[i].numShares + (double)A->shares;
                    double ratio1 = (double)(stocks[i].numShares)/totalShares;
                    double ratio2 = (double)(A->shares)/totalShares;
                    stocks[i].sharePrice = (ratio1*stocks[i].sharePrice) + (ratio2*A->price);
                    stocks[i].numShares += A->shares;
                }
                else {
                    stocks[i].numShares -= A->shares;
                }
                doneAction = true;
            }
            else if (stocks[i].ticker.compare(B->stock) == 0) {
                if (B->dividend != 0) {
                    dividendIncome += B->dividend*stocks[i].numShares;
                }
                else if (B->split != 0) {
                    stocks[i].numShares *= B->split;
                    stocks[i].sharePrice /= B->split;
                }
                doneStockAction = true;
            }
            
            if (doneAction == true && doneStockAction == true) {
                return 1;
            }
        }
        
        if (doneAction == false && A->action.compare("BUY") == 0) {
            //did not find the stock - add it here
            stockInfo newStock;
            newStock.ticker = A->ticker;
            newStock.numShares = A->shares;
            newStock.sharePrice = A->price;
            stocks.push_back(newStock);
            doneAction == true;
        }
        
        if (doneStockAction == true && doneAction == true) {
            return 1; // normal case, need to print both transactions
        }
        else if (doneStockAction == true && doneAction == false) {
            return 2; //only print the stock action
        }
        else if (doneStockAction == false && doneAction == true) {
            return 3; //only print the action
        }
        return -1;
    }
}

//prints all the stocks owned by going through the global array. Also prints
//dividend income
void printPortfolio() {
    cout.setf(ios::fixed, ios::floatfield);
    cout.precision(2);
    for (int i = 0; i < stocks.size(); i++) {
        if (stocks[i].numShares != 0) {
            cout << "    - " << stocks[i].numShares << " shares of " << stocks[i].ticker
                    << " at $" << stocks[i].sharePrice << " per share" << endl;
        }
    }
    if (dividendIncome == 0) {
        cout << "    - $0 of dividend income" << endl;
    }
    else {
        cout << "    - $" << dividendIncome << " of dividend income" << endl;
    }
}

int main(int argc, char** argv) {
    //initializing the inputs
    actions actionsArray[NUM_ACTIONS] =
    {
        actions("1992/07/14 11:12:30", "BUY", 12.3, "AAPL", 500),
        actions("1992/09/13 11:15:20", "SELL", 15.3, "AAPL", 100),
        actions("1992/10/14 15:14:20", "BUY", 20.0, "MSFT", 300),
        actions("1992/10/17 16:14:30", "SELL", 20.2, "MSFT", 200),
        actions("1992/10/19 15:14:20", "BUY", 21.0, "MSFT", 500),
        actions("1992/10/23 16:14:30", "SELL", 18.2, "MSFT", 600),
        actions("1992/10/25 10:15:20", "SELL", 20.3, "AAPL", 300),
        actions("1992/10/25 16:12:10", "BUY", 18.3, "MSFT", 500)
    };
    
    stock_actions stockActionsArray[NUM_STOCK_ACTIONS] =
    {
        stock_actions("1992/08/14", 0.10, 0, "AAPL"),
        stock_actions("1992/09/01", 0.0, 3, "AAPL"),
        stock_actions("1992/10/15", 0.20, 0, "MSFT"),
        stock_actions("1992/10/16", 0.20, 0, "ABC")
    };
    
    //indeces used to access elements in the input arrays
    int actionsIdx = 0;
    int stockActionsIdx = 0;
    
    //main loop to go through every element, only exits when every element is examined
    while ((actionsIdx < NUM_ACTIONS) || (stockActionsIdx < NUM_STOCK_ACTIONS)) {
        bool inBoth = false;
        bool actionOnly = false;
        bool stockActionOnly = false;
        if (actionsIdx < NUM_ACTIONS && stockActionsIdx < NUM_STOCK_ACTIONS) {
            inBoth = true;
        }
        else if (actionsIdx < NUM_ACTIONS && stockActionsIdx == NUM_STOCK_ACTIONS) {
            actionOnly = true;
        }
        else if (actionsIdx == NUM_ACTIONS && stockActionsIdx < NUM_STOCK_ACTIONS) {
            stockActionOnly = true;
        }
        
        int year1;
        int month1;
        int day1;
        int year2;
        int month2;
        int day2;
        string date1;
        string date2;
        
        //only assign if action array is still available
        if (inBoth == true || actionOnly == true) {
            date1 = actionsArray[actionsIdx].date;
            year1 = atoi(date1.substr(0,4).c_str());
            month1 = atoi(date1.substr(5,2).c_str());
            day1 = atoi(date1.substr(8,2).c_str());
        }
        
        //only assign if stock action array is still available
        if (inBoth == true || stockActionOnly == true) {
            date2 = stockActionsArray[stockActionsIdx].date;
            year2 = atoi(date2.substr(0,4).c_str());
            month2 = atoi(date2.substr(5,2).c_str());
            day2 = atoi(date2.substr(8,2).c_str());
        }
        
        if(inBoth == true && year1 == year2 && month1 == month2 && day1 == day2) {
            //if both the dates in both the arrays are the same, need to update using both transactions
            int owned = addOrUpdateStock(&actionsArray[actionsIdx], &stockActionsArray[stockActionsIdx]);
            int numActionDates = 0;
            int numStockActionDates = 0;
            string date = date1.substr(0, 10).c_str();
            //check for duplicate dates in later elements and update accordingly
            while (date.compare(actionsArray[actionsIdx+1].date.substr(0,10).c_str()) == 0) {
                actionsIdx++;
                owned = addOrUpdateStock(&actionsArray[actionsIdx], NULL);
                numActionDates++;
                if (actionsIdx == (NUM_ACTIONS-1)) {
                    break;
                }
            }
            
            while (date.compare(stockActionsArray[stockActionsIdx+1].date.substr(0,10).c_str()) == 0) {
                stockActionsIdx++;
                owned = addOrUpdateStock(NULL, &stockActionsArray[stockActionsIdx]);
                numStockActionDates++;
                if (stockActionsIdx == (NUM_STOCK_ACTIONS-1)) {
                    break;
                }
            }
            
            if (owned != -1) {  //print if stock actually owned
                cout << "On " << year1 << "-" << date1.substr(5,2).c_str() << "-" << 
                        date1.substr(8,2).c_str() << ", you have:" << endl;
                printPortfolio();
                
                cout << "  Transactions:" << endl;
                //print every transaction, including duplicates
                for (int i = numActionDates; i >= 0; i--) {
                    if (owned == 1 || owned == 3) {
                        cout << "    - ";
                        actionsArray[actionsIdx-i].print();
                    }
                }
                for (int i = numStockActionDates; i >= 0; i--) {
                    if (owned == 1 || owned == 2) {
                        cout << "    - ";
                        stockActionsArray[stockActionsIdx-i].print();
                    }
                }
            }
            
            //iterate the indeces
            actionsIdx++;
            stockActionsIdx++;
        }
        else if (actionOnly == true || ((year1 < year2) || (year1 == year2 && month1 < month2)
                || (year1 == year2 && month1 == month2 && day1 < day2))) {
            //only update using actions as its date is less than stock actions or it is
            //the only one available
            int owned = addOrUpdateStock(&actionsArray[actionsIdx], NULL);
            
            int numActionDates = 0;
            string date = date1.substr(0, 10).c_str();
            //check for duplicate dates in later elements and update accordingly
            while (date.compare(actionsArray[actionsIdx+1].date.substr(0,10).c_str()) == 0) {
                actionsIdx++;
                owned = addOrUpdateStock(&actionsArray[actionsIdx], NULL);
                numActionDates++;
                if (actionsIdx == (NUM_ACTIONS-1)) {
                    break;
                }
            }
            
            if (owned != -1) {  //print if stock actually owned
                cout << "On " << year1 << "-" << date1.substr(5,2).c_str() << "-" << 
                        date1.substr(8,2).c_str() << ", you have:" << endl;
                printPortfolio();
            
                cout << "  Transactions:" << endl;
                //print every transaction, including duplicates
                for (int i = numActionDates; i >= 0; i--) {
                    cout << "    - ";
                    actionsArray[actionsIdx-i].print();
                }
            }
            
            actionsIdx++;  //iterate the index
        }
        else if (stockActionOnly == true || ((year2 < year1) || (year2 == year1 && month2 < month1) 
                || (year2 == year1 && month2 == month1 && day2 < day1))) {
            //only update using stock actions as its date is less than stock actions or it is
            //the only one available
            int owned = addOrUpdateStock(NULL, &stockActionsArray[stockActionsIdx]);
            
            int numStockActionDates = 0;
            string date = date2.substr(0, 10).c_str();
            //check for duplicate dates in later elements and update accordingly
            while (date.compare(stockActionsArray[stockActionsIdx+1].date.substr(0,10).c_str()) == 0) {
                stockActionsIdx++;
                owned = addOrUpdateStock(NULL, &stockActionsArray[stockActionsIdx]);
                numStockActionDates++;
                if (stockActionsIdx == (NUM_STOCK_ACTIONS-1)) {
                    break;
                }
            }
            
            if (owned != -1) {  //print if stock actually owned
                cout << "On " << year1 << "-" << date2.substr(5,2).c_str() << "-" << 
                        date2.substr(8,2).c_str() << ", you have:" << endl;
                printPortfolio();

                cout << "  Transactions:" << endl;
                //print every transaction, including duplicates
                for (int i = numStockActionDates; i >= 0; i--) {
                    cout << "    - ";
                    stockActionsArray[stockActionsIdx-i].print();
                }
            }
            
            stockActionsIdx++;  //iterate the index
        }
    }
    
    return 0;
}