#include <ctype.h>
#include <regex.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  int pattern;                // -e works
  int ignore_upper_lower;     // -i works
  int invert_match;           // -v works
  int output_count_matching;  // -c works
  int output_file_matching;   // -l works
  int number_matching;        // -n works
  int no_file_names;          // -h works
  int suppress_errors;        // -s works
} Options;

int is_option(char* arg);

void search_file(char* pattern, char* filename, int argc, Options flags);

int main(int argc, char* argv[]) {
  FILE* fptr;

  Options flags = {0};
  char buffer[1024];
  int line_number = 1;
  int pattern_index;
  int first_pattern = 1;

  if (argc < 3) {
    printf("Too few arguments");
    return -1;
  } else {
    for (int i = 1; i < argc; i++) {
      if (is_option(argv[i])) {
        switch (argv[i][1]) {
          case 'e':
            flags.pattern = 1;
            break;
          case 'i':
            flags.ignore_upper_lower = 1;
            break;
          case 'v':
            flags.invert_match = 1;
            break;
          case 'c':
            flags.output_count_matching = 1;
            break;
          case 'l':
            flags.output_file_matching = 1;
            break;
          case 'n':
            flags.number_matching = 1;
            break;
          case 'h':
            flags.no_file_names = 1;
            break;
          case 's':
            flags.suppress_errors = 1;
            break;
        }
      } else if (strstr(argv[i], "\"") == 0 && first_pattern == 1) {
        pattern_index = i;
        first_pattern = 0;
      } else {
        search_file(argv[pattern_index], argv[i], argc, flags);
      }
    }
  }

  return 0;
}

int is_option(char* arg) { return arg[0] == '-'; }

void search_file(char* pattern, char* filename, int argc, Options flags) {
  FILE* file;

  int multiple_files;
  int match_count = 0;
  int match_line = 1;

  file = fopen(filename, "r");
  if (!file) {
    if (flags.suppress_errors) {
      return;
    } else {
      perror("Error opening file");
      return;
    }
  }

  if (flags.pattern || flags.ignore_upper_lower || flags.invert_match ||
      flags.output_count_matching || flags.number_matching ||
      flags.no_file_names || flags.suppress_errors) {
    if (argc > 4) {
      multiple_files = 1;
    } else {
      multiple_files = 0;
    }
  } else {
    if (argc > 3) {
      multiple_files = 1;
    } else {
      multiple_files = 0;
    }
  }

  regex_t regex;
  int reti;
  char line[1024];

  // if -i flag is on (ignore upper/lower case)
  if (flags.ignore_upper_lower) {
    tolower(*line);
  }

  // Compile the regular expression
  reti = regcomp(&regex, pattern, REG_EXTENDED);
  if (reti) {
    fprintf(stderr, "Could not compile regex\n");
    fclose(file);
    return;
  }

  // Read the file line by line
  while (fgets(line, sizeof(line), file)) {
    // Remove the newline character if present
    line[strcspn(line, "\n")] = '\0';

    // if -i flag is on (ignore upper/lower case)
    if (flags.ignore_upper_lower) {
      tolower(*line);
    }

    // Execute the regex on the line
    reti = regexec(&regex, line, 0, NULL, 0);
    if ((!flags.invert_match && !reti) ||
        (flags.invert_match && reti == REG_NOMATCH)) {
      // Match found + checking for flags.invert_match

      // checks where if they are true no need to print the matching line
      // (printing match_count or filename of a match instead)
      if (flags.output_count_matching) {
        match_count++;
        continue;
      } else if (flags.output_file_matching) {
        printf("%s\n", filename);
        regfree(&regex);
        fclose(file);
        return;
      }

      // checks where you essentially just modify the matched/inverse matched
      // line
      if (multiple_files) {
        if (flags.no_file_names) {
          if (flags.number_matching) {
            printf("%d:%s\n", match_line, line);
          } else {
            printf("%s\n", line);
          }
        } else {
          if (flags.number_matching) {
            printf("%s:%d:%s\n", filename, match_line, line);
          } else {
            printf("%s:%s\n", filename, line);
          }
        }
      } else {
        if (flags.number_matching) {
          printf("%d:%s\n", match_line, line);
        } else {
          printf("%s\n", line);
        }
      }
    } else if (reti != 0 && reti != REG_NOMATCH) {
      char errbuf[100];
      regerror(reti, &regex, errbuf, sizeof(errbuf));
      fprintf(stderr, "Regex match error: %s\n", errbuf);
      break;
    }
    match_line++;
  }

  if (flags.output_count_matching) {
    printf("%d\n", match_count);
  }

  // Clean up
  regfree(&regex);
  fclose(file);
}