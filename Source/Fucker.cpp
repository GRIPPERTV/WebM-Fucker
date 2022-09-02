#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>

namespace FS = std::filesystem;

int main(int Total, char* Arguments[]) {
	std::cout << "Starting...\n";
	if (!FS::exists("Workspace")) {
		FS::create_directory("Workspace");
	}

	if (!FS::exists("Images")) {
		FS::create_directory("Images");
	}

	if (!FS::exists("Frames")) {
		FS::create_directory("Frames");
	}

	int FIndex = 1;

	// Don't need to check the whole character sequence
	if (Arguments[1] != 0 && Arguments[1][0] == 'I') {
		FIndex = 2;
	}

	int Index = FIndex;
	#ifdef _WIN32
	std::ofstream Frames("Workspace\\Frames.txt");
	#else
	std::ofstream Frames("Workspace/Frames.txt");
	#endif

	std::cout << "Clearing Workspace, Frames and Images folder...\n";
	Frames.clear();
	FS::remove("Output.webm");
	#ifdef _WIN32
	FS::remove("Workspace\\Audio.aac");
	#else
	FS::remove("Workspace/Audio.aac");
	#endif

	for (const auto& File : FS::directory_iterator("Frames")) {
		FS::remove(File.path());
	}

	if (FIndex == 2) {
		for (const auto& File : FS::directory_iterator("Images")) {
			FS::remove(File.path());
		}

		#ifdef _WIN32
		system(std::string("ffmpeg.exe -loglevel panic -i " + std::string(Arguments[1]) + " \"Images\\Image %05d.png\"").c_str());
		system(std::string("ffmpeg.exe -loglevel panic -i " + std::string(Arguments[1]) + " -vn -acodec copy \"Workspace\\Audio.aac\"").c_str());
		#else
		system(std::string("ffmpeg -loglevel panic -i " + std::string(Arguments[1]) + " \"Images/Image %05d.png\"").c_str());
		system(std::string("ffmpeg -loglevel panic -i " + std::string(Arguments[1]) + " -vn -acodec copy \"Workspace/Audio.aac\"").c_str());
		#endif
	}

	int Current = 0, End = 0;
	std::string Name, Size;

	for (const auto& _ : FS::directory_iterator("Images")) {
		End++;
	}

	for (const auto& File : FS::directory_iterator("Images")) {
		Name = File.path().string().substr(7); Size = "";

		// For better perfomance, name pattern has the identifier "&"
		if (FIndex == 1 && Name.find('&') != std::string::npos) {
			Size = Name.substr(0, Name.length() - 4).substr(Name.find("&") + 1);
		} else {
			if (Arguments[FIndex] == 0) {
				goto SkipConvert;
			} else if (Arguments[FIndex + 1] == 0) {
				Size = Arguments[FIndex];
			} else {
				if (Index == Total) {
					Index = FIndex;
				}

				Size = Arguments[Index++];
			}
		}

		#ifdef _WIN32
		system(std::string("convert.exe \"Images\\" + Name + "\" -resize " + Size + " \"Images\\" + Name + "\"").c_str());
		#else
		system(std::string("convert \"Images/" + Name + "\" -resize " + Size + " \"Images/" + Name + "\"").c_str());
		#endif

		SkipConvert:
		std::string WebM = Name.substr(0, Name.find(".") + 1) + "webm"; 
		#ifdef _WIN32
		Frames << "file '..\\Frames\\" << WebM << "'\n";
		system(std::string("ffmpeg.exe -loglevel panic -framerate 25 -f image2 -i \"Images\\" + Name + "\" \"Frames\\" + WebM + "\"").c_str());
		#else
		Frames << "file '../Frames/" << WebM << "'\n";
		system(std::string("ffmpeg -loglevel panic -framerate 25 -f image2 -i \"Images/" + Name + "\" \"Frames/" + WebM + "\"").c_str());
		#endif
		std::cout << "Converting images resolution and encoder... (" << ++Current << " of " << End << " images)\r" << std::flush;
	}

	std::cout << "Concating WebM images to Output.webm...\n\r";
	Frames.close();
	#ifdef _WIN32
	if (FS::exists("Workspace\\Audio.aac")) {
		system("ffmpeg.exe -loglevel panic -f concat -safe 0 -i \"Workspace\\Frames.txt\" -i \"Workspace\\Audio.aac\" -c copy -c:a libopus Output.webm");
	} else {
		system("ffmpeg.exe -loglevel panic -f concat -safe 0 -i \"Workspace\\Frames.txt\" -c copy Output.webm");
	}
	#else
	if (FS::exists("Workspace/Audio.aac")) {
		system("ffmpeg -loglevel panic -f concat -safe 0 -i \"Workspace/Frames.txt\" -i \"Workspace/Audio.aac\" -c copy -c:a libopus Output.webm");
	} else {
		system("ffmpeg -loglevel panic -f concat -safe 0 -i \"Workspace/Frames.txt\" -c copy Output.webm");
	}
	#endif
	return 0;
}
