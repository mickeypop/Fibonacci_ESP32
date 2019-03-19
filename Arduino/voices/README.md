
The male and Female voice files were made with text to speech
on http://www.fromtexttospeech.com/  of all the voices they sounded the most human.

The DFPlayer can read most any MP3 so feel free to replace your own voices.

File names are not important since the player uses the file index on the SD's FAT file system not the file name.

Setting up the SD card is important because the order the files and directories in the index will determine what plays since they are played by number.

**1st** make the empty folders, look at the Dir_Struct.txt file and note the order of the files is important since DFPlayer library plays the files by their FAT index number not the file name

the order you make them will set the index.
- ie;
  - folder0   would have index 0
    - folder1
      - playFolder(folderNumber, fileNumber);
		
**2nd** copy the files one by one to thier respective folders in the order of their index.
	
**3rd** in the **Soft_tools** folder extract DriveSort_en.zip and run the DriveSort to sort the FAT table on the SD card.
- sort the folders first, then sort each folder seperatly.

DriveSort can show the sort order when done.  You want to match the sort order so the right sounds play at the right times.
