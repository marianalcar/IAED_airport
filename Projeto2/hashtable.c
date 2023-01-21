#include "main_headers.h"

/*creates a hashtbale and set the entries to NULL*/
hash_table_def* create_table(int size){
    hash_table_def* table = (hash_table_def*)malloc(sizeof(hash_table_def));
    table->size = size;
    table->items = (reservation**)calloc(table->size,sizeof(reservation*));
    table->ocupation = 0;
    return table;
}

/*creates an hash id based om the reservation code and size of the table*/
int create_hash_id(char res_code[],int size){
    int comp = strlen(res_code),i =0, sum = 0, a = 127;

    for (i = 0;i < comp; i++){
        sum += (a * sum + res_code[i]) % 17;
    }
    return sum % size;
}


/*creates an item to put in hash table*/
reservation* create_item(char *flight_code) {
    reservation* res1 = (reservation*)malloc (sizeof(reservation));
    res1->flight_code = (char*)malloc(strlen(flight_code) +1);
    strcpy(res1->flight_code,flight_code);

    return res1;
}

/*inserts the item in the hash table with the given hash id*/
void insert_hash(reservation *new_node, hash_table_def *res_table){
    int id = create_hash_id(new_node->res_code,res_table->size);
    
    while (res_table->items[id] != NULL){
        id++;
    }
    res_table->items[id] = new_node;
    res_table->ocupation++;
    return;
}

