#include "headers/io.h"

void saveResultsLean(stats *statistics) {
    FILE *f_wl, *f_ob, *f_tr, *f_act;
    char path[MAX_LEN];
    char text[MAX_LEN];
    char *policy, *s, *format;
    int i,j;

#ifdef ABO_ID
    policy = "id";
#else
    policy = "comp";
#endif

#ifdef IMPROVEMENT
    policy = "incomp";
#endif
    stats st = {0};

    // waiting list
    format = "Results for WAITING_LIST:\n"
             "Arrivals ............... = %f\n"
             "Completions ............ = %f\n"
             "Reneged ................ = %f\n"
             "Average wait ........... = %f\n"
             "Average delay .......... = %f\n"
             "Average service time ... = %f\n";

    for (int k = 0; k < NUM_BLOOD_TYPES; ++k) {
        for (int l = 0; l < NUM_PRIORITIES; ++l) {
            snprintf(path, MAX_LEN, "output/lean/waiting_list_%s_%u_%u.txt", policy, (PRIORITY)l, (BLOOD_TYPE)k);
            OPEN_FILE(f_wl, path)
            sprintf(text, format,
                    statistics->wl_stats->sum_patient_arrivals[k][l],
                    statistics->wl_stats->sum_patient_served[k][l],
                    statistics->wl_stats->sum_patient_reneges[k][l]+statistics->wl_stats->sum_patient_deaths[k][l],
                    statistics->wl_stats->avg_wait[k][l],
                    statistics->wl_stats->avg_delay[k][l],
                    statistics->wl_stats->avg_service[k][l]);
            fprintf(f_wl, "%s", text);
        }
    }

    // organs
    format = "Results for ORGAN_BANK:\n"
             "Arrivals ............... = %f\n"
             "Completions ............ = %f\n"
             "Reneged ................ = %f\n"
             "Average wait ........... = %f\n"
             "Average delay .......... = %f\n"
             "Average service time ... = %f\n";

    for (int k = 0; k < NUM_BLOOD_TYPES; ++k) {
        snprintf(path, MAX_LEN, "output/lean/organs_%s_%u.txt", policy, (BLOOD_TYPE)k);
        OPEN_FILE(f_ob, path)
        sprintf(text, format,
                statistics->ob_stats->sum_organ_arrivals[k][0],
                statistics->ob_stats->sum_organs_completions[k][0],
                statistics->ob_stats->sum_organ_outdatings[k],
                statistics->ob_stats->avg_wait[k][0],
                statistics->ob_stats->avg_delay[k][0],
                statistics->ob_stats->avg_service[k][0]);
        fprintf(f_ob, "%s", text);
    }

    // Activation center
    format = "Results for ACTIVATION_CENTER:\n"
             "Arrivals ............... = %f\n"
             "Completions ............ = %f\n"
             "Reneged ................ = %f\n"
             "Average wait ........... = %f\n"
             "Average delay .......... = %f\n"
             "Average service time ... = %f\n";

    for (int k = 0; k < NUM_BLOOD_TYPES; ++k) {
        snprintf(path, MAX_LEN, "output/lean/activation_%s_%u.txt", policy, (BLOOD_TYPE)k);
        OPEN_FILE(f_act, path)
        sprintf(text, format,
                statistics->act_stats->sum_arrivals[k],
                statistics->act_stats->sum_activated[k],
                statistics->act_stats->sum_reneges[k]+statistics->act_stats->sum_deaths[k],
                statistics->act_stats->avg_wait[k],
                statistics->act_stats->avg_delay[k],
                statistics->act_stats->avg_service[k]);
        fprintf(f_act, "%s", text);
    }

    // Transplant center
    format = "Results for TRANSPLANT_CENTER:\n"
             "Arrivals ............... = %f\n"
             "Completions ............ = %f\n"
             "Rejections ............. = %f\n"
             "Average wait ........... = %f\n"
             "Average delay .......... = %f\n"
             "Average service time ... = %f\n";

    for (int k = 0; k < NUM_BLOOD_TYPES; ++k) {
        for (int l = 0; l < NUM_PRIORITIES-1; ++l) {
            snprintf(path, MAX_LEN, "output/lean/transplant_%s_%u.txt", policy, (BLOOD_TYPE)k);
            OPEN_FILE(f_tr, path)
            sprintf(text, format,
                    statistics->trans_stats->sum_arrivals[k][l],
                    statistics->trans_stats->sum_transplanted[k][l],
                    statistics->trans_stats->sum_rejected[k][l],
                    statistics->trans_stats->avg_wait[k][l],
                    statistics->trans_stats->avg_delay[k][l],
                    statistics->trans_stats->avg_service[k][l]);
            fprintf(f_tr, "%s", text);
        }
    }

}

void saveResultsCsv(int iter, stats *statistics, bool batch, int batch_num) {
    FILE *f_wl, *f_ob, *f_tr, *f_act;
    char path[MAX_LEN];
    char *policy, *header;
    int i,j;

#ifdef ABO_ID
    policy = "id";
#else
    policy = "comp";
#endif

#ifdef IMPROVEMENT
    policy = "incomp";
#endif

    // ----------------------------------------------- HEADERS -------------------------------------------------------

    // Patients
    if (batch) {
        snprintf(path, MAX_LEN, "output/batch/waiting_list/%s/waiting_list_%s_%d.csv", policy, policy, batch_num);
    } else {
        snprintf(path, MAX_LEN, "output/%d_waiting_list_%s.csv", iter, policy);
    }
    OPEN_FILE(f_wl, path)
    header = "Blood type,Priority,"
             "(Avg) Patients arrived,CI arrivals,"
             "(Avg) Patients dead,CI deaths,"
             "(Avg) Patients reneged,CI reneges,"
             "Patients in queue,"
             "Avg inter-arrival times,CI inter-arrival times,"
             "Avg wait,CI wait,"
             "Avg delay,CI delay,"
             "Avg service time,CI service time,"
             "Avg # in the node,CI # in the node,"
             "Avg # in the queue,CI # in the queue,"
             "Utilization,CI utilization\n";
    fprintf(f_wl, "%s", header);

    // Organs
    if (batch) {
        snprintf(path, MAX_LEN, "output/batch/organs/%s/organs_%s_%d.csv", policy, policy, batch_num);
    } else {
        snprintf(path, MAX_LEN, "output/%d_organs_%s.csv", iter, policy);
    }
    OPEN_FILE(f_ob, path)
    header = "Blood type,(Avg) Deceased donor organs arrived,(Avg) Living donor organs arrived,(Avg) Organs outdated,Organs in queue,"
             "Avg inter-arrival times,CI deceased donor arrivals, CI living donor arrivals, CI outdatings,"
             " CI inter-arrival times,Avg # in the queue,CI # in the queue\n";
    fprintf(f_ob, "%s", header);

    // Activation
    if (batch) {
        snprintf(path, MAX_LEN, "output/batch/activation/%s/activation_%s_%d.csv", policy, policy, batch_num);
    } else {
        snprintf(path, MAX_LEN, "output/%d_activation_%s.csv", iter, policy);
    }
    OPEN_FILE(f_act, path)
    header = "(Avg) Patients arrived,CI arrivals,(Avg) Patients activated,CI activations,(Avg) Patients dead,CI deaths,(Avg) Patients reneged,CI reneges,Avg delay,CI delay,"
             "Avg # in the node,CI # in the node\n";
    fprintf(f_act, "%s", header);

    // Transplant
    if (batch) {
        snprintf(path, MAX_LEN, "output/batch/transplant/%s/transplant_%s_%d.csv", policy, policy, batch_num);
    } else {
        snprintf(path, MAX_LEN, "output/%d_transplant_%s.csv", iter, policy);
    }
    OPEN_FILE(f_tr, path)
    header = "Blood type,Priority,Successful transplant,Rejected transplant,Rejection percentage,"
             "Avg # in the node,CI # in the node\n";
    fprintf(f_tr, "%s", header);

    // ------------------------------------------------- DATA ------------------------------------------------------

    waiting_list_stats *wl_stats = statistics->wl_stats;
    organ_bank_stats *ob_stats = statistics->ob_stats;
    transplant_stats *trans_stats = statistics->trans_stats;
    activation_stats *act_stats = statistics->act_stats;

    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        fprintf(f_ob, "%s,%f,%f,%f,%f,%f,+/-%f,+/-%f,+/-%f,+/-%f,%f,+/-%f\n", bt_to_str[i],
                ob_stats->avg_arrivals[i][0], ob_stats->avg_arrivals[i][1], ob_stats->avg_outdatings[i],
                ob_stats->num_organs_in_queue[i], ob_stats->avg_interarrival_time[i], ob_stats->std_arrivals[i][0],
                ob_stats->std_arrivals[i][1], ob_stats->std_outdatings[i],ob_stats->std_interarrival_time[i],
                ob_stats->avg_in_queue[i], ob_stats->std_in_queue[i]);

        for (j = 0; j < NUM_PRIORITIES; ++j) {
            fprintf(f_wl, "%s,%s,%f,+/-%f,%f,+/-%f,%f,+/-%f,%f,%f,+/-%f,%f,+/-%f,%f,+/-%f,%f,+/-%f,"
                          "%f,+/-%f,%f,+/-%f,%f,+/-%f\n", bt_to_str[i], pr_to_str[j],
                    wl_stats->avg_arrivals[i][j], wl_stats->std_arrivals[i][j],
                    wl_stats->avg_deaths[i][j], wl_stats->std_deaths[i][j],
                    wl_stats->avg_reneges[i][j], wl_stats->std_reneges[i][j],
                    wl_stats->num_patients_in_queue[i][j],
                    wl_stats->avg_interarrival_time[i][j], wl_stats->std_interarrival_time[i][j],
                    wl_stats->avg_wait[i][j], wl_stats->std_wait[i][j],
                    wl_stats->avg_delay[i][j], wl_stats->std_delay[i][j],
                    wl_stats->avg_service[i][j], wl_stats->std_service[i][j],
                    wl_stats->avg_in_node[i][j], wl_stats->std_in_node[i][j],
                    wl_stats->avg_in_queue[i][j], wl_stats->std_in_queue[i][j],
                    wl_stats->utilization[i][j], wl_stats->std_utilization[i][j]);

            fprintf(f_tr, "%s,%s,%f,%f,%f,%f,+/-%f\n", bt_to_str[i], pr_to_str[j],
                    trans_stats->num_transplanted[i][j], trans_stats->num_rejected[i][j],
                    trans_stats->rejection_perc[i][j], trans_stats->avg_in_node, trans_stats->std_in_node);
        }

        fprintf(f_act, "%f,+/-%f,%f,+/-%f,%f,+/-%f,%f,+/-%f,%f,+/-%f,%f,+/-%f\n",
                act_stats->avg_arrivals[i], act_stats->std_arrivals[i],
                act_stats->avg_activated[i], act_stats->std_activated[i],
                act_stats->avg_deaths[i], act_stats->std_deaths[i],
                act_stats->avg_reneges[i], act_stats->std_reneges[i],
                act_stats->avg_delay[i], act_stats->std_delay[i],
                act_stats->avg_in_node[i], act_stats->std_in_node[i]);
    }

    fclose(f_wl);
    fclose(f_ob);
    fclose(f_act);
    fclose(f_tr);
}
