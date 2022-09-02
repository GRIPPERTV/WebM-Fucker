#include <string>
#include <cstdio>
#include <iostream>
#include <filesystem>

int main(int _, char* Arguments[]) {
	std::string Zeros = std::string(Arguments[1]);
	const int Start = std::atoi(Arguments[1]);
	const int End = std::atoi(Arguments[2]);
	std::string Name, Variant, Index;
	Zeros = Zeros.substr(Zeros.find("0"), Zeros.find_last_of("0") + 1);

	for (const auto& File : std::filesystem::directory_iterator("Images")) {
		Name = File.path().string().substr(7);

		for (int I = Start + 1; I < End + 1; I++) {
			Variant = std::to_string(I);

			if (Name.find(Zeros + Variant) != std::string::npos) {
				Name.replace(Name.find(Variant) + Variant.length(), Name.find('.') - 1, " &" + std::string(Arguments[3]) + ".png");
				std::filesystem::rename(File.path(), "Images\\" + Name);
			}
		}
	}

	return 0;
}