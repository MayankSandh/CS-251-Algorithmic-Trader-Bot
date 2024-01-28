// This is your trader. Place your orders from here
//Concerns
//1. It seems that the indexing of the stocks is not starting from 0. Please ensure this.
//2. Please ensure that maxindex + 1 is equal to number of stocks present.
//3. Why broker_name is appearing in this StocksTrie.
//4. some changes has been done in the constructor of Maxheap. Please look at them.
//5. check the validity of isvalid bool of order. I want you to ensure that when the stuff get expired the trades get invalid.
//6. One more attribute in the stock_quantity that is stock index is required.
//7. I have changed the struct cycle. Added a new attribute quantities.


#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <atomic>
#include <mutex>
#include <chrono>
#include <vector>
const int ALPHABET_SIZE = 26;
// #include "trie.h"
// #include "trie2.h"
struct stock_quantity {
    int index;
    std::string stock_name;
    int quantity;
};

std::vector< std::vector <int> > completemarker;
struct order {
    bool isValid; //trade 
    bool medianvalid; //run before arbitrage.
    int expiry;
    int t0;
    int quantity;
    int price;
    std::vector <stock_quantity> stock_map;
    std::string stock_name;
    std::string broker_name;
    bool action; //0 for buy and 1 for sell
    int orderno;
    int minindex; 
    std::string minstockname;
};



struct stock_level {
    std::vector <int> trades; //remember the trades to be conisdred in the arbitrge.
    int stock_index; 
    stock_level* next_stock;
};
struct nodeB {
    std::vector<nodeB*> children;
    stock_level* info; // Pointer to stock_info struct, initialized to nullptr

    nodeB() : children(ALPHABET_SIZE, nullptr), info(nullptr) {}
};
class StocksTrie {

public:
    StocksTrie() {
        root = new nodeB();
    }
    int max_index = 0;
    stock_level* lastAddedStock = NULL;
    stock_level* firstAddedStock = NULL;\
    

    stock_level* navigate(std::string stock_name) {
        nodeB* curr = root;
        int index;
        for (char c : stock_name) {
            index = c - 'A'; // IMPORTANT ERROR : WORKS ON THE ASSUMPTION THAT ALL STOCK NAMES ARE  CAPITAL LETTERS
            if (curr->children[index] == nullptr) {
                curr->children[index] = new nodeB();
            }

            curr = curr->children[index];
        }

        if (curr->info == nullptr) {
            curr->info = new stock_level; // ADDITION OF NEW STOCK_INFO
	       std::vector<int> x;
		  completemarker.push_back(x);
            curr->info->stock_index = max_index++;
            if (max_index == 1){
                firstAddedStock = curr->info;
                lastAddedStock = curr->info;
                curr->info->next_stock = NULL;
            }
            else{
                lastAddedStock->next_stock = curr->info;
                lastAddedStock = curr->info;
                curr->info->next_stock = NULL;
            }
        }
        return curr->info;

    }

    nodeB* getRoot() const {
        return root;
    }

private:
    nodeB* root; 
};

struct cycle {
    std::vector <int> cycle_trades;
    std::vector <int> quantities;	
    int profit;	//profit is properly modified. 
    int non_null_count; //non_null_count is increased by one for that. 
};
bool compareTwoCycles(cycle c1, cycle c2){ // 0 means left is bigger, 1 means right is bigger what is the meaning of bigger?
    if (c1.non_null_count != c2.non_null_count){
        if (c1.non_null_count < c2.non_null_count){
            return 1;
        }
        else{
            return 0;
        }
    }
    else {
        if(c1.profit<c2.profit){
            return 0;
        }
        else{
            return 1;
        }
    }

}

class MaxHeap {
private:
    
    int parent(int i) {
        return (i - 1) / 2;
    }

    int leftChild(int i) {
        return 2 * i + 1;
    }

    int rightChild(int i) {
        return 2 * i + 2;
    }

    void heapify(int i) {
        int l = leftChild(i);
        int r = rightChild(i);
        int largest = i;

        if (l < heapSize && !compareTwoCycles(heap[l], heap[i])) {
            largest = l;
        }

        if (r < heapSize && compareTwoCycles(heap[largest], heap[r])) {
            largest = r;
        }

        if (largest != i) {
            std::swap(heap[i], heap[largest]);
            heapify(largest);
        }
    }

    void buildHeap() {
        for (int i = (heapSize - 1) / 2; i >= 0; --i) {
            heapify(i);
        }
    }

public:
    std::vector<cycle> heap;
    int capacity; 
    int heapSize;

    MaxHeap(int cap) : capacity(cap), heapSize(0) {
	   
	   if( capacity < 50 ) capacity = 50;
        heap.reserve(capacity);
    }

    void insert(cycle key) {
        if (heapSize < capacity) {
            heap.push_back(key);
		 
            int current = heapSize;
            heapSize++;

            while (current != 0 && compareTwoCycles(heap[parent(current)], heap[current])) {
                std::swap(heap[current], heap[parent(current)]);
                current = parent(current);
            }
        } else {
            if (compareTwoCycles(key, heap[0])) {
                heap[0] = key;
                heapify(0);
            }
        }
    }

};


class MedianFinder {
private:
    
    std::vector<int> maxHeap; // max-heap for the lower half
    std::vector<int> minHeap; // min-heap for the upper half
//what is the size of lower half when size of the list \is odd.
public:
    // Function to insert a new element into the median finder
    //Why do we need median ?
    int median;
    void insertOrder(order curr_order) {
        // Insert into max-heap if it's empty or the element is less than or equal to the maximum element
        for(int i = 0; i<curr_order.quantity; i++){
            insert(curr_order.price); //These many deals 
        }
    }    
    void insert(int num) 
	{
        // Insert into max-heap if it's empty or the element is less than or equal to the maximum element
        if (maxHeap.empty() || num <= maxHeap[0]) {
            maxHeap.push_back(num);
            maxHeapifyUp();
        } 
        // Insert into min-heap if it's empty or the element is greater than or equal to the minimum element
        else if (minHeap.empty() || num >= minHeap[0]) {
            minHeap.push_back(num);
            minHeapifyUp();
        } 
        // Insert into max-heap and move the maximum element to min-heap
        else {
            maxHeap.push_back(num);
            maxHeapifyUp();
            minHeap.push_back(maxHeap[0]);
            maxHeap[0] = maxHeap.back();
            maxHeap.pop_back();
            minHeapifyDown(0);
        }

        // Rebalance the heaps if necessary
        balanceHeaps();
    }

    // Function to find the median
    double findMedian() {
        // Total number of elements
        int totalElements = maxHeap.size() + minHeap.size();

        // If the total number of elements is odd, return the top of the max-heap
        if (totalElements % 2 != 0) {
            return static_cast<double>(maxHeap[0]);
        } 
        // If the total number of elements is even, return the average of the tops of max-heap and min-heap
        else {
            return (static_cast<double>(maxHeap[0]) + minHeap[0]) / 2.0;
        }
    }

private:
    // Function to balance the heaps by ensuring that the size difference is at most 1
    void balanceHeaps() {
        // If the size difference between the heaps is greater than 1, move the root of the larger heap to the smaller heap
        while (maxHeap.size() > minHeap.size() + 1) {
            minHeap.push_back(maxHeap[0]);
            maxHeap[0] = maxHeap.back();
            maxHeap.pop_back();
            maxHeapifyDown(0);
            minHeapifyUp();
        }
        while (minHeap.size() > maxHeap.size()) {
            maxHeap.push_back(minHeap[0]);
            minHeap[0] = minHeap.back();
            minHeap.pop_back();
            minHeapifyDown(0);
            maxHeapifyUp();
        }
    }

    // Function to perform max-heapify operation after inserting an element
    void maxHeapifyUp() {
        int index = maxHeap.size() - 1;
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (maxHeap[parent] < maxHeap[index]) {
                std::swap(maxHeap[parent], maxHeap[index]);
                index = parent;
            } else {
                break;
            }
        }
    }

    // Function to perform min-heapify operation after inserting an element
    void minHeapifyUp() {
        int index = minHeap.size() - 1;
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (minHeap[parent] > minHeap[index]) {
                std::swap(minHeap[parent], minHeap[index]);
                index = parent;
            } else {
                break;
            }
        }
    }

    // Function to perform min-heapify operation after moving the maximum element to min-heap
    void minHeapifyDown(int index) {
        int leftChild = 2 * index + 1;
        int rightChild = 2 * index + 2;
        int smallest = index;

        if (leftChild < minHeap.size() && minHeap[leftChild] < minHeap[smallest]) {
            smallest = leftChild;
        }

        if (rightChild < minHeap.size() && minHeap[rightChild] < minHeap[smallest]) {
            smallest = rightChild;
        }

        if (smallest != index) {
            std::swap(minHeap[index], minHeap[smallest]);
            minHeapifyDown(smallest);
        }
    }

    // Function to perform max-heapify operation after moving the minimum element to max-heap
    void maxHeapifyDown(int index) {
        int leftChild = 2 * index + 1;
        int rightChild = 2 * index + 2;
        int largest = index;

        if (leftChild < maxHeap.size() && maxHeap[leftChild] > maxHeap[largest]) {
            largest = leftChild;
        }

        if (rightChild < maxHeap.size() && maxHeap[rightChild] > maxHeap[largest]) {
            largest = rightChild;
        }

        if (largest != index) {
            std::swap(maxHeap[index], maxHeap[largest]);
            maxHeapifyDown(largest);
        }
    }
};
