#pragma once
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <vector>
#include <ctime>
#include <memory>
#include <algorithm>
#include <stack>
#include <chrono>

#include "../BUSINESS_ENTITIES/BusStation.h"
#include "../BUSINESS_ENTITIES/BusTrip.h"
#include "../BUSINESS_ENTITIES/TargetInterTrip.h"

#include "../StringOperations/StringOps.h"

#include "../HANDLERS/BusStationsHandler.h"
#include "../HANDLERS/BusTripsHandler.h"
#include "../HANDLERS/BusInterTripsHandler.h"

#include "StatisticsCalculator.h"

// #include "GraphGenerator.h"
using namespace std; 

//the following are UBUNTU/LINUX, and MacOS ONLY terminal color codes.
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

typedef unordered_map<string, multiset<BusTrip> *> TRIPS_MAP;
typedef map<string, set<TargetInterTrip> *> INTER_TRIPS;
typedef vector<pair<BusTrip, bool>> TEMP_TYPE;

#define BUS_LINE_RAW_COUNT 8
#define INTER_TRIPS_RAW_COUNT 4
#define BUS_STATIONS_RAW_COUNT 2

#define PARTIAL_TRIPS stoi(getenv(string("PARTIAL_TRIPS").c_str()) ? getenv(string("PARTIAL_TRIPS").c_str()) : throw "The PARTIAL TRIPS var is not set") 
#define VEHICULE_POURCENTAGE 0.07

#define c_a stof(getenv(string("C_A").c_str()) ? getenv(string("C_A").c_str()) : throw "the C_A env variables is unseted")
#define c_v stof(getenv(string("C_V").c_str()) ? getenv(string("C_V").c_str()) : throw "the C_V env variables is unseted")
#define MIN_WAIT stoi(getenv(string("MIN_WAIT").c_str()) ? getenv(string("MIN_WAIT").c_str()): throw "the MIN_WAIT env variables is unseted")
#define FIX_COST stof(getenv(string("FIX_COST").c_str()) ? getenv(string("FIX_COST").c_str()): throw "the FIX_COST env variables is unseted")
#define PATTERN getenv(string("PATTERN_CLUSTERS").c_str()) ? getenv(string("PATTERN_CLUSTERS").c_str()): throw "the RegEX pattern env variables is unseted"