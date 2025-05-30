//
// Created by luopw on 25-5-1.
//

#ifndef EXPERIMENT_H
#define EXPERIMENT_H
#include <vector>

#include "Job.h"
#include "Machine.h"
#include "Schedule.h"

// FIFO：First in First Out  EET: Earliest End Time  SPT: Shortest Processing Time
Schedule FIFO_EET(std::vector<Job> &jobs, std::vector<Machine> &machines, std::vector<std::string> &jobList);
Schedule FIFO_SPT(std::vector<Job> &jobs, std::vector<Machine> &machines, std::vector<std::string> &jobList);

// MOPNR：Most Operation Number Remaining  EET: Earliest End Time  SPT: Shortest Processing Time
Schedule MOPNR_EET(std::vector<Job> &jobs, std::vector<Machine> &machines, std::vector<std::string> &jobList);
Schedule MOPNR_SPT(std::vector<Job> &jobs, std::vector<Machine> &machines, std::vector<std::string> &jobList);

// MWKR：Most Work Remaining   EET: Earliest End Time  SPT: Shortest Processing Time
Schedule MWKR_EET(std::vector<Job> &jobs, std::vector<Machine> &machines, std::vector<std::string> &jobList);
Schedule MWKR_SPT(std::vector<Job> &jobs, std::vector<Machine> &machines, std::vector<std::string> &jobList);


#endif //EXPERIMENT_H
