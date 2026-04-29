//init.c
#include "codexion.h"

t_sim *init_sim(t_config cfg)
{
    t_sim   *sim;
    int     i;

    sim = malloc(sizeof(t_sim));
    if (!sim)
        return (NULL);

    sim->cfg        = cfg;
    sim->stop       = 0;
    sim->start_time = get_time_ms();

    sim->coders = malloc(sizeof(t_coder) * cfg.number_of_coders);
    if (!sim->coders)
    {
        free(sim);
        return (NULL);
    }

    sim->dongles = malloc(sizeof(t_dongle) * cfg.number_of_coders);
    if (!sim->dongles)
    {
        free(sim->coders);
        free(sim);
        return (NULL);
    }

    pthread_mutex_init(&sim->log_mutex, NULL);
    pthread_mutex_init(&sim->stop_mutex, NULL);

    i = 0;
    while (i < cfg.number_of_coders)
    {
        pthread_mutex_init(&sim->dongles[i].mutex, NULL);
        sim->dongles[i].available_at = 0;
        i++;
    }

    i = 0;
    while (i < cfg.number_of_coders)
    {
        sim->coders[i].id            = i + 1;
        sim->coders[i].sim           = sim;
        sim->coders[i].compile_count = 0;
        sim->coders[i].last_compile  = sim->start_time;

        sim->coders[i].left  = &sim->dongles[i];
        sim->coders[i].right = &sim->dongles[(i + 1) % cfg.number_of_coders];
        pthread_mutex_init(&sim->coders[i].meal_mutex, NULL);

        i++;
    }

    return (sim);
}