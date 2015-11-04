#include <stdlib.h>
#include <stdio.h>

#include <common/Common.h>

typedef struct
{
    bool        is_inc;
    long long   i,j,k;
} StateT;
#define STATET(ISPLUS,I,J,K) (StateT){.is_inc = (ISPLUS), .i = (I)-1, .j = (J)-1, .k = (K)-1}
#define INC(I,J) STATET(true,I,J,0)
#define DEC(I,J,K) STATET(false,I,J,K)

#define REG_CT 2048
typedef struct
{
    StateT          *program;
    size_t          instct;
    unsigned int    reg_touched,
                    index;
    long long       registers[REG_CT];
} RegMachine;

#define Create_Register_Machine_Program(name,...)                               \
    RegMachine *(name) = new_register_machine();                                \
    {                                                                           \
        StateT proglist[] = {__VA_ARGS__,(StateT){.is_inc = false, .i = -1}};   \
        for(unsigned int i = 0;proglist[i].i >= 0;++i)                          \
        {                                                                       \
            add_register_state((name), proglist[i]);                            \
        }                                                                       \
    }                                                                           

#define Initial_Memory_Layout(name, ...)                                        \
    {                                                                           \
        long long regarray[] = {__VA_ARGS__};                                   \
        (name)->reg_touched  = (sizeof(regarray)/sizeof(long long)) - 1;        \
        for(unsigned int i = 0;i<(sizeof(regarray)/sizeof(long long));++i)      \
        {                                                                       \
            (name)->registers[i] = regarray[i];                                 \
        }                                                                       \
    }



RegMachine *new_register_machine()
{
    RegMachine *new = malloc(sizeof(RegMachine));
    new->instct         = 0;
    new->reg_touched    = 0;
    new->index          = 0;
    new->program        = NULL;

    for(unsigned int i = 0;i<REG_CT;++i)
        new->registers[i] = 0;

    return new;
}

void step_register_machine(RegMachine *machine)
{
    if(machine->index < machine->instct)
    {
       if(machine->program[machine->index].i > machine->reg_touched)
           machine->reg_touched = machine->program[machine->index].i;

       switch(machine->program[machine->index].is_inc)
       {
            case 1:
                machine->registers[machine->program[machine->index].i] += 1;
                machine->index = machine->program[machine->index].j;
                break;
            case 0:
                if(machine->registers[machine->program[machine->index].i] == 0)
                {
                    machine->index = machine->program[machine->index].k;
                }
                else
                {
                    machine->registers[machine->program[machine->index].i] -= 1;
                    machine->index = machine->program[machine->index].j;
                }
            break;
       } 
    }
}

void add_register_state(RegMachine *machine, StateT s)
{
    if(machine->program == NULL)
    {
        machine->program = malloc(sizeof(StateT));
    }
    else
    {
        machine->program = realloc(machine->program, sizeof(StateT) * (machine->instct + 1));
    }

    machine->program[machine->instct] = s;
    machine->instct += 1;
}

void print_register_machine(RegMachine *machine)
{
    for(unsigned int i = 0;i<=machine->reg_touched;++i)
    {
        printf("|%10lld|\n", machine->registers[i]);
    }
}

int main () 
{
    Create_Register_Machine_Program(regM, DEC(2,2,0), INC(1,1));
    Initial_Memory_Layout(regM, 0, 34);
    while(true)
    {
        print_register_machine(regM);
        step_register_machine(regM);
        getchar();
    }
    return EXIT_SUCCESS;
}
