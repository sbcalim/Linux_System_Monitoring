# pc_to_mongo

!Works for only 4 core cpus!

Creates database named INPUT and collections CPU, RAM in localhost, port 27017 and pushes CPU usage percentages of total and each core to CPU collection; total ram in mb (will change), used ram in mb, free ram in mb, 100 times ram usage percentage (to make an int) to RAM collection in every second.

COMPILATION AND RUNNING

$ gcc -o pc_to_mongo pc_to_mongo.c $(pkg-config --cflags --libs libmongoc-1.0)
$ ./pc_to_mongo
