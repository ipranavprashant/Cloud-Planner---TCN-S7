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

void sliceAdmissionControl(int total_slots, sector<Request>& requests, int available_resources[3], bool enableBuyback) {
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
    int total_slots = 12;
    int available_resources[3] = {3000, 3000, 3000};

    vector<Request> requests = {
        {1, 1, 6, 3, {500, 400, 300}, 1000, 0, false},
        {2, 2, 8, 2, {300, 200, 400}, 800, 0, false},
        {3, 3, 10, 4, {400, 300, 500}, 1200, 0, false}
    };

    sliceAdmissionControl(total_slots, requests, available_resources, false);

    for (auto& req : requests) {
        req.admitted = false;
    }

    sliceAdmissionControl(total_slots, requests, available_resources, true);
    return 0;
}
