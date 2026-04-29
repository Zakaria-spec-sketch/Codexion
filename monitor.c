//monitor.c
#include "codexion.h"

void *monitor_routine(void *arg)
{
    t_sim   *sim = (t_sim *)arg;
    int     i;
    int     all_done;

    while (!is_stopped(sim))
    {
        i        = 0;
        all_done = 1;

        while (i < sim->cfg.number_of_coders)
        {
            long    last;
            long    now = get_time_ms();
            int     compile_count;

            pthread_mutex_lock(&sim->coders[i].meal_mutex);
            last          = sim->coders[i].last_compile;
            compile_count = sim->coders[i].compile_count;
            pthread_mutex_unlock(&sim->coders[i].meal_mutex);

            if (compile_count >= sim->cfg.number_of_compiles_required)
            {
                i++;
                continue;
            }

            all_done = 0;

            if (now - last > sim->cfg.time_to_burnout)
            {
                pthread_mutex_lock(&sim->stop_mutex);
                sim->stop = 1;
                pthread_mutex_unlock(&sim->stop_mutex);

                pthread_mutex_lock(&sim->log_mutex);
                printf("%ld %d burned out\n",
                    now - sim->start_time,
                    sim->coders[i].id);
                pthread_mutex_unlock(&sim->log_mutex);

                return (NULL);
            }
            i++;
        }

        if (all_done)
        {
            pthread_mutex_lock(&sim->stop_mutex);
            sim->stop = 1;
            pthread_mutex_unlock(&sim->stop_mutex);
            return (NULL);
        }

        usleep(1000);
    }
    return (NULL);
}