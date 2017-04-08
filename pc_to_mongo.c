#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <mongoc.h>
#include <bson.h>

#define D_MEMTOTAL 0
#define D_USED 0
#define D_MEMFREE 1
#define D_BUFFERS 2
#define D_CACHED 3

#define MAX_BUF 128

#define PROCMEMINFO "/proc/meminfo"
#define PROCSTAT "/proc/stat"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  memory_in_kb
 *  Description:  Calculates memory details (usage, free, cached, buffered) in kB
 * =====================================================================================
 */
unsigned long long *memory_in_kb()
{
	FILE *fp;
	unsigned long long *array;
	array = (unsigned long long *)malloc(4 * sizeof(unsigned long long));
	char buffers[5][MAX_BUF];
	unsigned long long total_kb;
	int i;

	fp = fopen(PROCMEMINFO, "r");
	
	//Each row of the array contains the values we will use in a minute
	fgets(buffers[D_MEMTOTAL], MAX_BUF, fp);
	fgets(buffers[D_MEMFREE], MAX_BUF, fp);
	fgets(buffers[4], MAX_BUF, fp);		//For MemAvailable part. We will not use it
	fgets(buffers[D_BUFFERS], MAX_BUF, fp);
	fgets(buffers[D_CACHED], MAX_BUF, fp);

	fclose(fp);

	//Take the values from buffered lines
	sscanf(buffers[D_MEMTOTAL], "MemTotal: %llu", &total_kb);
	sscanf(buffers[D_MEMFREE], "MemFree: %llu", &array[D_MEMFREE]);
	sscanf(buffers[D_BUFFERS], "Buffers: %llu", &array[D_BUFFERS]);
	sscanf(buffers[D_CACHED], "Cached: %llu", &array[D_CACHED]);

	array[D_USED] = total_kb - (array[D_MEMFREE] + array[D_BUFFERS] + array[D_CACHED]);

	//Now we got all 4 values we need in array[]
	return array;
}		/* -----  end of function memory_in_kb  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  memory_in_mb
 *  Description:  Gets results from memory_in_kb (usage, free, cached, buffered) and turn them into MB
 * =====================================================================================
 */
int *memory_in_mb()
{
	//We do not need to work for all these reading non-sense. memory_in_kb calculates them for us.
	int i;
	unsigned long long *array = memory_in_kb();
	int *ret_array;
	ret_array = (int *)malloc(4 * sizeof(int));

	//Although we just need to convert kB in MB by dividing with 1024
	for(i = 0; i<4; i++)
	{
		ret_array[i] = (array[i] / 1024);
	}

	return ret_array;
}		/* -----  end of function memory_in_mb  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  total_memory_kb
 *  Description:  Finds out what is total memory in kB
 * =====================================================================================
 */
unsigned long long total_memory_kb()
{
	char buffer[MAX_BUF];
	unsigned long long val;
	FILE *fp = fopen(PROCMEMINFO, "r");

	//Total memory is the first line of /proc/meminfo file, which starts with MemTotal
	fgets(buffer, MAX_BUF, fp);
	sscanf(buffer, "MemTotal: %llu", &val);

	return val;
}		/* -----  end of function memory_in_kb  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  total_memory_mb
 *  Description:  Converts total_memory_kb into MB
 * =====================================================================================
 */
int total_memory_mb()
{
	return (int)total_memory_kb()/1024;
}		/* -----  end of function total_memory_mb  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  memory_percentage
 *  Description:  Gets the same array from memory_in_kb() and calculates usage percetages over total memory
 * =====================================================================================
 */
double *memory_percentage()
{
	int i;
	unsigned long long *array = memory_in_kb();
	unsigned long long t = total_memory_kb();
	double *ret_array;
	ret_array = (double *)malloc(4 * sizeof(double));
	
	ret_array[D_MEMFREE] = ((double)array[D_MEMFREE] / (double)t) * 100;
	ret_array[D_CACHED] = ((double)array[D_CACHED] / (double)t) * 100;
	ret_array[D_BUFFERS] = ((double)array[D_BUFFERS] / (double)t) * 100;
	ret_array[D_USED] = 100 - (ret_array[D_MEMFREE] + ret_array[D_CACHED] + ret_array[D_BUFFERS]);

	return ret_array;
}		/* -----  end of function memory_percentage  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  get_number_of_cpu_cores
 *  Description:  How many CPU cores are there?
 * =====================================================================================
 */
	int	
get_number_of_cpu_cores ( )
{
	FILE *fp = fopen(PROCSTAT, "r");

	int n = 0;
	char line[100];
				
	while(!feof(fp))
	{
		fgets(line, 100, fp); 
		if(line[0] == 'c' && line[1] == 'p' && line[2] == 'u') n++;
	}
	fclose(fp);

	return n - 1;
}		/* -----  end of function get_number_of_cpu_cores  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  read_cpu
 *  Description:  Reads /proc/stat file and returns an array[cpu_num][process_type] of process count
 * =====================================================================================
 */
	unsigned long long
**read_cpu ( )
{
	FILE *fp;
	unsigned long long **array;
	char buffer[1024];
	int i, j;
	unsigned long long ignore[6];
	int cpus = get_number_of_cpu_cores();

	array = (unsigned long long **)malloc((cpus + 1) * sizeof(unsigned long long));

	fp = fopen(PROCSTAT, "r");

	for(i = 0; i<cpus + 1; i++)
	{
		array[i] = (unsigned long long *)malloc(4 * sizeof(unsigned long long));
		fscanf(fp, "%*s %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu ",
				&array[i][0], &array[i][1], &array[i][2], &array[i][3], 
				&ignore[0], &ignore[1], &ignore[2], &ignore[3], &ignore[4], &ignore[5]);
	}
	fclose(fp);
	return array;
}		/* -----  end of function read_cpu  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  get_cpu_percentage
 *  Description:  Returns a double value of CPU usage percentage from given array[4] = {USER_PROC, NICE_PROC, SYSTEM_PROC, IDLE_PROC}
 * =====================================================================================
 */
	double
get_cpu_percentage ( unsigned long long *a1, unsigned long long *a2 )
{
	return (double)(((double)((a1[0] - a2[0]) + (a1[1] - a2[1]) + (a1[2] - a2[2])) / 
				(double)((a1[0] - a2[0]) + (a1[1] - a2[1]) + (a1[2] - a2[2]) + (a1[3] - a2[3]))) * 100);
}		/* -----  end of function get_cpu_percentage  ----- */


	void
insert_to_db()
{
	int i;
	double *percentage;
	mongoc_client_t *client;
	mongoc_collection_t *collection;
	mongoc_collection_t *collection_ram;
	bson_error_t error;
	bson_error_t error_ram;
	bson_oid_t oid;
	bson_oid_t oid_ram;
	bson_t *doc;
	bson_t *doc_ram;

	mongoc_init();

	client = mongoc_client_new("mongodb://localhost:27017/");
	collection = mongoc_client_get_collection(client, "INPUT", "CPU");
	collection_ram = mongoc_client_get_collection(client, "INPUT", "RAM");

	while(1)
	{
		db_action(doc, doc_ram, oid, oid_ram, collection, collection_ram, error, error_ram);
	}

	bson_destroy(doc);
	bson_destroy(doc_ram);
	mongoc_collection_destroy(collection);
	mongoc_collection_destroy(collection_ram);
	mongoc_client_destroy(client);
	mongoc_cleanup();

}

	void
db_action(bson_t *doc, bson_t *doc_ram, bson_oid_t oid, bson_oid_t oid_ram, mongoc_collection_t *collection, mongoc_collection_t *collection_ram, bson_error_t error, bson_error_t error_ram)
{
	int num_of_cpus = get_number_of_cpu_cores(), i;
	int *ram_mb = memory_in_mb();
	double *ram_percentage = memory_percentage();
	doc = bson_new();
	bson_oid_init(&oid, NULL);
	BSON_APPEND_OID(doc, "_id", &oid);
	bson_append_now_utc(doc, "time", -1);
	
	unsigned long long **p_cpu_array = read_cpu();
	sleep(1);
	unsigned long long **n_cpu_array = read_cpu();

	BSON_APPEND_INT32(doc, "CPU", 100 * get_cpu_percentage(n_cpu_array[0], p_cpu_array[0]));
	BSON_APPEND_INT32(doc, "CPU1", 100 * get_cpu_percentage(n_cpu_array[1], p_cpu_array[1]));
	BSON_APPEND_INT32(doc, "CPU2", 100 * get_cpu_percentage(n_cpu_array[2], p_cpu_array[2]));
	BSON_APPEND_INT32(doc, "CPU3", 100 * get_cpu_percentage(n_cpu_array[3], p_cpu_array[3]));
	BSON_APPEND_INT32(doc, "CPU4", 100 * get_cpu_percentage(n_cpu_array[4], p_cpu_array[4]));

	doc_ram = bson_new();
	bson_oid_init(&oid_ram, NULL);
	BSON_APPEND_OID(doc_ram, "_id", &oid_ram);
	bson_append_now_utc(doc_ram, "time", -1);
	
	BSON_APPEND_INT32(doc_ram, "RAM_TOTAL", total_memory_mb());
	BSON_APPEND_INT32(doc_ram, "RAM_USED", ram_mb[0]);
	BSON_APPEND_INT32(doc_ram, "RAM_FREE", ram_mb[1]);
	BSON_APPEND_INT32(doc_ram, "RAM_PERCENTAGE", 100 * (int)ram_percentage[0]);
		
	for(i = 0; i < num_of_cpus; i++)
	{
		free(p_cpu_array[i]);
		free(n_cpu_array[i]);
	}

	if(!mongoc_collection_insert(collection, MONGOC_INSERT_NONE, doc, NULL, &error))
	{
		fprintf(stderr, "%s\n", error.message);
	}

	if(!mongoc_collection_insert(collection_ram, MONGOC_INSERT_NONE, doc_ram, NULL, &error_ram))
	{
		fprintf(stderr, "%s\n", error_ram.message);
	}
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
	int
main ( int argc, char *argv[] )
{

	insert_to_db();

	
	return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */
