//main.c

#include "codexion.h"

void *coder_routine(void *arg)
{
    t_coder *c = (t_coder *)arg;

    if (c->sim->cfg.number_of_coders == 1)
    {
        log_action(c, "has taken a dongle");
        smart_sleep(c->sim->cfg.time_to_burnout);
        log_action(c, "burned out");
        return (NULL);
    }

    while (!is_stopped(c->sim) &&
           c->compile_count < c->sim->cfg.number_of_compiles_required)
    {
        take_dongles(c);

        if (is_stopped(c->sim))
        {
            release_dongles(c);
            break;
        }

        pthread_mutex_lock(&c->meal_mutex);
        c->last_compile = get_time_ms();
        pthread_mutex_unlock(&c->meal_mutex);

        log_action(c, "is compiling");
        smart_sleep(c->sim->cfg.time_to_compile);

        pthread_mutex_lock(&c->meal_mutex);
        c->compile_count++;
        pthread_mutex_unlock(&c->meal_mutex);

        release_dongles(c);

        if (is_stopped(c->sim))
            break;

        log_action(c, "is debugging");
        smart_sleep(c->sim->cfg.time_to_debug);

        if (is_stopped(c->sim))
            break;

        log_action(c, "is refactoring");
        smart_sleep(c->sim->cfg.time_to_refactor);
    }

    return (NULL);
}

int main(int argc, char **argv)
{
    t_config    cfg;
    t_sim       *sim;
    int         i;
    pthread_t   monitor;

    if (!pars_args(argc, argv, &cfg))
        return (1);

    sim = init_sim(cfg);
    if (!sim)
    {
        printf("Error: failed to allocate simulation\n");
        return (1);
    }

    pthread_create(&monitor, NULL, monitor_routine, sim);

    i = 0;
    while (i < cfg.number_of_coders)
    {
        pthread_create(&sim->coders[i].thread,
                       NULL,
                       coder_routine,
                       &sim->coders[i]);
        i++;
    }

    i = 0;
    while (i < cfg.number_of_coders)
    {
        pthread_join(sim->coders[i].thread, NULL);
        i++;
    }

    pthread_join(monitor, NULL);

    pthread_mutex_destroy(&sim->log_mutex);
    pthread_mutex_destroy(&sim->stop_mutex);

    i = 0;
    while (i < cfg.number_of_coders)
    {
        pthread_mutex_destroy(&sim->coders[i].meal_mutex);
        pthread_mutex_destroy(&sim->dongles[i].mutex);
        i++;
    }

    free(sim->coders);
    free(sim->dongles);
    free(sim);

    return (0);
}