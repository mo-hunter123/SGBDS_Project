#pragma once
#include <vector>
#include <set>
#include "../SGBDS_Project/preproc.h"
#include "LogicalAnalyser.h"

/**
 * @brief
 * This class defines the needed methods to calculate different statistics
 *
 */

class StatisticsAnalyser
{

private:
    vector<vector<string>> matrixData;
    ofstream stats_out;

    // depot statistics
    int totalTripDuration;

    int totalTrips;

    int clustersCount;
    int clustersWithOneTrip;
    float coutTotal;

    int HLPCounter;
    int HLPDuration; // + the porcentage of hlps on the total

    int attenteCounter;
    int attenteDuration; // + the porcentage of attentes on the total

public:
    StatisticsAnalyser(vector<vector<string>> &matData, string stats_out)
    {
        this->matrixData = matData;
        this->stats_out.open(stats_out);
        if (!this->stats_out.is_open())
        {
            cerr << "Sorry we can't open the file : " << stats_out << endl;
            exit(-1);
        }

        this->totalTripDuration = 0;
        this->totalTrips = 0;
        this->clustersCount = 0;
        this->clustersWithOneTrip = 0;
        this->coutTotal = 0.0;
        this->HLPCounter = 0;
        this->HLPDuration = 0;
        this->attenteCounter = 0;
        this->attenteDuration = 0;
    }

    vector<float> calculateClusterData(int clusterPosition, vector<string> cluster)
    {
        int totalClusterDuration = 0;

        // when == 1 add it to clusters with one trip
        int totalTripsCluster = 0;
        float coutTotalCluster = 0.0;

        int HLPClusterCounter = 0;
        int HLPClusterDuration = 0; // + the porcentage of hlps on the total

        int attenteClusterCounter = 0;
        int attenteClusterDuration = 0; // + the porcentage of attentes on the total

        for (auto str : cluster)
        {
            vector<string> temp = StringOps::split(StringOps::ltrim(str));
            totalClusterDuration += stoi(temp[0]);
            switch (temp.size())
            {
            // attente
            case 1:
                attenteClusterDuration += stoi(temp[0]);
                attenteClusterCounter++;
                break;

            // hlp or from or to depot
            case 2:

                HLPClusterCounter++;
                HLPClusterDuration += stoi(temp[0]);
                attenteClusterDuration += stoi(temp[1]);
                totalClusterDuration += (stoi(temp[1]));
                break;

            // trip
            case 3:
                totalTripsCluster++;
                break;

            default:
                break;
            }

            coutTotalCluster = (attenteClusterDuration * c_a) + (HLPClusterDuration * c_v);
        }

        float hlpPour = ((HLPClusterDuration * 1.0) / (totalClusterDuration * 1.0)) * 100;
        float attPour = ((attenteClusterDuration * 1.0) / (totalClusterDuration * 1.0)) * 100;

        vector<float> clusterStats = {(float)clusterPosition, (float)(totalClusterDuration * 1.0), coutTotalCluster, (float)(HLPClusterCounter * 1.0), (float)(HLPClusterDuration * 1.0), hlpPour, (float)(attenteClusterDuration * 1.0), attPour, (float)(totalTripsCluster * 1.0)};

        this->totalTripDuration += totalClusterDuration;
        this->clustersCount++;
        if (totalTripsCluster == 1)
            this->clustersWithOneTrip++;
        this->coutTotal += coutTotalCluster;
        this->HLPCounter += (HLPClusterCounter - 2);
        this->HLPDuration += HLPClusterDuration; // + the porcentage of hlps on the total
        this->attenteCounter += attenteClusterCounter;
        this->attenteDuration += attenteClusterDuration;

        this->totalTrips += totalTripsCluster;

        return clusterStats;
    }

    void handleDepot()
    {
        int i = 1;
        stats_out << "+---------+----------+----------------+----------------+--------------+-----------------+---------------+------------------+-----------------+" << endl;
        stats_out << "| Cluster | Duration | Cluster's Cost | Number of HLPs | HLP Duration | HLP pourcentage | Wait Duration | Wait Pourcentage | Number of trips |" << endl;
        stats_out << "+---------+----------+----------------+----------------+--------------+-----------------+---------------+------------------+-----------------+" << endl;

        for (auto cluster : this->matrixData)
        {
            vector<float> data = this->calculateClusterData(i, cluster);
            stats_out << "|" << setw(9) << i;
            stats_out << "|" << setw(10) << data[1];
            stats_out << "|" << setw(16) << data[2];
            stats_out << "|" << setw(16) << data[3];
            stats_out << "|" << setw(14) << data[4];
            stats_out << "|" << setw(17) << data[5];
            stats_out << "|" << setw(15) << data[6];
            stats_out << "|" << setw(18) << data[7];
            stats_out << "|" << setw(17) << data[8] << "|" << endl;
            i++;
        }

        stats_out << "+---------+----------+----------------+----------------+--------------+-----------------+---------------+------------------+-----------------+" << endl;
        stats_out << endl;

        stats_out << "DEPOT STATS : " << endl;
        stats_out << "+----------------+--------------+----------------+--------------+-----------------+---------------+------------------+-----------------+--------------------+" << endl;
        stats_out << "| Total Duration | Depot's Cost | Number of HLPs | HLP Duration | HLP pourcentage | Wait Duration | Wait Pourcentage | Number of trips | Number of Clusters |" << endl;
        stats_out << "+----------------+--------------+----------------+--------------+-----------------+---------------+------------------+-----------------+--------------------+" << endl;
        stats_out << "|" << setw(16) << this->totalTripDuration;
        stats_out << "|" << setw(14) << this->coutTotalCalculator();
        stats_out << "|" << setw(16) << this->HLPCounter;
        stats_out << "|" << setw(14) << this->HLPDuration;
        stats_out << "|" << setw(17) << (this->HLPDuration * 1.0 / this->totalTripDuration * 1.0) * 100;
        stats_out << "|" << setw(15) << this->attenteDuration;
        stats_out << "|" << setw(18) << (this->attenteDuration * 1.0 / this->totalTripDuration * 1.0) * 100;
        stats_out << "|" << setw(17) << this->totalTrips;
        stats_out << "|" << setw(20) << this->clustersCount << "|" << endl;
        stats_out << "+----------------+--------------+----------------+--------------+-----------------+---------------+------------------+-----------------+--------------------+" << endl;

        stats_out.close();
    }

    //
    float coutTotalCalculator()
    {
        return this->coutTotal + (1000 * clustersCount);
    }
};