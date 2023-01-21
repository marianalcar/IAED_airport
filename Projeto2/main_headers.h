#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_ID 3
#define MAX_COUNTRY 30
#define MAX_CITY 50
#define MAX_AIRPORT 40
#define LEN_ID 3
#define MAX_CODE 6
#define MAX_DATE 10
#define MAX_HOUR 5
#define LEN_CODE 2
#define MAX_FLIGHTS 30000

#define MIN_RES_CODE 10
#define TABLE_SIZE 30323

typedef struct{
    char id[MAX_ID+1],country[MAX_COUNTRY+1],city[MAX_CITY+1];
    int flight_counter;
} airport;

typedef struct{
    int day,month,year;
}date;

typedef struct{
    int hour,minutes;
}hours;

typedef struct res{
    int pass_num;
    char *res_code;
    char *flight_code;
    struct res *next;
    struct res *prev;
}reservation;


typedef struct{
    reservation** items;
    int size;
    int ocupation;
}hash_table_def;

typedef struct{
    char code[MAX_CODE+1],id_partida[MAX_ID+1],id_chegada[MAX_ID+1];
    date departure_date,arrival_date;
    hours length;
    hours departure_hour,arrival_hour;
    int capacity;
    reservation *res_list;
    int counter_res;
}flights;


int create_hash_id(char res[],int size);

hash_table_def* create_table();

reservation* create_item(char *flight_code);

void insert_hash(reservation *new_node, hash_table_def *res_table);

int insert_hash_res_code(reservation *res1, hash_table_def *res_table);
 