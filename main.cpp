#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <regex>
#include <exception>
#include "csvstream.h"
#include <cmath>
#include <algorithm>

using namespace std;

class Indentifier
{
private:
    bool debug;

    // The total number of posts in the entire training set.
    int post_count = 0;

    // The number of unique words in the entire training set. (The vocabulary size.)
    int unique_word_count = 0;

    // For each word w, the number of posts in the entire training set that contain w
    map<string, int> post_count_per_word;

    // For each label C, the number of posts with that label.
    map<string, int> post_count_per_label;

    // For each label C and word  w, the number of posts with label C that contain w
    map<pair<string, string>, int> label_word_freq_map;

    vector<string> tag_list;
    vector<string> content_list;

    vector<string> unique_words(const string &str)
    {
        istringstream source(str);
        vector<string> words;
        string word;
        while (source >> word)
        {
            words.push_back(word);
        }
        return words;
    }

    bool key_exists_in_map(map<string, int> count_map, string key)
    {
        return count_map.find(key) != count_map.end();
    }

public:
    Indentifier(bool debug_true)
    {
        debug = debug_true;
    }

    void classify(string filename)
    {
        vector<string> calculated_labels;
        vector<string> correct_labels;
        vector<string> label_unique_list;
        vector<vector<string>> classify_list;

        // converts file into string stream
        csvstream csvin(filename);

        map<string, string> row;

        while (csvin >> row)
        {
            string tag = row["tag"];
            string content = row["content"];
            correct_labels.push_back(tag);
            if (find(label_unique_list.begin(), label_unique_list.end(), tag) == label_unique_list.end())
            {
                label_unique_list.push_back(tag);
            }

            classify_list.push_back(unique_words(content));
        }

        double highest_prob = 0;

        for (vector<string> i : classify_list)
        {
            highest_prob = 0;
            map<string, double> prob_list;
            string highest_prob_tag;
            for (string tag : label_unique_list)
            {
                double new_prob = log(post_count_per_label[tag] / post_count);
                for (string word : i)
                {

                    if (key_exists_in_map(post_count_per_word, word))
                    {
                        pair<string, string> p = {tag, word};
                        new_prob += log(label_word_freq_map[p] / post_count_per_label[tag]);
                    }
                    else if (!key_exists_in_map(post_count_per_word, word))
                    {
                        new_prob += log(1 / post_count);
                    }
                    else
                    {
                        new_prob += log(post_count_per_word[word]);
                    }
                }
                prob_list[tag] = new_prob;

                if (new_prob > highest_prob)
                {
                    highest_prob_tag = tag;
                    highest_prob = new_prob;
                }
            }
            // high chance of being in reverse order
            correct_labels.push_back(highest_prob_tag);
        }
    }
    void train_on_file(string filename)
    {
        // converts file into string stream
        csvstream csvin(filename);
        // vector<string> tag_list, content_list;
        map<string, string> row;
        vector<string> total_unique_words;

        if (debug)
            cout << "training data\n";

        while (csvin >> row)
        {

            string tag = row["tag"];
            string content = row["content"];

            // debug stuff
            if (debug)
            {
                cout << "label = " << tag << ", content = " << content << endl;
            }

            // adds to post_count_per_word map
            if (!key_exists_in_map(post_count_per_label, tag))
                post_count_per_label[tag] = 1;
            else
                post_count_per_label[tag]++;

            vector<string> content_words = unique_words(content);

            for (string word : content_words)
            {
                bool word_is_unique = true;
                for (string unique_word : total_unique_words)
                {
                    if (unique_word == word)
                    {
                        word_is_unique = false;
                    }
                }
                unique_word_count += word_is_unique;
                total_unique_words.push_back(word);

                if (!key_exists_in_map(post_count_per_word, word))
                    post_count_per_word[word] = 1;
                else
                    post_count_per_word[word]++;

                pair<string, string> label_word = {tag, word};
                if (label_word_freq_map.find(label_word) == label_word_freq_map.end())
                    label_word_freq_map[label_word] = 1;
                else
                    label_word_freq_map[label_word]++;
            }

            // adds to other map

            tag_list.push_back(tag);
            content_list.push_back(content);
            post_count++;
        }
        if (debug)
        {
            cout << "trained on " << post_count << " examples\n";
            cout << "vocabulary size = " << unique_word_count << endl
                 << endl;
        }
    }
};

int main(int argc, char *argv[])
{
    cout.precision(3);
    bool debug = false;
    // error checking
    if (argc != 3 && argc != 4)
    {
        cout << "Usage: main.exe TRAIN_FILE TEST_FILE [--debug]" << endl;
        return 1;
    }
    if (argc == 4)
    {
        if (strcmp(argv[3], "--debug") != 0)
        {
            cout << "Usage: main.exe TRAIN_FILE TEST_FILE [--debug]" << endl;
            return 1;
        }
        else
        {
            debug = true;
        }
    }

    string filename = argv[1];

    fstream test_open(filename);
    if (!test_open.is_open())
    {
        cout << "Error opening file: " << filename << endl;
        return 1;
    }
    test_open.close();

    Indentifier ident(debug);
    ident.train_on_file(filename);
    ident.classify(argv[2]);
}