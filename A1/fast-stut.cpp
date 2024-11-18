//Ali Kirmani
//Tutorial 04
//30115539
//Assignment 1 fast-stut program that returns longest stutter reading file
//Alot of the code from this file was taken directly from the fast-int.cpp file included in the git repository Pavol Federl (2020) aswell as the slow-stut file

#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <string>
#include <vector>
using namespace std;
// split string p_line into a vector of strings (words)
// the delimiters are 1 or more whitespaces

// we'll use global variables to track of characters that we
// read() into memory
unsigned char buffer[1024 * 1024]; // 1MB sotrage to hold results of read()
int buff_size = 0;                 // stores how many characters are stored in buffer
int buff_pos = 0;                  // position in bufffer[] where we extract next char

// returns the next byte from stdin, or -1 on EOF
// returns characters stored in buffer[] until it is empty
// when buffer[] gets empty, it is replenished by calling read()

//This method was taken from fast-int.cpp file from the git repository Pavol Federl (2020)
int fast_read_one_character_from_stdin()
{
  // check if buffer[] is empty
  if (buff_pos >= buff_size)
  {
    // buffer is empty, let's replenish it
    buff_size = read(STDIN_FILENO, buffer, sizeof(buffer));
    // detect EOF
    if (buff_size <= 0)
      return -1;
    // reset position from where we'll return characters
    buff_pos = 0;
  }
  // return the next character from the buffer and update position
  return buffer[buff_pos++];
}

//This file was taken from the slow-stut.cpp file 2024 Jonathan Hudson
std::vector<std::string>
split(const std::string &p_line)
{
  auto line = p_line + " ";
  bool in_str = false;
  std::string curr_word = "";
  std::vector<std::string> res;
  for (auto c : line)
  {
    if (isspace(c))
    {
      if (in_str)
        res.push_back(curr_word);
      in_str = false;
      curr_word = "";
    }
    else
    {
      curr_word.push_back(c);
      in_str = true;
    }
  }
  return res;
}

//The code from fast-int.cpp file from the git repository Pavol Federl (2020)
std::string
stdin_readline()
{
  std::string result;
  while (true)
  {
    int c = fast_read_one_character_from_stdin();
    if (c == -1)
      break;
    result.push_back(c);
    if (c == '\n')
      break;
  }
  return result;
}

// returns true if a word is stutter
// stutter is a string that reads the same twice in a row
// after converting all characters to lower case
//This is taken from slow-stut by Jonathon Hudson
bool is_stutter(const std::string &s)
{
  if (s.size() % 2 != 0)
    return false;
  for (size_t i = 0; i < s.size() / 2; i++)
    if (tolower(s[i]) != tolower(s[i + s.size() / 2]))
      return false;
  return true;
}

// Returns the longest stutter on standard input.
// In case of ties for length, returns the first stutter found.
//
// Algorithm:
// Input is broken into lines, each line into words, and each word
// is checked to see if it is stutter, and if it is, whether it
// is longer than the largest stutter encountered so far.
//
// A word is a sequence of characters separated by white space
// white space is whatever isspace() says it is
//    i.e. ' ', '\n', '\r', '\t', '\n', '\f'

//I got this from the slow-stut file by Hudson 2024
std::string
get_longest_stutter()
{
  std::string max_stut;
  while (1)
  {
    std::string line = stdin_readline();
    if (line.size() == 0)
      break;
    auto words = split(line);
    for (auto word : words)
    {
      if (word.size() <= max_stut.size())
        continue;
      if (is_stutter(word))
        max_stut = word;
    }
  }
  return max_stut;
}

//This was also mostly from the slow-stut file by Hudson 2024
int main()
{
  std::string max_stut = get_longest_stutter();
  printf("Longest stutter: %s\n", max_stut.c_str());
  return 0;
}
