echo "Run on Linux!"
gcc -O0 array_add.c -o array_add
gcc -O0 mat_mut.c -o mat_mut
valgrind --tool=lackey --basic-counts=no --trace-mem=yes ./array_add 2> array_add_memory_trace_lackey.txt 1>/dev/null
valgrind --tool=lackey --basic-counts=no --trace-mem=yes ./mat_mut 2> mat_mut_memory_trace_lackey.txt 1> /dev/null
echo "Finish."
