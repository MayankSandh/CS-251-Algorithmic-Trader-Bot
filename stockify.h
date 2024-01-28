#include <iostream>
#include <vector>
#include <string>

// Function to split the input string into words
//concerns 
//1. Implement a faster algorithm - for sorting purpose you can use a simple O(n) algo.
//How you can implement an )(n) algo for this.
//when you have finally formed the words, which are basically  list of (stockname and stockquantity), whenever you find a new stock name which is not present in stock trie, insert it into the stocktrie so that it get allocated a index.
//Now make a bool array of size = no of stocks in stock trie. ( remember to allocate it in heap and also remember to delete it). initialize to zero. After that iterate over the stocknames present in words, for each stockname get the index and at the index turn the bool to 1. Also maintain an array of integers. when you turn the bool 1 then at the corresponding index in the integer array turn the value equal to the qunatity. 

//Now read the bool array from starting, wherever there is 1 corresponding to that concatenate the stockname and stockquantity to the output. 

//You also have to maintain a array of string also, so when you turn the bool 1, also turn the string = stockname.
//so there will be three arrays/vectors/list each of size = number of stocks.
//The algo is overall O(number of stocks).
//3. Your final sortedString terminate with the whitespace.
 
std::string stockify(const std::string &input) {
    std::string temp;
    bool kya = 0; //represent that the next token should be stockname.
    std::vector<std::string> tokens;
    for (char i : input) {
        if (i == ' ' || i == '+' || i == '-') {
            if (!temp.empty()) {
			if( kya == 0 )
			{
                		tokens.push_back(temp);
				kya = 1;
			}
			else
			{
				if( isalpha(temp[0]) )
				{
					tokens.push_back("1");
					tokens.push_back(temp);
				}
				else
				{
					tokens.push_back(temp);
					kya = 0;
				} 
			}
            }
            temp.clear();
            if (i != ' ') {
                temp.push_back(i);
            }
        } else {
            temp.push_back(i);
        }
    }
    if (!temp.empty()) {
	 	     if( kya == 0 )
			{
                		tokens.push_back(temp);
				kya = 1;
			}
			else
			{
				if( isalpha(temp[0]) )
				{
					tokens.push_back("1");
					tokens.push_back(temp);
				}
				else
				{
					tokens.push_back(temp);
					kya = 0;
				} 
			}    
		}
if( kya == 1 ) tokens.push_back("1");
//Please ensure that the input string do not include \n.
    	
    for (size_t i = 1; i < tokens.size(); i += 2) {
         if (tokens[i][0] != '-' && tokens[i][0] != '+') {
            
        } 
    }

    std::string output;
    bool isfirst = true;
    for (const auto& token : tokens) {
        if (isfirst){
            output+=token;
            isfirst = false;
            continue;
        }
        output += " " + token;
    }
    std::vector<std::string> words;
    std::string word;
    bool secondSpace = false; 
    for (char c : output) {
        if (c == ' ') {
            if (!secondSpace){
                secondSpace = true;
                word+=c;
                continue;
            }
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
                secondSpace = false;
            }
        } else {
            word += c;
        }
    }

		
    if (!word.empty()) {
        words.push_back(word);
    }
	//on which basis are you sorting brother.
	
    for (size_t i = 0; i < words.size() - 1; ++i) {
        size_t minIndex = i;
        for (size_t j = i + 1; j < words.size(); ++j) {
            if (words[j] < words[minIndex]) {
                minIndex = j;
            }
        }
        if (minIndex != i) {
            std::swap(words[i], words[minIndex]);
        }
    }
    std::string sortedString;
    for (const auto &word : words) {
        sortedString += word + " ";
	   
    }
	 sortedString = sortedString.substr(0, sortedString.length() - 1);
	//this means that the last character of your sortedString is whitespace.
    return sortedString;
}





