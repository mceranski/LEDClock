#ifndef CMD_H
#define CMD_H

#define MAX_MSG_SIZE    60
#include <stdint.h>

// command line structure
typedef struct _cmd_t
{
    char *cmd;
    void (*func)(int argc, char **argv);
    struct _cmd_t *next;
} cmd_t;

void cmdInit(uint32_t speed);
void cmdPoll();
void cmdAdd(char *name, void (*func)(int argc, char **argv));
uint32_t cmdStr2Num(char *str, uint8_t base);

#endif //CMD_H
