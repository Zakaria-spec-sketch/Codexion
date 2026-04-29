//helper.c
#include "codexion.h"

void log_action(t_coder *c, char *msg)
{
    long time;

    if (is_stopped(c->sim))
        return;

    pthread_mutex_lock(&c->sim->log_mutex);
    time = get_time_ms() - c->sim->start_time;
    printf("%ld %d %s\n", time, c->id, msg);
    pthread_mutex_unlock(&c->sim->log_mutex);
}


static void acquire_dongle(t_dongle *d, t_sim *sim)
{
    while (1)
    {
        pthread_mutex_lock(&d->mutex);
        if (is_stopped(sim) || get_time_ms() >= d->available_at)
            return;
        pthread_mutex_unlock(&d->mutex);
        usleep(500);
    }
}

void take_dongles(t_coder *c)
{
    t_dongle *first;
    t_dongle *second;

    if (c->left < c->right)
    {
        first  = c->left;
        second = c->right;
    }
    else
    {
        first  = c->right;
        second = c->left;
    }

    acquire_dongle(first, c->sim);
    log_action(c, "has taken a dongle");

    acquire_dongle(second, c->sim);
    log_action(c, "has taken a dongle");
}


void release_dongles(t_coder *c)
{
    long until;

    until = get_time_ms() + c->sim->cfg.dongle_cooldown;

    c->left->available_at  = until;
    c->right->available_at = until;

    pthread_mutex_unlock(&c->left->mutex);
    pthread_mutex_unlock(&c->right->mutex);
}

long get_time_ms(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000L + tv.tv_usec / 1000);
}

void smart_sleep(long ms)
{
    long start;

    start = get_time_ms();
    while (get_time_ms() - start < ms)
        usleep(500);
}

int is_stopped(t_sim *sim)
{
    int stop;

    pthread_mutex_lock(&sim->stop_mutex);
    stop = sim->stop;
    pthread_mutex_unlock(&sim->stop_mutex);

    return stop;
}