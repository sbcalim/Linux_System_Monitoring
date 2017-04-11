/*
 * =====================================================================================
 *
 *       Filename:  find_mongo.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/30/2017 12:35:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Sitki Burak Calim (sbc), sburakcalim@gmail.com
 *        Company:  IzmirComp
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <bson.h>
#include <mongoc.h>




    int
main ( int argc, char *argv[] )
{
    mongoc_client_t *client;
    mongoc_collection_t *collection;
    mongoc_collection_t *collection_ram;
    bson_t reply;
    bson_t reply_ram;
    bson_t *query;
    bson_t *query_ram;
    bson_error_t error;
    bson_error_t error_ram;
    char *str;
    int i = 0;
    int *cpus;
    int *rams;

    mongoc_init();

    client = mongoc_client_new("mongodb://localhost:27017/");
    collection = mongoc_client_get_collection(client, "INPUT", "CPU");
    collection_ram = mongoc_client_get_collection(client, "INPUT", "RAM");

    query = BCON_NEW("find", BCON_UTF8("CPU"),
            "limit", BCON_INT32(1),
            "sort", "{", "time", BCON_INT32(-1), "}");

    query_ram = BCON_NEW("find", BCON_UTF8("RAM"),
            "limit", BCON_INT32(1),
            "sort", "{", "time", BCON_INT32(-1), "}");

    printf("CPU\tCPU1\tCPU2\tCPU3\tCPU4\t\tTotal\t\tUsed\tFree\tUsed\n");
    while(1)
    {
        if(mongoc_collection_command_simple(collection, query, NULL, &reply, &error))
        {
            str = bson_as_json(&reply, NULL);
            char *pch;
            i = 0;
            cpus = (int *)malloc(5 * sizeof(int));
            pch = strtok(str, "}");
            while(pch != NULL)
            {
                if(i==2)
                {
                    sscanf(pch, ", \"CPU\" : %d, \"CPU1\" : %d, \"CPU2\" : %d, \"CPU3\" : %d, \"CPU4\" : %d", &cpus[0], &cpus[1], &cpus[2], &cpus[3], &cpus[4]);
                    break;
                }
                else
                {
                    pch = strtok(NULL, "}");
                }
                i++; 
            }
        } 

        else
        {
            fprintf(stderr, "Failed to run the command. %s\n", error.message);
        }

        if(mongoc_collection_command_simple(collection, query_ram, NULL, &reply_ram, &error_ram))
        {
            str = bson_as_json(&reply, NULL);
            i = 0;
            char *pch;
            rams = (int *)malloc(4 * sizeof(int));
            pch = strtok(str, "}");

            while(pch != NULL)
            {
                if((i++) == 2)
                {
                    sscanf(pch, ", \"RAM_TOTAL\" : %d, \"RAM_USED\" : %d, \"RAM_FREE\" : %d, \"RAM_PERCENTAGE\" : %d", &rams[0], &rams[1], &rams[2], &rams[3]);
                    break;
                }
                pch = strtok(NULL, "}");
            }
        }       
        else
        {
            fprintf(stderr, "Failed to run the command. %s\n", error_ram.message);
        }     

        printf("%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t\t%dM\t%dM\t%dM\t%.2f\n", (float)cpus[0]/100, (float)cpus[1]/100, (float)cpus[2]/100, (float)cpus[3]/100, (float)cpus[4]/100, rams[0], rams[1], rams[2], (float)rams[3]/100);
        bson_free(str);
        

        sleep(1);
    }

    bson_destroy(query);
    bson_destroy(&reply);
    mongoc_collection_destroy(collection);
    mongoc_client_destroy(client);
    mongoc_cleanup();

    return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */
