#------------------------
# Do not modify this file
#------------------------

all: trader

# For part1: marked with a blank trader
market: stockify.h trie.h main.cpp market.cpp worker.cpp blank_trader.cpp
	g++ -std=c++17 main.cpp market.cpp worker.cpp blank_trader.cpp -lpthread -o market
	./market 2 

# For part2
trader: dummy_market.h market.h stockify.h stockify2.h trader_objects.h trie.h main.cpp market.cpp worker.cpp trader.cpp
	g++ -std=c++17 main.cpp market.cpp worker.cpp trader.cpp -lpthread -o trader
	./trader 0 >> output.txt

clean:
	rm -rf market trader output.txt
	rm -rf *~
