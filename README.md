# PlexSort (not a sorting technique)
Bulk renames filenames of TV show episodes according to season in a format easily recognized by Plex media servers.
PlexSort is useful when you have a TV series with many episodes and need to rename every episode to a format like "ShowName s01e001.mkv". This program is in its early stages of development and should not, by any means, be considered stable.

# Restrictions
PlexSort is currently only supported in GNU/Linux. Don't expect this to work in a fresh installation of Windows or macOS without any additional software. Windows support is the next thing on my list to fix, unless more bugs are found.

# How to use
You can execute this like any C program once it is compiled. We'll say you compiled PlexSort.c in your user's home folder and renamed a.out to "PlexSort". Next you navigate to the directory where your TV series is located. I'd recommend you remove anything unnecessary in the directory beforehand, but it shouldn't matter too much. Execute PlexSort in the current directory with "~/PlexSort". It automatically reads your current working directory and prints it to the screen so you can confirm that it will scan the right files. It will ask you for a filename prefix; this is what every filename should have in common. So if you had "Show Name - 001 episode title.mkv" you would just enter "Show Name -" for the prefix. Next it will ask for Season number, just make sure you put a '0' in front of the season number if it is less than 10. It will ask for the first episode of the season's number; make sure you enter it in a format like "001". Next enter the last episode number for the season in the same format. Finally it will ask for a three-letter file extension. For our example it would be "mkv". It should scan the directory print the names of all valid files and the total number of episodes to be renamed. It will ask for confirmation; simply enter 'Y' to proceed. If you enter anything else, it will abort. Finally the files will be renamed with the new names being printed to the screen. Make a new directory for the season, like "Season 01" and move the renamed files into that directory "mv Show\ Name\ s01e0* Season\ 01". Repeat these steps for all consecutive seasons and Plex should successfully recognize all episodes of the series.





This is the first program I've ever produced that has nothing to do with a college assignment, so cut me some slack if my code is sloppy or inefficient. I plan to improve it over time, but wanted to release it to the world first as I think some people could really make use of it.
