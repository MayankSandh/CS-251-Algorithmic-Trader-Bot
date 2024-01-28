#include "market.h"
#include <iostream>
#include "trie.h"
#include <fstream>
#include "stockify.h"
// #include "main_market.cpp"
market::market(int argc, char** argv)
{
	
}
void read_order(order &order1, std::string line1, int orderno){
        
        std::vector<int>spaces_index;
        int curr = 0;
        for (auto it1 = line1.begin(); it1 != line1.end(); it1++){ 
            if (*it1 == ' '){
                spaces_index.push_back(curr);
            }
            curr++;
        }
		
		
        order1.expiry = stoi(line1.substr(spaces_index[spaces_index.size()-1]+1, line1.capacity() - spaces_index[spaces_index.size()-1]-1));
		
        order1.t0 = stoi(line1.substr(0, spaces_index[0]));
        order1.quantity = stoi(line1.substr(spaces_index[spaces_index.size()-2]+2, spaces_index[spaces_index.size()-1]-spaces_index[spaces_index.size()-2]-2));
        std::string priced = line1.substr(spaces_index[spaces_index.size()-3]+1, spaces_index[spaces_index.size()-2] - spaces_index[spaces_index.size()-3]-1);

	   if( priced[0] == '-' ){ priced[1] = '-'; order1.price = stoi(priced.substr(1, priced.length() - 1) ); }
	   else{ order1.price = stoi(priced.substr(1, priced.length() - 1)); }
	  
        order1.broker_name = line1.substr(spaces_index[0]+1, spaces_index[1]-spaces_index[0]-1);
        order1.stock_name = line1.substr(spaces_index[2]+1, spaces_index[spaces_index.size()-3]-spaces_index[2]-1);
        if (line1.substr(spaces_index[1], spaces_index[2]-spaces_index[1]) == " BUY"){
            order1.action = 0;
        }
        else{
            order1.action = 1;
        }
        order1.orderno = orderno;
}

void file_pull(std::vector<std::string> &lines){
    //FILE PULL CODE STARTS//
    std::ifstream inputFile("output.txt");
    std::string line;

    if (inputFile.is_open()) {
        while (std::getline(inputFile, line) && line!= "@!") {
            if (line == "TL" || line == "!@"){            
                continue;
            }
            lines.push_back(line);
            // std::string s1 = stockify(line); //STOCK LEFT TO STOCKIFY
            std::string s1 = (line);
            

        }

        inputFile.close();
    } else {
        std::cerr << "Unable to open the file." << std::endl;
        return;
    }
    //FILE PULL CODE ENDS///
}

void market::start()
{   
    std::vector<std::string> lines;
    file_pull(lines);
    Trie Stocks;
    BrokerTrie Brokers;
    int time = 0;
    int orderno = 0;

    // std::cout << "Successfully Initiated!" << std::endl;
    for (auto it1 = lines.begin(); it1 != lines.end(); it1++){ 
        
	   order* order1 = new order;
        std::string line1 = *it1;
        orderno++;
	  
        read_order(*order1,line1, orderno);
	   
	   (*order1).stock_name = stockify((*order1).stock_name); 
        if (order1->t0 != time){
            time = order1->t0;
        }
//	   std::cout<<"bi"<<std::endl; 
	   
        Stocks.navigate(*order1, Brokers);

        //     print_order_details(*order1);
        //     std::cout << std::endl;
        // std::cout << "ORDER NO: " << orderno << std::endl;
         //    Stocks.seeAllStocks(time);
        // std::cout << std::endl << std::endl;
        //     Stocks.search(*order1);
    // std::cout<<line1<<std::endl;    
//	Stocks.seeAllStocks(time);

    }
    std::cout << std::endl;
    std::cout << "---End of din--- " << std::endl;
    std::cout << "Total Amount of Money Transferred: $"<< Stocks.total_money_transferred << std::endl;
    std::cout << "Number of Completed Trades: " << Stocks.trade_count << std::endl;
    std::cout << "Number of Shares Traded: " <<Stocks.share_count << std::endl;
	
    for(auto it = Brokers.broker_names.begin(); it!= Brokers.broker_names.end(); it++){
        std::cout << *it << " bought "<<Brokers.navigate(*it)->buy_count<<" and sold "<<Brokers.navigate(*it)->sell_count<<" for a net transfer of $"<<Brokers.navigate(*it)->net_transfer<< std::endl;
    }
}
