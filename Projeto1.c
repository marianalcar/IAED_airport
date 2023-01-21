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

typedef struct{
    char code[MAX_CODE+1],id_partida[MAX_ID+1],id_chegada[MAX_ID+1];
    date departure_date,arrival_date;
    hours length;
    hours departure_hour,arrival_hour;
    int capacity;
}flights;

airport airports[MAX_AIRPORT];/*where airports are saved*/
int num_airports = 0;/*airports counter*/
date data;
hours h;
flights flight_list[MAX_FLIGHTS];/*where flights are saved*/
int num_flights = 0;/*flights counter*/

/*checks if the date is in the correct range*/
int verify_date_aux(int d, int m, int y){
    /*checks if the new date is in the past*/
    if (y==data.year){
        if (m<data.month){
            return 0;
        }
        else if (m==data.month){
            if (d<data.day){
                return 0;
            } 
        }
    }
    /*checks if the new date is more than one year in the future*/
    else if (y>data.year){
        if (m>data.month){
            return 0;
        }
        else if (m==data.month){
            if (d>data.day){
                return 0;
            }
        }
    }
    else if (y<data.year){
        return 0;
    }
    return 1;
}

/*calculates the number of days in month with its number*/
int days_in_month(int month){
    return 28 + ((month + (month/8)) % 2) + (2 % month) + 2*(1/month); 
}

/*advances the hour with the time of length*/
void forward_hour(flights *f1){
    int sum_minutes,sum_hours;
    sum_minutes = (*f1).length.minutes + (*f1).departure_hour.minutes;
    sum_hours = (*f1).length.hour + (*f1).departure_hour.hour;
    /*if only the minutes change*/
    if (sum_minutes<60){
        (*f1).arrival_hour.minutes = sum_minutes;
    }
    else{
        (*f1).arrival_hour.minutes = sum_minutes % 60;
        sum_hours++;
    }
    /*if only the hours change*/
    if (sum_hours<24){
        (*f1).arrival_hour.hour = sum_hours;
    }
    else{
        (*f1).arrival_hour.hour = sum_hours%24;
        (*f1).arrival_date.day++;
    }
    /*if the day current day is the last of the month and it changes*/
    if (((*f1).arrival_date.day==32 && 
        days_in_month((*f1).arrival_date.month)==31) ||
            ((*f1).arrival_date.day==31 && 
            days_in_month((*f1).arrival_date.month)==30) ||
                ((*f1).arrival_date.day==29 && 
                days_in_month((*f1).arrival_date.month)==28)){
        (*f1).arrival_date.month++;
        (*f1).arrival_date.day = 1;
    }
    /*if the current day is the last day of the year and it changes*/
    if ((*f1).arrival_date.month==13){
        (*f1).arrival_date.month = 1;
        (*f1).arrival_date.year++;
    }
    return;
}

/*returns false and error if the id of aiport is incorrect*/
int aux_error_invalid_id(char id[]) {
    int i;
    for (i = 0; i < MAX_ID; i++) {
        if (!isupper(id[i])){
            printf("invalid airport ID\n");
            return 0;
        }
    }
    return 1;
}

/*returns false and error if the limit of airports is reached*/
int aux_error_many_airports(int num_airports){
    if (num_airports==MAX_AIRPORT){
        printf("too many airports\n");
        return 0;
    }
    return 1;
}

/*returns false and error if there is two airports with the same id*/
int aux_error_duplicate_airport(char id[], int num_airports) {
    int i;
    for (i = 0; i < num_airports; i++) {
        if (strcmp(airports[i].id, id) == 0) {
            printf("duplicate airport\n");
            return 0;
        }
    }
    return 1;
}

/*sorts the aiports by the id in alphabetic order*/
void insertion_sort_airport(){
    airport a;
    int i,j;
    for (i = 1; i < num_airports; i++) {
        a = airports[i];
        j = i-1;
        while (j>=0 && (strcmp(a.id, airports[j].id) < 0) ) {
            airports[j+1] = airports[j];
            j--;
        }
        airports[j+1] = a;
    }
}

/*returns true if the id exists in the airport*/
int id_exists(char id[MAX_ID]){
    int i;
    for (i=0;i<num_airports;i++){
        if (strcmp(airports[i].id,id)==0){
            return 1;
        }
    }
    return 0;
}

/*returns false and error if the letter in the code are not upper case*/
int error_is_upper(char code[]){
    if (!isupper(code[0]) || !isupper(code[1])){
        printf("invalid flight code\n");
        return 0;
    }
    return 1;
}

/*returns false and error if the digits in the code are not digits*/
int error_flight_code_digits(int comp,char code[]){
    int i;
    for (i=2;i<comp;i++){
        if (!isdigit(code[i])){
            printf("invalid flight code\n");
            return 0;
        }
    }
    return 1;
}

/*returns false and error if the flight code already exists in the flight list*/
int error_flight_already_exists(int num_flights,char code[],int d, int m,int y){
    int i;
    for (i=0;i<num_flights;i++){
        if (strcmp(flight_list[i].code,code)==0 && 
            flight_list[i].departure_date.day==d && 
            flight_list[i].departure_date.month==m && 
            flight_list[i].departure_date.year==y){
            printf("flight already exists\n");
            return 0; 
        }
    }
    return 1;
}

/*returns false and error if the limit of flights is reached*/
int error_too_many_flights(int num_flights){
    if (num_flights==MAX_FLIGHTS){
        printf("too many flights\n");
        return 0;
    }
    return 1;
}

/*returns false and error if the flight duration is greater than 12*/
int error_invalid_duration(int h,int min){
    if (h==12 && min>0){
        printf("invalid duration\n");
        return 0;
    }
    return 1;
}

/*returns false and error if the capacity is no in the wanted range*/
int error_invalid_capacity(int capacity){
    if (capacity<10 || capacity>100){
        printf("invalid capacity\n");
        return 0;
    }
    return 1;
}

/*returns true if the first date is first than the second*/
int compare_date_hour(date d1, date d2, hours h1, hours h2){
    /*checks which date has the less number of months*/
    if ((d1.year*12 + d1.month) < (d2.year*12 + d2.month)){
        return 1;
    }
    else if ((d1.year*12 + d1.month) > (d2.year*12 + d2.month)){
        return 0;
    }
    else{
        if (d1.day<d2.day){
            return 1;
        }
        else if (d1.day>d2.day){
            return 0;
        }
        else{
            if (h1.hour<h2.hour){
                return 1;
            }
            else if (h1.hour>h2.hour){
                return 0;
            }
            else{
                if (h1.minutes<h2.minutes){
                    return 1;
                }
                else if (h1.minutes>h2.minutes){
                    return 0;
                }
            }
        }
    }
    return 0;
}

/*sorts the departure dates by day and hour*/
void sort_date_departure_hour(flights *flight_departure_list,int n){
    flights f;
    int i,j;
    for (i = 1; i < n; i++) {
        f = flight_departure_list[i];
        j = i-1;
        while (j>=0 && 
                compare_date_hour(f.departure_date,
                flight_departure_list[j].departure_date,f.departure_hour,
                flight_departure_list[j].departure_hour)){
            flight_departure_list[j+1] = flight_departure_list[j];
            j--;
        }
        flight_departure_list[j+1] = f;
    }
}

/*sorts the arrival dates by day and hour*/
void sort_date_arrival_hour(flights *flight_arrival_list, int n){
    flights f;
    int i,j;
    for (i = 1; i < n; i++) {
        f = flight_arrival_list[i];
        j = i-1;
        while (j>=0 && 
                compare_date_hour(f.arrival_date,
                flight_arrival_list[j].arrival_date,f.arrival_hour,
                flight_arrival_list[j].arrival_hour)){
            flight_arrival_list[j+1] = flight_arrival_list[j];
            j--;
        }
        flight_arrival_list[j+1] = f;
    }
}

/*adds the flight if it doesnt contain any errors and if so returns the errors*/
void airport_add_errors(){
    airport a1;

    scanf("%s %s %[^\n]",a1.id,a1.country,a1.city);
    a1.flight_counter = 0;
    
    if (!aux_error_invalid_id(a1.id)) {
        return;
    }
    if (!aux_error_many_airports(num_airports)){
        return;
    }
    if (!aux_error_duplicate_airport(a1.id,num_airports)){
        return;
    }
    airports[num_airports] = a1;
    num_airports++;
    printf("airport %s\n",a1.id);
}

/*lists the airports with the given ids by the given order*/
void airport_list_w_ids(){
    int i = 0,existe = 0,pos = 0;
    char id[MAX_ID], test = ' ';

    while (test!='\n'){
        scanf("%3s",id);
        for (i=0;i<num_airports;i++){
            if (strcmp(airports[i].id,id)==0){
                existe = 1;
                pos = i;
                break;
            }
        }
        if (existe!=1){
            printf("%s: no such airport ID\n",id);
        }
        else{
            printf("%s %s %s %d\n",airports[pos].id,airports[pos].city,
            airports[pos].country,num_flights);
        } 
        test = getchar();
        existe = 0;
    }
    return;
}

/*lists the airports in the airports list by alphabetic order*/
void airport_list_wo_ids(){
    int i;
    insertion_sort_airport();
    for (i = 0; i<num_airports;i++){
        printf("%s %s %s %d\n",airports[i].id,airports[i].city,
        airports[i].country,airports[i].flight_counter);
    }
    return;
}

/*adds the flight to the flight list with the given parameters 
if it doesnt contain erros and if so returns the error*/
void add_flights(){
    flights f1;
    int d,m,y;
    int h1,min1;
    int h2,min2;
    int comp,i;
    char departure_date[MAX_DATE+1],get_length[MAX_HOUR+1];
    char departure_hour[MAX_HOUR+1];

    scanf("%s %s %s %s %s %s %d",f1.code,f1.id_partida,f1.id_chegada,
    departure_date,departure_hour,get_length,&f1.capacity);


    sscanf(departure_date,"%d-%d-%d",&d,&m,&y);

    f1.departure_date.day = d;
    f1.departure_date.month = m;
    f1.departure_date.year = y;

    f1.arrival_date.day = d;
    f1.arrival_date.month = m;
    f1.arrival_date.year = y;

    sscanf(departure_hour,"%d:%d",&h1,&min1);

    f1.departure_hour.hour = h1;
    f1.departure_hour.minutes = min1;

    f1.arrival_hour.hour = h1;
    f1.arrival_hour.minutes = min1;

    sscanf(get_length,"%d:%d",&h2,&min2);

    f1.length.hour = h2;
    f1.length.minutes = min2;

    if (!error_is_upper(f1.code)){
        return;
    }
    comp=strlen(f1.code);
    if (!error_flight_code_digits(comp,f1.code)){
        return;
    }
    if (!error_flight_already_exists(num_flights,f1.code,d,m,y)){
        return;
    }

    if (!id_exists(f1.id_partida)){
        printf("%s: no such airport ID\n",f1.id_partida);
        return;
    }
    else if (!id_exists(f1.id_chegada)){
        printf("%s: no such airport ID\n",f1.id_chegada);
        return;
    }
    if (!error_too_many_flights(num_flights)){
        return;
    }
    if (!verify_date_aux(d,m,y)){
        printf("invalid date\n");
        return;
    }
    if (!error_invalid_duration(h2,min2)){
        return;
    }
    if (!error_invalid_capacity(f1.capacity)){
        return;
    }
    forward_hour(&f1);
    flight_list[num_flights] = f1;
    
    num_flights++;

    for (i = 0;i<num_airports;i++){
        if (strcmp(airports[i].id,f1.id_partida)==0){
            airports[i].flight_counter++;
            break;
        }
    }
    
    return;
}

/*lists the flights by the given order*/
void list_flights(){
    int i;
    for (i = 0;i<num_flights;i++){
        printf("%s %s %s %02d-%02d-%04d %02d:%02d\n",flight_list[i].code,
        flight_list[i].id_partida,flight_list[i].id_chegada,
        flight_list[i].departure_date.day,flight_list[i].departure_date.month,
        flight_list[i].departure_date.year,flight_list[i].departure_hour.hour,
        flight_list[i].departure_hour.minutes);
    }
    return;
}

/*lists the flights that have the given id as the departure id*/
void list_departure_flights(){
    int existe = 0,n = 0,i;
    char id[MAX_ID+1];
    flights flight_departure_list[MAX_FLIGHTS];

    scanf("%s",id);

    for (i = 0;i<num_airports;i++){
        if (strcmp(airports[i].id,id)==0){
            existe = 1;
            break;
        }
    }
    if (existe!=1){
        printf("%s: no such airport ID\n",id);
    }
    for (i = 0;i<num_flights;i++){
        if (strcmp(flight_list[i].id_partida,id)==0){
            flight_departure_list[n] = flight_list[i];
            n++;
        }
    }
    sort_date_departure_hour(flight_departure_list,n);
    for (i = 0;i<n;i++){
        printf("%s %s %02d-%02d-%02d %02d:%02d\n",flight_departure_list[i].code,
        flight_departure_list[i].id_chegada,
        flight_departure_list[i].departure_date.day,
        flight_departure_list[i].departure_date.month,
        flight_departure_list[i].departure_date.year,
        flight_departure_list[i].departure_hour.hour,
        flight_departure_list[i].departure_hour.minutes);
    }
    return;

}

/*lists the flights that have the given id as the arrival id*/
void list_arrival_flights(){
    int existe = 0,n = 0,i;
    char id[MAX_ID+1];
    flights flight_arrival_list[MAX_FLIGHTS];

    scanf("%s",id);

    for (i = 0;i<num_airports;i++){
        if (strcmp(airports[i].id,id)==0){
            existe = 1;
            break;
        }
    }
    if (existe!=1){
        printf("%s: no such airport ID\n",id);
    }
    for (i = 0;i<num_flights;i++){
        if (strcmp(flight_list[i].id_chegada,id)==0){
            flight_arrival_list[n] = flight_list[i];
            n++;
        }
    }
    sort_date_arrival_hour(flight_arrival_list, n);
    for (i = 0;i<n;i++){
        printf("%s %s %02d-%02d-%02d %02d:%02d\n",flight_arrival_list[i].code,
        flight_arrival_list[i].id_partida,
        flight_arrival_list[i].arrival_date.day,
        flight_arrival_list[i].arrival_date.month,
        flight_arrival_list[i].arrival_date.year,
        flight_arrival_list[i].arrival_hour.hour,
        flight_arrival_list[i].arrival_hour.minutes);
    }
    return;

}

/*updates the date with the given day month and year*/
void forward_data(date *data){
    int d,m,y;

    scanf("%d-%d-%d",&d,&m,&y);

    if (verify_date_aux(d,m,y)){
        (*data).day=d;
        (*data).month=m;
        (*data).year=y;

        printf("%02d-%02d-%d\n",(*data).day,(*data).month,(*data).year);
        return;
    }
    printf("invalid date\n");
    return;
}

int main(){
    char entrada,space_notspace;

    entrada = getchar();

    data.day = 1;
    data.month = 1;
    data.year = 2022;

    while (1){
        switch (entrada){
            case 'q':
                return 0;
                break;

            case 'a':
                airport_add_errors();
                break;

            case 'l':
                space_notspace = getchar();
                if (space_notspace=='\n'){
                    airport_list_wo_ids();
                    break;
                }
                else{
                    airport_list_w_ids();
                    break;
                }
            case 'v':
                space_notspace = getchar();
                if (space_notspace=='\n'){
                    list_flights();
                    break;
                }
                else{
                    add_flights();
                    break;
                }
            case 'p':
                list_departure_flights();
                break;

            case 'c':
                list_arrival_flights();
                break;

            case 't':
                forward_data(&data);
                break;
        }
        entrada = getchar();
    }
    return 0;
}