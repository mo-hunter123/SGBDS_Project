#include <iostream>

#include <stdio.h>
#include "../GRAPH_Generators/GraphGenerator.h"
#include "../SGBDS_Project/preproc.h"
#include "ilcplex/ilocplex.h"
#include <iterator>
#include <chrono>

using namespace std;

#define NOT_EXIST_VALUE (MAXFLOAT - 1000.0)

double cost_tot = 0;
int tot_clusters = 0;
int handled_trips = 0;

typedef IloArray<IloNumVarArray> NumVar2D; // enables us to defien a 2-D decision varialbe
typedef IloArray<NumVar2D> NumVar3D;

string get_trip_id_from_trips_population(int index, multiset<BusTrip> &tripsPopulation);
float existCostFromDepotAndCalculate(string depotId, string target, INTER_TRIPS interTrips);
float existCostFromTripToDepotAndCalculate(string source, string depotId, INTER_TRIPS interTrips);
float existCostBetweenTwotTripsAndCalculate(BusTrip startTrip, BusTrip currentTrip, INTER_TRIPS interTrips);
string get_state_between_two_trips(string trip1, string trip2, multiset<BusTrip> &tripsPopulation);
void treatPartial(vector<vector<string>> &clusters, multiset<BusTrip> &busTripsPopulation, INTER_TRIPS &interTrips, string depotId, multiset<BusTrip>::iterator &it_begin, multiset<BusTrip>::iterator &it_end, int index_iteration);

int main(int argc, char const *argv[])
{
    auto start = chrono::high_resolution_clock::now();

    string data_file = argv[1];
    string output_file = argv[2];

    set<BusStation> *busStations = nullptr;
    INTER_TRIPS interTrips;
    TRIPS_MAP tripsStations;

    ifstream dataFile;
    ofstream dataOutFileOc;

    dataFile.open(data_file);
    dataOutFileOc.open(output_file);
    if (!dataFile.is_open() || !dataOutFileOc.is_open())
    {
        cerr << "Couldn't move forward " << endl;
        exit(-1);
    }
    string line;
    while (getline(dataFile, line))
    {
        if (line.find("{") != string::npos)
        {
            if (line.find("BusStations") != string::npos)
            {
                busStations = handle_file_stream_bus_stations(dataFile);
            }
            else if (line.find("BusTrip") != string::npos)
            {
                handle_file_stream_bus_trips(tripsStations, dataFile, busStations);
            }
            else if (line.find("InterTrips") != string::npos)
            {
                handle_file_stream_inter_trips(interTrips, dataFile, busStations);
            }
        }
        else if (line.find("}") != string::npos)
        {
            // std::cout << "Op. ends at " << line << endl;
        }
    }

    string depotId = findDepotId(busStations);

    multiset<BusTrip> busTripsPopulation;
    detach_lines(busTripsPopulation, tripsStations);

    vector<vector<string>> clusters;

    int total_trips_count = 0;

    // iterators that will treat the partial data
    multiset<BusTrip>::iterator begin_ite = busTripsPopulation.begin();
    multiset<BusTrip>::iterator end_ite = busTripsPopulation.begin();
    int temp_count = 0;
    int iteration_index = 0;
    while (total_trips_count != busTripsPopulation.size())
    {

        while (temp_count != PARTIAL_TRIPS && end_ite != busTripsPopulation.end())
        {
            ++end_ite;
            temp_count++;
        }

        treatPartial(clusters, busTripsPopulation, interTrips, depotId, begin_ite, end_ite, iteration_index);

        if (end_ite == busTripsPopulation.end())
            break;
        begin_ite = end_ite;

        total_trips_count += temp_count;
        temp_count = 0;

        iteration_index++;
    }

    cout << "FINISHING ALL THE DATA SET";
    
    // vector<vector<string>> second_collection = optimize_generated_solution(interTrips, busStations, busTripsPopulation, clusters); 
    // vector<vector<string>> third_collection = optimize_generated_solution(interTrips, busStations, busTripsPopulation, second_collection); 
    // vector<vector<string>> fourth_collection = optimize_generated_solution(interTrips, busStations, busTripsPopulation, third_collection); 
    // vector<vector<string>> fifth_collection = optimize_generated_solution(interTrips, busStations, busTripsPopulation, fourth_collection); 
    
    write_cluster_to_file(dataOutFileOc, clusters);

    auto end = chrono::high_resolution_clock::now();
    auto Elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "\t Elapsed Time(s): " << Elapsed.count() / 60 << endl;
    return 0;
}

void treatPartial(vector<vector<string>> &clusters, multiset<BusTrip> &busTripsPopulation, INTER_TRIPS &interTrips, string depotId, multiset<BusTrip>::iterator &it_begin, multiset<BusTrip>::iterator &it_end, int index_iteration)
{

#pragma region Problem Data

    float **C;
    int N = std::distance(it_begin, it_end);
    // int M = N * VEHICULE_POURCENTAGE;
    int M = 2;

    C = (float **)malloc(sizeof(float *) * (N + 2));

    for (int i = 0; i < N + 2; i++)
    {
        C[i] = (float *)malloc(sizeof(float) * (N + 2));
    }

    int i = 0;
    int j = 0;
    int k = 0;

    cout << "Size : " << N << endl;

    multiset<BusTrip>::iterator it_to_i_trip;
    multiset<BusTrip>::iterator it_to_j_trip;

    C[0][0] = NOT_EXIST_VALUE;
    C[N + 1][N + 1] = NOT_EXIST_VALUE;
    C[0][N + 1] = NOT_EXIST_VALUE;
    C[N + 1][0] = NOT_EXIST_VALUE;

    for (i = 1, it_to_i_trip = it_begin; it_to_i_trip != it_end; ++i, ++it_to_i_trip)
    {

        C[i][0] = existCostFromTripToDepotAndCalculate((*it_to_i_trip).getBusStationArr()->getId(), depotId, interTrips);
        C[i][N + 1] = existCostFromTripToDepotAndCalculate((*it_to_i_trip).getBusStationArr()->getId(), depotId, interTrips) + 1000.0;
        C[0][i] = existCostFromDepotAndCalculate(depotId, (*it_to_i_trip).getBusStationDep()->getId(), interTrips);
        C[N + 1][i] = existCostFromDepotAndCalculate(depotId, (*it_to_i_trip).getBusStationDep()->getId(), interTrips);

        C[i][i] = NOT_EXIST_VALUE;

        for (j = 1, it_to_j_trip = it_begin; it_to_j_trip != it_end; ++it_to_j_trip, ++j)
        {
            if (i != j)
            {
                C[i][j] = existCostBetweenTwotTripsAndCalculate((*it_to_i_trip), (*it_to_j_trip), interTrips);
            }
        }
    }

    cout << endl
         << "done" << endl;
#pragma endregion
    cout << "Calculating problem data : DONE" << endl;

#pragma region Set up environment
    IloEnv env;
    IloModel Model(env);
#pragma endregion
    cout << "Set up environment : DONE" << endl;

#pragma region Define decision variables
    NumVar3D X(env, N + 2);

    for (i = 0; i < N + 2; i++)
    {
        X[i] = NumVar2D(env, N + 2);
        for (j = 0; j < N + 2; j++)
        {
            X[i][j] = IloNumVarArray(env, N + 2);
            for (k = 0; k < M; k++)
            {
                X[i][j][k] = IloNumVar(env, IloFalse, IloTrue, ILOINT);
            }
        }
    }

#pragma endregion
    cout << "Define decicion variables : DONE" << endl;

#pragma region Define Objective Function
    // objective function
    IloExpr exp0(env);

    // N : la somme total des trips
    // M : le nombre total des vehicule

    for (k = 0; k < M; k++)
    {
        for (i = 0; i < N + 2; i++)
        {
            for (j = 0; j < N + 2; j++)
            {
                exp0 += (C[i][j] * X[i][j][k]);
            }
        }
    }

    Model.add(IloMinimize(env, exp0));

#pragma endregion
    cout << "Define objective function : DONE" << endl;

#pragma region Constraints

    // 3.2
    for (i = 1; i < N + 1; i++)
    {
        IloExpr exp2(env);
        for (k = 0; k < M; k++)
        {
            for (j = 0; j < N + 2; j++)
            {
                exp2 += X[i][j][k];
            }
        }
        Model.add(exp2 == 1);
    }

    // 3.4
    for (k = 0; k < M; k++)
    {
        IloExpr exp3(env);
        for (j = 0; j < N + 2; j++)
        {
            exp3 += X[0][j][k];
        }
        Model.add(exp3 == 1);
    }

    // 3.5
    int h = 0;
    for (h = 1; h < N + 1; h++)
    {
        for (k = 0; k < M; k++)
        {
            IloExpr exp4(env);
            IloExpr exp5(env);
            for (i = 0; i < N + 2; i++)
            {
                exp4 += X[i][h][k];
                exp5 += X[h][i][k];
            }
            Model.add((exp4 - exp5) == 0);
        }
    }

    // 3.6
    for (k = 0; k < M; k++)
    {
        IloExpr exp6(env);
        for (i = 0; i < N + 2; i++)
        {
            exp6 += X[i][N + 1][k];
        }
        Model.add(exp6 == 1);
    }

    // for (k = 0; k < M; k++)
    // {
    //     IloExpr exp7(env);
    //     for (i = 0; i < N + 2; i++)
    //     {
    //         exp7 += X[N + 1][i][k];
    //     }
    //     Model.add(exp7 == 0);
    // }

#pragma endregion

    IloCplex cplex(Model);
    cplex.setOut(env.getNullStream());

    // vector<vector<string>>

    if (!cplex.solve())
    {
        env.error() << "Failed to optimize the Master Problem!!!" << endl;
        throw(-1);
    }

    vector<string> sub_cluster;

    double obj = cplex.getObjValue();
    // tot_clusters += M;
    cost_tot += obj;

    cout << "cost: " << cost_tot << endl;
    cout << "Clusters Tot: " << tot_clusters << endl;

    // trying to show clusters
    for (k = 0; k < M; k++)
    {
        int next = 0;
        stack<int> s;
        int temp_clusters_count = 1;
        for (i = 0; i < N + 2; i++)
        {

            int Xval = cplex.getValue(X[N + 1][i][k]);
            if (Xval == 1)
            {
                temp_clusters_count++;
                s.push(i);
            }
        }

        int start = 0;
        for (j = 0; j < N + 2; j++)
        {
            int tt = cplex.getValue(X[0][j][k]);
            if (tt == 1)
                start = j;
        }
        string startTripId = get_trip_id_from_trips_population((start - 1) + (index_iteration * PARTIAL_TRIPS), busTripsPopulation);

        int treated_clusters = 0;
        sub_cluster.push_back(startTripId);

        while (treated_clusters < temp_clusters_count)
        {
            for (j = 0; j < N + 2; j++)
            {
                int tt = cplex.getValue(X[start][j][k]);
                if (tt == 1)
                    next = j;
            }
            if (next == 0 || next == N + 1)
            {
                // end cluster
                clusters.push_back(sub_cluster);
                sub_cluster.clear();
                treated_clusters++;

                if (!s.empty())
                {
                    start = s.top();
                    s.pop();
                }
                else
                    break;
                startTripId = get_trip_id_from_trips_population(start - 1 + (index_iteration * PARTIAL_TRIPS), busTripsPopulation);
                sub_cluster.push_back(startTripId);
            }
            else
            {
                string arrivalTripId = get_trip_id_from_trips_population(next - 1 + (index_iteration * PARTIAL_TRIPS), busTripsPopulation);
                string betweenTripsState = get_state_between_two_trips(startTripId, arrivalTripId, busTripsPopulation);

                sub_cluster.push_back(betweenTripsState);
                sub_cluster.push_back(arrivalTripId);

                start = next;
                startTripId = arrivalTripId;
            }
        }
    }

    // serching the first start
    /*
    for (k = 0; k < M; k++)
    {
        vector<string> cluster_data;
        int start = 0;
        for (j = 0; j < N + 2; j++)
        {
            int tt = cplex.getValue(X[0][j][k]);
            if (tt == 1)
                start = j;
        }

        string startTripId = get_trip_id_from_trips_population(start - 1, busTripsPopulation);

        cout << "cluster " << k + 1 << " : " << startTripId << " ";
        int next = start;

        // cluster_data.push_back("cluster " + to_string(k+1) + " : ");
        // cluster_data.push_back(startTripId);

        while (next < N + 1)
        {
            // search the next trip
            for (j = 0; j < N + 2; j++)
            {
                int tt = cplex.getValue(X[start][j][k]);
                if (tt == 1)
                    next = j;
            }

                string arrivalTripId = get_trip_id_from_trips_population(next - 1, busTripsPopulation);
                cout << get_state_between_two_trips(startTripId, arrivalTripId, busTripsPopulation) << " " << arrivalTripId << " ";
                start = next;
                startTripId = arrivalTripId;

        }
        cout << ";" << endl;
    }
    cout << endl
         << "=----------------=" << endl;

    cout << endl;*/

    /*for (k = 0; k < M; k++)
    {
        for (i = 0; i < N + 2; i++)
        {
            for (j = 0; j < N + 2; j++)
            {
                int Xval = cplex.getValue(X[i][j][k]);
                if (Xval == 1)
                {
                    cout << "X[" << i << "][" << j << "][" << k << "]"
                         << " = 1  ";
                }
            }
            cout << " | ";
        }
        cout << endl
             << "=----------------=" << endl;
    }*/

    // delete cost matrix
    for (i = 0; i < N + 2; i++)
        delete[] C[i];
    delete[] C;

    // exp2.end();
    // exp3.end();
    // exp4.end();
    // exp5.end();
    // exp6.end();
    env.end();

    // delete cplex env and Model
    // Model.remove();
}

float existCostFromDepotAndCalculate(string depotId, string target, INTER_TRIPS interTrips)
{
    int duration = TargetInterTrip::findDurationByTargetId(target, interTrips.at(depotId));
    if (duration == -1)
    {
        return NOT_EXIST_VALUE;
    }
    return duration * c_v;
}

float existCostFromTripToDepotAndCalculate(string source, string depotId, INTER_TRIPS interTrips)
{
    int duration = TargetInterTrip::findDurationByTargetId(depotId, interTrips.at(source));
    if (duration == -1)
    {
        return NOT_EXIST_VALUE;
    }
    return duration * c_v;
}

float existCostBetweenTwotTripsAndCalculate(BusTrip startTrip, BusTrip currentTrip, INTER_TRIPS interTrips)
{
    // if we have the same arr and dep stations
    if (startTrip.getBusStationArr()->getId() == currentTrip.getBusStationDep()->getId())
    {
        int diff_time = difftime(currentTrip.getDateDep(), startTrip.getDateArr()) / 60;
        // SI ON PEUT TOLERER ATTENTE = 0 DANS UNE STATION
        if (0 <= diff_time && diff_time <= MIN_WAIT) // the successor could be the next
        {
            return diff_time * c_a;
        }
    }
    // when  (HLP)
    else
    {
        int temp_dur = TargetInterTrip::findDurationByTargetId(currentTrip.getBusStationDep()->getId(), interTrips.at(startTrip.getBusStationArr()->getId()));
        if (temp_dur != -1)
        {
            int attente_hlp = (difftime(currentTrip.getDateDep(), startTrip.getDateArr()) / 60.0) - temp_dur;

            if (0 <= attente_hlp && attente_hlp <= MIN_WAIT)
            {
                // verifier si HLP
                return temp_dur * c_v + attente_hlp * c_a;
            }
        }
    }
    return NOT_EXIST_VALUE;
}

// This function will take the multiset and an index and gonna return the Trip id
string get_trip_id_from_trips_population(int index, multiset<BusTrip> &tripsPopulation)
{

    if (0 <= index && index < tripsPopulation.size())
    {
        int temp_index = 0;
        multiset<BusTrip>::iterator it;
        for (it = tripsPopulation.begin(); it != tripsPopulation.end(); ++it)
        {
            if (temp_index == index)
                return it->getTripId();

            temp_index++;
        }
    }
    return "NULL";
}

string get_state_between_two_trips(string trip1, string trip2, multiset<BusTrip> &tripsPopulation)
{
    BusTrip start, target;
    if (findTripById(start, trip1, tripsPopulation) && findTripById(target, trip2, tripsPopulation))
    {
        if (start.getBusStationArr()->getId() == target.getBusStationDep()->getId())
            return "WS";

        else
            return "HLP";
    }

    // cerr << endl
    //      << RED << "[ Fatal error ] " << RESET << YELLOW << "No trip found with folowing data " << RESET << trip1 << " && " << trip2 << endl;
    // exit(-1);
    return "ERROR";
}
