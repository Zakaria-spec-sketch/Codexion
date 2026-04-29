//parsing.c
#include "codexion.h"

static int safe_atoi(const char *str, long *out)
{
    long result = 0;
    int i = 0;

    if (!str || !str[0])
        return (0);

    while (str[i])
    {
        if (str[i] < '0' || str[i] > '9')
            return (0);
        result = result * 10 + (str[i] - '0');
        if (result > INT_MAX)
            return (0);
        i++;
    }
    *out = result;
    return (1);
}

static int parse_scheduler(const char *str, int *scheduler)
{
    if (strcmp(str, "fifo") == 0)
        *scheduler = 0;
    else if (strcmp(str, "edf") == 0)
        *scheduler = 1;
    else
        return (0);
    return (1);
}

int pars_args(int argc, char **argv, t_config *cfg)
{
   long tmp;

    if (argc != 9)
    {
        printf("Error: wrong number of arguments\n");
        return (0);
    }

    if (!safe_atoi(argv[1], &tmp) || tmp < 0)
        return (printf("Error: invalid number_of_coders\n"), 0);
    cfg->number_of_coders = (int)tmp;

    if (!safe_atoi(argv[2], &tmp) || tmp < 0)
        return (printf("Error: invalid time_to_burnout\n"), 0);
    cfg->time_to_burnout = tmp;

    if (!safe_atoi(argv[3], &tmp) || tmp < 0)
        return (printf("Error: invalid time_to_compile\n"), 0);
    cfg->time_to_compile = tmp;

    if (!safe_atoi(argv[4], &tmp) || tmp < 0)
        return (printf("Error: invalid time_to_debug\n"), 0);
    cfg->time_to_debug = tmp;

    if (!safe_atoi(argv[5], &tmp) || tmp < 0)
        return (printf("Error: invalid time_to_refactor\n"), 0);
    cfg->time_to_refactor = tmp;

    if (!safe_atoi(argv[6], &tmp) || tmp < 0)
        return (printf("Error: invalid number_of_compiles_required\n"), 0);
    cfg->number_of_compiles_required = (int)tmp;

    if (!safe_atoi(argv[7], &tmp) || tmp < 0)
        return (printf("Error: invalid dongle_cooldown\n"), 0);
    cfg->dongle_cooldown = tmp;

    if (!parse_scheduler(argv[8], &cfg->scheduler))
        return (printf("Error: scheduler must be 'fifo' or 'edf'\n"), 0);

    return (1);
}
