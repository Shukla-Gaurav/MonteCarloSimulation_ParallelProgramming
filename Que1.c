#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BILLION  1000000000.0

void* pointsInsideUnitCircle(void* args){
    long int times = *(int*) args;
    long int* count = (long int*)malloc(sizeof(long int));
    *count = 0;
    while(times--){
        int t = (int)time(NULL) ^ (int)times;
        double rand1 = (double)rand_r(&t) / (double)RAND_MAX ;
        t = t^2;
        double rand2 = (double)rand_r(&t) / (double)RAND_MAX ;
        double x = 2*rand1 - 1;
        double y = 2*rand2 - 1;
        if(x*x + y*y <=1){
            (*count)++;
        }
    }
    //printf("%ld\n",*count);
    pthread_exit(count);
}
void plot(){
    FILE *gnup = popen("gnuplot", "w"); 
    if (gnup == NULL) { 
        fprintf(stderr,"Couldn't open file..."); 
        exit(0); 
    } 
    fprintf(gnup,"set terminal png size 800,600\n");
    fprintf(gnup,"set output \"pi_vs_cores.png\"\n");
    fprintf(gnup,"set xlabel \"Number of Threads\"\n");
    fprintf(gnup,"set ylabel \"Time (in seconds)\"\n");
    fprintf(gnup,"plot \"que1_plot_data.txt\"  linetype 2 linecolor 1 with linespoint\n");
    pclose(gnup);
}
int main(int argc, char **argv){
    int thread_count = atoi(argv[1]);
    long int N = 10000000;
    long int n_per_thread = N / thread_count;
    pthread_t thread[thread_count];
    int i=0;
    long int *result = NULL,sum = 0;

    //time computation
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    while(i<thread_count){
        if(pthread_create(thread+i, NULL, pointsInsideUnitCircle, (void*)&n_per_thread)) {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
        i++;
    }
    i=0;
    while(i<thread_count){
        if(pthread_join(thread[i], (void**)&result)) {
            fprintf(stderr, "Error joining thread\n");
            return 2;
        }
        sum += *result;
        free(result);
        result = NULL;
        i++;
    }
    double pi = (4.0*sum)/(n_per_thread*thread_count);
    
    clock_gettime(CLOCK_REALTIME, &end);

	// time_spent = end - start
	double time_spent = (end.tv_sec - start.tv_sec) +
						(end.tv_nsec - start.tv_nsec) / BILLION;

    FILE *fp1 = fopen("report.txt", "a"); 
    fprintf(fp1,"Que1: #threads = %d, pi = %lf, Time elpased is %lf seconds\n",thread_count,pi,time_spent);
    fclose(fp1);

    //writing data into file
    FILE *fp = fopen("que1_plot_data.txt", "a"); 
    if (fp == NULL) { 
        fprintf(stderr,"Couldn't open file..."); 
        return 3; 
    } 
    fprintf(fp,"%d %lf \n",thread_count,time_spent);
    fclose(fp);
    return 0;
}