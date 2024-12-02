#include <bits/stdc++.h>
using namespace std;

struct Request {
    int id;
    int arrival_time;
    int duration;
    int max_waiting_time;
    int resources[3];
    int profit;
    double priority_ratio;
    bool admitted;
};

bool comparePriority(const Request& a, const Request& b) {
    return a.priority_ratio > b.priority_ratio;
}


void sliceAdmissionControlModified(int total_slots, vector<Request>&requests, int available_resources[3], bool enableBuyback) {
    vector<Request> acceptedRequests, waitingRequests;
    int current_resources[3] = {available_resources[0], available_resources[1], available_resources[2]};
    int totalProfit = 0, totalRejected = 0, totalAccepted = 0;
    double totalUtilization = 0;

    for (int t = 1; t <= total_slots; ++t) {
        for (auto& req : requests) {
            if (req.arrival_time <= t && !req.admitted && t <= req.max_waiting_time) {
                double cost = req.resources[0] + req.resources[1] + req.resources[2];
                double profit_per_cost = req.profit / cost;
                double inverse_duration = 1.0/req.duration;
                req.priority_ratio = 0.8 * profit_per_cost + 0.2 * inverse_duration;
            }
        }

        sort(requests.begin(), requests.end(), comparePriority);

        for (auto& req : requests) {
            if (req.arrival_time <= t && !req.admitted && t <= req.max_waiting_time) {
                bool canAdmit = true;
                for (int j = 0; j < 3; ++j) {
                    if (req.resources[j] > current_resources[j]) {
                        canAdmit = false;
                        break;
                    }
                }

                if (canAdmit) {
                    for (int j = 0; j < 3; ++j) {
                        current_resources[j] -= req.resources[j];
                    }
                    req.admitted = true;
                    totalProfit += req.profit;
                    totalAccepted++;
                    acceptedRequests.push_back(req);
                } else {
                    waitingRequests.push_back(req);
                }
            }
        }

        if (enableBuyback) {
            for (auto& acc : acceptedRequests) {
                if (t >= acc.arrival_time + acc.duration) {
                    for (int j = 0; j < 3; ++j) {
                        current_resources[j] += acc.resources[j];
                    }
                    acc.admitted = false;
                }
            }
        }

        int usedResources = available_resources[0] - current_resources[0] +
                            available_resources[1] - current_resources[1] +
                            available_resources[2] - current_resources[2];
        int totalResources = available_resources[0] + available_resources[1] + available_resources[2];
        totalUtilization += (usedResources / (double)totalResources) * 100;
    }

    for (const auto& req : requests) {
        if (!req.admitted) {
            totalRejected++;
        }
    }

    cout << (enableBuyback ? "Modified Algorithm With Buyback:\n" : "Modified Algorithm Without Buyback:\n");
    cout << "Accepted Requests: " << totalAccepted << "\n";
    cout << "Rejected Requests: " << totalRejected << "\n";
    cout << "Total Profit: $" << totalProfit << "\n";
    cout << "Average Resource Utilization: " << fixed << setprecision(2)
         << (totalUtilization / total_slots) << "%\n\n";
}

void sliceAdmissionControl(int total_slots, vector<Request>& requests, int available_resources[3], bool enableBuyback) {
    vector<Request> acceptedRequests, waitingRequests;
    int current_resources[3] = {available_resources[0], available_resources[1], available_resources[2]};
    int totalProfit = 0, totalRejected = 0, totalAccepted = 0;
    double totalUtilization = 0;

    for (int t = 1; t <= total_slots; ++t) {
        for (auto& req : requests) {
            if (req.arrival_time <= t && !req.admitted && t <= req.max_waiting_time) {
                req.priority_ratio = (req.profit) / (req.resources[0] + req.resources[1] + req.resources[2]);
            }
        }

        sort(requests.begin(), requests.end(), comparePriority);

        for (auto& req : requests) {
            if (req.arrival_time <= t && !req.admitted && t <= req.max_waiting_time) {
                bool canAdmit = true;
                for (int j = 0; j < 3; ++j) {
                    if (req.resources[j] > current_resources[j]) {
                        canAdmit = false;
                        break;
                    }
                }

                if (canAdmit) {
                    for (int j = 0; j < 3; ++j) {
                        current_resources[j] -= req.resources[j];
                    }
                    req.admitted = true;
                    totalProfit += req.profit;
                    totalAccepted++;
                    acceptedRequests.push_back(req);
                } else {
                    waitingRequests.push_back(req);
                }
            }
        }

        if (enableBuyback) {
            for (auto& acc : acceptedRequests) {
                if (t >= acc.arrival_time + acc.duration) {
                    for (int j = 0; j < 3; ++j) {
                        current_resources[j] += acc.resources[j];
                    }
                    acc.admitted = false;
                }
            }
        }


        int usedResources = available_resources[0] - current_resources[0] +
                            available_resources[1] - current_resources[1] +
                            available_resources[2] - current_resources[2];
        int totalResources = available_resources[0] + available_resources[1] + available_resources[2];
        totalUtilization += (usedResources / (double)totalResources) * 100;
    }

    for (const auto& req : requests) {
        if (!req.admitted) {
            totalRejected++;
        }
    }

    cout << (enableBuyback ? "With Buyback:\n" : "Without Buyback:\n");
    cout << "Accepted Requests: " << totalAccepted << "\n";
    cout << "Rejected Requests: " << totalRejected << "\n";
    cout << "Total Profit: $" << totalProfit << "\n";
    cout << "Average Resource Utilization: " << fixed << setprecision(2) 
              << (totalUtilization / total_slots) << "%\n\n";
}

int main() {

int total_slots = 10;
int available_resources[3] = {2000, 2000, 2000};

vector<Request> requests = {
    {1, 1, 10, 10, {2000, 2000, 2000}, 10000, 0, false}, // Extremely high cost, high profit, long duration
    {2, 1, 0.1, 10, {500, 500, 500}, 2000, 0, false},      // Low cost, short duration, moderate profit
    {3, 3, 0.2, 10, {500, 500, 500}, 2000, 0, false},      // Low cost, short duration, moderate profit
    {4, 5, 0.2, 10, {500, 500, 500}, 2000, 0, false}       // Low cost, short duration, moderate profit
};



    //sliceAdmissionControl(total_slots, requests, available_resources, false);
   sliceAdmissionControlModified(total_slots, requests, available_resources, false);

    for (auto& req : requests) {
        req.admitted = false;
    }

   //sliceAdmissionControl(total_slots, requests, available_resources, true);
   //sliceAdmissionControlModified(total_slots, requests, available_resources, true);
    return 0;
}
