#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>

void printEpisodes(char **episodes, int epCount)
{

  int i;

  printf("\n");

  for(i = 0; i < epCount; i++)
    printf("%s\n", episodes[i]);
}

// Figure out what the formatted episode number should be
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

  // Iterate through the string until a number is found
  for(i = lenPrefix; i < lenEp; i++)
    if(isdigit(episode[i]))
      return i;

  // Returns -1 if no number could be found in the string
  return -1;
}

char **scanDirectory(int *scanCount, char prefix[100], int firstEp, int lastEp, char *path, int epCount)
{

  DIR *dir;
  struct dirent *ent;
  int i = 0, j, lenPrefix, num, digit;
  char *tempPrefix, **temp;

  // Ensure the passed prefix is not NULL
  if(prefix == NULL)
  {
    printf("prefix is null\n");
    return NULL;
  }

  lenPrefix = strlen(prefix);
  tempPrefix = malloc(sizeof(char) * (lenPrefix + 1));
  printf("\n");

  // Attempt to open directory
  dir = opendir(path);
  if(dir != NULL)
  {

    // Allocate a string array to hold scanned episode filenames
    temp = malloc(sizeof(char *) * epCount);

    // Scan directory's filenames
    for(i = 0; (ent = readdir(dir)) != NULL; i++)
    {

      // Fill tempPrefix with the prefix of scanned filename
      for(j = 0; j < lenPrefix; j++)
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

      // Ensure the digit position for the episode was found
      if(digit == -1)
      {
        printf("Failed to find digit for %s\nAborting\n", ent->d_name);
        return NULL;
      }

      // Identify episode number
      num = identifyEpisode(ent->d_name, digit);

      // Ensure episode number is not less than first episode of season
      if(num < firstEp)
      {
        i--;
        continue;
      }

      // Ensure episode number is not greater than last episode of season
      if(num > lastEp)
      {
        i--;
        continue;
      }

      // Insert scanned filename into temp
      temp[i] = malloc(sizeof(char) * strlen(ent->d_name) + 1);
      strcpy(temp[i], ent->d_name);
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

char *formEpisode(char **episodes, int scanCount, int count, int season, char extension[4], int digit, int firstEp, int lastEp, char prefix[100])
{

  char *episode, prompt;
  int length, i, numEp, lenPrefix;

  lenPrefix = strlen(prefix);
  length = lenPrefix + 12;
  numEp = identifyEpisode(episodes[count], digit);

  // Ensure scanned episode # is within the bounds of the entered Season
  if(numEp < firstEp || numEp > lastEp)
  {
    printf("Out of season episode detected: %s\n", episodes[count]);
    return NULL;
  }

  // Allocate memory for episode string
  episode = malloc(sizeof(char) * length);

  // Fill first parts of episode string with prefix
  for(i = 0; i < lenPrefix; i++)
    episode[i] = prefix[i];

  // Format the rest of the episode string with season, episode, and file extension
  episode[i] = ' ';

  // Determine and format season number
  episode[i + 1] = 's';
  if(season >= 10)
  {
    episode[i + 2] = (season / 10) + '0';
    episode[i + 3] = (season % 10) + '0';
  }
  else
  {
    episode[i + 2] = '0';
    episode[i + 3] = season + '0';
  }

  // Determine and format episode number
  episode[i + 4] = 'e';
  if(numEp >= 100)
  {
    episode[i + 5] = episodes[count][digit];
    episode[i + 6] = episodes[count][digit + 1];
    episode[i + 7] = episodes[count][digit + 2];
  }
  else if(numEp >= 10)
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
    episode[i + 7] = numEp + '0';
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

  // Insert file extension and terminator at the end of the string
  episode[i + 8] = '.';
  episode[i + 9] = extension[0];
  episode[i + 10] = extension[1];
  episode[i + 11] = extension[2];
  episode[i + 12] = '\0';

  return episode;
}

// Sort filename strings in episodic order
char **sortEpisodes(char **episodes, int epCount, int firstEp, int digit)
{

  char **temp;
  int i, num;

  temp = malloc(sizeof(char *) * epCount);

  // Place episode string from passed array to temp based on episode # offset from firstEp
  for(i = 0; i < epCount; i++)
  {
    num = identifyEpisode(episodes[i], digit);
    temp[num - firstEp] = episodes[i];
  }

  // Destroy original array
  free(episodes);

  // Return temp array to replace original array that was passed
  return temp;
}

int main(void)
{
  char path[1024], prefix[100], extension[4], **episodes, *episode, prompt;
  int i, digit, scanCount, epCount, season, firstEp, lastEp;

  // Identify current working directory
  getcwd(path, sizeof(path));

  // Ensure that path was properly assigned
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
  printf("Enter Season #. (Ex. 1, 7, 10, etc.)\n");
  fflush(stdout);
  scanf("%d", &season);
  printf("Enter first episode of Season %d. (Ex. 9, 15, 101, etc.)\n", season);
  fflush(stdout);
  scanf("%d", &firstEp);
  printf("Enter last episode of Season %d.\n", season);
  fflush(stdout);
  scanf("%d", &lastEp);
  printf("Enter the three character file extension excluding the period. (Ex. mkv, avi, mov, etc.)\n");
  fflush(stdout);
  scanf("%s", extension);

  // Count the number of episodes in the season
  epCount = (lastEp - firstEp) + 1;

  // Scan directory and form string array of episode filenames
  episodes = scanDirectory(&scanCount, prefix, firstEp, lastEp, path, epCount);

  // Ensure that episodes did not fail to fill
  if(episodes == NULL)
  {
    printf("Failed to scan directory\n");
    return 0;
  }

  // Ensure that the number of episodes scanned matches the number of episodes in the season
  if(scanCount != epCount)
  {
    printf("Number of items scanned (%d) does not equal number of episodes in season (%d).\nAborting\n", scanCount, epCount);
    return 0;
  }

  printf("Successfully scanned directory\n");

  // Find location of first digit in first string of episodes
  digit = findDigit(episodes[0], prefix);

  // Ensure the episode digit location was properly identified
  if(digit == -1)
  {
    printf("Failed to find number location in scanned episode filename.\n");
    return 0;
  }

  // Sort episodes order
  episodes = sortEpisodes(episodes, epCount, firstEp, digit);

  // Print out the newly sorted scanned list of episode filenames
  printEpisodes(episodes, epCount);

  // Prompt user to confirm the execution of renaming process
  printf("Enter 'Y' (capital y) to continue or anything else to abort.\n");
  fflush(stdout);
  scanf(" %c", &prompt);
  if(prompt != 'Y')
  {
    printf("You entered %c\nAborting\n", prompt);
    return 0;
  }

  printf("\n");

  // Form individual episode names and then rename actual files in scanned directory
  for(i = 0; i < scanCount; i++)
  {

    // Forms the properly formatted filename string for the episode
    episode = formEpisode(episodes, scanCount, i, season, extension, digit, firstEp, lastEp, prefix);

    // Ensure that the episode did not fail to form
    if(episode == NULL)
    {
      printf("Failed to form episode for %s.\nAborting\n", episodes[i]);
      return 0;
    }

    // Print the newly formatted name and then rename the actual file
    printf("%s\n", episode);
    rename(episodes[i], episode);
  }

  return 0;
}
