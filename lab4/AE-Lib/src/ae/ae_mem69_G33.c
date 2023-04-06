

#include "ae_mem69_G33.h"
#include "ae_tasks.h"
#include "uart_polling.h"
#include "printf.h"
#include "ae_util.h"
#include "ae_tasks_util.h"


typedef struct
{
	int level;
	int HP;
	int attack;
	int defence;
	int speed;
} Test_Game_Entity;

#define NUM_TESTS       1       // number of tests
#define NUM_INIT_TASKS  1       // number of tasks during initialization

/*
 *===========================================================================
 *                             GLOBAL VARIABLES 
 *===========================================================================
 */

TASK_INIT    g_init_tasks[NUM_INIT_TASKS];
const char   PREFIX[]      = "G99-TS100";
const char   PREFIX_LOG[]  = "G99-TS100-LOG ";
const char   PREFIX_LOG2[] = "G99-TS100-LOG2";

AE_XTEST     g_ae_xtest;                // test data, re-use for each test
AE_CASE      g_ae_cases[NUM_TESTS];
AE_CASE_TSK  g_tsk_cases[NUM_TESTS];

void set_ae_init_tasks (TASK_INIT **pp_tasks, int *p_num)
{
    *p_num = NUM_INIT_TASKS;
    *pp_tasks = g_init_tasks;
    set_ae_tasks(*pp_tasks, *p_num);
}

// initial task configuration
void set_ae_tasks(TASK_INIT *tasks, int num)
{
    for (int i = 0; i < num; i++ ) {                                                 
        tasks[i].u_stack_size = PROC_STACK_SIZE;    
        tasks[i].prio = HIGH + i;
        tasks[i].priv = 1;
    }
    tasks[0].priv  = 1;
    tasks[0].ptask = &priv_task1;
    
    init_ae_tsk_test();
}

void init_ae_tsk_test(void)
{
    g_ae_xtest.test_id = 0;
    g_ae_xtest.index = 0;
    g_ae_xtest.num_tests = NUM_TESTS;
    g_ae_xtest.num_tests_run = 0;
    
    for ( int i = 0; i< NUM_TESTS; i++ ) {
        g_tsk_cases[i].p_ae_case = &g_ae_cases[i];
        g_tsk_cases[i].p_ae_case->results  = 0x0;
        g_tsk_cases[i].p_ae_case->test_id  = i;
        g_tsk_cases[i].p_ae_case->num_bits = 0;
        g_tsk_cases[i].pos = 0;  // first avaiable slot to write exec seq tid
        // *_expt fields are case specific, deligate to specific test case to initialize
    }
    printf("%s: START\r\n", PREFIX);
}

void update_ae_xtest(int test_id)
{
    g_ae_xtest.test_id = test_id;
    g_ae_xtest.index = 0;
    g_ae_xtest.num_tests_run++;
}

void gen_req0(int test_id)
{
    g_tsk_cases[test_id].p_ae_case->num_bits = 8;  
    g_tsk_cases[test_id].p_ae_case->results = 0;
    g_tsk_cases[test_id].p_ae_case->test_id = test_id;
    g_tsk_cases[test_id].len = 16; // assign a value no greater than MAX_LEN_SEQ
    g_tsk_cases[test_id].pos_expt = 0; // N/A for P1 tests
       
    g_ae_xtest.test_id = test_id;
    g_ae_xtest.index = 0;
    g_ae_xtest.num_tests_run++;
}
/*
 * basic should:
 * Simulate normal usage
 */
int mem_test_basic()
{
	/* 
	 * Allocate space for the player character and 3 enemies.
	 * Deallocate the enemies
	 * Allocate space for the boss
	 * Dump the memory
	 * done
	 */
	
	printf( "G%d-T%d: Begin basic test\r\n", ECE350_GN, Test_Suite_Num );
	
	Test_Game_Entity * Player = NULL;
	Test_Game_Entity * Enemy1 = NULL;
	Test_Game_Entity * Enemy2 = NULL;
	Test_Game_Entity * Enemy3 = NULL;
	Test_Game_Entity * Boss = NULL;
	
	Player = mem_alloc( sizeof(Test_Game_Entity) );
	Enemy1 = mem_alloc( sizeof(Test_Game_Entity) );
	Enemy2 = mem_alloc( sizeof(Test_Game_Entity) );
	Enemy3 = mem_alloc( sizeof(Test_Game_Entity) );
	
	if( (Player == NULL) || (Enemy1 == NULL) || (Enemy2 == NULL) || (Enemy3 == NULL) )
	{
		printf( "G%d-T%d: Memory was not allocated for at least 1 variable, pointers are P = 0x%x, E(0,1,2) = (0x%x, 0x%x, 0x%x)\r\n", ECE350_GN, Test_Suite_Num, Player, Enemy1, Enemy2, Enemy3 );
		return 0;
	}
	
	printf( "G%d-T%d: Allocated 4 times, each %d bytes, pointers are 0x%x, 0x%x, 0x%x, 0x%x\r\n", ECE350_GN, Test_Suite_Num, sizeof(Test_Game_Entity), Player, Enemy1, Enemy2, Enemy3 );
	
	Player->level = 25;
	Player->HP = 20;
	Player->attack = 20;
	Player->defence = 5;
	Player->speed = 10;
	
	Enemy1->level = 10;
	Enemy1->HP = 10;
	Enemy1->attack = 5;
	Enemy1->defence = 2;
	Enemy1->speed = 2;
	
	Enemy2->level = 10;
	Enemy2->HP = 10;
	Enemy2->attack = 5;
	Enemy2->defence = 2;
	Enemy2->speed = 2;
	
	Enemy3->level = 10;
	Enemy3->HP = 10;
	Enemy3->attack = 5;
	Enemy3->defence = 2;
	Enemy3->speed = 2;
	
	Enemy1->HP -= (Player->attack / Enemy1->defence);
	if( Enemy1->HP <= 0 )
	{
		if( mem_dealloc( Enemy1 ) == RTX_ERR )
		{
			printf( "G%d-T%d: Could not deallocate pointer 0x%x\r\n", ECE350_GN, Test_Suite_Num, Enemy1 );
			mem_dump();
			return 0;
		}
		printf( "G%d-T%d: Deallocated ptr 0x%x\r\n", ECE350_GN, Test_Suite_Num, Enemy1 );
		Enemy1 = NULL;
	}
	
	Player->HP -= (Enemy2->attack / Player->defence);
	Player->HP -= (Enemy3->attack / Player->defence);
	Enemy2->HP -= (Player->attack / Enemy2->defence);
	
	if( Enemy2->HP <= 0 )
	{
		if( mem_dealloc( Enemy2 ) == RTX_ERR )
		{
			printf( "G%d-T%d: Could not deallocate pointer 0x%x\r\n", ECE350_GN, Test_Suite_Num, Enemy2 );
			mem_dump();
			return 0;
		}
		printf( "G%d-T%d: Deallocated ptr 0x%x\r\n", ECE350_GN, Test_Suite_Num, Enemy2 );
		Enemy2 = NULL;
	}
	
	Player->HP -= (Enemy3->attack / Player->defence);
	Enemy3->HP -= (Player->attack / Enemy3->defence);
	
	if( Enemy3-> HP <= 0 )
	{
		if( mem_dealloc( Enemy3 ) == RTX_ERR )
		{
			printf( "G%d-T%d: Could not deallocate pointer 0x%x\r\n", ECE350_GN, Test_Suite_Num, Enemy3 );
			mem_dump();
			return 0;
		}
		printf( "G%d-T%d: Deallocated ptr 0x%x\r\n", ECE350_GN, Test_Suite_Num, Enemy3 );
		Enemy3 = NULL;
	}
	
	Boss = mem_alloc( sizeof(Test_Game_Entity) );
	
	if( Boss == NULL )
	{
		printf( "G%d-T%d: Could not allocate size %d\r\n", ECE350_GN, Test_Suite_Num, Boss );
		mem_dump();
		return 0;
	}
	
	Boss->level = 50;
	Boss->HP = 50;
	Boss->attack = 100;
	Boss->defence = 100;
	Boss->speed = 35;
	
	printf( "G%d-T%d: Allocated %d bytes, pointer is 0x%x\r\n", ECE350_GN, Test_Suite_Num, sizeof(Test_Game_Entity), Boss );
	
	Player->HP -= Boss->attack / Player->defence;
	
	if( mem_dump() == RTX_ERR )
	{
		printf( "G%d-T%d: Could not dump memory\r\n", ECE350_GN, Test_Suite_Num );
		return 0;
	}
	
	if ( mem_dealloc(Player) == RTX_ERR )
	{
		printf( "G%d-T%d: Could not deallocate pointer 0x%x\r\n", ECE350_GN, Test_Suite_Num, Player );
		mem_dump();
		return 0;
	}
	
	Player = NULL;
	Boss = NULL;
	
	printf( "G%d-T%d: End basic test\r\n", ECE350_GN, Test_Suite_Num );
	return 1;
}

int mem_test_robust()
{
	printf( "G%d-T%d: Begin robust test\r\n", ECE350_GN, Test_Suite_Num );
	
	int result = 1;
	size_t randSize = 1;
	int iterator;
	for( iterator = 0; iterator < 4098; iterator++ )
	{
		/* Those are just 2 large enough primes, they could be anything */
		randSize = (2938948301*randSize + 5254103177);
		void * mem = mem_alloc( (randSize % 829) + 4 );
		if ( ((randSize % 829) + 4) > 0x1000 )
			result &= (mem == NULL);
		else
		{
			result &= (mem != NULL);
			result &= (mem_dealloc(mem) == RTX_OK);
		}
			
		mem = NULL;
	}
	
	if( result == 0 )
	{
		printf( "G%d-T%d: Could not allocate and deallocate 4098 times sequentially.\r\n", ECE350_GN, Test_Suite_Num );
		mem_dump();
		return 0;
	}
	printf( "G%d-T%d: Was able to allocate and deallocate 4098 times successfully.\r\n", ECE350_GN, Test_Suite_Num );
	
	void * memArray[16];
	for( iterator = 0; iterator < 64; iterator++ )
	{
		memArray[iterator] = mem_alloc(32);
		result &= (memArray[iterator] != NULL);
	}
	if( result != 0 )
		for( iterator = 0; iterator < 64; iterator++ )
		{
			mem_dealloc( memArray[iterator] );
			memArray[iterator] = NULL;
		}
	
	if( result == 0 )
	{
		printf( "G%d-T%d: Could not allocate 64 memory blocks (at once).\r\n", ECE350_GN, Test_Suite_Num );
		mem_dump();
		return 0;
	}
	printf( "G%d-T%d: Successfully allocated 64 memory blocks (at once).\r\n", ECE350_GN, Test_Suite_Num );
	
	for( iterator = 0; iterator < 4098; iterator++ )
	{
		void * mem1 = mem_alloc( 512 );
		void * mem2 = mem_alloc( 128 );
		void * mem3 = mem_alloc( 256 );
		
		result &= ( mem1 != NULL )&( mem2 != NULL )&( mem3 != NULL );
		
		if( result != 1 )
			break;
		
		result &= ( mem_dealloc( mem1 ) == RTX_OK );
		result &= ( mem_dealloc( mem2 ) == RTX_OK );
		result &= ( mem_dealloc( mem3 ) == RTX_OK );
		mem1 = NULL;
		mem2 = NULL;
		mem3 = NULL;
		if( result != 1	)
			break;
	}
	
	if( result == 0 )
	{
		printf( "G%d-T%d: Could not allocate huge blocks of mem over and over again\r\n", ECE350_GN, Test_Suite_Num );
		mem_dump();
		return 0;
	}
	printf( "G%d-T%d: Successfully allocated huge blocks of mem 4098 times\r\n", ECE350_GN, Test_Suite_Num );
	
	printf( "G%d-T%d: End robust test\r\n", ECE350_GN, Test_Suite_Num );
	
	return 1;
}
int mem_test_limits()
{
	return 1;
}

int mem_test_speed()
{
	
	return 1;
}

int mem_test_all()
{
	int result = 1;
	int numPassed = 0;
	
	int basic_result = mem_test_basic();
	result &= basic_result;
	numPassed += basic_result;
	
	int robust_result = mem_test_robust();
	result &= robust_result;
	numPassed += robust_result;
	
	int limits_result = mem_test_limits();
	result &= limits_result;
	numPassed += limits_result;
	
	int speed_result = mem_test_speed();
	result &= speed_result;
	numPassed += speed_result;
	
	return result;
}


void priv_task1(void)
{
    int test_id = 0;
    
    printf("%s: priv_task1: basic memory test on IRAM2\r\n", PREFIX_LOG2);
#ifdef ECE350_P1    
    mem_test_all();
#endif // ECE350_P1
    test_exit();
}
