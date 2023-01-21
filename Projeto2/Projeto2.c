#include "main_headers.h"

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

/*returns false and error if the given flight code is wrong*/
int error_flight_code(int comp,char code[]){
    int i = 0;
    if (comp > 8){
        printf("invalid flight code\n");
        return 0;
    }
    if (!isalpha(code[0]) || !isalpha(code[1])){
        printf("invalid flight code\n");
        return 0;
    }
    if (isdigit(code[2])){
        for (i=2;i<comp;i++){
            if ((comp > 6) || !isdigit(code[i])){
                printf("invalid flight code\n");
                return 0;
            }
        }
        if (isdigit(code[3])){
            for (i=3;i<comp;i++){
                if ((comp > 7) || !isdigit(code[i])){
                    printf("invalid flight code\n");
                    return 0;
                }
            }
        }
    }
    if (!isdigit(code[4])){
        printf("invalid flight code\n");
        return 0;
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
    if (capacity<10){
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

    comp=strlen(f1.code);

    if (!error_flight_code(comp,f1.code)){
        return;
    }

    if (!error_flight_already_exists(num_flights,f1.code,d,m,y)){
        printf("flight already exists\n");
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
    f1.res_list = NULL;
    f1.counter_res = 0;
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
void list_arrival_flights(date data){
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
        /*with the same hour it only compares the day*/
        if (compare_date_hour(data, flight_arrival_list[i].arrival_date, 
            flight_arrival_list[i].arrival_hour, flight_arrival_list[i].arrival_hour)){
            printf("%s %s %02d-%02d-%02d %02d:%02d\n",flight_arrival_list[i].code,
            flight_arrival_list[i].id_partida,
            flight_arrival_list[i].arrival_date.day,
            flight_arrival_list[i].arrival_date.month,
            flight_arrival_list[i].arrival_date.year,
            flight_arrival_list[i].arrival_hour.hour,
            flight_arrival_list[i].arrival_hour.minutes);
            }
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

/*searchs flight in fligh list by the given flight code and date*/
flights* search_flight_with_data(char flight_code[], date* flight_date){
    int i;
    for (i = 0; i < num_flights; i++){
        if (strcmp(flight_list[i].code,flight_code) == 0 && 
            flight_list[i].departure_date.day == flight_date->day &&
            flight_list[i].departure_date.month == flight_date->month &&
            flight_list[i].departure_date.year == flight_date->year){
                return &flight_list[i];

        }
    }
    return NULL;
}

/*searchs flight in fligh list by the given flight code*/
flights search_flight(char *code){
    int i,pos = 0;
    for (i = 0; i < num_flights; i++){
        if (strcmp(flight_list[i].code,code) == 0){
            pos = i;
        }
    }
    return flight_list[pos];
}

/*returns the id of the given flight in the flight list*/
int search_flight_id(char *code){
    int i;
    for (i = 0; i < num_flights; i++){
        if (strcmp(flight_list[i].code,code) == 0){
            return i;
        }
    }
    return -1;
}

/*creates the node that is going to be in the linked list*/
reservation *create_new_node(reservation *new_res){
    reservation *new_node = (reservation*)malloc(sizeof(reservation));
    
    new_node = new_res;
    new_node->next = NULL;

    return new_node;
}

/*inserts the reservation in the linked list in a sorted way*/
void sorted_push(reservation **head, reservation *new_node){
    reservation *current;

    /*if the head is after the given reservation or there is no head*/
    if ((*head == NULL) || strcmp((*head)->res_code,new_node->res_code) >= 0){
        new_node->next = *head;
        *head = new_node;
    }
    else{/*rest of the cases*/
        current = *head;
        while (current->next != NULL) {
            if (strcmp(current->next->res_code,new_node->res_code) > 0) break;
            current = current->next;
        }
        new_node->next = current->next;
        current->next = new_node;
    }
}

/*prints all the resrvations in the linked list*/
void print_res_list(reservation *head){
    reservation* temp = head;

    while (temp != NULL){
        printf("%s %d\n",temp->res_code, temp->pass_num);
        temp = temp->next;
    }
    
}

/*deletes the given reservation in its flight's linked list*/
void delete_res(reservation **head, char *res_code){
    reservation *temp, *prev;
    temp = *head;

    /*if head points to reservation with the given res code*/
    if (temp != NULL && temp->res_code == res_code){
        *head = temp->next;
        free(temp);
        return;
    }
    /*search for the res in list that has the given res code*/
    while (temp != NULL && temp->res_code != res_code){
        prev = temp;
        temp = temp->next;
    }
    /*if the res code was not present in no reservation in the list*/
    if (temp == NULL){
        return;
    }
    prev->next = temp->next;
    free(temp);
}

/*checks if the given reservation code has already been used*/
int error_reservation_already_used(char res_code[], hash_table_def *hash_table){
    int id = create_hash_id(res_code,hash_table->size);
    int iter = id;
    
    if (hash_table->items[id] != NULL){
        while(iter<TABLE_SIZE){
            /*checks if the reservation code is the same as 
            the one in the hash table*/
            if (strcmp(hash_table->items[iter]->res_code,res_code) == 0){
                printf("%s: flight reservation already used\n",res_code);
                return 0;
            }
            iter++;
        }
    }
    return 1;
}

/*checks if the given reservation code is incorrect*/
int error_invalid_reservation(char res_code[]){
    int comp, i = 0;

    comp = strlen(res_code);
    if (comp < MIN_RES_CODE){
        printf("invalid reservation code\n");
        return 0;
    }
    for (i = 0;i < comp;i++){
        if (!isupper(res_code[i]) && !isdigit(res_code[i])){
            printf("invalid reservation code\n");
            return 0;
        }
    }
    return 1;
}

/*checks if the reservation when added exceeds the flight's capicity*/
int error_too_many_reservations(flights *f1){
    if (f1->counter_res > f1->capacity){
        printf("too many reservations\n");
        return 0;
    }
    return 1;
}

/*checks if the given passenger number is negative*/
int error_invalid_pass_num(int pass_num){
    if (pass_num < 0){
        printf("invalid passenger number\n");
        return 0;
    }
    return 1;
}


int errors_in_r(int pass_num, char res_code[],hash_table_def *hash_table,
    date *flight_date,char *flight_code,flights *f1){
    int d,m,y;

    d = flight_date->day;
    m = flight_date->month;
    y = flight_date->year;

    if (!error_invalid_reservation(res_code)){
        return 0;
    }
    if (error_flight_already_exists(num_flights, flight_code, d, m, y)){
        printf("%s: flight does not exist\n",flight_code);
        return 0;
    }
    if(!error_reservation_already_used(res_code, hash_table)||
    !error_too_many_reservations(f1)){
        return 0;
    }
    if (!verify_date_aux(d, m, y)){
        printf("invalid date\n");
        return 0;
    }
    if (!error_invalid_pass_num(pass_num)){
        return 0;
    }
    return 1;
}

/*checks if the reservation is in the hashlist*/
int exists_reservation(char *code,hash_table_def *res_table){
    int hash = create_hash_id(code,res_table->size);
    if (res_table->items[hash] == NULL){
        return 0;
    }
    return 1;
}

/*adds the reservation in linked list of the given flight code 
and in the hash list*/
void add_reservations(char *flight_code,date *flight_date,
    hash_table_def *res_table){
    char string[65535];
    reservation* res1;
    reservation *new_node;
    char *res_code, *token2;
    char delim[2] = " ";
    int pass_num;
    flights* f1;

    if (error_flight_already_exists(num_flights,flight_code,flight_date->day,
        flight_date->month,flight_date->year)){
            printf("%s: flight does not exist\n",flight_code);
            return;
    }
    
    f1 = search_flight_with_data(flight_code,flight_date);

    res1 = create_item(flight_code);

    scanf("%[^\n]",string);
    res_code = strtok(string, delim);
    if (res_code != NULL) {

        token2 = strtok(NULL, delim);
        pass_num = atoi(token2);

        res1->res_code = (char*)malloc(strlen(res_code) +1);
        res1->pass_num = pass_num;
        strcpy(res1->res_code,res_code);
    }
    f1->counter_res += res1->pass_num;

    if (!errors_in_r(res1->pass_num, res1->res_code, res_table, flight_date, 
        flight_code,f1)){
        f1->counter_res -= res1->pass_num;
        return;
    }
    new_node = create_new_node(res1);
    insert_hash(new_node,res_table);
    sorted_push(&(f1->res_list),new_node);
    return;
}

/*prints all the reservations in the linked list of the given flight*/
void list_reservations(char *flight_code,date *flight_date){
    flights *f1;
    if (error_flight_already_exists(num_flights, flight_code, flight_date->day, 
        flight_date->month, flight_date->year)){
        printf("flight does not exist\n");
        return;
    }
    if (!verify_date_aux(flight_date->day, flight_date->month,
        flight_date->year)){
        printf("invalid date\n");
        return ;
    }
    f1 = search_flight_with_data(flight_code,flight_date);
    print_res_list(f1->res_list);

    return;
}

/*eliminates the given flight or reservation and frees its memory*/
void free_flight_res(hash_table_def *res_table,date data){
    char code[65535];
    char *flight_code;
    reservation* curr_res, *next_res;
    int i, i_voo,comp,hash;
    flights f1;
    int removeu = 0;

    scanf("%s",code);

    comp = strlen(code);       

    if (comp<10){
        if ((i_voo = search_flight_id(code)) !=-1) {
            /*with the same hours, it only compares the day*/
            if(compare_date_hour(flight_list[i_voo].arrival_date, data,
                flight_list[i_voo].arrival_hour,flight_list[i_voo].arrival_hour)){
                    removeu = 1;
                while ((i_voo = search_flight_id(code)) !=-1){
                    if (strcmp(code,flight_list[i_voo].code) == 0){
                        f1 = flight_list[i_voo];
                        curr_res = f1.res_list;
                        while (curr_res != NULL){
                            hash = create_hash_id(curr_res->res_code, res_table->size);
                            res_table->items[hash] = NULL;
                            next_res = curr_res->next;
                            free(curr_res->res_code);
                            free(curr_res->flight_code);
                            curr_res->res_code = NULL;
                            curr_res->flight_code = NULL;
                            free(curr_res);
                            curr_res = next_res;
                        }
                        for (i = i_voo; i < num_flights; i++){  
                            flight_list[i] = flight_list[i+1]; 
                        }
                        num_flights--;
                    }
                }
            }
            if (removeu == 0){
                printf("not found\n");
                return; 
            }
            return;
        }
        else {
            printf("not found\n");
            return;
        }
        
    }
    if (!exists_reservation(code,res_table)){
        printf("not found\n");
        return;
    }
    hash = create_hash_id(code, res_table->size); 
    flight_code = res_table->items[hash]->flight_code;
    f1 = search_flight(flight_code);
    delete_res(&(f1.res_list), code);
    res_table->items[hash] = NULL;
    return;
}

/*frees all the allocated memory*/
void free_all(hash_table_def *res_table){
    int i;
    for (i = 0; i < res_table->size;i++){
        if (res_table->items[i] != NULL){
            free(res_table->items[i]->res_code);
            free(res_table->items[i]->flight_code);
            free(res_table->items[i]);
        }
    }
    free(res_table->items);
    free(res_table);
    return;
}





int main(){
    char entrada,space_notspace;
    char flight_code[MAX_CODE];
    date flight_date; 

    
    hash_table_def* res_table = create_table(TABLE_SIZE);
    /*printf("size_reserva = %lu, size_hash = %lu\n", sizeof(reservation), sizeof(hash_table_def));
*/
    entrada = getchar();

    data.day = 1;
    data.month = 1;
    data.year = 2022;

    while (1){
        switch (entrada){
            case 'q':
                free_all(res_table);
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
                list_arrival_flights(data);
                break;

            case 't':
                forward_data(&data);
                break;
            
            case 'r':
                scanf("%s %d-%d-%d",flight_code, &flight_date.day,
                    &flight_date.month, &flight_date.year);

                if (getchar() == '\n'){
                    list_reservations(flight_code, &flight_date);
                }
                else{
                    add_reservations(flight_code, &flight_date, res_table);
                }
                break;
            case 'e':
                free_flight_res(res_table,data);
                break;
        }
        entrada = getchar();
    }
    return 0;
}