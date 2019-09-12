#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define BILLION  1000000000.0

long int len = 0;
int *data[2] = {NULL,NULL};

long int get_no_of_lines(char* filename){
    FILE* fp = fopen(filename,"r");
    char ch;
    long int lines = 0;
    while(!feof(fp)){
        ch = fgetc(fp);
        if(ch == '\n'){
            lines++;
        }
    }
    fclose(fp);
    //printf("lines=%ld\n",lines);
    return lines;
}
void create_csv(char* filename){
    FILE* fp = fopen(filename,"w");
    for(long int i=0;i<10000;i++){
        int x = ((double)rand() / (double)(RAND_MAX+1.0) )*1000;
        int y = ((double)rand() / (double)(RAND_MAX+1.0) )*1000;
        fprintf(fp,"%d,%d\n",x,y);
    }
    fclose(fp);
}
void read_file(char* filename){
    FILE* fp = fopen(filename,"r");
    for(long int i=0;i<len;i++){
        fscanf(fp,"%d,%d\n",&data[0][i],&data[1][i]);
    }
    fclose(fp);
}

double compute_t(long int len,int* values[2]){
    double sum1 = 0, sum2 = 0;
    for(long int i=0;i<len;i++){
        sum1 += values[0][i];
        sum2 += values[1][i];
    }
    double mean1 = sum1 / len;
    double mean2 = sum2 / len;
    double SD1 = 0,SD2 = 0;
    for(long int i=0;i<len;i++){
        SD1 += (values[0][i]-mean1)*(values[0][i]-mean1);
        SD2 += (values[1][i]-mean2)*(values[1][i]-mean2);
    }
    SD1 = SD1/len;
    SD2 = SD1/len;

    double t = (mean2-mean1)/sqrt((SD1+SD2)/len);
    return t;
}
void get_sample(int size, int* sample[2], int times){
    int i=0;
    while(i<size){
        int t = (int)time(NULL) ^ i ^ times;
        double rand = (double)rand_r(&t) / (double)(RAND_MAX+1.0) ;
        int index = len*rand;
        sample[0][i] = data[0][index];
        sample[1][i] = data[1][index];
        i++;
    }
}
struct args
{
    long int times;
    int size;
    double original_t;
};

void* get_higher_t_count(void* params){
    struct args *param = (struct args*) params;
    long int times = param->times;
    const int size = param->size;
    double original_t = param->original_t;
    //declare sample
    int sample[2][size];
    //no of times we get a higher t_stats
    long int* count = (long int*)malloc(sizeof(long int));
    *count = 0;
    double t;
    int* sample_data[2] = {sample[0],sample[1]}; 
    while(times--){
        get_sample(size, sample_data, times);//times for random generator
        t = compute_t(size, sample_data);
        if(t >= original_t){
            (*count)++;
        }
    }
    //printf("count = %ld\n",*count);
    pthread_exit(count);
}

int main(int argc, char **argv){
    int thread_count = atoi(argv[1]);
    long int nTimes = atoi(argv[2]);
    int size = 20;

    long int n_per_thread = nTimes / thread_count;
    pthread_t thread[thread_count];
    int i=0;
    long int *result = NULL,sum = 0;

    //create file
    char filename[] = "data.csv";
    create_csv(filename);

    //read data from file
    const long int length = get_no_of_lines(filename);
    int values[2][length];
    len = length;
    data[0] = values[0];
    data[1] = values[1];
    read_file(filename);
    
    //time computation
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    //get original t
    double original_t = compute_t(len, data);
    printf("original_t = %lf\n",original_t);
    while(i<thread_count){
        struct args param = { n_per_thread, size, original_t };
        if(pthread_create(thread+i, NULL, get_higher_t_count, (void*)&param)) {
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
    clock_gettime(CLOCK_REALTIME, &end);

	// time_spent = end - start
	double time_spent = (end.tv_sec - start.tv_sec) +
						(end.tv_nsec - start.tv_nsec) / BILLION;

    printf("no of times we got higher value = %ld\n",sum);
	printf("Time elpased is %lf seconds\n", time_spent);

    FILE *fp1 = fopen("report.txt", "a"); 
    fprintf(fp1,"Que2: #Threads = %d, #Higher_than_t = %ld, Time elpased is %lf seconds\n",thread_count,sum,time_spent);
    fclose(fp1);

    //writing data into file
    FILE *fp = NULL;
    if(nTimes == 10000000){
        fp = fopen("que2_plot_data_10M.txt", "a"); 
    }
    else
        fp = fopen("que2_plot_data_1M.txt", "a"); 
    if (fp == NULL) { 
        fprintf(stderr,"Couldn't open file..."); 
        return 3; 
    } 
    fprintf(fp,"%d %lf \n",thread_count,time_spent);
    fclose(fp);

    return 0;
}