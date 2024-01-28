//changes
//1. comparetwoorders
//2. aretwoequal
//3. sellorders_heap_insert
//4. line number 394 big issue in sentinel value return.
#include <iostream>
#include <string>
#include <vector>
#include "trader_objects.h"
// #include "trie.h"

const int SIGN_SIZE = 2;
const int DIGIT_SIZE = 10;
int total = 0;
//given that the last character of your sortedString is whitespace, be careful.
//Concerns 
//1. (DONE) This order do not have the bool isvalid. But please ensure that for part2 market modifies this bool wherever required. 
//2. (DONE, thanks)In line 295 where you have pushed a new broker info. You haven't initialized the buy_colunt, sell_count, net_trasnfer to zero.
//3. I am doubtful in line no.37, 38. why you considering it curr_order.quantity times. Confirm it once.
//4. why trade is increased by one and not by trade quantity.
//5. (DONE)The basis on which you are deciding trade price must be confirmed.
//6. (DONE)when we print the stock name after a trade is made, in what order will you print the stocks. Answer: No particular order
//7. (DONE)why are you heapifying the buy_orders and sell_orders at the end of trade, there is no need of it. Answer: Me stoopid broo
//8. (DONE)Are you ensuring that the order that the stock name we have stored has all capitals. Answer: Arrey please forigve my stupidity
//7. (DONE)We are calling the function remove_all many times. This is O(nlogn) operation and therfore if done n times will lead to (n2logn) which is very bad. Answer: I forgot a case. I assumed that a stockname appearing would always change with time, but I forgot the possiblility when the stock appears multipler times


bool compareTwoOrders_d (order order1, order order2) { //returns 1 if order1 is higher in priority or 0 if order2 is higher in priority
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
bool areOrdersEqual_d(order order1, order order2){
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



class stock_info_d { 
public:
    stock_info_d() {  lasttimeexpiry = 0;  }
    std::string stock_name;
    std::vector<order> buy_orders;
    std::vector<order> sell_orders;
    order prev_buy_front;
    order prev_sell_front;
    MedianFinder stock_median;
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
//1.(DONE) Why you are passing the buy order vector. it is an attribute which is already availble to the function.     																				
    //Some misconception regarding function not being able to access the object. So I had explicitly passed a reference.
    void buy_orders_heap_insert(const order& new_order, std::vector<order> &buy_orders) {
        buy_orders.push_back(new_order);

        int index = buy_orders.size() - 1;
        
        while (index > 0 && !compareTwoOrders_d(buy_orders[(index - 1) / 2], buy_orders[index])) {
            std::swap(buy_orders[index], buy_orders[(index - 1) / 2]);
            index = (index - 1) / 2;
        }
    }

    void sell_orders_heap_insert(const order& new_order, std::vector<order> &sell_orders) {
        sell_orders.push_back(new_order);
        int index = sell_orders.size() - 1;
        while (index > 0 && !areOrdersEqual_d(sell_orders[(index - 1) / 2], sell_orders[index])) {
            std::swap(sell_orders[index], sell_orders[(index - 1) / 2]);
            index = (index - 1) / 2;
        }
    }

    void stock_insert(const order& new_order){
	   stock_median.insertOrder(new_order);
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


    void trade(std::string stock_name, std::vector <order> &trade_list ) 
	{      	   
        
	   if( buy_orders.size() == 0 || sell_orders.size() == 0 ) return; //check if anything else has to be done.
        int max_buy_price = buy_orders_heap_max();
        int min_sell_price = sell_orders_heap_min();
        
	   //(DONE)why do we need the stock name as an arguement.
                // Minor misconception (I hope it is not something too concerning)
	   //(DONE)what is the meaning of price being negative.
                //Heap returns -1 when the heap is empty. Will need to change it for sentinal value
        if (max_buy_price >= min_sell_price && max_buy_price != -1 && min_sell_price != -1) {
		 
            int buy_quantity = buy_orders.front().quantity;
            std::string buyer = buy_orders.front().broker_name;
            int sell_quantity = sell_orders.front().quantity;
            std::string seller = sell_orders.front().broker_name;
            int trade_quantity = (buy_quantity < sell_quantity) ? buy_quantity : sell_quantity;
            // Update quantities in orders
        	    
	        buy_orders.front().quantity -= trade_quantity;
            sell_orders.front().quantity -= trade_quantity;
            trade_list[buy_orders.front().orderno].quantity = buy_orders.front().quantity;
            trade_list[sell_orders.front().orderno].quantity = sell_orders.front().quantity;
        
            int trade_price = 0;
            if (buy_orders.front().orderno < sell_orders.front().orderno){
                trade_price = buy_orders.front().price;
            }
            else{
                trade_price = sell_orders.front().price;
            }
             
            // Remove orders with zero quantity
            if (buy_orders.front().quantity <= 0) {
                trade_list[buy_orders.front().orderno].isValid = false;
                std::swap(buy_orders.front(), buy_orders.back());
                buy_orders.pop_back();
                max_heapify(buy_orders, buy_orders.size(), 0);
            
            }
            if (sell_orders.front().quantity <= 0) {
                trade_list[sell_orders.front().orderno].isValid = false;
                std::swap(sell_orders.front(), sell_orders.back());
                sell_orders.pop_back();
                min_heapify(sell_orders, sell_orders.size(), 0);
            }
	
		 // Heapify after removal
            //buy_orders_heapify();
            //sell_orders_heapify();
            // Recursive call to continue trading 
            trade(stock_name, trade_list);
        }
    }

private:
    void max_heapify(std::vector<order>& arr, int n, int index) {
        int largest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        if (left < n && compareTwoOrders_d(arr[left], arr[largest])) {
            largest = left;
        }
        if (right < n && compareTwoOrders_d(arr[right], arr[largest])) {
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
        if (left < n && areOrdersEqual_d(arr[left], arr[smallest])) {
            smallest = left;
        }


        if (right < n && areOrdersEqual_d(arr[right], arr[smallest])) {
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
    stock_info_d* info; // Pointer to stock_info_d struct, initialized to nullptr

    node() : children(ALPHABET_SIZE + SIGN_SIZE + DIGIT_SIZE, nullptr), info(nullptr) {}
};

class Trie {
public:
    Trie() {
        root = new node();
        
    }
    std::vector < stock_info_d *> stock_list;
    int trade_count = 0;
    int total_money_transferred = 0;
    int share_count = 0;

    void navigate(order &curr_order, std::vector<order>&trade_list) {
        
	  std::string stock_name = curr_order.stock_name;
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
		//std::cout<<"bi"<<std::endl;
            curr->info = new stock_info_d; // (DONE)ADDITION OF NEW STOCK_INFO //what about initialization. Not required (bad practice i know, but handled by vector)
            curr->info->stock_name = stock_name;
            curr->info->stock_insert(curr_order); // DOUBT  
            stock_list.push_back(curr->info);
        } else {
		    
            curr->info->stock_insert(curr_order); // ADDITION OF CURRENT ORDER HAPPENS HERE
	
        }
	
    int time = curr_order.t0;
   // curr->info->stock_median.insertOrder(curr_order);
	if( curr->info->lasttimeexpiry != time ) {
    curr->info->remove_all_from_buy_orders(time);
    curr->info->remove_all_from_sell_orders(time); }

	//std::cout<<trade_list.size()<<std::endl;
    curr->info->trade(curr_order.stock_name, trade_list);
			
    }                

    node* getRoot() const {
        return root;
    }

private:
    node* root; 
};