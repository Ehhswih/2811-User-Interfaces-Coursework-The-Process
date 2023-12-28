# Qt Video Player Project

This Qt project is a simple video player that allows you to play video files with language localization support.

## Usage

Before running the application, please follow these instructions:

1. **Command Line Parameters:**
   - The first parameter (mandatory) is the address of the video file.
     - It is recommended to use the command in the `/videos` folder of the current project or provide the address of your own video file.
   - The second parameter (optional) is the address of the corresponding translation file (.qm).
     - The address should be under the `/translations` folder in the project directory.
     - If this parameter is empty, it defaults to English.

## Example Command

./the.exe /videos /translations/the_zh.qm

Replace `/videos` with the actual address of your video file and `/translations/the_zh.qm` with the appropriate translation file if needed.

## Note

- Make sure to provide valid paths for both parameters to ensure the correct functionality of the video player.
- For translation support, consider using the provided translation files in the `/translations` folder.
- Screenshot files are saved in .png format in the working directory.

Feel free to customize the command according to your project structure and file locations.
