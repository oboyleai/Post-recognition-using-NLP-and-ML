// Project UID db1f506d06d84ab787baf250c265e24e
// uniqnames: mileslow and oboyleai
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

    // The number of unique words in the entire training set.
    //  (The vocabulary size.)
    int unique_word_count = 0;

    // For each word w, the number of posts in the entire
    //  training set that contain w
    map<string, int> post_count_per_word;

    // For each label C, the number of posts with that label.
    map<string, int> post_count_per_label;

    // For each label C and word  w, the number of posts
    //  with label C that contain w
    map<pair<string, string>, int> label_word_freq_map;

    vector<string> tag_list;

    vector<string> unique_words(const string &str)
    {
        istringstream source(str);
        set<string> words;
        string word;
        while (source >> word)
        {
            words.insert(word);
        }
        return vector<string>(words.begin(), words.end());
    }

    bool key_exists_in_map(const map<string, int> &count_map, string key)
    {
        return count_map.find(key) != count_map.end();
    }

    void print_debug()
    {
        cout << "vocabulary size = " << unique_word_count << endl
             << endl;
        cout << "classes:" << endl;

        vector<string> prev_tags;
        for (string current_tag : tag_list)
        {
            vector<string>::iterator prev_begin = prev_tags.begin();
            vector<string>::iterator prev_end = prev_tags.end();
            vector<string>::iterator find_it = find(
                prev_begin, prev_end, current_tag);
            bool prev_tag_no_exist = find_it == prev_end;
            if (prev_tag_no_exist)
            {
                prev_tags.push_back(current_tag);
                double post_with_label_c = post_count_per_label[current_tag];
                double log_prior = log(post_with_label_c / post_count);
                cout << "  " << current_tag << ", "
                     << post_with_label_c
                     << " examples, log-prior = " << log_prior << endl;
            }
        }

        // classifier parameters
        cout << "classifier parameters:" << endl;

        map<pair<string, string>, int>::iterator it;
        map<pair<string, string>, int>::iterator begin;

        for (it = label_word_freq_map.begin(); it != label_word_freq_map.end(); it++)
        {
            double count = it->second;
            string current_tag = it->first.first;
            string current_word = it->first.second;
            double post_count_label_c = post_count_per_label[current_tag];
            double log_likelihood = log(count / post_count_label_c);
            cout << "  " << current_tag << ":" << current_word << ", count = "
                 << count << ", log-likelihood = " << log_likelihood << endl;
        }
        cout << endl;
    }

    void classify_helper(
        const string &tag,
        const string &word,
        double &new_prob,
        pair<double, double> counts)
    {
        pair<string, string> tag_word = {tag, word};
        double tag_post_count = counts.first;
        double post_count_double = counts.second;

        // adds the log liklihood probability to the log prior probability

        // if w is seen in the post do this
        //  new_prob += log(label_word_freq_map[p]
        // / post_count_per_label[tag]);
        //  if w does not occur in posts labeled d,
        // but does occur in training data
        //   new_prob += log(post_count_per_word[word] / post_count);
        //  if w doesn't occur anywhere
        // new_prob += log(1 / post_count);
        if (label_word_freq_map[tag_word] >= 1)
        {
            float tag_word_count = label_word_freq_map[tag_word];
            // double tag_word_count = ;
            new_prob += log(tag_word_count / tag_post_count);
        }
        else if (post_count_per_word[word] >= 1)
        {
            double word_post_count = post_count_per_word[word];
            // double post_count_double = )
            new_prob += log(word_post_count / post_count_double);
        }
        else
        {
            new_prob += log(1.0 / post_count_double);
        }
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
        vector<double> calculated_log;
        vector<string> post_contents;
        int new_post_count = 0;
        // converts file into string stream
        csvstream csvin(filename);

        map<string, string> row;

        while (csvin >> row)
        {
            string tag = row["tag"];
            string content = row["content"];
            correct_labels.push_back(tag);

            if (find(
                    label_unique_list.begin(),
                    label_unique_list.end(), tag) ==
                label_unique_list.end())
            {
                label_unique_list.push_back(tag);
            }

            // put everything in right here
            post_contents.push_back(content);
            classify_list.push_back(unique_words(content));
            new_post_count++;
        }

        double highest_prob = 0;

        // for every post in the new file
        for (int i = 0; i < classify_list.size(); i++)
        {
            highest_prob = 0;
            string highest_prob_tag;
            // for every unique post label
            for (string tag : label_unique_list)
            {
                double post_count_double = (double)post_count;

                // calculates log prior probability
                double tag_post_count = post_count_per_label[tag];
                double new_prob = log(tag_post_count / post_count_double); // good
                // for every unique word in each post
                for (string word : classify_list[i])
                {
                    pair<double, double> counts = {tag_post_count, post_count_double};
                    classify_helper(tag, word, new_prob, counts);
                    // pair<string, string> tag_word = {tag, word};
                    // // adds the log liklihood probability to the log prior probability

                    // // if w is seen in the post do this
                    // //  new_prob += log(label_word_freq_map[p]
                    // // / post_count_per_label[tag]);
                    // //  if w does not occur in posts labeled d,
                    // // but does occur in training data
                    // //   new_prob += log(post_count_per_word[word] / post_count);
                    // //  if w doesn't occur anywhere
                    // // new_prob += log(1 / post_count);
                    // if (label_word_freq_map[tag_word] >= 1)
                    // {
                    //     float tag_word_count = label_word_freq_map[tag_word];
                    //     // double tag_word_count = ;
                    //     new_prob += log(tag_word_count / tag_post_count);
                    // }
                    // else if (post_count_per_word[word] >= 1)
                    // {
                    //     double word_post_count = post_count_per_word[word];
                    //     // double post_count_double = )
                    //     new_prob += log(word_post_count / post_count_double);
                    // }
                    // else
                    // {
                    //     new_prob += log(1.0 / post_count_double);
                    // }
                }

                if (abs(new_prob) < abs(highest_prob) || highest_prob == 0)
                {
                    highest_prob_tag = tag;
                    highest_prob = new_prob;
                }
            }
            calculated_labels.push_back(highest_prob_tag);
            calculated_log.push_back(highest_prob);
        }

        int num_guessed_properly = 0;

        cout
            << "test data:" << endl;
        for (int i = 0; i < new_post_count; i++)
        {
            string correct_label = correct_labels[i];
            string calculated_label = calculated_labels[i];
            cout << "  "
                 << "correct = " << correct_label << ", predicted = ";
            cout << calculated_label << ", log-probability score = "
                 << calculated_log[i] << endl;
            cout << "  "
                 << "content = " << post_contents[i] << endl
                 << endl;
            num_guessed_properly += calculated_label == correct_label;
        }
        cout << "performance: " << num_guessed_properly
             << " / " << new_post_count
             << " posts predicted correctly" << endl;
        //  << endl;
    }

    void train_on_file(string filename)
    {
        // converts file into string stream
        csvstream csvin(filename);
        map<string, string> row;

        if (debug)
            cout << "training data:\n";
        vector<string> words_done;

        while (csvin >> row)
        {

            string tag = row["tag"];
            string content = row["content"];

            // debug stuff
            if (debug)
            {
                cout << "  label = " << tag << ", content = " << content << endl;
            }

            // adds to post_count_per_word map
            if (!key_exists_in_map(post_count_per_label, tag))
                post_count_per_label[tag] = 1;
            else
                post_count_per_label[tag]++;

            vector<string> content_words = unique_words(content);
            vector<string> total_unique_words;

            for (string word : content_words)
            {

                // adds word to total unique words if it
                // doesn't already exist in list
                bool word_is_unique = true;
                for (string unique_word : total_unique_words)
                {
                    if (unique_word == word)
                    {
                        word_is_unique = false;
                    }
                }
                if (word_is_unique)
                {
                    if (find(words_done.begin(),
                             words_done.end(), word) == words_done.end())
                    {
                        words_done.push_back(word);
                        unique_word_count += word_is_unique;
                    }
                    total_unique_words.push_back(word);

                    // increments spot in map for word
                    if (!key_exists_in_map(post_count_per_word, word))
                        post_count_per_word[word] = 1;
                    else
                        post_count_per_word[word]++;

                    // increments spot in map for pair
                    pair<string, string> label_word = {tag, word};
                    if (label_word_freq_map.find(label_word) ==
                        label_word_freq_map.end())
                    {
                        label_word_freq_map[label_word] = 1;
                    }
                    else
                    {
                        label_word_freq_map[label_word]++;
                    }
                }
            }

            // if not already contained in tag_list
            // if (find(tag_list.begin(), tag_list.end(), tag) == tag_list.end())
            tag_list.push_back(tag);

            post_count++;
        }
        sort(tag_list.begin(), tag_list.end());
        cout << "trained on " << post_count << " examples\n";

        if (!debug)
        {
            cout << endl;
        }
        else
        {
            print_debug();
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

    // fstream test_open(filename);
    // if (!test_open.is_open())
    // {
    //     cout << "Error opening file: " << filename << endl;
    //     return 1;
    // }
    // test_open.close();

    Indentifier ident(debug);
    ident.train_on_file(filename);
    ident.classify(argv[2]);
}