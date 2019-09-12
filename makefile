all: compile_run
compile_run: all_compile all_run

all_run: run1 run2_1M run2_10M 

run1: 
		./que1_output 1
		./que1_output 2
		./que1_output 4
		./que1_output 8
		./que1_output 16
		./que1_output 32
		./que1_output 40	
		gnuplot -e "set terminal png size 800,600; set output 'pi_vs_cores.png';set xlabel 'Number of Threads';set ylabel 'Time (in seconds)'; plot 'que1_plot_data.txt'  linetype 2 linecolor 1 with linespoint;quit"

run2_1M: 
			./que2_output 1 1000000
			./que2_output 2 1000000
			./que2_output 4 1000000
			./que2_output 8 1000000
			./que2_output 16 1000000
			./que2_output 32 1000000
			./que2_output 40 1000000
			gnuplot -e "set terminal png size 800,600; set output 't_stats_1M.png';set xlabel 'Number of Threads';set ylabel 'Time (in seconds)'; plot 'que2_plot_data_1M.txt'  linetype 2 linecolor 1 with linespoint;quit"

run2_10M: 
			./que2_output 1 10000000
			./que2_output 2 10000000
			./que2_output 4 10000000
			./que2_output 8 10000000
			./que2_output 16 10000000
			./que2_output 32 10000000
			./que2_output 40 10000000

			gnuplot -e "set terminal png size 800,600; set output 't_stats_10M.pdf';set xlabel 'Number of Threads';set ylabel 'Time (in seconds)'; plot 'que2_plot_data_10M.txt'  linetype 2 linecolor 1 with linespoint;quit"


all_compile: que1 que2
que1: 
	gcc Que1.c -o que1_output -lm -lpthread
que2: 	
	gcc Que2.c -o que2_output -lm -lpthread

clean: 
	rm *.output *.txt *.png *.plt
