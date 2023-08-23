import os

import matplotlib.pyplot as plt
from enum import Enum
import pandas as pd
import numpy as np
import fnmatch


# centers:
#   - 1: waiting list
#   - 2: transplant
#   - 3: activation
#   - 4: organs


class Policy(Enum):
    ID = 1
    COMP = 2
    INCOMP = 3


class Center(Enum):
    WAITING_LIST = 1
    TRANSPLANT = 2
    ACTIVATION = 3
    ORGANS = 4


def plot_delay(policy, center, idx, key, value):
    i = 0
    j = 0
    if center == "waiting_list":
        fig, axs = plt.subplots(ncols=2, nrows=2, layout="constrained")
        fig.suptitle("Avg. delay in waiting list")

        for bt, v in value.items():
            # create a plot for each blood type
            c = v.get("c")
            n = v.get("n")
            means = []
            for m in range(0, len(c)):
                means.append((c[m] + n[m]) / 2)

            axs[i, j].plot(idx, means)
            axs[i, j].set_xlabel("batch")
            axs[i, j].set_ylabel("delay")
            axs[i, j].autoscale()
            axs[i, j].set_title("blood type {}".format(bt))

            if i == 0 and j == 0:
                j += 1
            elif i == 0 and j == 1:
                i += 1
                j = 0
            else:
                i = 1
                j = 1

        plt.savefig("output/plot/{}/waiting_list_{}.png".format(policy.name.lower(), key.lower()))
        plt.clf()
    elif center == "activation":
        fig, axs = plt.subplots(ncols=4, nrows=1, layout="constrained", figsize=(10, 5), dpi=100)
        fig.suptitle("Avg. delay in activation center")

        for bt, v in value.items():
            # create a plot for each blood type
            vals = v.get("v")
            s_vals = [v.get("steady")[0] for _ in vals]
            axs[j].plot(idx, vals)
            l, = axs[j].plot(idx, s_vals, color="black")
            l.set_linestyle(":")
            ticks = axs[j].get_yticks()
            new_ticks = np.append(ticks, s_vals[0])
            axs[j].set_yticks(new_ticks)
            axs[j].set_xlabel("batch")
            axs[j].set_ylabel("delay")
            axs[j].autoscale()
            axs[j].set_title("blood type {}".format(bt))

            j += 1

        plt.savefig("output/plot/{}/activation_{}.png".format(policy.name.lower(), key.lower()))
        plt.clf()


def plot_arrivals(policy, center, idx, key, value):
    j = 0
    i = 0
    if center == "waiting_list":
        fig, axs = plt.subplots(ncols=4, nrows=2, layout="constrained", figsize=(25, 10), dpi=100)
        fig.suptitle("Avg. arrivals in waiting list")

        for bt, v in value.items():
            # create a plot for each blood type
            c = v.get("c")
            n = v.get("n")
            s_c = [v.get("steady")[0] for _ in c]
            s_n = [v.get("steady")[1] for _ in n]

            # critical
            axs[0, j].plot(idx, c)
            l, = axs[0, j].plot(idx, s_c, color="black")
            l.set_linestyle(":")
            ticks = axs[0, j].get_yticks()
            new_ticks = np.append(ticks, s_c[0])
            axs[0, j].set_yticks(new_ticks)
            axs[0, j].set_xlabel("batch")
            axs[0, j].set_ylabel("arrivals (critical)")
            axs[0, j].autoscale()
            axs[0, j].set_title("blood type {}".format(bt))

            # normal
            axs[1, j].plot(idx, n)
            l, = axs[1, j].plot(idx, s_n, color="black")
            l.set_linestyle(":")
            ticks = axs[1, j].get_yticks()
            new_ticks = np.append(ticks, s_n[0])
            axs[1, j].set_yticks(new_ticks)
            axs[1, j].set_xlabel("batch")
            axs[1, j].set_ylabel("arrivals (normal)")
            axs[1, j].autoscale()
            axs[1, j].set_title("blood type {}".format(bt))

            j += 1

        plt.savefig("output/plot/{}/waiting_list_{}.png".format(policy.name.lower(), key.lower()))
        plt.clf()
    elif center == "activation":
        fig, axs = plt.subplots(ncols=4, nrows=1, layout="constrained", figsize=(10, 5), dpi=100)
        fig.suptitle("Avg. arrivals in activation center")

        for bt, v in value.items():
            # create a plot for each blood type
            vals = v.get("v")
            s_vals = [v.get("steady")[0] for _ in vals]
            axs[j].plot(idx, vals)
            l, = axs[j].plot(idx, s_vals, color="black")
            l.set_linestyle(":")
            ticks = axs[j].get_yticks()
            new_ticks = np.append(ticks, s_vals[0])
            axs[j].set_yticks(new_ticks)
            axs[j].set_xlabel("batch")
            axs[j].set_ylabel("arrivals")
            axs[j].autoscale()
            axs[j].set_title("blood type {}".format(bt))

            j += 1

        plt.savefig("output/plot/{}/activation_{}.png".format(policy.name.lower(), key.lower()))
        plt.clf()
    elif center == "organs":
        fig, axs = plt.subplots(ncols=4, nrows=2, layout="constrained", figsize=(25, 10), dpi=100)
        fig.suptitle("Avg. arrivals in organ bank center")

        for bt, v in value.items():
            # create a plot for each blood type
            d = v.get("d")
            lv = v.get("l")
            s_d = [v.get("steady")[0] for _ in d]
            s_l = [v.get("steady")[1] for _ in lv]

            # deceased
            axs[0, j].plot(idx, d)
            l, = axs[0, j].plot(idx, s_d, color="black")
            l.set_linestyle(":")
            ticks = axs[0, j].get_yticks()
            new_ticks = np.append(ticks, s_d[0])
            axs[0, j].set_yticks(new_ticks)
            axs[0, j].set_xlabel("batch")
            axs[0, j].set_ylabel("arrivals (critical)")
            axs[0, j].autoscale()
            axs[0, j].set_title("blood type {}".format(bt))

            # living
            axs[1, j].plot(idx, lv)
            l, = axs[1, j].plot(idx, s_l, color="black")
            l.set_linestyle(":")
            ticks = axs[1, j].get_yticks()
            new_ticks = np.append(ticks, s_l[0])
            axs[1, j].set_yticks(new_ticks)
            axs[1, j].set_xlabel("batch")
            axs[1, j].set_ylabel("arrivals (normal)")
            axs[1, j].autoscale()
            axs[1, j].set_title("blood type {}".format(bt))

            j += 1

        plt.savefig("output/plot/{}/organs_{}.png".format(policy.name.lower(), key.lower()))
        plt.clf()


def plot_prob_loss(policy, center, idx, key, value):
    j = 0
    if center == "waiting_list":
        fig, axs = plt.subplots(ncols=4, nrows=2, layout="constrained", figsize=(25, 10), dpi=100)
        fig.suptitle("Prob. loss in waiting list")

        for bt, v in value.items():
            # create a plot for each blood type
            c = v.get("c")
            n = v.get("n")
            s_c = [v.get("steady")[0] for _ in c]
            s_n = [v.get("steady")[1] for _ in n]

            # critical
            axs[0, j].plot(idx, c)
            l, = axs[0, j].plot(idx, s_c, color="black")
            l.set_linestyle(":")
            ticks = axs[0, j].get_yticks()
            new_ticks = np.append(ticks, s_c[0])
            axs[0, j].set_yticks(new_ticks)
            axs[0, j].set_xlabel("batch")
            axs[0, j].set_ylabel("prob. loss (critical)")
            axs[0, j].autoscale()
            axs[0, j].set_title("blood type {}".format(bt))

            # normal
            axs[1, j].plot(idx, n)
            l, = axs[1, j].plot(idx, s_n, color="black")
            l.set_linestyle(":")
            ticks = axs[1, j].get_yticks()
            new_ticks = np.append(ticks, s_n[0])
            axs[1, j].set_yticks(new_ticks)
            axs[1, j].set_xlabel("batch")
            axs[1, j].set_ylabel("prob. loss (normal)")
            axs[1, j].autoscale()
            axs[1, j].set_title("blood type {}".format(bt))

            j += 1

        plt.savefig("output/plot/{}/waiting_list_{}.png".format(policy.name.lower(), key.lower()))
        plt.clf()
    elif center == "activation":
        fig, axs = plt.subplots(ncols=4, nrows=1, layout="constrained", figsize=(10, 5), dpi=100)
        fig.suptitle("Avg. prob. loss in activation center")

        for bt, v in value.items():
            # create a plot for each blood type
            vals = v.get("v")
            s_vals = [v.get("steady")[0] for _ in vals]
            axs[j].plot(idx, vals)
            l, = axs[j].plot(idx, s_vals, color="black")
            l.set_linestyle(":")
            ticks = axs[j].get_yticks()
            new_ticks = np.append(ticks, s_vals[0])
            axs[j].set_yticks(new_ticks)
            axs[j].set_xlabel("batch")
            axs[j].set_ylabel("prob. loss")
            axs[j].autoscale()
            axs[j].set_title("blood type {}".format(bt))

            j += 1

        plt.savefig("output/plot/{}/activation_{}.png".format(policy.name.lower(), key.lower()))
        plt.clf()
    elif center == "organs":
        fig, axs = plt.subplots(ncols=4, nrows=1, layout="constrained", figsize=(10, 5), dpi=100)
        fig.suptitle("Avg. prob. loss in organ bank")

        for bt, v in value.items():
            # create a plot for each blood type
            vals = v.get("v")
            s_vals = [v.get("steady")[0] for _ in vals]
            axs[j].plot(idx, vals)
            l, = axs[j].plot(idx, s_vals, color="black")
            l.set_linestyle(":")
            ticks = axs[j].get_yticks()
            new_ticks = np.append(ticks, s_vals[0])
            axs[j].set_yticks(new_ticks)
            axs[j].set_xlabel("batch")
            axs[j].set_ylabel("prob. loss")
            axs[j].autoscale()
            axs[j].set_title("blood type {}".format(bt))

            j += 1

        plt.savefig("output/plot/{}/organs_{}.png".format(policy.name.lower(), key.lower()))
        plt.clf()


def plot_inter_arr(policy, center, idx, key, value):
    j = 0
    if center == "waiting_list":
        fig, axs = plt.subplots(ncols=4, nrows=2, layout="constrained", figsize=(25, 10), dpi=100)
        fig.suptitle("Avg. inter-arrival times in waiting list")

        for bt, v in value.items():
            # create a plot for each blood type
            c = v.get("c")
            n = v.get("n")
            s_c = [v.get("steady")[0] for _ in c]
            s_n = [v.get("steady")[1] for _ in n]

            # critical
            axs[0, j].plot(idx, c)
            l, = axs[0, j].plot(idx, s_c, color="black")
            l.set_linestyle(":")
            ticks = axs[0, j].get_yticks()
            new_ticks = np.append(ticks, s_c[0])
            axs[0, j].set_yticks(new_ticks)
            axs[0, j].set_xlabel("batch")
            axs[0, j].set_ylabel("int. arrival time (critical)")
            axs[0, j].autoscale()
            axs[0, j].set_title("blood type {}".format(bt))

            # normal
            axs[1, j].plot(idx, n)
            l, = axs[1, j].plot(idx, s_n, color="black")
            l.set_linestyle(":")
            ticks = axs[1, j].get_yticks()
            new_ticks = np.append(ticks, s_n[0])
            axs[1, j].set_yticks(new_ticks)
            axs[1, j].set_xlabel("batch")
            axs[1, j].set_ylabel("int. arrival time (normal)")
            axs[1, j].autoscale()
            axs[1, j].set_title("blood type {}".format(bt))

            j += 1

        plt.savefig("output/plot/{}/waiting_list_{}.png".format(policy.name.lower(), key.lower()))
        plt.clf()
    elif center == "activation":
        fig, axs = plt.subplots(ncols=4, nrows=1, layout="constrained", figsize=(10, 5), dpi=100)
        fig.suptitle("Avg. inter-arrival times in activation center")

        for bt, v in value.items():
            # create a plot for each blood type
            vals = v.get("v")
            s_vals = [v.get("steady")[0] for _ in vals]
            axs[j].plot(idx, vals)
            l, = axs[j].plot(idx, s_vals, color="black")
            l.set_linestyle(":")
            ticks = axs[j].get_yticks()
            new_ticks = np.append(ticks, s_vals[0])
            axs[j].set_yticks(new_ticks)
            axs[j].set_xlabel("batch")
            axs[j].set_ylabel("inter-arrival times")
            axs[j].autoscale()
            axs[j].set_title("blood type {}".format(bt))

            j += 1

        plt.savefig("output/plot/{}/activation_{}.png".format(policy.name.lower(), key.lower()))
        plt.clf()
    elif center == "organs":
        fig, axs = plt.subplots(ncols=4, nrows=1, layout="constrained", figsize=(10, 5), dpi=100)
        fig.suptitle("Avg. inter-arrival times in organ bank")

        for bt, v in value.items():
            # create a plot for each blood type
            vals = v.get("v")
            s_vals = [v.get("steady")[0] for _ in vals]
            axs[j].plot(idx, vals)
            l, = axs[j].plot(idx, s_vals, color="black")
            l.set_linestyle(":")
            ticks = axs[j].get_yticks()
            new_ticks = np.append(ticks, s_vals[0])
            axs[j].set_yticks(new_ticks)
            axs[j].set_xlabel("batch")
            axs[j].set_ylabel("inter-arrival times")
            axs[j].autoscale()
            axs[j].set_title("blood type {}".format(bt))

            j += 1

        plt.savefig("output/plot/{}/organs_{}.png".format(policy.name.lower(), key.lower()))
        plt.clf()


def plot_node(policy, center, idx, key, value):
    j = 0
    if center == "activation":
        fig, axs = plt.subplots(ncols=4, nrows=1, layout="constrained", figsize=(10, 5), dpi=100)
        fig.suptitle("Avg. number in node activation center")

        for bt, v in value.items():
            # create a plot for each blood type
            vals = v.get("v")
            s_vals = [v.get("steady")[0] for _ in vals]
            axs[j].plot(idx, vals)
            l, = axs[j].plot(idx, s_vals, color="black")
            l.set_linestyle(":")
            ticks = axs[j].get_yticks()
            new_ticks = np.append(ticks, s_vals[0])
            axs[j].set_yticks(new_ticks)
            axs[j].set_xlabel("batch")
            axs[j].set_ylabel("avg. num of jobs")
            axs[j].autoscale()
            axs[j].set_title("blood type {}".format(bt))

            j += 1

        plt.savefig("output/plot/{}/activation_{}.png".format(policy.name.lower(), key.lower()))
        plt.clf()
    elif center == "organs":
        fig, axs = plt.subplots(ncols=4, nrows=1, layout="constrained", figsize=(10, 5), dpi=100)
        fig.suptitle("Avg. number in organ bank center")

        for bt, v in value.items():
            # create a plot for each blood type
            vals = v.get("v")
            s_vals = [v.get("steady")[0] for _ in vals]
            axs[j].plot(idx, vals)
            l, = axs[j].plot(idx, s_vals, color="black")
            l.set_linestyle(":")
            ticks = axs[j].get_yticks()
            new_ticks = np.append(ticks, s_vals[0])
            axs[j].set_yticks(new_ticks)
            axs[j].set_xlabel("batch")
            axs[j].set_ylabel("avg. num of jobs")
            axs[j].autoscale()
            axs[j].set_title("blood type {}".format(bt))

            j += 1

        plt.savefig("output/plot/{}/organs_{}.png".format(policy.name.lower(), key.lower()))
        plt.clf()
    if center == "transplant":
        fig, axs = plt.subplots(ncols=4, nrows=2, layout="constrained", figsize=(25, 10), dpi=100)
        fig.suptitle("Avg. number of jobs in transplant center")

        for bt, v in value.items():
            # create a plot for each blood type
            c = v.get("c")
            n = v.get("n")
            s_c = [v.get("steady")[0] for _ in c]
            s_n = [v.get("steady")[1] for _ in n]

            # critical
            axs[0, j].plot(idx, c)
            l, = axs[0, j].plot(idx, s_c, color="black")
            l.set_linestyle(":")
            ticks = axs[0, j].get_yticks()
            new_ticks = np.append(ticks, s_c[0])
            axs[0, j].set_yticks(new_ticks)
            axs[0, j].set_xlabel("batch")
            axs[0, j].set_ylabel("avg. num of jobs (critical)")
            axs[0, j].autoscale()
            axs[0, j].set_title("blood type {}".format(bt))

            # normal
            axs[1, j].plot(idx, n)
            l, = axs[1, j].plot(idx, s_n, color="black")
            l.set_linestyle(":")
            ticks = axs[1, j].get_yticks()
            new_ticks = np.append(ticks, s_n[0])
            axs[1, j].set_yticks(new_ticks)
            axs[1, j].set_xlabel("batch")
            axs[1, j].set_ylabel("avg. num. of jobs (normal)")
            axs[1, j].autoscale()
            axs[1, j].set_title("blood type {}".format(bt))

            j += 1

        plt.savefig("output/plot/{}/transplant_{}.png".format(policy.name.lower(), key.lower()))
        plt.clf()


def plot_reject(policy, center, idx, key, value):
    j = 0
    if center == "transplant":
        fig, axs = plt.subplots(ncols=4, nrows=2, layout="constrained", figsize=(25, 10), dpi=100)
        fig.suptitle("Rejection probability in transplant")

        for bt, v in value.items():
            # create a plot for each blood type
            c = v.get("c")
            n = v.get("n")
            s_c = [v.get("steady")[0] for _ in c]
            s_n = [v.get("steady")[1] for _ in n]

            # critical
            axs[0, j].plot(idx, c)
            l, = axs[0, j].plot(idx, s_c, color="black")
            l.set_linestyle(":")
            ticks = axs[0, j].get_yticks()
            new_ticks = np.append(ticks, s_c[0])
            axs[0, j].set_yticks(new_ticks)
            axs[0, j].set_xlabel("batch")
            axs[0, j].set_ylabel("int. arrival time (critical)")
            axs[0, j].autoscale()
            axs[0, j].set_title("blood type {}".format(bt))

            # normal
            axs[1, j].plot(idx, n)
            l, = axs[1, j].plot(idx, s_n, color="black")
            l.set_linestyle(":")
            ticks = axs[1, j].get_yticks()
            new_ticks = np.append(ticks, s_n[0])
            axs[1, j].set_yticks(new_ticks)
            axs[1, j].set_xlabel("batch")
            axs[1, j].set_ylabel("int. arrival time (normal)")
            axs[1, j].autoscale()
            axs[1, j].set_title("blood type {}".format(bt))

            j += 1

        plt.savefig("output/plot/{}/transplant_{}.png".format(policy.name.lower(), key.lower()))
        plt.clf()


def handle_wl_plot(num_batches, base, policy):
    arrivals = {"O": {"c": [], "n": [], "steady": [17.90, 12975.9]},
                "A": {"c": [], "n": [], "steady": [10, 8612.8]},
                "B": {"c": [], "n": [], "steady": [5, 3934.4]},
                "AB": {"c": [], "n": [], "steady": [1.5, 992]}}
    delay = {"O": {"c": [], "n": [], "steady": [913.13, 913.13]},
             "A": {"c": [], "n": [], "steady": [913.13, 913.13]},
             "B": {"c": [], "n": [], "steady": [913.13, 913.13]},
             "AB": {"c": [], "n": [], "steady": [913.13, 913.13]}}
    loss_prob = {"O": {"c": [], "n": [], "steady": [0, 0]},
                 "A": {"c": [], "n": [], "steady": [0, 0]},
                 "B": {"c": [], "n": [], "steady": [0, 0]},
                 "AB": {"c": [], "n": [], "steady": [0, 0]}}
    int_arr = {"O": {"c": [], "n": [], "steady": [20.391061, 0.028129]},
               "A": {"c": [], "n": [], "steady": [36.500000, 0.042379]},
               "B": {"c": [], "n": [], "steady": [73.000000, 0.092771]},
               "AB": {"c": [], "n": [], "steady": [243.333333, 0.367944]}}
    # num_center = {"O": {"c": [], "n": []}, "A": {"c": [], "n": []}, "B": {"c": [], "n": []}, "AB": {"c": [], "n": []}}
    bt_keys = ["O", "A", "B", "AB"]
    pr_keys = ["c", "n"]
    for i in range(0, num_batches):
        filename = base + "{}.csv".format(i)
        df = pd.read_csv(filename)
        idx = 0
        for b in bt_keys:
            for p in pr_keys:
                if p == "steady":
                    continue
                arrivals.get(b).get(p).append(df.at[idx, "(Avg) Patients arrived"])
                int_arr.get(b).get(p).append(df.at[idx, "Avg inter-arrival times"])
                delay.get(b).get(p).append(df.at[idx, "Avg delay"])
                # num_center.get(b).get(p).append(df.at[idx, "Avg # in the queue"])
                if df.at[idx, "(Avg) Patients arrived"] != 0:
                    loss_prob.get(b).get(p).append(
                        (df.at[idx, "(Avg) Patients dead"] + df.at[idx, "(Avg) Patients reneged"]) / df.at[
                            idx, "(Avg) Patients arrived"])
                else:
                    loss_prob.get(b).get(p).append(0)
                idx += 1
    stats = {"ARRIVALS": arrivals, "DELAY": delay, "PROB. LOSS": loss_prob, "AVG. INTERARRIVALS": int_arr}

    idx = range(num_batches)
    center = "waiting_list"
    for key, value in stats.items():
        if key.lower() == "delay":
            plot_delay(policy, center, idx, key, value)
        elif key.lower() == "arrivals":
            plot_arrivals(policy, center, idx, key, value)
        elif key.lower() == "prob. loss":
            plot_prob_loss(policy, center, idx, key, value)
        else:
            plot_inter_arr(policy, center, idx, key, value)


def handle_activ_plot(num_batches, base, policy):
    arrivals = {"O": {"v": [], "steady": [0]},
                "A": {"v": [], "steady": [0]},
                "B": {"v": [], "steady": [0]},
                "AB": {"v": [], "steady": [0]}}
    activations = {"O": {"v": [], "steady": [0]},
                   "A": {"v": [], "steady": [0]},
                   "B": {"v": [], "steady": [0]},
                   "AB": {"v": [], "steady": [0]}}
    num_center = {"O": {"v": [], "steady": [0]},
                  "A": {"v": [], "steady": [0]},
                  "B": {"v": [], "steady": [0]},
                  "AB": {"v": [], "steady": [0]}}
    delay = {"O": {"v": [], "steady": [0]},
             "A": {"v": [], "steady": [0]},
             "B": {"v": [], "steady": [0]},
             "AB": {"v": [], "steady": [0]}}
    prob_loss = {"O": {"v": [], "steady": [0]},
                 "A": {"v": [], "steady": [0]},
                 "B": {"v": [], "steady": [0]},
                 "AB": {"v": [], "steady": [0]}}
    bt_keys = ["O", "A", "B", "AB"]
    pr_keys = ["v", "steady"]

    for i in range(0, num_batches):
        filename = base + "{}.csv".format(i)
        df = pd.read_csv(filename)
        for b in bt_keys:
            for p in pr_keys:
                if p == "steady":
                    continue
                arrivals.get(b).get(p).append(df.at[0, "(Avg) Patients arrived"])
                activations.get(b).get(p).append(df.at[0, "(Avg) Patients activated"])
                (prob_loss.get(b).get(p).append((df.at[0, "(Avg) Patients dead"] + df.at[0, "(Avg) Patients reneged"]) / df.at[0, "(Avg) Patients arrived"]))
                num_center.get(b).get(p).append(df.at[0, "Avg # in the node"])
                delay.get(b).get(p).append(df.at[0, "Avg delay"])
    stats = {"ARRIVALS": arrivals, "ACTIVATIONS": activations, "AVG. IN NODE": num_center, "DELAY": delay,
             "PROB. LOSS": prob_loss}

    idx = range(num_batches)
    center = "activation"
    for key, value in stats.items():
        # create integers from strings
        if key.lower() == "arrivals":
            plot_arrivals(policy, center, idx, key, value)
        elif key.lower() == "activations":
            plot_inter_arr(policy, center, idx, key, value)
        elif key.lower() == "avg. in node":
            plot_node(policy, center, idx, key, value)
        elif key.lower() == "delay":
            plot_delay(policy, center, idx, key, value)
        elif key.lower() == "prob. loss":
            plot_prob_loss(policy, center, idx, key, value)


def handle_trans_plot(num_batches, base, policy):
    rejection_perc = {"O": {"c": [], "n": [], "steady": [0.13, 0.13]},
                      "A": {"c": [], "n": [], "steady": [0.13, 0.13]},
                      "B": {"c": [], "n": [], "steady": [0.13, 0.13]},
                      "AB": {"c": [], "n": [], "steady": [0.13, 0.13]}}
    num_center = {"O": {"c": [], "n": [], "steady": [0, 0]},
                  "A": {"c": [], "n": [], "steady": [0, 0]},
                  "B": {"c": [], "n": [], "steady": [0, 0]},
                  "AB": {"c": [], "n": [], "steady": [0, 0]}}
    bt_keys = ["O", "A", "B", "AB"]
    pr_keys = ["c", "n"]
    for i in range(0, num_batches):
        filename = base + "{}.csv".format(i)
        df = pd.read_csv(filename)
        idx = 0
        for b in bt_keys:
            for p in pr_keys:
                if p == "steady":
                    continue
                rejection_perc.get(b).get(p).append(df.at[idx, "Rejection percentage"])
                num_center.get(b).get(p).append(df.at[idx, "Avg # in the node"])
                idx += 1

    stats = {"REJECT. PERC": rejection_perc, "AVG. IN NODE": num_center}

    idx = range(num_batches)
    center = "transplant"
    for key, value in stats.items():
        if key.lower() == "reject. perc":
            plot_reject(policy, center, idx, key, value)
        elif key.lower() == "avg. in node":
            plot_node(policy, center, idx, key, value)


def handle_organs_plot(num_batches, base, policy):
    arrivals = {"O": {"d": [], "l": [], "steady": [0, 0]},
                "A": {"d": [], "l": [], "steady": [0, 0]},
                "B": {"d": [], "l": [], "steady": [0, 0]},
                "AB": {"d": [], "l": [], "steady": [0, 0]}}
    int_arrival = {"O": {"v": [], "steady": [0.089888]},
                   "A": {"v": [], "steady": [0.115811]},
                   "B": {"v": [], "steady": [0.360067]},
                   "AB": {"v": [], "steady": [1.244460]}}
    loss_prob = {"O": {"v": [], "steady": [0]},
                 "A": {"v": [], "steady": [0]},
                 "B": {"v": [], "steady": [0]},
                 "AB": {"v": [], "steady": [0]}}
    num_center = {"O": {"v": [], "steady": [0]},
                  "A": {"v": [], "steady": [0]},
                  "B": {"v": [], "steady": [0]},
                  "AB": {"v": [], "steady": [0]}}
    for i in range(0, num_batches):
        filename = base + "{}.csv".format(i)
        df = pd.read_csv(filename)
        arrivals.get("O").get("d").append(df.at[0, "(Avg) Deceased donor organs arrived"])
        arrivals.get("A").get("d").append(df.at[1, "(Avg) Deceased donor organs arrived"])
        arrivals.get("B").get("d").append(df.at[2, "(Avg) Deceased donor organs arrived"])
        arrivals.get("AB").get("d").append(df.at[3, "(Avg) Deceased donor organs arrived"])

        arrivals.get("O").get("l").append(df.at[0, "(Avg) Living donor organs arrived"])
        arrivals.get("A").get("l").append(df.at[1, "(Avg) Living donor organs arrived"])
        arrivals.get("B").get("l").append(df.at[2, "(Avg) Living donor organs arrived"])
        arrivals.get("AB").get("l").append(df.at[3, "(Avg) Living donor organs arrived"])

        num_center.get("O").get("v").append(df.at[0, "Avg # in the queue"])
        num_center.get("A").get("v").append(df.at[1, "Avg # in the queue"])
        num_center.get("B").get("v").append(df.at[2, "Avg # in the queue"])
        num_center.get("AB").get("v").append(df.at[3, "Avg # in the queue"])

        loss_prob.get("O").get("v").append(df.at[0, "(Avg) Organs outdated"] / df.at[0, "(Avg) Deceased donor organs arrived"])
        loss_prob.get("A").get("v").append(df.at[1, "(Avg) Organs outdated"] / df.at[1, "(Avg) Deceased donor organs arrived"])
        loss_prob.get("B").get("v").append(df.at[2, "(Avg) Organs outdated"] / df.at[2, "(Avg) Deceased donor organs arrived"])
        loss_prob.get("AB").get("v").append(df.at[3, "(Avg) Organs outdated"] / df.at[3, "(Avg) Deceased donor organs arrived"])

        int_arrival.get("O").get("v").append(df.at[0, "Avg inter-arrival times"])
        int_arrival.get("A").get("v").append(df.at[1, "Avg inter-arrival times"])
        int_arrival.get("B").get("v").append(df.at[2, "Avg inter-arrival times"])
        int_arrival.get("AB").get("v").append(df.at[3, "Avg inter-arrival times"])

    stats = {"ARRIVALS": arrivals, "INT. ARRIVALS": int_arrival, "AVG. IN NODE": num_center, "PROB. LOSS": loss_prob}

    idx = range(num_batches)
    center = "organs"
    for key, value in stats.items():
        if key.lower() == "arrivals":
            plot_arrivals(policy, center, idx, key, value)
        elif key.lower() == "int. arrivals":
            plot_inter_arr(policy, center, idx, key, value)
        elif key.lower() == "prob. loss":
            plot_prob_loss(policy, center, idx, key, value)
        elif key.lower() == "avg. in node":
            plot_node(policy, center, idx, key, value)


def plot_infinite_horizon_sim(center: Center, policy: Policy):
    if center.value < 0 | center.value > 5:
        print(center.value)
        print("Center is not valid...\n")
        return 1

    # count the number of files in directory
    num_batches = len(fnmatch.filter(os.listdir("output/batch/{}/{}".format(center.name.lower(), policy.name.lower())),
                                     '*.*'))

    filename_base = "output/batch/{}/{}/{}_{}_".format(center.name.lower(), policy.name.lower(), center.name.lower(),
                                                       policy.name.lower())

    if center == Center.WAITING_LIST:
        handle_wl_plot(num_batches, filename_base, policy)
    elif center == Center.TRANSPLANT:
        handle_trans_plot(num_batches, filename_base, policy)
    elif center == Center.ACTIVATION:
        handle_activ_plot(num_batches, filename_base, policy)
    elif center == Center.ORGANS:
        handle_organs_plot(num_batches, filename_base, policy)
