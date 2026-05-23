#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
using namespace std;

#include "utilities.h"
#include "movies.h"

bool parseLine(string &line, string &movieName, double &movieRating);
bool samePrefix(const string &name, const string &prefix);

int main(int argc, char** argv){
    if (argc < 2){
        cerr << "Not enough arguments provided (need at least 1 argument)." << endl;
        cerr << "Usage: " << argv[ 0 ] << " moviesFilename prefixFilename " << endl;
        exit(1);
    }

    ifstream movieFile (argv[1]);
 
    if (movieFile.fail()){
        cerr << "Could not open file " << argv[1];
        exit(1);
    }
  
    vector<Movie> movies;

    string line, movieName;
    double movieRating;

    while (getline (movieFile, line) && parseLine(line, movieName, movieRating)){
        Movie m;
        m.name = movieName;
        m.rating = movieRating;
        movies.push_back(m);
    }

    movieFile.close();

    sort(movies.begin(), movies.end(), [](const Movie &a, const Movie &b) {
        return a.name < b.name;
    });

    if (argc == 2){
        for (int i = 0; i < (int)movies.size(); i++) {
            cout << movies[i].name << ", " << fixed << setprecision(1) << movies[i].rating << endl;
        }
        return 0;
    }

    ifstream prefixFile (argv[2]);

    if (prefixFile.fail()) {
        cerr << "Could not open file " << argv[2];
        exit(1);
    }

    vector<string> prefixes;
    while (getline (prefixFile, line)) {
        if (!line.empty()) {
            prefixes.push_back(line);
        }
    }

    vector<Movie> bestMovies;
    vector<string> bestPrefixes;
    bool lastHadMovies = false;

    for (int i = 0; i < (int)prefixes.size(); i++) {
        string prefix = prefixes[i];
        vector<Movie> matches;

        Movie key;
        key.name = prefix;
        key.rating = 0;

        vector<Movie>::iterator it = lower_bound(movies.begin(), movies.end(), key,
            [](const Movie &a, const Movie &b) {
                return a.name < b.name;
            });

        while (it != movies.end() && samePrefix(it->name, prefix)) {
            matches.push_back(*it);
            it++;
        }

        if (matches.empty()) {
            cout << "No movies found with prefix " << prefix << endl;
            lastHadMovies = false;
        } else {
            sort(matches.begin(), matches.end(), [](const Movie &a, const Movie &b) {
                if (a.rating == b.rating) {
                    return a.name < b.name;
                }
                return a.rating > b.rating;
            });

            for (int j = 0; j < (int)matches.size(); j++) {
                cout << matches[j].name << ", " << fixed << setprecision(1) << matches[j].rating << endl;
            }

            bestPrefixes.push_back(prefix);
            bestMovies.push_back(matches[0]);
            lastHadMovies = true;
        }

        if (i != (int)prefixes.size() - 1) {
            cout << endl;
        }
    }

    if (lastHadMovies) {
        cout << endl;
    }

    for (int i = 0; i < (int)bestMovies.size(); i++) {
        cout << "Best movie with prefix " << bestPrefixes[i] << " is: "
             << bestMovies[i].name << " with rating "
             << fixed << setprecision(1) << bestMovies[i].rating << endl;
    }

    return 0;
}

bool parseLine(string &line, string &movieName, double &movieRating) {
    int commaIndex = line.find_last_of(",");
    movieName = line.substr(0, commaIndex);
    movieRating = stod(line.substr(commaIndex+1));
    if (movieName[0] == '\"') {
        movieName = movieName.substr(1, movieName.length() - 2);
    }
    return true;
}

bool samePrefix(const string &name, const string &prefix) {
    if (prefix.length() > name.length()) {
        return false;
    }

    for (int i = 0; i < (int)prefix.length(); i++) {
        if (name[i] != prefix[i]) {
            return false;
        }
    }

    return true;
}
