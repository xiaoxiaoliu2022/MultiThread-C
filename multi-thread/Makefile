all: run_mt_search_small run_mt_search_large run_valgrind clean

mt_search: mt_search.c
	gcc -std=c99 -pthread -o mt_search mt_search.c -g

run_mt_search_small: mt_search
	@echo -e  "\033[1;93mRunning on large data...\033[0m"
	./mt_search inputs/nums10000.txt 100 4 10000
	
run_mt_search_large: mt_search
	@echo -e  "\033[1;93mRunning on small data...\033[0m"
	./mt_search inputs/input2.txt 100 4 26

run_valgrind: mt_search
	@echo -e  "\033[1;93mChecking memory leak...\033[0m"
	valgrind --leak-check=full ./mt_search
	
clean:
	rm mt_search; rm -rf mt_search.dSYM
