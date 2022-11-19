
#include "preproc.h"
#include "../GRAPH_Generators/GraphGenerator.h"
#include "../GRAPH_Generators/GraphNotLinesGenerator.h"

using namespace std;

int main(int argc, char const *argv[])
{

    // here test if all the env variables are set

    cout << "starting depot treatement with the folowing data : " << endl;

    try
    {
        cout << "waiting constant (c_a) = " << c_a << endl; 
        cout << "traveling constant (c_v) = " << c_v << endl; 
        cout << "Maximal waiting duration (MIN_WAIT) = " << MIN_WAIT << endl; 
        cout << "Fixed Cluster cost (FIX_COST) = " << FIX_COST << endl; 

    }
    catch(const char* message)
    {
        std::cerr << message << '\n';
    }
     

    auto start = chrono::high_resolution_clock::now();
    string filename = argv[1];
    string outfile = argv[2];
    string outfileOc = argv[3];

    std::cout << "Handling data set : " << filename 
              << " / " << endl;
    set<BusStation> *busStations = nullptr;

    INTER_TRIPS interTrips;
    TRIPS_MAP tripsStations;

    ifstream dataFile;
    ofstream dataOutFile;
    ofstream dataOutFileOc;

    dataOutFile.open(outfile);
    dataOutFileOc.open(outfileOc);
    dataFile.open(filename, ios::in);

    if (dataFile.is_open() && dataOutFile.is_open() && dataOutFileOc.is_open())
    {
        string line;
        int lineNumber = 0;

        while (getline(dataFile, line))
        {
            lineNumber++;
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

        cout << "Handling data set : " << GREEN << "DONE" << RESET << endl; 

        // livrable I
        vector<vector<string>> allClusters;
        for (auto it : tripsStations)
        {
            if(it.second != nullptr && it.second->size() != 0){
                multiset<BusTrip> temp = *(it.second);               
                vector<vector<string>> oneLineClusters = clusters_generator_fromTripsSet(interTrips, busStations,temp);
                for (auto itj : oneLineClusters)
                {
                    allClusters.push_back(itj);
                }
            }
            else {
                cout << "is null" << endl;
            }
            
        }
        cout << "Building Clusters : " << GREEN << "DONE" << RESET << endl;
        multiset<BusTrip> busTripsPopulation;
        detach_lines(busTripsPopulation, tripsStations);

        cout << "Full statistics calculation will be hosted in the file : " << outfile << endl; 
        cout << "showing sub data : " << endl;
        
        
        write_cluster_to_file(dataOutFileOc, allClusters);
        vector<vector<double>> clustersStats;
        vector<double> depotStats;
        stats_calculator(
            clustersStats,
            depotStats,
            allClusters,
            interTrips,
            busStations,
            busTripsPopulation);
        writeStatsIntoScreen(
            clustersStats,
            depotStats, 
            dataOutFile);
    }
    else if (!dataFile.is_open())
    {
        std::cout << "cannot open " << filename << endl;
    }
    else if (!dataOutFileOc.is_open())
    {
        std::cout << "cannot open " << outfileOc << endl;
    }
    else
    {
        std::cout << "cannot open " << outfile << endl;
    }
    dataFile.close();

    auto end = chrono::high_resolution_clock::now();
    auto Elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << YELLOW << "CPU time used (s):  " << Elapsed.count() / 1000.0 << RESET << endl;
}