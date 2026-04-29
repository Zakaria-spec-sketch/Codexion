#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <string.h>
# include <limits.h>

# define FIFO 0
# define EDF  1

typedef struct s_sim t_sim;


typedef struct s_config
{
    int     number_of_coders;
    long    time_to_burnout;
    long    time_to_compile;
    long    time_to_debug;
    long    time_to_refactor;
    int     number_of_compiles_required;
    long    dongle_cooldown;
    int     scheduler;
}   t_config;

typedef struct s_coder
{
    int             id;
    int             compile_count;
    long            last_compile;
    pthread_t       thread;
    struct s_sim    *sim;
    struct s_dongle *left;
    struct s_dongle *right;
    pthread_mutex_t meal_mutex;
}   t_coder;

typedef struct s_dongle
{
    pthread_mutex_t mutex;
    long            available_at;
}   t_dongle;

struct s_sim
{
    t_config        cfg;

    int             stop;

    long            start_time;

    t_coder         *coders;
    t_dongle        *dongles;

    pthread_mutex_t log_mutex;
    pthread_mutex_t stop_mutex;
};

int pars_args(int argc, char **argv, t_config *cfg);
long get_time_ms(void);
void smart_sleep(long ms);
t_sim *init_sim(t_config cfg);
void *coder_routine(void *arg);
void take_dongles(t_coder *c);
void release_dongles(t_coder *c);
void log_action(t_coder *c, char *msg);
void *monitor_routine(void *arg);
int is_stopped(t_sim *sim);

#endif