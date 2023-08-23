#include "headers/io.h"

static void waitingListLean(waiting_list_stats *wl_stats, char *policy){
    FILE *f_wl;
    int i,j;
    char path[MAX_LEN];

    char *format = FORMAT("WAITING_LIST", "Reneged");

    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        for (j = 0; j < NUM_PRIORITIES; ++j) {
            snprintf(path, MAX_LEN, "output/lean/waiting_list_%s_%u_%u.txt", policy, (PRIORITY)j, (BLOOD_TYPE)i);
            OPEN_FILE(f_wl, path)
            fprintf(f_wl, format,
                    wl_stats->avg_arrivals[i][j],
                    wl_stats->avg_served[i][j],
                    wl_stats->avg_reneges[i][j] + wl_stats->avg_deaths[i][j],
                    wl_stats->avg_wait[i][j],
                    wl_stats->avg_delay[i][j],
                    wl_stats->avg_service[i][j]);
            fclose(f_wl);
        }
    }
}

static void organBankLean(organ_bank_stats *ob_stats, char *policy){
    FILE *f_ob;
    char path[MAX_LEN];
    int i;

    char* format = FORMAT("ORGAN_BANK", "Reneged");

    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        snprintf(path, MAX_LEN, "output/lean/organs_%s_%u.txt", policy, (BLOOD_TYPE)i);
        OPEN_FILE(f_ob, path)
        fprintf(f_ob, format,
                ob_stats->avg_arrivals[i][deceased],
                ob_stats->avg_completions[i][deceased],
                ob_stats->avg_outdatings[i],
                ob_stats->avg_wait[i][deceased],
                ob_stats->avg_delay[i][deceased],
                ob_stats->avg_service[i][deceased]);
        fclose(f_ob);
    }
}

static void transplantLean(transplant_stats *t_stats, char *policy){
    FILE *f_tr;
    char path[MAX_LEN];
    int i,j;

    char *format = FORMAT("TRANSPLANT_CENTER", "Rejections");

    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        for (j = 0; j < NUM_PRIORITIES; ++j) {
            snprintf(path, MAX_LEN, "output/lean/transplant_%s_%u.txt", policy, (BLOOD_TYPE)i);
            OPEN_FILE(f_tr, path)
            fprintf(f_tr, format,
                    t_stats->avg_arrivals[i][j],
                    t_stats->avg_transplanted[i][j],
                    t_stats->avg_rejected[i][j],
                    t_stats->avg_wait[i][j],
                    t_stats->avg_delay[i][j],
                    t_stats->avg_service[i][j]);
            fclose(f_tr);
        }
    }
}

static void activationLean(activation_stats *a_stats, char *policy){
    FILE *f_act;
    char path[MAX_LEN];
    int i;

    char* format = FORMAT("ACTIVATION_CENTER", "Reneged");

    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        snprintf(path, MAX_LEN, "output/lean/activation_%s_%u.txt", policy, (BLOOD_TYPE)i);
        OPEN_FILE(f_act, path)
        fprintf(f_act, format,
                a_stats->avg_arrivals[i],
                a_stats->avg_activated[i],
                a_stats->avg_reneges[i] + a_stats->avg_deaths[i],
                a_stats->avg_wait[i],
                a_stats->avg_delay[i],
                a_stats->avg_service[i]);
        fclose(f_act);
    }
}

void saveResultsLean(stats *statistics) {
    char *policy;

#ifdef IMPROVEMENT
    policy = "incomp";
#else
#ifdef ABO_ID
    policy = "id";
#else
    policy = "comp";
#endif
#endif

    waitingListLean(statistics->wl_stats, policy);
    organBankLean(statistics->ob_stats, policy);
    transplantLean(statistics->trans_stats, policy);
    activationLean(statistics->act_stats, policy);
}

static void waitingListResults(waiting_list_stats *wl_stats, char *path){
    int i,j;
    FILE *f_wl;
    OPEN_FILE(f_wl, path)
    char *header = "Blood type,Priority,"
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

    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
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
        }
    }
}

static void organBankResults(organ_bank_stats *ob_stats, char *path){
    FILE *f_ob;
    OPEN_FILE(f_ob, path)
    char *header = "Blood type,"
                   "(Avg) Deceased donor organs arrived,CI deceased donor arrivals,"
                   "(Avg) Living donor organs arrived,CI living donor arrivals,"
                   "(Avg) Organs outdated,CI outdatings,"
                   "Organs in queue,"
                   "Avg inter-arrival times,CI inter-arrival times,"
                   "Avg # in the queue,CI # in the queue\n";
    fprintf(f_ob, "%s", header);

    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        fprintf(f_ob, "%s,%f,%f,%f,%f,%f,+/-%f,+/-%f,+/-%f,+/-%f,%f,+/-%f\n", bt_to_str[i],
                ob_stats->avg_arrivals[i][deceased], ob_stats->std_arrivals[i][deceased],
                ob_stats->avg_arrivals[i][living], ob_stats->std_arrivals[i][living],
                ob_stats->avg_outdatings[i],ob_stats->std_outdatings[i],
                ob_stats->num_organs_in_queue[i],
                ob_stats->avg_interarrival_time[i], ob_stats->std_interarrival_time[i],
                ob_stats->avg_in_queue[i], ob_stats->std_in_queue[i]);
    }
}

static void transplantResults(transplant_stats *trans_stats, char *path){
    FILE *f_tr;
    int i,j;

    OPEN_FILE(f_tr, path)
    char *header = "Blood type,Priority,"
                   "Successful transplant,Rejected transplant,Rejection percentage,"
                   "Avg # in the node,CI # in the node\n";
    fprintf(f_tr, "%s", header);

    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        for (j = 0; j < NUM_PRIORITIES; ++j) {
            fprintf(f_tr, "%s,%s,%f,%f,%f,%f,+/-%f\n", bt_to_str[i], pr_to_str[j],
                    trans_stats->num_transplanted[i][j], trans_stats->num_rejected[i][j],
                    trans_stats->rejection_perc[i][j], trans_stats->avg_in_node, trans_stats->std_in_node);
        }
    }
}

static void activationResults(activation_stats *act_stats, char *path){
    FILE *f_act;

    OPEN_FILE(f_act, path)
    char *header = "(Avg) Patients arrived,CI arrivals,"
                   "(Avg) Patients activated,CI activations,"
                   "(Avg) Patients dead,CI deaths,"
                   "(Avg) Patients reneged,CI reneges,"
                   "Avg delay,CI delay,"
                   "Avg # in the node,CI # in the node\n";
    fprintf(f_act, "%s", header);

    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        fprintf(f_act, "%f,+/-%f,%f,+/-%f,%f,+/-%f,%f,+/-%f,%f,+/-%f,%f,+/-%f\n",
                act_stats->avg_arrivals[i], act_stats->std_arrivals[i],
                act_stats->avg_activated[i], act_stats->std_activated[i],
                act_stats->avg_deaths[i], act_stats->std_deaths[i],
                act_stats->avg_reneges[i], act_stats->std_reneges[i],
                act_stats->avg_delay[i], act_stats->std_delay[i],
                act_stats->avg_in_node[i], act_stats->std_in_node[i]);
    }
}

void saveResultsCsv(int iter, stats *statistics, bool batch, int batch_num) {
    char path_wl[MAX_LEN], path_ob[MAX_LEN], path_t[MAX_LEN], path_a[MAX_LEN];
    char *policy;

#ifdef ABO_ID
    policy = "id";
#else
    policy = "comp";
#endif

#ifdef IMPROVEMENT
    policy = "incomp";
#endif

    if (batch) {
        snprintf(path_a, MAX_LEN, "output/batch/activation/%s/activation_%s_%d.csv", policy, policy, batch_num);
        snprintf(path_ob, MAX_LEN, "output/batch/transplant/%s/transplant_%s_%d.csv", policy, policy, batch_num);
        snprintf(path_t, MAX_LEN, "output/batch/organs/%s/organs_%s_%d.csv", policy, policy, batch_num);
        snprintf(path_wl, MAX_LEN, "output/batch/waiting_list/%s/waiting_list_%s_%d.csv", policy, policy, batch_num);
    } else {
        snprintf(path_a, MAX_LEN, "output/%d_activation_%s.csv", iter, policy);
        snprintf(path_ob, MAX_LEN, "output/%d_transplant_%s.csv", iter, policy);
        snprintf(path_t, MAX_LEN, "output/%d_organs_%s.csv", iter, policy);
        snprintf(path_wl, MAX_LEN, "output/%d_waiting_list_%s.csv", iter, policy);
    }

    waitingListResults(statistics->wl_stats, path_wl);
    organBankResults(statistics->ob_stats, path_ob);
    transplantResults(statistics->trans_stats, path_t);
    activationResults(statistics->act_stats, path_a);
}
