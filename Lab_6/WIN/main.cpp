#if defined(_WIN32) || defined(_WIN64)
#include <iostream>
#include <conio.h>

using namespace std;

#define ALL_MEMORY 65536 
#define BLOCK_SIZE 512 

char program_memory[ALL_MEMORY];
static unsigned long available_memory = ALL_MEMORY;

char* block_buf[BLOCK_SIZE];
unsigned block_buf_size[BLOCK_SIZE];
static unsigned block_buf_counter = 1;

char* use_buf[BLOCK_SIZE];
static unsigned use_buf_counter = 0;
static unsigned last_block_use_buf;

unsigned block_size[BLOCK_SIZE];


void* pointers[64];

void init();
void* Malloc(unsigned);
int Free(void*);
void* Realloc(void*, unsigned);

// организация памяти с  границами разделов
int main()
{
	init();
	unsigned size;
	unsigned index;
	while (true)
	{
		cout << "1. malloc" << endl;
		cout << "2. free" << endl;
		cout << "3. realloc" << endl;
		cout << "4. quit" << endl;
		switch (_getch()) {
		case '1': {
			cout << "Enter index:" << endl;
			cin >> index;
			if (index > 64) {
				cerr << "Wrong index!" << endl;
				break;
			}
			if (pointers[index] == NULL) {
				cout << "Input size:" << endl;
				cin >> size;
				pointers[index] = Malloc(size);
				if (pointers[index] == NULL) {
					cerr << "Error! No memory!" << endl;
					return EXIT_FAILURE;
				}
				cout << "Use malloc:" << endl;
				printf("Blocks used: %d\n", use_buf_counter);
			}
			else {
				cerr << "Error!" << endl;
				break;
			}
			break;
		}
		case '2': {
			cout << "Input index:" << endl;
			cin >> index;
			if (Free(pointers[index]) == -1) {
				cout << "Our block no found" << endl;
				break;
			}
			printf("Blocks used: %d\n", use_buf_counter);
			break;
		}
		case '3': {
			cout << "Input index:" << endl;
			cin >> index;
			cout << "Input size:" << endl;
			cin >> size;
			pointers[index] = Realloc(pointers[index], size);
			if (pointers[index] == NULL) {
				cerr << "Error! No memory!" << endl;
				return 1;
			}
			printf("Blocks used: %d\n", use_buf_counter);
			break;
		}
		case '4':
			return 0;
		default:
			break;
		}
		system("cls");
	}
}

void init()
{
	*block_buf = program_memory;
	*block_buf_size = available_memory;
}

void* Malloc(const unsigned size)
{
	if (size > available_memory) {
		return NULL;
	}
	unsigned long int last;
	char *temp = NULL;
	for (unsigned i = 0; i < block_buf_counter; i++) {
		if (size <= block_buf_size[i]) {
			temp = block_buf[i];
			last = i;
			break;
		}
	}
	if (!temp) {
		return NULL;
	}
	use_buf[use_buf_counter] = temp;
	block_size[use_buf_counter] = size;
	use_buf_counter++;
	last_block_use_buf++;
	block_buf[last] = (char*)(last + size + 1);
	block_buf[last] = block_buf[last] - size;
	available_memory -= size;
	return temp;
}

int Free(void* memblock)
{
	unsigned int last;
	char* temp = NULL;
	for (unsigned i = 0; i < last_block_use_buf; i++) {
		if (memblock == use_buf[i]) {
			temp = use_buf[i];
			last = i;
			break;
		}
	}
	if (!temp) {
		return -1;
	}
	use_buf[last] = NULL;
	use_buf_counter--;
	block_buf[block_buf_counter] = (char*)memblock;
	block_buf_size[block_buf_counter] = block_size[last];
	block_buf_counter++;
	available_memory += block_size[last];
	return 0;
}

void* Realloc(void* memory_block, unsigned size)
{
	unsigned int i, last;
	char* tempArray = NULL;
	for (i = 0; i < last_block_use_buf; i++) {
		if (memory_block == use_buf[i]) {
			if (use_buf[i] == NULL) {
				return Malloc(size);
			}
		}
	}
	if (size == 0) {
		Free(memory_block);
		return NULL;
	}
	for (i = 0; i < last_block_use_buf; i++) {
		if (memory_block == use_buf[i]) {
			tempArray = use_buf[i];
			last = i;
			break;
		}
	}
	if (!tempArray) {
		return NULL;
	}
	Free(memory_block);
	return Malloc(size);
}
////////////////////////////////////////////////////////////////////////////////////////////
//int has_initialized = 0;
//void *managed_memory_start;
//void *last_valid_address;
//
//void malloc_init()
//{
//	last_valid_address = sbrk(0);
//	managed_memory_start = last_valid_address;
//	has_initialized = 1;
//}
//
//struct mem_control_block {
//	int is_available;
//	int size;
//};
//
//void free(void *firstbyte) {
//	struct mem_control_block *mcb;
//	mcb = firstbyte - sizeof(struct mem_control_block);
//	mcb->is_available = 1;
//	return;
//}
//
//void *malloc(long numbytes) {
//	void *current_location;
//	struct mem_control_block *current_location_mcb;
//	void *memory_location;
//	if (!has_initialized) {
//		malloc_init();
//	}
//	numbytes += sizeof(struct mem_control_block);
//	memory_location = NULL;
//	current_location = managed_memory_start;
//	while (current_location != last_valid_address) {
//		current_location_mcb = (struct mem_control_block *)current_location;
//		if (current_location_mcb->is_available) {
//			if (current_location_mcb->size >= numbytes) {
//				current_location_mcb->is_available = 0;
//				memory_location = current_location;
//				break;
//			}
//		}
//		current_location = current_location + current_location_mcb->size;
//	}
//	if (!memory_location) {
//		sbrk(numbytes);
//		memory_location = last_valid_address;
//		last_valid_address = last_valid_address + numbytes;
//		current_location_mcb = memory_location;
//		current_location_mcb->is_available = 0;
//		current_location_mcb->size = numbytes;
//	}
//	memory_location = memory_location + sizeof(struct mem_control_block);
//	return memory_location;
//}

#endif