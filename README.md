# Bus Stations process optimization

## Description

The following project is a solution to real bus network optimaztion

The project contains three main programs `Moulinette, Livrable1, Livrable2, Livrable3, Simplex Method`

## How to use the Repo

### for visual studio users

```
git clone https://github.com/mo-hunter123/SGBDS-project
# then just run the app
```

### for linux users

## To use The graph generators project

First Run
``` 
cd SGBDS_Project 
```

In the following directory exists two main files :
- `./build.sh` : This file will let you build the solution depending on the desire livrable index 
  - livrable1 : the greedy solution of reading data by bus lines 
  - livrable2 : a more optimised solution of generating clusters data 
  ```
    # the ./build.sh follows the following pattern 
    ./build.sh [livrableIndex]
  ```
- `./run.sh` : The following script will let you run the desired livrable by following this pattern 
  ```
  ./run.sh [livrableIndex] [path_to_datafile.txt] [path_to_output_statistics.txt] [path_to_clusters_generation_file.txt]
  ```
## To use the Moulinette project

First Run

```

cd MOULINETTE_Project

```

You will find in `./MOULINETTE_Project` two files :

- `./build.sh` : this file will let you build the moulinette solution [no arguments needed]
- `./run.sh` : this file will let you run the executable. The pattern to run the following is 
  - ```./run.sh [path_to_data_file.txt] [path_to_cluster_data.txt] [path_to_statistics_destination.txt]```

## To use the Simplex method 

```
cd ExactMethodVRP 

```

then run the makeFile to build the program and run the executable 

```
make VRPSimplex2 
```


### Happy hacking 