CFLAGS = -g -G
LIBS = 
LDFLAGS = 
CC = nvcc
RM = rm -f
OBJ = initdata.o initpop.o gene.o father_select.o crossover.o mutation.o fitness.o elism_select.o non_dom_sort.o log_front0.o conf.o time_record.o cuda_initdata.o cuda_fitness.o cuda_time_record.o cuda_crossover.o main.o
OPTION = -c 
OUTPUT_OPTION = -o
EXE = nsga2

$(EXE):$(OBJ) $(LIBS)
	$(CC) $(LDFLAGS) $(CFLAGS) -o $(EXE) $(OBJ) $(LIBS) $(LDFLAGS)
initdata.o : initdata.h initdata.cc
	$(CC) $(CFLAGS) $(OPTION) initdata.cc $(OUTPUT_OPTION) initdata.o
initpop.o : initpop.h initpop.cc
	$(CC) $(CFLAGS) $(OPTION) initpop.cc $(OUTPUT_OPTION) initpop.o
father_select.o : father_select.h father_select.cc
	$(CC) $(CFLAGS) $(OPTION) father_select.cc $(OUTPUT_OPTION) father_select.o
non_dom_sort.o : non_dom_sort.h non_dom_sort.cc
	$(CC) $(CFLAGS) $(OPTION) non_dom_sort.cc $(OUTPUT_OPTION) non_dom_sort.o
crossover.o : crossover.h crossover.cc
	$(CC) $(CFLAGS) $(OPTION) crossover.cc $(OUTPUT_OPTION) crossover.o
mutation.o : mutation.h mutation.cc
	$(CC) $(CFLAGS) $(OPTION) mutation.cc $(OUTPUT_OPTION) mutation.o
gene.o : gene.h gene.cc
	$(CC) $(CFLAGS) $(OPTION) gene.cc $(OUTPUT_OPTION) gene.o
fitness.o : fitness.h fitness.cc
	$(CC) $(CFLAGS) $(OPTION) fitness.cc $(OUTPUT_OPTION) fitness.o
elism_select.o : elism_select.h elism_select.cc
	$(CC) $(CFLAGS) $(OPTION) elism_select.cc $(OUTPUT_OPTION) elism_select.o
log_front0.o : log_front0.h log_front0.cc
	$(CC) $(CFLAGS) $(OPTION) log_front0.cc $(OUTPUT_OPTION) log_front0.o
conf.o : conf.h conf.cc
	$(CC) $(CFLAGS) $(OPTION) conf.cc $(OUTPUT_OPTION) conf.o
time_record.o : time_record.h time_record.cc
	$(CC) $(CFLAGS) $(OPTION) time_record.cc $(OUTPUT_OPTION) time_record.o
cuda_initdata.o : cuda_initdata.h cuda_initdata.cu
	$(CC) $(CFLAGS) $(OPTION) cuda_initdata.cu $(OUTPUT_OPTION) cuda_initdata.o
cuda_crossover.o : cuda_crossover.h cuda_crossover.cu
	$(CC) $(CFLAGS) $(OPTION) cuda_crossover.cu $(OUTPUT_OPTION) cuda_crossover.o
cuda_fitness.o : cuda_fitness.h cuda_fitness.cu
	$(CC) $(CFLAGS) $(OPTION) cuda_fitness.cu $(OUTPUT_OPTION) cuda_fitness.o
cuda_time_record.o : cuda_time_record.h cuda_time_record.cc
	$(CC) $(CFLAGS) $(OPTION) cuda_time_record.cc $(OUTPUT_OPTION) cuda_time_record.o
main.o: main.cc
	$(CC) $(CFLAGS) $(OPTION) main.cc $(OUTPUT_OPTION) main.o

.PHONY:clean
clean:
	$(RM) $(OBJ) $(EXE)
