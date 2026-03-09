CLI Toolkit: File Manager & YouTube Media Downloader

A fast, C-based command-line utility built for Windows. This tool serves two primary purposes: bulk-managing directories (listing, reordering, and bulk-renaming files) and acting as an organized wrapper for yt-dlp to automatically download and sort media into structured folders.
Features

    Directory Management (dir): Target specific directories to list contents, bulk-cut characters from filenames, or automatically prefix files with numbered indexes for custom sorting.

    Structured Media Downloading (dl): Wrap yt-dlp commands to automatically download YouTube videos/playlists and sort them neatly into designated Band/Album folder structures.

    Batch Scripting (fread): Automate repetitive tasks by reading and executing commands directly from a text file.

Prerequisites

    OS: Windows (uses <windows.h> native API).

    Compiler: GCC / MinGW (TDM-GCC or MSYS2).

    yt-dlp.exe: Already placed directly in the source project directory.

    FFmpeg: Required for yt-dlp media conversions (e.g., extracting audio, converting .mp4 to .mp3). Ensure FFmpeg is installed and added to your system's PATH environment variable, or place its executables in the same directory as yt-dlp.exe.

Usage & Commands

The application is run via the command line by passing a primary command (dir, dl, fread) followed by its specific arguments.
1. Directory Management (dir)

Use the dir command to manipulate and view files within a specific path.

Basic Usage:
Bash

# Show all files and folders in a path
main.exe dir --show -p "C:\Path"

Display Filters:
You can toggle what gets printed to the console using 1 (show) or 0 (hide):

    -s_path 0|1: Toggle full path display.

    -s_number 0|1: Toggle index number display.

    -s_type 0|1: Toggle file type display.

    -s_name 0|1: Toggle file name display.

    -s_file 0|1: Show or hide standard files.

    -s_folder 0|1: Show or hide directories.

Bulk Renaming & Ordering:
Bash

# Cut characters from filenames (Provide starting position and number of chars to cut)
main.exe dir --cut 0 5 -p "C:\Path"

# Custom order files by prefixing them with index numbers (e.g., 01 - file, 02 - file)
# Provide the new order of indexes separated by spaces
main.exe dir --order "3 1 2 0" -p "C:\Path"

2. Media Downloader (dl)

Use the dl command to download media via yt-dlp and automatically organize it into folders.

Simple Playlist Download:
The simple command is the fastest way to download a playlist. It requires exactly three arguments and will automatically create the folder structure: Output_Directory/Band/Album/01 - Title.ext.
Bash

# Syntax: dl simple <Band> <Album> <URL>
main.exe dl simple "Pink Floyd" "The Dark Side of the Moon" "https://youtube.com/playlist_url"

Advanced Download Options:
You can also build custom download commands using flags:

    -u <url> or -url <url>: Specify the target URL.

    --search: Treat the input string as a YouTube search query (ytsearch:).

    --order: Automatically prepend the playlist index to the downloaded files.

    -ba <Band> <Album>: Specify the Band and Album folder names.

    -path <folder1> <folder2> ... -end: Specify a custom nested folder path.

    -o <format>: Overwrite the yt-dlp output format string.

    -op <options>: Pass custom configuration flags directly to yt-dlp.

3. Script Execution (fread / read)

Execute a series of tool commands sequentially from a text file or standard input.
Bash

# Execute commands listed in script.txt
main.exe fread "C:\path_to_script.txt"
main.exe fread NWA.txt #inside the downloaded folder

# Read exactly 5 commands from standard input
main.exe read 5

Building from Source

To compile the project, navigate to the root directory containing your source code and run the included batch script:
Bash

compile.bat
