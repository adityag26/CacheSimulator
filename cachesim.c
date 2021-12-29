#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int inlog2(unsigned long x){
    unsigned long ans = 0;
    while(x > 1){
        x >>= 1;
        ans++;
    }
    return ans;
}

int cache_hit = 0;
int cache_miss = 0;
int mem_read = 0;
int mem_wri = 0;



int main(int argc, char* argv[]){
    int i, j;
    FILE * fPointer1;
    char* type = argv[2];
    char* poli = argv[3];
    char* name = argv[5];
    int blocks = atoi(argv[4]);
    int caches = atoi(argv[1]);
    int sbits, bbits, sets, a;
    if(caches == 0 || (caches & (caches -1)) != 0 || blocks == 0 || (blocks & (blocks -1)) != 0) return 0;
    if(strstr(type, "assoc:") != NULL){
        sscanf(type, "assoc:%d", &a);
        if(a == 0 || (a & (a-1)) != 0) return 0;
        sets = caches / (a * blocks);
        sbits = inlog2(sets);
        bbits = inlog2(blocks);
        unsigned long mask = (1 << sbits) -1;
        unsigned long **cachesys = malloc(sizeof(unsigned long*)*sets);
        for(i = 0; i < sets; i++){
            cachesys[i] = malloc(sizeof(unsigned long)*a);
        }
        fPointer1 = fopen(name, "r");
        unsigned long add;
        char hold[5];
        int **indexs = malloc(sizeof(int*)*sets);
        if(strcmp(poli, "fifo") == 0){
        for(i = 0; i < sets; i++){
            indexs[i] = malloc(sizeof(int)*1);
        }
        for(i = 0; i < sets; i++){
            indexs[i][0] = 0;
        }
        }
        else{
             for(i = 0; i < sets; i++){
            indexs[i] = malloc(sizeof(int)*a);
        }
        for(i = 0; i < sets; i++){
            for(j = 0; j < a; j++){
                indexs[i][j] = -1;
            }
         }
        }
        for(i = 0; i < sets; i++){
            for(j = 0; j < a; j++){
                cachesys[i][j] = -1;
            }
        }
        int d = fscanf(fPointer1, "%*x: %c %lx", &hold[0], &add);
        while(d == 2){
            unsigned long blockid = add >> bbits;
            unsigned long setid = blockid & mask;
            unsigned long tag = blockid >> sbits;
            if(hold[0] == 'R'){
                int hit = 0;
                for(i = 0; i < a; i++){
                    if(tag == cachesys[setid][i]){
                    hit = 1;
                    cache_hit++;
                    if(strcmp(poli, "fifo") != 0){
                     for(j = 0; j < a; j++){
                             if(j == i) indexs[setid][i] = 0;
                             else if(indexs[setid][j] == -1) continue;
                             else{
                                 int wx = indexs[setid][j];
                                 indexs[setid][j] = wx+2;
                             }
                         }   
                    }
                    break;
                    }
                }
                if(hit == 0){
                    cache_miss++;
                    mem_read++;
                    if(strcmp(poli, "fifo") == 0){
                    int index = indexs[setid][0];
                    cachesys[setid][index] = tag;
                    if(index+1 == a) indexs[setid][0] = 0;
                    else indexs[setid][0] = index+1;
                    }
                    else{
                       
                       
                        int index = 0;
                        int min = indexs[setid][0];
                        for(i = 0; i < a; i++){
                            if(indexs[setid][i] == -1){
                                index = i;
                                break;
                            }
                            else if(min < indexs[setid][i]){
                                index = i; 
                                min = indexs[setid][i];
                            }
                        }
                         cachesys[setid][index] = tag;
                         for(i = 0; i < a; i++){
                             if(i == index) indexs[setid][index] = 0;
                             else if(indexs[setid][i] == -1) continue;
                             else{
                                 int wx = indexs[setid][i];
                                 indexs[setid][i] = wx+2;
                             }
                         }

                    }
                }
            }
            else if(hold[0] == 'W'){
                int hit = 0;
                for(i = 0; i < a; i++){
                    if(tag == cachesys[setid][i]){
                    hit = 1;
                    cache_hit++;
                    mem_wri++;
                    if(strcmp(poli, "fifo") != 0){
                     for(j = 0; j < a; j++){
                             if(j == i) indexs[setid][i] = 0;
                             else if(indexs[setid][j] == -1) continue;
                             else{
                                 int wx = indexs[setid][j];
                                 indexs[setid][j] = wx+2;
                             }
                         }   
                    }
                    break;
                    }
                }
                 if(hit == 0){
                    cache_miss++;
                    mem_read++;
                    if(strcmp(poli, "fifo") == 0){
                    int index = indexs[setid][0];
                    cachesys[setid][index] = tag;
                    if(index+1 == a) indexs[setid][0] = 0;
                    else indexs[setid][0] = index+1;
                    }
                    else{
                       
                        int index = 0;
                        int min = indexs[setid][0];
                        for(i = 0; i < a; i++){
                            if(indexs[setid][i] == -1){
                                index = i;
                                break;
                            }
                            else if(min < indexs[setid][i]){
                                index = i; 
                                min = indexs[setid][i];
                            }
                        }
                            cachesys[setid][index] = tag;
                            for(i = 0; i < a; i++){
                             if(i == index) indexs[setid][index] = 0;
                             else if(indexs[setid][i] == -1) continue;
                             else{
                                 int wx = indexs[setid][i];
                                 indexs[setid][i] = wx+2;
                             }
                         }
                        
                    }
                    mem_wri++;
                }
            }
            d = fscanf(fPointer1, "%*x: %c %lx", &hold[0], &add);
        }
       for(i = 0; i < sets; i++){
            free(cachesys[i]);
            cachesys[i] = NULL;
            free(indexs[i]);
            indexs[i] = NULL;
        }
        free(cachesys);
        cachesys = NULL;
        free(indexs);
        indexs = NULL;

    }
    else if(strcmp(type, "assoc") == 0){
        sets = 1;
        a = caches / blocks;
        sbits = 0;
        bbits = inlog2(blocks);
        unsigned long **cachesys = malloc(sizeof(unsigned long*)*sets);
        for(i = 0; i < sets; i++){
            cachesys[i] = malloc(sizeof(unsigned long)*a);
        }
        fPointer1 = fopen(name, "r");
        unsigned long add;
        char hold[5];
        for(i = 0; i < sets; i++){
            for(j = 0; j < a; j++){
                cachesys[i][j] = -1;
            }
        }
        int index = 0;
        int *indexs = malloc(sizeof(int)*a);
        if(strcmp(poli, "fifo") == 0){
            for(i = 0; i < a; i++){
                indexs[i] = 0;
            }
        }
        else{
              for(i = 0; i < a; i++){
                indexs[i] = -1;
            }
        }
        int d = fscanf(fPointer1, "%*x: %c %lx", &hold[0], &add);
        while(d == 2){
            
            unsigned long tag = add >> bbits;
             if(hold[0] == 'R'){
                int hit = 0;
                for(i = 0; i < a; i++){
                    if(tag == cachesys[0][i]){
                    hit = 1;
                    cache_hit++;
                    if(strcmp(poli, "fifo") != 0){
                     for(j = 0; j < a; j++){
                             if(j == i) indexs[j] = 0;
                             else if(indexs[j] == -1) continue;
                             else{
                                 int wx = indexs[j];
                                 indexs[j] = wx+2;
                             }
                         }   
                    }
                    break;
                    }
                }
                if(hit == 0){
                    cache_miss++;
                    mem_read++;
                    if(strcmp(poli, "fifo") == 0){
                    cachesys[0][index] = tag;
                    if(index+1 == a) index = 0;
                    else index++;
                    }
                    else{
                        int ind = 0;
                        int min = indexs[0];
                        for(i = 0; i < a; i++){
                            if(indexs[i] == -1){
                                ind = i; 
                                break;
                            }
                            else if(min < indexs[i]){
                                ind = i; 
                                min = indexs[i];
                            }
                        }
                        cachesys[0][ind] = tag;
                        for(i = 0; i < a; i++){
                            if(i == ind) indexs[i] = 0;
                            else if(indexs[i] == -1) continue;
                            else{
                                int wx = indexs[i];
                                 indexs[i] = wx+2;
                            }
                        }

                    }
                }
            }
            else if(hold[0] == 'W'){
                int hit = 0;
                for(i = 0; i < a; i++){
                    if(tag == cachesys[0][i]){
                    hit = 1;
                    cache_hit++;
                    mem_wri++;
                     if(strcmp(poli, "fifo") != 0){
                     for(j = 0; j < a; j++){
                             if(j == i) indexs[j] = 0;
                             else if(indexs[j] == -1) continue;
                             else{
                                 int wx = indexs[j];
                                 indexs[j] = wx+2;
                             }
                         }   
                    }
                    break;
                    }
                }
                 if(hit == 0){
                    cache_miss++;
                    mem_read++;
                    if(strcmp(poli, "fifo") == 0){
                    cachesys[0][index] = tag;
                    if(index+1 == a) index = 0;
                    else index++;
                    }
                    else{
                        int ind = 0;
                        int min = indexs[0];
                        for(i = 0; i < a; i++){
                            if(indexs[i] == -1){
                                ind = i; 
                                break;
                            }
                            else if(min < indexs[i]){
                                ind = i; 
                                min = indexs[i];
                            }
                        }
                        cachesys[0][ind] = tag;
                        for(i = 0; i < a; i++){
                            if(i == ind) indexs[i] = 0;
                            else if(indexs[i] == -1) continue;
                            else{
                                int wx = indexs[i];
                                 indexs[i] = wx+2;
                            }
                        }

                    }
                    mem_wri++;
                }
            }
              d = fscanf(fPointer1, "%*x: %c %lx", &hold[0], &add);
            
        }
        for(i = 0; i < sets; i++){
            free(cachesys[i]);
            cachesys[i] = NULL;
        }
        free(cachesys);
        cachesys = NULL;
        free(indexs);
        indexs = NULL;
    }
    else if(strcmp(type, "direct") == 0){
        a = 1;
        sets = caches / blocks;
        sbits = inlog2(sets);
        bbits = inlog2(blocks);
        unsigned long mask = (1 << sbits) -1;
        unsigned long **cachesys = malloc(sizeof(unsigned long*)*sets);
        for(i = 0; i < sets; i++){
            cachesys[i] = malloc(sizeof(unsigned long)*a);
        }
        fPointer1 = fopen(name, "r");
        unsigned long add;
        char hold[5];
        for(i = 0; i < sets; i++){
            for(j = 0; j < a; j++){
                cachesys[i][j] = -1;
            }
        }
        int d = fscanf(fPointer1, "%*x: %c %lx", &hold[0], &add);
        while(d == 2){
            unsigned long blockid = add >> bbits;
            unsigned long setid = blockid & mask;
            unsigned long tag = blockid >> sbits;
             if(hold[0] == 'R'){
                if(tag == cachesys[setid][0]){
                    cache_hit++;
                }
                else{
                    cache_miss++;
                    mem_read++;
                    cachesys[setid][0] = tag;
                }
            }
            else if(hold[0] == 'W'){
                if(tag == cachesys[setid][0]){
                    cache_hit++;
                    mem_wri++;
                }
                else{
                    cache_miss++;
                    mem_read++;
                    cachesys[setid][0] = tag;
                    mem_wri++;
                }   
            }
            d = fscanf(fPointer1, "%*x: %c %lx", &hold[0], &add);
        }
         for(i = 0; i < sets; i++){
            free(cachesys[i]);
            cachesys[i] = NULL;
        }
        free(cachesys);
        cachesys = NULL;
        
    }
    fclose(fPointer1); 
    printf("Prefetch 0\n");
    printf("Memory reads: %d\n", mem_read);
        printf("Memory writes: %d\n", mem_wri);
        printf("Cache hits: %d\n", cache_hit);
        printf("Cache misses: %d\n", cache_miss);
        cache_hit = 0;
        cache_miss = 0;
        mem_read = 0;
        mem_wri = 0;
    if(strstr(type, "assoc:") != NULL){
        
        sscanf(type, "assoc:%d", &a);
        sets = caches / (a * blocks);
        sbits = inlog2(sets);
        bbits = inlog2(blocks);
        unsigned long mask = (1 << sbits) -1;
        unsigned long **cachesys = malloc(sizeof(unsigned long*)*sets);
        for(i = 0; i < sets; i++){
            cachesys[i] = malloc(sizeof(unsigned long)*a);
        }
        fPointer1 = fopen(name, "r");
        unsigned long add;
        char hold[5];
        int **indexs = malloc(sizeof(int*)*sets);
        if(strcmp(poli, "fifo") == 0){
        for(i = 0; i < sets; i++){
            indexs[i] = malloc(sizeof(int)*1);
        }
        for(i = 0; i < sets; i++){
            indexs[i][0] = 0;
        }
        }
        else{
             for(i = 0; i < sets; i++){
            indexs[i] = malloc(sizeof(int)*a);
        }
        for(i = 0; i < sets; i++){
            for(j = 0; j < a; j++){
                indexs[i][j] = -1;
            }
         }
        }
        for(i = 0; i < sets; i++){
            for(j = 0; j < a; j++){
                cachesys[i][j] = -1;
            }
        }
        int d = fscanf(fPointer1, "%*x: %c %lx", &hold[0], &add);
        while(d == 2){
            unsigned long blockid = add >> bbits;
            unsigned long setid = blockid & mask;
            unsigned long tag = blockid >> sbits;
        
            if(hold[0] == 'R'){
                int hit = 0;
                for(i = 0; i < a; i++){
                    if(tag == cachesys[setid][i]){
                    hit = 1;
                    cache_hit++;
                    if(strcmp(poli, "fifo") != 0){
                     for(j = 0; j < a; j++){
                        if(j == i){
                        indexs[setid][i] = 0;
                        }
                        else if(indexs[setid][j] == -1) continue;
                        else{
                            int wx = indexs[setid][j];
                            indexs[setid][j] = wx+2;
                                 
                             }
                         }   
                    }
                    break;
                    }
                }
                if(hit == 0){
                    cache_miss++;
                    mem_read++;
                   if(strcmp(poli, "fifo") == 0){
                    int ind = indexs[setid][0];
                    cachesys[setid][ind] = tag;
                    if(ind+1 == a) indexs[setid][0] = 0;
                    else indexs[setid][0] = ind+1;
                    
                    unsigned long tadd = add + blocks;
                    unsigned long tblockid = tadd >> bbits;
                    unsigned long tsetid = tblockid & mask;
                    unsigned long ttag = tblockid >> sbits;
                    int thit = 0;
                    for(i = 0; i < a; i++){
                    if(ttag == cachesys[tsetid][i]){
                    thit = 1;
                    }
                }
                if(thit == 0){
                    mem_read++;
                    int index = indexs[tsetid][0];
                    cachesys[tsetid][index] = ttag;
                    if(index+1 == a) indexs[tsetid][0] = 0;
                    else indexs[tsetid][0] = index+1;
                }
                   }
                   else{
                       int index = 0;
                        int min = indexs[setid][0];
                       
                        for(i = 0; i < a; i++){
                          
                            if(indexs[setid][i] == -1){
                             
                                index = i;
                                break;
                            }
                            else if(min < indexs[setid][i]){
                             
                                index = i; 
                                min = indexs[setid][i];
                            }
                        }
                    
                         cachesys[setid][index] = tag;
                         for(i = 0; i < a; i++){
                             if(i == index) indexs[setid][index] = 0;
                             else if(indexs[setid][i] == -1) continue;
                             else{
                                 int wx = indexs[setid][i];
                                 indexs[setid][i] = wx+2;
                             }
                         }
                        unsigned long tadd = add + blocks;
                        unsigned long tblockid = tadd >> bbits;
                        unsigned long tsetid = tblockid & mask;
                        unsigned long ttag = tblockid >> sbits;
                      
                        
                        int thit = 0;
                    for(i = 0; i < a; i++){
                    if(ttag == cachesys[tsetid][i]){
                    thit = 1;
                    }
                    }
                if(thit == 0){
                  
                    
                     mem_read++;
                   
                     int in = 0;
                        int min = indexs[tsetid][0];
                        for(i = 0; i < a; i++){
                            if(indexs[tsetid][i] == -1){
                                in = i;
                                break;
                            }
                            else if(min < indexs[tsetid][i]){
                               
                                in = i; 
                                min = indexs[tsetid][i];
                            }
                        }
                      
                         cachesys[tsetid][in] = ttag;
                         for(i = 0; i < a; i++){
                             if(i == in) indexs[tsetid][in] = 0;
                             else if(indexs[tsetid][i] == -1) continue;
                             else{
                                 int wx = indexs[tsetid][i];
                                 indexs[tsetid][i] = wx+2;
                             }
                         }
                }
                   }
                }
            }
            else if(hold[0] == 'W'){
                int hit = 0;
                for(i = 0; i < a; i++){
                    if(tag == cachesys[setid][i]){
                    hit = 1;
                    cache_hit++;
                    if(strcmp(poli, "fifo") != 0){
                     for(j = 0; j < a; j++){
                        if(j == i){
                        indexs[setid][i] = 0;
                        }
                        else if(indexs[setid][j] == -1) continue;
                        else{
                            int wx = indexs[setid][j];
                            indexs[setid][j] = wx+2;
                                 
                             }
                         }   
                    }
                    mem_wri++;
                    break;
                    }
                }
                 if(hit == 0){
                    cache_miss++;
                    mem_read++;
                     if(strcmp(poli, "fifo") == 0){
                    int ind = indexs[setid][0];
                    cachesys[setid][ind] = tag;
                    if(ind+1 == a) indexs[setid][0] = 0;
                    else indexs[setid][0] = ind+1;
                    
                    unsigned long tadd = add + blocks;
                    unsigned long tblockid = tadd >> bbits;
                    unsigned long tsetid = tblockid & mask;
                    unsigned long ttag = tblockid >> sbits;
                    int thit = 0;
                    for(i = 0; i < a; i++){
                    if(ttag == cachesys[tsetid][i]){
                    thit = 1;
                    }
                }
                if(thit == 0){
                    mem_read++;
                    int index = indexs[tsetid][0];
                    cachesys[tsetid][index] = ttag;
                    if(index+1 == a) indexs[tsetid][0] = 0;
                    else indexs[tsetid][0] = index+1;
                }
                   }
                   else{
                       int index = 0;
                        int min = indexs[setid][0];
                       
                        for(i = 0; i < a; i++){
                          
                            if(indexs[setid][i] == -1){
                             
                                index = i;
                                break;
                            }
                            else if(min < indexs[setid][i]){
                             
                                index = i; 
                                min = indexs[setid][i];
                            }
                        }
                    
                         cachesys[setid][index] = tag;
                         for(i = 0; i < a; i++){
                             if(i == index) indexs[setid][index] = 0;
                             else if(indexs[setid][i] == -1) continue;
                             else{
                                 int wx = indexs[setid][i];
                                 indexs[setid][i] = wx+2;
                             }
                         }
                        unsigned long tadd = add + blocks;
                        unsigned long tblockid = tadd >> bbits;
                        unsigned long tsetid = tblockid & mask;
                        unsigned long ttag = tblockid >> sbits;
                      
                        
                        int thit = 0;
                    for(i = 0; i < a; i++){
                    if(ttag == cachesys[tsetid][i]){
                    thit = 1;
                    }
                    }
                if(thit == 0){
                     mem_read++;
                   
                     int in = 0;
                        int min = indexs[tsetid][0];
                        for(i = 0; i < a; i++){
                            if(indexs[tsetid][i] == -1){
                                in = i;
                                break;
                            }
                            else if(min < indexs[tsetid][i]){
                               
                                in = i; 
                                min = indexs[tsetid][i];
                            }
                        }
                      
                         cachesys[tsetid][in] = ttag;
                         for(i = 0; i < a; i++){
                             if(i == in) indexs[tsetid][in] = 0;
                             else if(indexs[tsetid][i] == -1) continue;
                             else{
                                 int wx = indexs[tsetid][i];
                                 indexs[tsetid][i] = wx+2;
                             }
                         }
                }
                   }
                    mem_wri++;
                }
            }
            d = fscanf(fPointer1, "%*x: %c %lx", &hold[0], &add);
        }
          for(i = 0; i < sets; i++){
            free(cachesys[i]);
            cachesys[i] = NULL;
            free(indexs[i]);
            indexs[i] = NULL;
        }
        free(cachesys);
        cachesys = NULL;
        free(indexs);
        indexs = NULL;

       
    }
    else if(strcmp(type, "assoc") == 0){
        sets = 1;
        a = caches / blocks;
        sbits = 0;
        bbits = inlog2(blocks);
        unsigned long **cachesys = malloc(sizeof(unsigned long*)*sets);
        for(i = 0; i < sets; i++){
            cachesys[i] = malloc(sizeof(unsigned long)*a);
        }
        fPointer1 = fopen(name, "r");
        unsigned long add;
        char hold[5];
        for(i = 0; i < sets; i++){
            for(j = 0; j < a; j++){
                cachesys[i][j] = -1;
            }
        }
        int index = 0;
        int *indexs = malloc(sizeof(int)*a);
        if(strcmp(poli, "fifo") == 0){
            for(i = 0; i < a; i++){
                indexs[i] = 0;
            }
        }
        else{
              for(i = 0; i < a; i++){
                indexs[i] = -1;
            }
        }
        int d = fscanf(fPointer1, "%*x: %c %lx", &hold[0], &add);
        while(d == 2){
            
            unsigned long tag = add >> bbits;
             if(hold[0] == 'R'){
                int hit = 0;
                for(i = 0; i < a; i++){
                    if(tag == cachesys[0][i]){
                    hit = 1;
                    cache_hit++;
                    if(strcmp(poli, "fifo") != 0){
                     for(j = 0; j < a; j++){
                             if(j == i) indexs[j] = 0;
                             else if(indexs[j] == -1) continue;
                             else{
                                 int wx = indexs[j];
                                 indexs[j] = wx+2;
                             }
                         }   
                    }
                    break;
                    }
                }
                if(hit == 0){
                    cache_miss++;
                    mem_read++;
                    if(strcmp(poli, "fifo") == 0){
                    cachesys[0][index] = tag;
                    if(index+1 == a) index = 0;
                    else index++;
                    unsigned long tadd = add + blocks;
                    unsigned long ttag = tadd >> bbits;
                    int thit = 0;
                    for(i = 0; i < a; i++){
                    if(ttag == cachesys[0][i]){
                    thit = 1;
                    }
                }
                if(thit == 0){
                    mem_read++;
                    cachesys[0][index] = ttag;
                    if(index+1 == a) index = 0;
                    else index++;
                }
                }
                else{
                    int ind = 0;
                        int min = indexs[0];
                        for(i = 0; i < a; i++){
                            if(indexs[i] == -1){
                                ind = i; 
                                break;
                            }
                            else if(min < indexs[i]){
                                ind = i; 
                                min = indexs[i];
                            }
                        }
                        cachesys[0][ind] = tag;
                        for(i = 0; i < a; i++){
                            if(i == ind) indexs[i] = 0;
                            else if(indexs[i] == -1) continue;
                            else{
                                int wx = indexs[i];
                                 indexs[i] = wx+2;
                            }
                        }
                        unsigned long tadd = add + blocks;
                        unsigned long ttag = tadd >> bbits;
                        int thit = 0;
                        for(i = 0; i < a; i++){
                        if(ttag == cachesys[0][i]){
                        thit = 1;
                        }
                    }
                    if(thit == 0){
                        mem_read++;
                        int inde = 0;
                        int mini = indexs[0];
                        for(i = 0; i < a; i++){
                            if(indexs[i] == -1){
                                inde = i; 
                                break;
                            }
                            else if(mini < indexs[i]){
                                inde = i; 
                                mini = indexs[i];
                            }
                        }
                        cachesys[0][inde] = ttag;
                        for(i = 0; i < a; i++){
                            if(i == inde) indexs[i] = 0;
                            else if(indexs[i] == -1) continue;
                            else{
                                int wx = indexs[i];
                                 indexs[i] = wx+2;
                            }
                        }
                    }
                }
                }
            }
            else if(hold[0] == 'W'){
                int hit = 0;
                for(i = 0; i < a; i++){
                    if(tag == cachesys[0][i]){
                    hit = 1;
                    cache_hit++;
                    mem_wri++;
                     if(strcmp(poli, "fifo") != 0){
                     for(j = 0; j < a; j++){
                             if(j == i) indexs[j] = 0;
                             else if(indexs[j] == -1) continue;
                             else{
                                 int wx = indexs[j];
                                 indexs[j] = wx+2;
                             }
                         }   
                    }
                    break;
                    }
                }
                 if(hit == 0){
                    cache_miss++;
                    mem_read++;
                   if(strcmp(poli, "fifo") == 0){
                    cachesys[0][index] = tag;
                    if(index+1 == a) index = 0;
                    else index++;
                    unsigned long tadd = add + blocks;
                    unsigned long ttag = tadd >> bbits;
                    int thit = 0;
                    for(i = 0; i < a; i++){
                    if(ttag == cachesys[0][i]){
                    thit = 1;
                    }
                }
                if(thit == 0){
                    mem_read++;
                    cachesys[0][index] = ttag;
                    if(index+1 == a) index = 0;
                    else index++;
                }
                }
                else{
                    int ind = 0;
                        int min = indexs[0];
                        for(i = 0; i < a; i++){
                            if(indexs[i] == -1){
                                ind = i; 
                                break;
                            }
                            else if(min < indexs[i]){
                                ind = i; 
                                min = indexs[i];
                            }
                        }
                        cachesys[0][ind] = tag;
                        for(i = 0; i < a; i++){
                            if(i == ind) indexs[i] = 0;
                            else if(indexs[i] == -1) continue;
                            else{
                                int wx = indexs[i];
                                 indexs[i] = wx+2;
                            }
                        }
                        unsigned long tadd = add + blocks;
                        unsigned long ttag = tadd >> bbits;
                        int thit = 0;
                        for(i = 0; i < a; i++){
                        if(ttag == cachesys[0][i]){
                        thit = 1;
                        }
                    }
                    if(thit == 0){
                        mem_read++;
                        int inde = 0;
                        int mini = indexs[0];
                        for(i = 0; i < a; i++){
                            if(indexs[i] == -1){
                                inde = i; 
                                break;
                            }
                            else if(mini < indexs[i]){
                                inde = i; 
                                mini = indexs[i];
                            }
                        }
                        cachesys[0][inde] = ttag;
                        for(i = 0; i < a; i++){
                            if(i == inde) indexs[i] = 0;
                            else if(indexs[i] == -1) continue;
                            else{
                                int wx = indexs[i];
                                 indexs[i] = wx+2;
                            }
                        }
                    }
                }
                    mem_wri++;
                }
            }
              d = fscanf(fPointer1, "%*x: %c %lx", &hold[0], &add);
            
        }
         for(i = 0; i < sets; i++){
            free(cachesys[i]);
            cachesys[i] = NULL;
        }
        free(cachesys);
        cachesys = NULL;
        free(indexs);
        indexs = NULL;
        
    }
    else if(strcmp(type, "direct") == 0){
        a = 1;
        sets = caches / blocks;
        sbits = inlog2(sets);
        bbits = inlog2(blocks);
        unsigned long mask = (1 << sbits) -1;
        unsigned long **cachesys = malloc(sizeof(unsigned long*)*sets);
        for(i = 0; i < sets; i++){
            cachesys[i] = malloc(sizeof(unsigned long)*a);
        }
        fPointer1 = fopen(name, "r");
        unsigned long add;
        char hold[5];
        for(i = 0; i < sets; i++){
            for(j = 0; j < a; j++){
                cachesys[i][j] = -1;
            }
        }
        int d = fscanf(fPointer1, "%*x: %c %lx", &hold[0], &add);
        while(d == 2){
            unsigned long blockid = add >> bbits;
            unsigned long setid = blockid & mask;
            unsigned long tag = blockid >> sbits;
             if(hold[0] == 'R'){
                if(tag == cachesys[setid][0]){
                    cache_hit++;
                }
                else{
                    cache_miss++;
                    mem_read++;
                    cachesys[setid][0] = tag;
                    unsigned long tadd = add + blocks;
                    unsigned long tblockid = tadd >> bbits;
                    unsigned long tsetid = tblockid & mask;
                    unsigned long ttag = tblockid >> sbits;
                    int thit = 0;
                    if(ttag == cachesys[tsetid][0]){
                    thit = 1;
                    }
                    if(thit == 0){
                        mem_read++;
                        cachesys[tsetid][0] = ttag;
                    }
                }
            }
            else if(hold[0] == 'W'){
                if(tag == cachesys[setid][0]){
                    cache_hit++;
                    mem_wri++;
                }
                else{
                    cache_miss++;
                    mem_read++;
                    cachesys[setid][0] = tag;
                    unsigned long tadd = add + blocks;
                    unsigned long tblockid = tadd >> bbits;
                    unsigned long tsetid = tblockid & mask;
                    unsigned long ttag = tblockid >> sbits;
                    int thit = 0;
                    if(ttag == cachesys[tsetid][0]){
                    thit = 1;
                    }
                    if(thit == 0){
                        mem_read++;
                        cachesys[tsetid][0] = ttag;
                    }
                    mem_wri++;
                }   
            }
            d = fscanf(fPointer1, "%*x: %c %lx", &hold[0], &add);
        }
         for(i = 0; i < sets; i++){
            free(cachesys[i]);
            cachesys[i] = NULL;
        }
        free(cachesys);
        cachesys = NULL;
        
    }
    printf("Prefetch 1\n");
    printf("Memory reads: %d\n", mem_read);
    printf("Memory writes: %d\n", mem_wri);
    printf("Cache hits: %d\n", cache_hit);
    printf("Cache misses: %d\n", cache_miss);
    
}