//changes
//1. comparetwoorders
//2. aretwoequal
//3. sellorders_heap_insert
//4. line number 394 big issue in sentinel value return.
#include <iostream>
#include <string>
#include <vector>

const int ALPHABET_SIZE = 26;
const int SIGN_SIZE = 2;
const int DIGIT_SIZE = 10;

//given that the last character of your sortedString is whitespace, be careful.
//Concerns 
//1. This order do not have the bool isvalid. But please ensure that for part2 market modifies this bool wherever required.
//2. In line 295 where you have pushed a new broker info. You haven't initialized the buy_colunt, sell_count, net_trasnfer to zero.
//3. I am doubtful in line no.37, 38. why you considering it curr_order.quantity times. Confirm it once.
//4. why trade is increased by one and not by trade quantity.
//5. The basis on which you are deciding trade price must be confirmed.
//6. when we print the stock name after a trade is made, in what order will you print the stocks.
//7. why are you heapifying the buy_orders and sell_orders at the end of trade, there is no need of it.
//8. Are you ensuring that the order that the stock name we have stored has all capitals.
//7. We are calling the function remove_all many times. This is O(nlogn) operation and therfore if done n times will lead to (n2logn) which is very bad.


struct order{
    int expiry;
    int t0;
    int quantity;
    int price;
    std::string stock_name;
    std::string broker_name;
    bool action; //0 for buy and 1 for sell
    int orderno; //what is orderno.? //I think it is line number. 
};

//Information is sufficient but let's see what does MedianFinder do.
// void print_order_details(order order1){
//         std::cout << std::endl << std::endl;
//         std::cout << order1.t0 << std::endl;
//         std::cout << order1.broker_name << std::endl;
//         std::cout << order1.action << std::endl; //0 for buy and 1 for sell
//         std::cout << order1.stock_name << std::endl;
//         std::cout << order1.price << std::endl;
//         std::cout << order1.quantity << std::endl;
//         std::cout << order1.expiry << std::endl;
//         std::cout << std::endl << std::endl;
// }

void print_order_details(const order& order1 ) 
   {
        // std::cout << std::endl << std::endl;
        std::string action;
        if (order1.action == 0){action = "BUY";}
        else{action = "SELL";}
        std::cout << "~~~ "<<order1.stock_name << " " << order1.broker_name << " $" << order1.price << " #" << order1.quantity << " (t = " << order1.t0 + order1.expiry + 1 << ")  "<<action<< std::endl;
   }
bool compareTwoOrders (order order1, order order2) { //returns 1 if order1 is higher in priority or 0 if order2 is higher in priority
    if (order1.price > order2.price){
        return 1;
    }
    if (order1.price < order2.price){
        return 0;
    }  
    else{
        if (order1.t0 > order2.t0){ // if it has appeared before, it will be given more priority
            return 0; //There is a mistake here the one with lesser time is more prior.
        }
        if (order1.t0 < order2.t0){
            return 1;
        }
        else{
            if (order1.broker_name < order2.broker_name){ //if it appears before in a dictionary, then more priority
                return 1;
            }
            if (order1.broker_name > order2.broker_name){
                return 0;
            }   
            else{
                return 1;
            }       
        }
    }    
    
}
//This is okay.
//These are true sell orders and therfore I am assuming that you haven't converted them into equivalent buy orders.
//only then there will be difference.
bool areOrdersEqual(order order1, order order2){
    if (order1.price > order2.price){
        return 0;
    }
    if (order1.price < order2.price){
        return 1;
    }  
    else{
        if (order1.t0 > order2.t0){ 
            return 0;
        }
        if (order1.t0 < order2.t0){
            return 1;
        }
        else{
            if (order1.broker_name < order2.broker_name){ 
                return 1;
            }
            if (order1.broker_name > order2.broker_name){
                return 0;
            }   
            else{
                return 1;
            }       
        }
    } 
}

//I will have to see what is all required.
struct broker_info {
    std::string broker_name;
    int buy_count;
    int sell_count;
    int net_transfer; //sold contributes +vely and bought contributes negatively
				// this means that the net_transfer remembers how much the broker has got.
};

struct nodeB {
    std::vector<nodeB*> children;
    broker_info* info; // Pointer to stock_info struct, initialized to nullptr
		
    nodeB() : children(ALPHABET_SIZE + SIGN_SIZE + DIGIT_SIZE, nullptr), info(nullptr) {}
    };

class BrokerTrie {
public:
    BrokerTrie() {
        root = new nodeB();
    }
    std::vector <std::string> broker_names;
    //why you need this vector? 
    broker_info* navigate(std::string broker_name) {
        nodeB* curr = root;   
        int index;
        
        for (char c : broker_name) {
            if (c == ' '){
                continue;
            }
            if (isalpha(c)) {
			if( c > 'Z') index = c - 'a';
                else{index = c - 'A';}

            } else if (c == '+') {
                index = ALPHABET_SIZE;
            } else if (c == '-') {
                index = ALPHABET_SIZE + 1;
            } else {
                index = ALPHABET_SIZE + SIGN_SIZE + (c - '0');
            }
		//std::cout<<index<<std::endl;
            if (curr->children[index] == nullptr) {
                curr->children[index] = new nodeB();
            }

            curr = curr->children[index];
	  //  std::cout<<"Hi"<<std::endl; 
        }

        if (curr->info == nullptr) {
             curr->info = new broker_info; // ADDITION OF NEW STOCK_INFO
             broker_names.push_back(broker_name);
             curr->info->broker_name = broker_name;
		  curr->info->buy_count = 0;
		  curr->info->sell_count = 0;
		  curr->info->net_transfer = 0;
        }
        return curr->info;

    }

    nodeB* getRoot() const {
        return root;
    }

private:
    nodeB* root; 
};

class stock_info { 
public:
    stock_info() {  lasttimeexpiry = 0;  }
    std::string stock_name;
    std::vector<order> buy_orders;
    std::vector<order> sell_orders;
    order prev_buy_front;
    order prev_sell_front;
  //  MedianFinder stock_median;
    int lasttimeexpiry; 
//This is okay.
    void buy_orders_heapify() {
        int n = buy_orders.size();
        if (n <= 1) {
            // Do nothing or handle the special case as needed
            return;
        }
        for (int i = n / 2 - 1; i >= 0; --i) {
            max_heapify(buy_orders, n, i);
        }
    }

//This is okay.  
    void sell_orders_heapify() {
        int n = sell_orders.size();
        if (n <= 1) {
            // Do nothing or handle the special case as needed
            return;
        }
        for (int i = n / 2 - 1; i >= 0; --i) {
            min_heapify(sell_orders, n, i);
        }
    }

//Concerns
//1. Why you are passing the buy order vector. it is an attribute which is already availble to the function.     																				
    void buy_orders_heap_insert(const order& new_order, std::vector<order> &buy_orders) {
        buy_orders.push_back(new_order);

        int index = buy_orders.size() - 1;
        
        while (index > 0 && !compareTwoOrders(buy_orders[(index - 1) / 2], buy_orders[index])) {
            std::swap(buy_orders[index], buy_orders[(index - 1) / 2]);
            index = (index - 1) / 2;
        }
    }

    void sell_orders_heap_insert(const order& new_order, std::vector<order> &sell_orders) {
        sell_orders.push_back(new_order);
        int index = sell_orders.size() - 1;
        while (index > 0 && !areOrdersEqual(sell_orders[(index - 1) / 2], sell_orders[index])) {
            std::swap(sell_orders[index], sell_orders[(index - 1) / 2]);
            index = (index - 1) / 2;
        }
    }

    void stock_insert(const order& new_order){
        if (new_order.action == 0){
            buy_orders_heap_insert(new_order, buy_orders);
        }
        else {
            sell_orders_heap_insert(new_order, sell_orders);
        }
    } 
	//I want to know if you are ensuring that the action has been properly updated.
	
    	
    int buy_orders_heap_max() const {
        if (!buy_orders.empty()) {
            return buy_orders.front().price;
        }
        return -1; // Return a sentinel value indicating an empty heap
		// -1 is not a correct sentinel value because it can match with some price.
		
    }

    int sell_orders_heap_min() const {
        if (!sell_orders.empty()) {
            return sell_orders.front().price;
        }
        return -1; // Return a sentinel value indicating an empty heap
		//we will correspondingly modify the trade function then.
    }

    void remove_all_from_buy_orders(int time) {
        lasttimeexpiry = time;
	   auto it = buy_orders.begin();
        int t = 0;
        while (it != buy_orders.end()) {
            if (it->expiry != -1 && it->t0 + it->expiry + 1 <= time) {
                order temp = *it;
                *it = buy_orders.back();
                buy_orders.back() = temp;
                buy_orders.pop_back();
                max_heapify(buy_orders, buy_orders.size(), it - buy_orders.begin());
            } else {
                ++it;
            }
        }
    }

    void remove_all_from_sell_orders(int time) {
        
	   lasttimeexpiry = time;  
	   auto it = sell_orders.begin();
        while (it != sell_orders.end()) {
            if (it->expiry != -1 && it->t0 + it->expiry + 1 <= time) {
                order temp = *it;
                *it = sell_orders.back();
                sell_orders.back() = temp;
                sell_orders.pop_back();
                min_heapify(sell_orders, sell_orders.size(), it - sell_orders.begin());
            } else {
                ++it;
            }
        }
    }

    void print_order_details(const order& order1 )  {
        // std::cout << std::endl << std::endl;
		int printer;
	   if( order1.expiry == -1 ) printer = -1;
		else{ printer = order1.t0 + order1.expiry + 1; }
        std::cout << "                 "<< order1.broker_name << " $" << order1.price << " #" << order1.quantity << " (t = " << printer << ")"<< std::endl;
    }

    void print_all_orders(std::vector<order> &sell_orders, std::vector<order> &buy_orders)  {
        std::cout << stock_name << std::endl;
        std::cout << "Buy Orders:" <<buy_orders.size() <<std::endl;
        for (const auto& buy_order : buy_orders) {
            print_order_details(buy_order);
        }

        std::cout << "Sell Orders:" <<sell_orders.size() <<std::endl;
        for (const auto& sell_order : sell_orders) {
            print_order_details(sell_order);
        }
        std::cout << std::endl ;
    }

    void trade(std::string stock_name, int & trade_count, int& total_money_transferred, int&share_count, BrokerTrie& brokerData) 
	{      	   //  std::cout<<"I"<<std::endl; 
        
	   if( buy_orders.size() == 0 || sell_orders.size() == 0 ) return; //check if anything else has to be done.
        int max_buy_price = buy_orders_heap_max();
        int min_sell_price = sell_orders_heap_min();
        
	   //why do we need the stock name as an arguement.
	   //what is the meaning of price being negative.
        if (max_buy_price >= min_sell_price && max_buy_price != -1 && min_sell_price != -1) {
            trade_count++; //why trade count has only increased by one it might depend on trade quantities.
            int buy_quantity = buy_orders.front().quantity;
            std::string buyer = buy_orders.front().broker_name;
            int sell_quantity = sell_orders.front().quantity;
            std::string seller = sell_orders.front().broker_name;
            int trade_quantity = (buy_quantity < sell_quantity) ? buy_quantity : sell_quantity;
            // Update quantities in orders
        	    
	       buy_orders.front().quantity -= trade_quantity;
            sell_orders.front().quantity -= trade_quantity;
	// std::cout<<"Hi"<<std::endl; 
            brokerData.navigate(seller)->sell_count+=trade_quantity;
            brokerData.navigate(buyer)->buy_count+=trade_quantity;  
            share_count+=trade_quantity; //what is share_count?
        
            int trade_price = 0;
            if (buy_orders.front().orderno < sell_orders.front().orderno){
                trade_price = buy_orders.front().price;
            }
            else{
                trade_price = sell_orders.front().price;
            }
             
            // Remove orders with zero quantity
            if (buy_orders.front().quantity <= 0) {
                std::swap(buy_orders.front(), buy_orders.back());
                buy_orders.pop_back();
                max_heapify(buy_orders, buy_orders.size(), 0);
            
            }
            if (sell_orders.front().quantity <= 0) {
                std::swap(sell_orders.front(), sell_orders.back());
                sell_orders.pop_back();
                min_heapify(sell_orders, sell_orders.size(), 0);
            }
            total_money_transferred += trade_quantity*trade_price; 
            std::cout << buyer << " purchased " << trade_quantity << " share of " << stock_name << " from " << seller << " for $"<<trade_price<<"/share"<< std::endl;
	
		 // Heapify after removal
            //buy_orders_heapify();
            //sell_orders_heapify();
            // Recursive call to continue trading 
            brokerData.navigate(seller)->net_transfer+=trade_quantity*trade_price;
            brokerData.navigate(buyer)->net_transfer-=trade_quantity*trade_price;
            trade(stock_name, trade_count, total_money_transferred, share_count, brokerData);
        }
    }

private:
    void max_heapify(std::vector<order>& arr, int n, int index) {
        int largest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        if (left < n && compareTwoOrders(arr[left], arr[largest])) {
            largest = left;
        }
        if (right < n && compareTwoOrders(arr[right], arr[largest])) {
            largest = right;
        }
        if (largest != index) {
            order temp = arr[index];
            arr[index] = arr[largest];
            arr[largest] = temp;
            max_heapify(arr, n, largest);
        }
    }
    void min_heapify(std::vector<order>& arr, int n, int index) {
        int smallest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        if (left < n && areOrdersEqual(arr[left], arr[smallest])) {
            smallest = left;
        }


        if (right < n && areOrdersEqual(arr[right], arr[smallest])) {
            smallest = right;
        }

        if (smallest != index) {
            order temp = arr[index];
            arr[index] = arr[smallest];
            arr[smallest] = temp;
            min_heapify(arr, n, smallest);
        }
    }

private:


    // Add more member variables and functions as needed
};
struct node {
    std::vector<node*> children;
    stock_info* info; // Pointer to stock_info struct, initialized to nullptr

    node() : children(ALPHABET_SIZE + SIGN_SIZE + DIGIT_SIZE, nullptr), info(nullptr) {}
};

class Trie {
public:
    Trie() {
        root = new node();
        
    }
    std::vector < stock_info *> stock_list;
    int trade_count = 0;
    int total_money_transferred = 0;
    int share_count = 0;

    void seeAllStocks (int time){
        std::cout << "=======STOCK-DETAILS-START========"<<std::endl;
        std::cout << "current-time: "<<time<<std::endl;
        auto it = stock_list.begin();
        while(it!=stock_list.end()){
            stock_info stock1 = **it;

            stock1.print_all_orders(stock1.sell_orders, stock1.buy_orders);
            it++;
        }
        std::cout << "=======STOCK-DETAILS-END========"<<std::endl;
    }
    
    void navigate(order &curr_order, BrokerTrie&broker) {
        std::string stock_name = curr_order.stock_name;
	   broker_info* tomake = broker.navigate(curr_order.broker_name);
	   tomake = NULL;
        node* curr = root;
        int index;
        for (char c : stock_name) {
            if (c == ' '){
                continue;
            }
            if (isalpha(c)) {
                if( c > 'Z' ) { index = c - 'a'; }
			else{ index = c -'A'; }
            } else if (c == '+') {
                index = ALPHABET_SIZE;
            } else if (c == '-') {
                index = ALPHABET_SIZE + 1;
            } else {
                index = ALPHABET_SIZE + SIGN_SIZE + (c - '0');
            }

            if (curr->children[index] == nullptr) {
                curr->children[index] = new node();
            }

            curr = curr->children[index];
        }

        if (curr->info == nullptr) {
            curr->info = new stock_info; // ADDITION OF NEW STOCK_INFO //what about initialization.
            curr->info->stock_name = stock_name;
            curr->info->stock_insert(curr_order); // DOUBT  
            stock_list.push_back(curr->info);
        } else {
            curr->info->stock_insert(curr_order); // ADDITION OF CURRENT ORDER HAPPENS HERE
        }
    int time = curr_order.t0;
	//std::cout<<"hi"<<std::endl;
   // curr->info->stock_median.insertOrder(curr_order);
	if( curr->info->lasttimeexpiry != time ) {
    curr->info->remove_all_from_buy_orders(time);
    curr->info->remove_all_from_sell_orders(time); }

	
    curr->info->trade(curr_order.stock_name, trade_count, total_money_transferred, share_count, broker);
			
    }                
    void search(order curr_order) {
        std::string stock_name = curr_order.stock_name;
        node* curr = root;
        int index;
        for (char c : stock_name) {
            if (c == ' '){
                continue;
            }
            if (isalpha(c)) {
                index = c - 'A';
            } else if (c == '+') {
                index = ALPHABET_SIZE;
            } else if (c == '-') {
                index = ALPHABET_SIZE + 1;
            } else {
                index = ALPHABET_SIZE + SIGN_SIZE + (c - '0');
            }

            if (curr->children[index] == nullptr) {
                curr->children[index] = new node();
            }

            curr = curr->children[index];
        }

        if (curr->info == nullptr) {
            std::cout << "STOCK NOT FOUND!!" << std::endl;
        } else {
            curr->info->print_all_orders(curr->info->buy_orders, curr->info->sell_orders); // ADDITION OF CURRENT ORDER HAPPENS HERE
        }
        std::cout << "~~~~~~~searching ends ~~~~~~~~~~~";
    }

    node* getRoot() const {
        return root;
    }

private:
    node* root; 
};

