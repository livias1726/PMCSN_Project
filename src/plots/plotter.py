import os

import matplotlib.pyplot as plt
from enum import Enum
import pandas as pd
from scipy.interpolate import make_interp_spline
import numpy as np
import fnmatch

# centers:
#   - 1: waiting list
#   - 2: transplant
#   - 3: activation
#   - 4: organs

FONT_LABEL = {'color': 'black', 'size': 14}
FONT_TITLE = {'color': 'black', 'size': 26}
FONT_NUM = 14

class Policy(Enum):
    ID = 1
    COMP = 2
    INCOMP = 3


class Center(Enum):
    WAITING_LIST = 1
    TRANSPLANT = 2
    ACTIVATION = 3
    ORGANS = 4


def handle_wl_plot(num_batches, base, policy):
    arrivals = {"O": {"c": [], "n": []}, "A": {"c": [], "n": []}, "B": {"c": [], "n": []}, "AB": {"c": [], "n": []}}
    delay = {"O": {"c": [], "n": []}, "A": {"c": [], "n": []}, "B": {"c": [], "n": []}, "AB": {"c": [], "n": []}}
    loss_prob = {"O": {"c": [], "n": []}, "A": {"c": [], "n": []}, "B": {"c": [], "n": []}, "AB": {"c": [], "n": []}}
    num_center = {"O": {"c": [], "n": []}, "A": {"c": [], "n": []}, "B": {"c": [], "n": []}, "AB": {"c": [], "n": []}}
    bt_keys = ["O", "A", "B", "AB"]
    pr_keys = ["c", "n"]
    for i in range(0, num_batches):
        filename = base + "{}.csv".format(i)
        df = pd.read_csv(filename)
        idx = 0
        for b in bt_keys:
            for p in pr_keys:
                arrivals.get(b).get(p).append(df.at[idx, "(Avg) Patients arrived"])
                delay.get(b).get(p).append(df.at[idx, "Avg delay"])
                num_center.get(b).get(p).append(df.at[idx, "Avg # in the queue"])
                if df.at[idx, "(Avg) Patients arrived"] != 0:
                    loss_prob.get(b).get(p).append(
                        (df.at[idx, "(Avg) Patients dead"] + df.at[idx, "(Avg) Patients reneged"]) / df.at[idx, "(Avg) Patients arrived"])
                else:
                    loss_prob.get(b).get(p).append(0)
                idx += 1
    stats = {"ARRIVALS": arrivals, "DELAY": delay, "PROB. LOSS": loss_prob, "AVG. IN NODE": num_center}

    # todo: modificare grafico non mi piace come sono rappresentate le linee (sono troppe!)
    for key, value in stats.items():
        idx = range(num_batches)
        for bt, v in value.items():
            for pr, v_pr in v.items():
                x_new = np.linspace(min(idx), max(idx), 300)
                spl = make_interp_spline(idx, v_pr, k=3)
                y = spl(x_new)
                plt.plot(x_new, y, label=bt + "-" + pr)
        plt.ylabel(key.lower(), fontdict=FONT_LABEL)
        plt.xlabel("Batch", fontdict=FONT_LABEL)
        plt.title(key, fontdict=FONT_TITLE)
        plt.legend()
        plt.savefig("output/plot/{}/waiting_list_{}.png".format(policy.name.lower(), key.lower()))
        plt.clf()


def handle_trans_plot(num_batches, base, policy):
    rejection_perc = {"O": {"c": [], "n": []}, "A": {"c": [], "n": []}, "B": {"c": [], "n": []},
                      "AB": {"c": [], "n": []}}
    num_center = {"O": {"c": [], "n": []}, "A": {"c": [], "n": []}, "B": {"c": [], "n": []}, "AB": {"c": [], "n": []}}
    bt_keys = ["O", "A", "B", "AB"]
    pr_keys = ["c", "n"]
    for i in range(0, num_batches):
        filename = base + "{}.csv".format(i)
        df = pd.read_csv(filename)
        idx = 0
        for b in bt_keys:
            for p in pr_keys:
                rejection_perc.get(b).get(p).append(df.at[idx, "Rejection percentage"])
                num_center.get(b).get(p).append(df.at[idx, "Avg # in the node"])
                idx += 1

    stats = {"REJECT. PERC": rejection_perc, "AVG. IN NODE": num_center}

    # todo: modificare grafico non mi piace come sono rappresentate le linee (sono troppe!)- zoom?
    for key, value in stats.items():
        idx = range(num_batches)
        for bt, v in value.items():
            for pr, v_pr in v.items():
                x_new = np.linspace(min(idx), max(idx), 300)
                spl = make_interp_spline(idx, v_pr, k=3)
                y = spl(x_new)
                plt.plot(x_new, y, label=bt + "-" + pr)
        plt.ylabel(key.lower(), fontdict=FONT_LABEL)
        plt.xlabel("Batch", fontdict=FONT_LABEL)
        plt.title(key, fontdict=FONT_TITLE)
        plt.legend()
        plt.savefig("output/plot/{}/transplant_{}.png".format(policy.name.lower(), policy.name.lower(), key.lower()))
        plt.clf()


def handle_activ_plot(num_batches, base, policy):
    arrivals = []
    activations = []
    num_center = []
    delay = []
    prob_loss = []
    for i in range(0, num_batches):
        filename = base + "{}.csv".format(i)
        df = pd.read_csv(filename)
        arrivals.append(df.at[0, "(Avg) Patients arrived"])
        activations.append(df.at[0, "(Avg) Patients activated"])
        prob_loss.append((df.at[0, "(Avg) Patients dead"] + df.at[0, "(Avg) Patients reneged"]) / df.at[0, "(Avg) Patients arrived"])
        num_center.append(df.at[0, "Avg # in the node"])
        delay.append(df.at[0, "Avg delay"])
    stats = {"ARRIVALS": arrivals, "ACTIVATIONS": activations, "AVG. IN NODE": num_center, "DELAY": delay,
             "PROB. LOSS": prob_loss}

    for key, value in stats.items():
        # create integers from strings
        idx = range(num_batches)
        x_new = np.linspace(min(idx), max(idx), 300)
        spl = make_interp_spline(idx, value, k=3)
        y = spl(x_new)
        plt.plot(y, color='r')
        plt.xticks(fontsize=FONT_NUM)
        plt.yticks(fontsize=FONT_NUM)
        plt.ylabel(key.lower(), fontdict=FONT_LABEL)
        plt.xlabel("Batch", fontdict=FONT_LABEL)
        plt.title(key, fontdict=FONT_TITLE)
        plt.savefig("output/plot/{}/activation_{}.png".format(policy.name.lower(), key.lower()))
        plt.clf()


def handle_organs_plot(num_batches, base, policy):
    arrivals = {"O": {"d": [], "l": []}, "A": {"d": [], "l": []}, "B": {"d": [], "l": []}, "AB": {"d": [], "l": []}}
    loss_prob = {"O": [], "A": [], "B": [], "AB": []}
    num_center = {"O": [], "A": [], "B": [], "AB": []}
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

        num_center.get("O").append(df.at[0, "Avg # in the queue"])
        num_center.get("A").append(df.at[1, "Avg # in the queue"])
        num_center.get("B").append(df.at[2, "Avg # in the queue"])
        num_center.get("AB").append(df.at[3, "Avg # in the queue"])

        loss_prob.get("O").append(df.at[0, "(Avg) Organs outdated"] / df.at[0, "(Avg) Deceased donor organs arrived"])
        loss_prob.get("A").append(df.at[1, "(Avg) Organs outdated"] / df.at[1, "(Avg) Deceased donor organs arrived"])
        loss_prob.get("B").append(df.at[2, "(Avg) Organs outdated"] / df.at[2, "(Avg) Deceased donor organs arrived"])
        loss_prob.get("AB").append(df.at[3, "(Avg) Organs outdated"] / df.at[3, "(Avg) Deceased donor organs arrived"])
    stats = {"ARRIVALS": arrivals, "AVG. IN NODE": num_center, "PROB. LOSS": loss_prob}

    for key, value in stats.items():
        idx = range(num_batches)
        if key == "ARRIVALS":
            for bt, v in value.items():
                for t, v_type in v.items():
                    x_new = np.linspace(min(idx), max(idx), 300)
                    spl = make_interp_spline(idx, v_type, k=3)
                    y = spl(x_new)
                    plt.plot(x_new, y, label=bt + "-" + t)
                    plt.ylabel(key.lower(), fontdict=FONT_LABEL)
                    plt.xlabel("Batch", fontdict=FONT_LABEL)
                    plt.title(key, fontdict=FONT_TITLE)
                    plt.legend()
                    plt.savefig("output/plot/{}/organs_{}_{}.png".format(policy.name.lower(), key.lower(), t))
                    plt.clf()
        else:
            for bt, v in value.items():
                x_new = np.linspace(min(idx), max(idx), 300)
                spl = make_interp_spline(idx, v, k=3)
                y = spl(x_new)
                plt.plot(x_new, y, label=bt)
            plt.ylabel(key.lower(), fontdict=FONT_LABEL)
            plt.xlabel("Batch", fontdict=FONT_LABEL)
            plt.title(key, fontdict=FONT_TITLE)
            plt.legend()
            plt.savefig("output/plot/{}/organs_{}.png".format(policy.name.lower(), key.lower()))
            plt.clf()


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
