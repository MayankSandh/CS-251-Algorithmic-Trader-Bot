// This is your trader. Place your orders from here

#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <atomic>
#include <mutex>
#include <chrono>
#include <vector>
#include "dummy_market.h"
#include "stockify2.h"
// IMPORTANT ERROR: IS IT POSSIBLE THAT A STOCKNAME APPEARS MULTIPLE TIMES IN A TRADE? 
// IMPORTANT ERROR: IF THREADS CALLS OUR FUNCTION FIRST, IT MIGHT RETURN ERROR; WE NEED TO RETURN BLANK IN THIS CASE
int STOCK_LEVEL_THRESHOLD = 150;
extern std::atomic<int> commonTimer;
std::fstream::pos_type lastPosition = 0;
extern std::mutex printMutex;
std::string trader_bot_name = "NavyaMantuTrader";
std::vector<order> trade_list;
StocksTrie stocks_trie;
Trie dummy_market;
std::vector<std::string> list_stockstr;

void read_order_t(order &order1, std::string line1, int& orderno){
        //line1 analysis 

		std::vector<int>spaces_index;
        int curr = 0;
        for (auto it1 = line1.begin(); it1 != line1.end(); it1++){ 
            if (*it1 == ' '){
                spaces_index.push_back(curr);
            }
            curr++;
        }        
        
	//curr job is done correctly.
	
	    if (line1.substr(spaces_index[1], spaces_index[2]-spaces_index[1]) == " BUY"){
            order1.action = 0;
        }
        else{
            order1.action = 1;
        }

        order1.expiry = stoi(line1.substr(spaces_index[spaces_index.size()-1]+1, line1.length() - spaces_index[spaces_index.size()-1]-1));
	//what does the capacity function do?
	   //expiry is fine.
	
        order1.t0 = stoi(line1.substr(0, spaces_index[0]));
        order1.quantity = stoi(line1.substr(spaces_index[spaces_index.size()-2]+2, spaces_index[spaces_index.size()-1]-spaces_index[spaces_index.size()-2]-2));
	   
	   total = total + order1.quantity;
	   
	   int multiplier; //CONVERTS SELL ORDER INTO CORRESPONDING BUY ORDER BY CONVERTING THE PRICE TO NEGATIVE
        if( order1.action ){multiplier = -1; }
	   else {multiplier = 1;}

	   order1.price = multiplier * stoi(line1.substr(spaces_index[spaces_index.size()-3]+2, spaces_index[spaces_index.size()-2]-spaces_index[spaces_index.size()-3]-2)); 
	   //price can not  handle the negative wala type right now.
        
        order1.broker_name = line1.substr(spaces_index[0]+1, spaces_index[1]-spaces_index[0]-1);
        order1.stock_name = line1.substr(spaces_index[2]+1, spaces_index[spaces_index.size()-3]-spaces_index[2]-1);
	   order1.stock_name = stockify2(order1.stock_name); 
	   
	   int minindex = 2147483647; //check it
     	   std::string minstockname;  
	 bool isEven = 0;
        std::string currentStock = "";
        std::string currentQ = "";
        for (char c: order1.stock_name)
		{      //IMPORTANT ERROR: CONSIDER THAT THERE MIGHT NOT BE ANY QUANTITY MENTIONED
            if (c == ' ')
		 {   
                if (isEven){
				
                    isEven = 0;
                    stock_quantity s1;
                    s1.stock_name = currentStock;
			     
                    				     s1.quantity = multiplier*stoi(currentQ); 

                    s1.index = stocks_trie.navigate(currentStock)->stock_index;
			     if( s1.index < minindex ) {minindex = s1.index; minstockname = s1.stock_name; }
                     currentStock = "";
                    currentQ = "";
                    order1.stock_map.push_back(s1);
                }
                else{
                    isEven = 1;
                }
            }
            else if (isalpha(c)){
			  if(isEven == 1)
			  { 
				isEven = 0;
                    stock_quantity s1;
                    s1.stock_name = currentStock;
			     
                    				     s1.quantity = multiplier; 

                    s1.index = stocks_trie.navigate(currentStock)->stock_index;
                    currentStock = "";
                    currentQ = "";
			     if( s1.index < minindex ) {minindex = s1.index; minstockname = s1.stock_name; }
                    order1.stock_map.push_back(s1);
			  }	
			
 
			currentStock+=c;
            }
            else{
                currentQ+=c;
            }

        }
        if( currentQ == "" ){ currentQ = "1";}
	   if( currentStock == "") { std::cout<<"some issue"<<std::endl;}
	   		    stock_quantity s1;
                    s1.stock_name = currentStock;			     
                    s1.quantity = multiplier * stoi(currentQ); 
                    s1.index = stocks_trie.navigate(currentStock)->stock_index;
                   currentStock = "";
                   currentQ = "";
				if( s1.index < minindex ) {minindex = s1.index; minstockname = s1.stock_name; }
                    order1.stock_map.push_back(s1);
	   order1.minindex = minindex;
		order1.minstockname = minstockname;
	  
        order1.orderno = orderno++;
	 completemarker[minindex].push_back(order1.orderno);

}

void insertOrderInStockLevel(stock_level* current, int orderno)
{   
    int capacity = 0;
    
    int trade_to_remove_index = 0; //index in the list;
    int stocksan = -1;
    int i = 0;
    for(auto offer : current->trades)
	{
	  capacity = capacity + trade_list[offer].quantity;
	  if( trade_list[offer].stock_map.size() > stocksan ){ trade_to_remove_index = i; stocksan = trade_list[offer].stock_map.size(); } 
	  i = i + 1;
	}
    
    
    if (capacity < STOCK_LEVEL_THRESHOLD)
    {
        current->trades.push_back(orderno);
    }
    else
    {
        if (trade_list[orderno].stock_map.size() < trade_list[current->trades[trade_to_remove_index]].stock_map.size() ){
            current->trades.erase(current->trades.begin() + trade_to_remove_index);
		 current->trades.push_back(orderno);
        }
    }

}

void processTradeLine (StocksTrie & stocks_trie, std::vector<order> & trade_list, std::string line1, order &order1, int &orderno){
    trade_list.push_back(order1);
    int min_index = stocks_trie.max_index;
    std::string stockname = "";
    std::string min_stockname = "";
    bool isEven = 0;
    for (char c:trade_list[orderno].stock_name){
        if (c == ' '){
            if (!isEven){
                isEven = 1;
                if (stockname!=""){
                    if (min_index < stocks_trie.navigate(stockname)->stock_index){
                        min_index = stocks_trie.navigate(stockname)->stock_index;
                        min_stockname = stockname;
                    }
                } 
                stockname = "";
            }
            else{
                isEven = 0;
            }
        }
        else if (isalpha(c)){
            stockname+=c;
        }
    }
    if (!isEven){
        isEven = 1;
        min_stockname = stockname;
    }
    
}

void invalidateExpiredTrades(std::vector<order>&trade_list, int curr_time){
    
    
    for(auto it = trade_list.begin();it!=trade_list.end();it++){
        if (it->isValid){       
            if (it->t0 + it->expiry + 1 == curr_time){
                    it->isValid = false;
                }
            }
    }
}

void fetchNewLines(std::vector<std::string> & lines, std::string filename, std::fstream::pos_type& lastPosition){
    std::ifstream file(filename);

    // Check if the file is open
    if (!file.is_open()) {
        std::cerr << "Unable to open file " << filename << std::endl;
        return;
    }

    // Get the length of the file
    file.seekg(0, std::ios::end);
    std::fstream::pos_type fileLength = file.tellg();

    // Set the position in the file
    file.seekg(lastPosition);

    std::string line;
    while (std::getline(file, line)) {
        if (line!= "TL" && line!= "!@"){
            line = line.substr(0,line.length()-1);
            lines.push_back(line);
        }
        if (line == "!@"){
            lines.push_back(line);
        }
    }

    // Update the last position
    lastPosition = fileLength;
}

void updateLastPosition(std::string filename, std::fstream::pos_type& lastPosition){
    std::ifstream file(filename);

    // Check if the file is open
    if (!file.is_open()) {
        std::cerr << "Unable to open file " << filename << std::endl;
        return;
    }

    // Get the length of the file
    file.seekg(0, std::ios::end);
    std::fstream::pos_type fileLength = file.tellg();

    // Set the position in the file
    file.seekg(lastPosition);

    std::string line;

    // Update the last position
    lastPosition = fileLength;
}

void updateTradeList(std::vector<order> &trade_list, StocksTrie& stocks_trie, std::vector<std::string> new_lines, int & orderno, int & toEndTrader) {
    // std::cout << "for loop size: " << new_lines.size() << std::endl;

    for(auto it = new_lines.begin(); it!=new_lines.end(); it++){
        order order1;
        read_order_t(order1, *it, orderno);
        processTradeLine(stocks_trie, trade_list, *it, order1, orderno);
        dummy_market.navigate(order1, trade_list);
        if (*it == "!@"){
            std::cout << "the end " << std::endl;
            toEndTrader = 1;
            return;
        }
        std::cout << "--->"<<*it << std::endl;
    }
}

void findcycles(cycle key, MaxHeap* heapwrite, std::vector <order> & trade_list, stock_level* current, StocksTrie & stocks_trie, int index)
{
	if( index >= current->trades.size() )
	{
		if( key.quantities[current->stock_index] == 0 ) { (*heapwrite).insert(key); } 
 		return; 
	}	
	
	int tradeindex = current->trades[index];
	order trade = trade_list[tradeindex];
	//so I am thinking about that wether the order will take a lot of space.
	
	findcycles(key, heapwrite, trade_list, current, stocks_trie, index + 1);
	key.non_null_count += 1;
	for(int j = 1; j <= trade_list[tradeindex].quantity; j++)
	{
		key.cycle_trades[tradeindex] = j;
		for( auto p : trade_list[tradeindex].stock_map )
		{
			key.quantities[p.index] = key.quantities[p.index] + p.quantity;		
		}
	
		key.profit = key.profit + trade_list[tradeindex].price;
		findcycles(key, heapwrite, trade_list, current, stocks_trie, index + 1);	
	}
	
	key.cycle_trades[tradeindex] = 0;
	for( auto p : trade.stock_map )
	{
		key.quantities[p.index] = key.quantities[p.index] - trade_list[tradeindex].quantity *  p.quantity;		
	}
	
	key.profit = key.profit - trade_list[tradeindex].quantity * trade_list[tradeindex].price;
	key.non_null_count -= 1;
	
	return;

}

cycle findArbitrage(std::vector <order> & trade_list, StocksTrie & stocks_trie)
{
    // findCycles(stocks_trie.firstAddedStock, null_stock_index, avlue)
//certain requirements
//1. The orders should be up to the date. Like if some are expiry then they should be invalid .
//2. isvalid == 1 means that it should be considered and if == 0 then should not be considered.
// My code now assumes that 

	//Welcome to find arbirtrage algorithm. :)
	int NOT = trade_list.size();
	int NOS = stocks_trie.max_index + 1;
	
	MaxHeap*  heapread = new MaxHeap(total );
	MaxHeap*  heapwrite = new MaxHeap(total );

     cycle emptyCycle;
     emptyCycle.profit = -1;
     emptyCycle.non_null_count = -1;
	//I want these to be empty as of now.
	//please ensure this.
	
	stock_level* current = stocks_trie.firstAddedStock;
	if(current->stock_index != 0 ){ std::cout<<"Error : The header is not pointing to index 0"<<std::endl; return emptyCycle;}
	
	struct cycle subset;
	for(int i = 0;  i < NOT; i++)
	{
		subset.cycle_trades.push_back(0);
	}
	for(int i = 0;  i < NOS; i++)
	{
		subset.quantities.push_back(0);
	}
	
	subset.profit = 0;
	subset.non_null_count = 0;
	
	heapread->insert(subset);
	if( heapread->heapSize != 1 ){ std::cout<<"Issue with the implementation of heap"<<std::endl; return emptyCycle;}
	
	//I have inserted an empty cycle in the heap. 
	while( current != NULL )
	{	
		current->trades.clear();
		int stock_index = current->stock_index;
		for(auto offer : completemarker[stock_index])
		{
			if( trade_list[offer].isValid == 0 || trade_list[offer].medianvalid == 0 )
			{  continue; }
			insertOrderInStockLevel( current, trade_list[offer].orderno );			
		}		
	
		int size_of_read = (*heapread).heapSize;
		while((*heapwrite).heapSize != 0 ){ heapwrite->heap.pop_back(); heapread->heapSize -= 1; }
		//this is mean to empty the heapwrite

		//This is an important point I think that we should convert the maxheap into a minheap.			
		for(int i = 0; i < size_of_read; i++)
		{
			findcycles(heapread->heap[i], heapwrite, trade_list, current, stocks_trie, 0); 
		}
		
		current = current->next_stock;
		MaxHeap* temp = heapread;
		heapread = heapwrite;
		heapwrite = temp;
				
	}
	
	if( heapread->heapSize == 0 ) 
	{
		return subset;
	}
	
	int index_of_best = 0;
	int best_profit = (*heapread).heap[0].profit;

	// EDITED HERE
	for(int j = 0; j < (*heapread).heapSize; j++)
	{
		if( heapread->heap[j].profit > best_profit ){ index_of_best = j; best_profit = heapread->heap[j].profit; }			
	}
	
	return (*heapread).heap[index_of_best];
	

// std::vector <int> cycle_trades;
// int profit;
// int non_null_count;
	
	
}

void executeArbitrage(std::vector <order> & trade_list, cycle finalArbitrage, int &orderno)
{
    int trade_no = 0;
    std::vector <int> trades_arby;
    std::vector <int> trades_arby_q;
    for(auto it = finalArbitrage.cycle_trades.begin(); it!=finalArbitrage.cycle_trades.end();it++){
        if (*it != 0){
            trades_arby.push_back(trade_no);
            trades_arby_q.push_back(*it);
        } 
        trade_no++;
    }
    std::string bot_action;
    int q_no = 0;
    for(auto it = trades_arby.rbegin(); it!=trades_arby.rend(); it++){
        if (trade_list[*it].action = 0){
            bot_action = "SELL";
        }
        else{
            bot_action = "BUY";
        }
        std::string myTradeOffer = std::to_string(commonTimer.load()) + " " + trader_bot_name + " " + bot_action + " " + trade_list[*it].stock_name + " " + std::to_string(trades_arby_q[q_no]) + " " +  std::to_string(trade_list[*it].price) + " 0";
        std::cout << myTradeOffer << std::endl;
        order myOrder;
        read_order_t(myOrder, myTradeOffer, orderno);
        dummy_market.navigate(myOrder, trade_list);
        q_no++;
    }

}

void medianvalidateTrades()
{	// I doubt that it is comparing between double and int.
	for(auto stock_info : dummy_market.stock_list)
	{
		double median = stock_info->stock_median.findMedian();
		for( auto order: stock_info->buy_orders )
		{
			if( order.price < median ){ trade_list[order.orderno].medianvalid = 0; }
			else{ trade_list[order.orderno].medianvalid = 1; } 
		}

		for( auto order: stock_info->sell_orders )
		{
			if( order.price > median ){ trade_list[order.orderno].medianvalid = 0; }
			else{ trade_list[order.orderno].medianvalid = 1; } 
		}
	}

}


int trader(std::string *message) //BLANK TRADER
{
    return 1;
}

int trader(int currentTime)
{	
    static int toEndTrader = 0;
    static int orderno = 0;
    std::vector <std::string> new_lines;
    fetchNewLines(new_lines, "output.txt", lastPosition);
    updateTradeList(trade_list, stocks_trie, new_lines, orderno, toEndTrader);
    invalidateExpiredTrades(trade_list, currentTime);
    medianvalidateTrades();
    cycle finalArbitrage = findArbitrage(trade_list, stocks_trie); // IMPORTANT ERROR: FOR NOW, QUANTITY IS ASSUMED TO BE THE VALUE OF CYCLE_TRADES AND PRICE IS ASSUMED TO BE THE SAME VALUE OF MATCHING TRADE'S PRICE
    executeArbitrage(trade_list, finalArbitrage, orderno);
    updateLastPosition("output.txt", lastPosition);
    return toEndTrader;
}
int reader(int currentTime)
{
	static std::streampos lastPos = 0;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::lock_guard<std::mutex> lock(printMutex);
    std::string message;
    int valid = trader(currentTime);
    return valid;
}

// IMPORTANT ERROR: MAKE SURE TO INVALIDATE OUR ORDERS AS WELL