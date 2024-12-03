#include <bits/stdc++.h>
using namespace std;

struct Request {
    int id;
    int arrival_time;
    int duration;
    int max_waiting_time;
    vector<int> resources;       // Resources required [R1, R2, R3]
    vector<double> unit_prices; 
    vector<int> utilized;// Unit price of resources [P1, P2, P3]
    double priority_metric;      // Alpha metric
    bool admitted;               // Admission status
};

bool comparePriority(const Request& a, const Request& b) {
    return a.priority_metric > b.priority_metric;
}

void calculateCPUUtilization(const vector<int>& available_resources, const vector<int>& current_resources, double& totalCPUUsage) {
    double totalAvailableResources = 0;
    double totalUsedResources = 0;

    for (int i = 0; i < 3; ++i) {
        totalAvailableResources += available_resources[i];
        totalUsedResources += (available_resources[i] - current_resources[i]); // Used resources
    }

    // Avoid division by zero if totalAvailableResources is zero
    if (totalAvailableResources > 0) {
        double currentCPUUsagePercent = (totalUsedResources / totalAvailableResources) * 100;
        totalCPUUsage += currentCPUUsagePercent;
    }
    else {
        // If no resources are available, set CPU usage to 0
        totalCPUUsage += 0;
    }
}

void modifiedSliceAdmissionControl(int total_slots, vector<Request>& requests, vector<int> available_resources) {
    vector<Request> acceptedRequests, waitingRequests;
    vector<int> current_resources = available_resources;
    vector<int> needed_resources(3, 0);  // Needed resources for waiting requests
    double totalCPUUsage = 0;
    int totalProfit = 0, totalCost = 0;

    for (int t = 1; t <= total_slots; ++t) {
        // Step 1: Calculate priority metric (Heuristic)
        for (auto& req : requests) {
            if (req.arrival_time <= t && !req.admitted && t <= req.max_waiting_time) {
                double cost = 0;
                double profit = 0;
                
                // Calculate cost (sum of resources)
                for (int j = 0; j < 3; ++j) {
                    cost += req.resources[j];
                    profit += req.resources[j] * req.unit_prices[j];
                }

                // Heuristic formula: 0.8 * (profit/cost) + 0.2 * (1/duration)
                double profit_per_cost = profit / cost;
                double inverse_duration = 1.0 / req.duration;
                req.priority_metric = 0.8 * profit_per_cost + 0.2 * inverse_duration;  // Weighted priority
            }
        }

        // Step 2: Sort requests by priority metric (descending order)
        sort(requests.begin(), requests.end(), comparePriority);

        // Step 3: Admission control without buyback - Admit requests based on resources
        for (auto& req : requests) {
            if (req.arrival_time <= t && !req.admitted && t <= req.max_waiting_time) {
                bool canAdmit = true;
                for (int j = 0; j < 3; ++j) {
                    if (req.resources[j] > current_resources[j]) {
                        canAdmit = false;
                        break;
                    }
                }

                // If resources are available, admit the request
                if (canAdmit) {
                    req.admitted = true;
                    acceptedRequests.push_back(req);
                    for (int j = 0; j < 3; ++j) {
                        current_resources[j] -= req.resources[j];  // Deduct resources
                    }

                    // Add to total profit and cost
                    for (int j = 0; j < 3; ++j) {
                        totalProfit += req.resources[j] * req.unit_prices[j] * req.duration;
                        totalCost += req.resources[j];
                    }
                } else {
                    waitingRequests.push_back(req);  // Add to waiting list if not admitted
                }
            }
        }

        // Step 4: Buyback process - Reclaim resources from expired requests
        vector<int> shareable_resources(3, 0);
        for (auto& acc : acceptedRequests) {
            if (t >= acc.arrival_time + acc.duration) {  // Request has completed
                for (int j = 0; j < 3; ++j) {
                    shareable_resources[j] += acc.resources[j];  // Collect freed resources
                }
                acc.admitted = false;  // Mark request as not admitted
            }
        }

        // Step 5: Partial pooling - Admit waiting requests if possible with buyback resources
        for (auto& req : waitingRequests) {
            bool canAdmit = true;
            for (int j = 0; j < 3; ++j) {
                if (current_resources[j] + shareable_resources[j] < req.resources[j]) {
                    canAdmit = false;
                    break;
                }
            }

            if (canAdmit) {
                // Admit the request, update resources
                for (int j = 0; j < 3; ++j) {
                    current_resources[j] += shareable_resources[j];
                    shareable_resources[j] = 0;  // All shareable resources used up
                }
                req.admitted = true;
                acceptedRequests.push_back(req);
            }
        }

        // Step 6: Full pooling - Fallback if partial pooling fails
        for (auto& req : waitingRequests) {
            bool canAdmitAfterFullPooling = true;
            for (int j = 0; j < 3; ++j) {
                if (req.resources[j] > (current_resources[j] + shareable_resources[j])) {
                    canAdmitAfterFullPooling = false;
                    break;
                }
            }

            if (canAdmitAfterFullPooling) {
                // Admit the request and deduct resources
                req.admitted = true;
                acceptedRequests.push_back(req);
                for (int j = 0; j < 3; ++j) {
                    current_resources[j] -= req.resources[j];
                }
            }
        }

        // Step 7: Calculate CPU utilization (percentage) for each time slot
        calculateCPUUtilization(available_resources, current_resources, totalCPUUsage);

        // Step 8: Reset waiting list for the next time slot
        waitingRequests.clear();
    }

    // Final Results: Calculate total profit and rejected requests
    int totalRejected = 0, totalAccepted = 0;
    for (const auto& req : requests) {
        if (req.admitted) {
            ++totalAccepted;
        } else {
            ++totalRejected;
        }
    }

    double averageCPUUsage = totalCPUUsage / total_slots;

    // Print final results
    cout << "Modified Slice Admission Control Results:\n";
    cout << "Accepted Requests: " << totalAccepted << "\n";
    cout << "Rejected Requests: " << totalRejected << "\n";
    cout << "Total Profit: $" << totalProfit << "\n";
    cout << "Total Cost: $" << totalCost << "\n";
    cout << "Average CPU Utilization: " << averageCPUUsage << "%\n";
}



void proposedSliceAdmissionControl(int total_slots, vector<Request>& requests, vector<int> available_resources) {
    vector<Request> acceptedRequests, waitingRequests;
    vector<int> current_resources = available_resources;
    vector<int> needed_resources(3, 0);  // Needed resources for waiting requests
    double totalCPUUsage = 0;

    for (int t = 1; t <= total_slots; ++t) {
        // Step 1: Calculate priority metric (Alpha)
        for (auto& req : requests) {
            if (req.arrival_time <= t && !req.admitted && t <= req.max_waiting_time) {
                double total_profit = 0, total_cost = 0;
                for (int j = 0; j < 3; ++j) {
                    total_profit += req.resources[j] * req.unit_prices[j] * req.duration;
                    total_cost += req.resources[j];
                }
                req.priority_metric = total_profit / total_cost;
            }
        }

        // Step 2: Sort requests by priority metric
        sort(requests.begin(), requests.end(), comparePriority);

        // Step 3: Admission without buyback
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
                    req.admitted = true;
                    acceptedRequests.push_back(req);
                    for (int j = 0; j < 3; ++j) {
                        current_resources[j] -= req.resources[j];
                    }
                } else {
                    waitingRequests.push_back(req);
                    for (int j = 0; j < 3; ++j) {
                        needed_resources[j] += req.resources[j];
                    }
                }
            }
        }

        // Step 4: Buyback process (for requests already admitted)
        vector<int> shareable_resources(3, 0);
        // vector<int> actual_utilised_resource //here we can actually change the actually utilised resource, maybe by hard coding or something
        for (const auto& acc : acceptedRequests) {
            for (int j = 0; j < 3; ++j) {
                shareable_resources[j] += max(0, acc.resources[j] - acc.utilized[j]);
            } 
        }

        // Step 5: Partial pooling (admit waiting requests if possible)
        for (auto& req : waitingRequests) {
            bool canAdmit = true;
            for (int j = 0; j < 3; ++j) {
                if (current_resources[j] + shareable_resources[j] < needed_resources[j]) {
                    canAdmit = false;
                    break;
                }
            }

            if (canAdmit) {
                for (int j = 0; j < 3; ++j) {
                    current_resources[j] += shareable_resources[j];
                    needed_resources[j] -= req.resources[j];
                }
                req.admitted = true;
                acceptedRequests.push_back(req);
            }
        }

        // Step 6: Full pooling (fallback if partial pooling fails)
        for (auto& req : waitingRequests) {
            bool canAdmitAfterFullPooling = true;
            for (int j = 0; j < 3; ++j) {
                if (req.resources[j] > (current_resources[j] + shareable_resources[j])) {
                    canAdmitAfterFullPooling = false;
                    break;
                }
            }

            if (canAdmitAfterFullPooling) {
                req.admitted = true;
                acceptedRequests.push_back(req);
                for (int j = 0; j < 3; ++j) {
                    current_resources[j] -= req.resources[j];
                }
            }
        }

        // Step 7: Release resources of expired requests
        for (auto& acc : acceptedRequests) {
            if (t >= acc.arrival_time + acc.duration) {
                for (int j = 0; j < 3; ++j) {
                    current_resources[j] += acc.resources[j];
                }
                acc.admitted = false;
            }
        }

        // Step 8: Calculate CPU Utilization in percentage
        calculateCPUUtilization(available_resources, current_resources, totalCPUUsage);

        // Reset waiting list for next time slot
        waitingRequests.clear();
    }

    // Final Results
    int totalProfit = 0, totalRejected = 0, totalAccepted = 0;
    for (const auto& req : requests) {
        if (req.admitted) {
            ++totalAccepted;
            for (int j = 0; j < 3; ++j) {
                totalProfit += req.resources[j] * req.unit_prices[j] * req.duration;
            }
        } else {
            ++totalRejected;
        }
    }

    double averageCPUUsage = totalCPUUsage / total_slots;

    cout << "Results with Buyback:\n";
    cout << "Accepted Requests: " << totalAccepted << "\n";
    cout << "Rejected Requests: " << totalRejected << "\n";
    cout << "Total Profit: $" << totalProfit << "\n";
    cout << "Average CPU Utilization: " << averageCPUUsage << "%\n";
}

int main() {
    int total_slots = 10;
    vector<int> available_resources = {2000, 2000, 2000}; // Initial capacity of the InP for 3 resources

    vector<Request> requests = {
        {1, 1, 10, 10, {2000, 2000, 200}, {5, 4, 3},{1000,1200,50}, 0, false},  // High cost, high profit, long duration
        {2, 1, 200, 10, {500, 500, 500}, {5, 4, 3},{175,186,320}, 0, false},       // Low cost, moderate profit, short duration
        {3, 3, 30000, 10, {700, 700, 700}, {5, 4, 3},{700, 700, 700},0, false},       // Medium cost, high profit, medium duration
        {4, 5, 4, 10, {400, 400, 400}, {5, 4, 3},{230,175,230}, 0, false},       // Low cost, moderate profit, short duration
    };

    proposedSliceAdmissionControl(total_slots, requests, available_resources);
    modifiedSliceAdmissionControl(total_slots, requests, available_resources);
    return 0;
}
