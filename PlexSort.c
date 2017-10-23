#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>

int identifyEpisode(char *episode, int digit)
{

  int x = 0;

  if(isdigit(episode[digit]) )
    x += (episode[digit] - '0') * 100;
  else
    return -1;
  if(isdigit(episode[digit+1]))
    x += (episode[digit+1] - '0') * 10;
  else
    return x / 100;
  if(isdigit(episode[digit+2]))
    x += episode[digit+2] - '0';
  else
    return x / 10;

  return x;

}

int findDigit(char *episode, char *prefix)
{

  int i, lenEp, lenPrefix;

  lenEp = strlen(episode) + 1;
  lenPrefix = strlen(prefix);
  for(i = lenPrefix; i < lenEp; i++)
    if(isdigit(episode[i]))
      return i;

  return -1;

}

char **scanDirectory(int *scanCount, char prefix[100], char firstEp[4], char lastEp[4], char *path)
{

  DIR *dir;
  struct dirent *ent;
  int i = 0, j, lenPrefix, intFirstEp, intLastEp, num, digit;
  char *tempPrefix, **temp;

  if(prefix == NULL)
  {
    printf("prefix is null\n");
    return NULL;
  }

  intFirstEp = atoi(firstEp);
  intLastEp = atoi(lastEp);
  lenPrefix = strlen(prefix) + 1;
  tempPrefix = malloc(sizeof(char) * lenPrefix);
  printf("\n");

  // Attempt to open directory
  if ((dir = opendir(path)) != NULL)
  {
    // Scan directory's filenames
    temp = malloc(sizeof(char *) * 100);
    for(i = 0; (ent = readdir(dir)) != NULL; i++)
    {
      for(j = 0; j < lenPrefix - 1; j++)
        tempPrefix[j] = ent->d_name[j];
      tempPrefix[j] = '\0';
      // Ensure no invalid filenames are put into temp
      if(strcmp(tempPrefix, prefix) != 0)
      {
        i--;
        continue;
      }
      // Find element of first number in char array
      digit = findDigit(ent->d_name, tempPrefix);
      if(digit == -1)
      {
        printf("Failed to find digit for %s\nAborting\n", ent->d_name);
        return NULL;
      }
      // Identify episode number
      num = identifyEpisode(ent->d_name, digit);
      // Ensure episode number is not less than first episode of season
      if(num < intFirstEp)
      {
        i--;
        continue;
      }
      // Ensure episode number is not greater than last episode of season
      if(num > intLastEp)
      {
        i--;
        continue;
      }
      // Insert scanned filename into temp
      temp[i] = malloc(sizeof(char) * strlen(ent->d_name) + 1);
      strcpy(temp[i], ent->d_name);
      // Print inserted filename
      printf ("valid: %s\n", temp[i]);
    }
    // Print # of episodes scanned
    *scanCount = i;
    printf("# of episodes to be renamed = %d\n", *scanCount);
    closedir (dir);
  }
  else
    return NULL;

  return temp;

}

char *formEpisode(char **episodes, int scanCount, int count, char season[3], char extension[4], int digit, char firstEp[4], char lastEp[4], char prefix[100])
{

  char *episode, prompt;
  int length, i, num, lenPrefix, intFirstEp, intLastEp;

  lenPrefix = strlen(prefix) + 1;
  intFirstEp = atoi(firstEp);
  intLastEp = atoi(lastEp);
  length = lenPrefix + 12;
  num = identifyEpisode(episodes[count], digit);

  // Ensure scanned episode # is within the bounds of the entered Season
  if(num < intFirstEp || num > intLastEp)
  {
    printf("Out of season episode detected: %s\n", episodes[count]);
    return NULL;
  }

  // Allocate memory for episode string
  episode = malloc(sizeof(char) * length);

  // Fill first parts of episode string with prefix
  for(i = 0; i < lenPrefix - 1; i++)
    episode[i] = prefix[i];

  // Format the rest of the episode string with season, episode, and file extension
  episode[i] = ' ';
  episode[i + 1] = 's';
  episode[i + 2] = season[0];
  episode[i + 3] = season[1];
  episode[i + 4] = 'e';
  if(num >= 100)
  {
    episode[i + 5] = episodes[count][digit];
    episode[i + 6] = episodes[count][digit + 1];
    episode[i + 7] = episodes[count][digit + 2];
  }
  else if(num >= 10)
  {
    episode[i + 5] = '0';
    if(episodes[count][digit] != '0')
    {
      episode[i + 6] = episodes[count][digit];
      episode[i + 7] = episodes[count][digit + 1];
    }
    else
    {
      episode[i + 6] = episodes[count][digit + 1];
      episode[i + 7] = episodes[count][digit + 2];
    }
  }
  else
  {
    episode[i + 5] = '0';
    episode[i + 6] = '0';
    episode[i + 7] = num + '0';
  }

  // Ensure a file is not improperly renamed to episode 000
  if(episode[i + 7] == '0' && episode[i + 6] == '0' && episode[i + 5] == '0')
  {
    printf("File: %s to be renamed as episode '000'. Enter Y to continue or N to abort\n", episodes[count]);
    while(prompt != 'Y' || prompt != 'y')
    {
      scanf("%c", &prompt);
      if(prompt == 'Y' || prompt == 'y')
        continue;
      else if(prompt == 'N' || prompt == 'n')
        return NULL;
      else
        printf("Invalid entry\n");
    }
  }
  episode[i + 8] = '.';
  episode[i + 9] = extension[0];
  episode[i + 10] = extension[1];
  episode[i + 11] = extension[2];
  episode[i + 12] = '\0';

  return episode;

}

int main(void)
{
  char path[1024], prefix[100], season[3], firstEp[4], lastEp[4], extension[4], **episodes, *episode, prompt;
  int i, digit, scanCount, epCount;

  // Identify and print current working directory
  getcwd(path, sizeof(path));
  if(path == NULL)
  {
    printf("Unable to find current working directory\nAborting\n");
    return 0;
  }
  printf("Current working directory: %s\n\n", path);

  // Prompt for file naming conventions
  printf("Enter filename prefix.\n");
  fflush(stdout);
  scanf ("%[^\n]%*c", prefix);
  printf("Enter Season #. (Ex. 01, 02, 10, etc.)\n");
  fflush(stdout);
  scanf("%s", season);
  fflush(stdin);
  printf("Enter first episode of Season %s. (Ex. 009, 015, 101, etc.)\n", season);
  fflush(stdout);
  scanf("%s", firstEp);
  fflush(stdin);
  printf("Enter last episode of Season %s.\n", season);
  fflush(stdout);
  scanf("%s", lastEp);
  printf("Enter the three character file extension excluding the period. (Ex. mkv, avi, etc.)\n");
  fflush(stdout);
  scanf("%s", extension);

  // Count the number of episodes in the season
  epCount = (atoi(lastEp)) - (atoi(firstEp)) + 1;

  // Scan directory and form string array of episode filenames
  episodes = scanDirectory(&scanCount, prefix, firstEp, lastEp, path);
  if(episodes == NULL)
  {
    printf("Failed to scan directory\n");
    return 0;
  }
  if(scanCount != epCount)
  {
    printf("Number of items scanned (%d) does not equal number of episodes in season (%d).\nAborting\n", scanCount, epCount);
    return 0;
  }
  printf("Successfully scanned directory\n");

  // Find location of first digit in first string of **episodes
  digit = findDigit(episodes[0], prefix);
  if(digit == -1)
  {
    printf("Failed to find number location in scanned episode filename.\n");
    return 0;
  }
  printf("Enter 'Y' (capital y) to continue or anything else to abort.\n");
  fflush(stdout);
  scanf(" %c", &prompt);
  if(prompt != 'Y')
  {
    printf("You entered %c\nAborting\n", prompt);
    return 0;
  }

  // Form individual episode names and then rename actual files in scanned directory
  for(i = 0; i < scanCount; i++)
  {
    episode = formEpisode(episodes, scanCount, i, season, extension, digit, firstEp, lastEp, prefix);
    if(episode == NULL)
    {
      printf("Failed to form episode for %s.\nAborting\n", episodes[i]);
      return 0;
    }
    printf("%s\n", episode);
    rename(episodes[i], episode);
  }

  return 0;
}
