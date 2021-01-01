#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

using namespace std;

const unsigned char ENV_COLOR_WRAPPER[] = { 0xFB, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xDE, 0x01, 0x00, 0x00, 0x06, 0x00, 0x53, 0x90 };
const int WRAPPER_SIZE = 0x10;
const unsigned long LUT_DL_FPS_RARM_SLINGSHOT_OFFSET = 0x53A0;

int main(int argc, char* argv[]) {
	if (argc > 1) {
		filesystem::path p = argv[1];

		if (filesystem::exists(p) && !filesystem::is_directory(p)) {
			ifstream zobj = ifstream(p, ios::binary);

			unsigned long long fileSize = filesystem::file_size(p);

			if (fileSize < 0x5800)
				return -1;

			char* buffer = new char[fileSize + WRAPPER_SIZE];

			// append new display list
			if (zobj.read(buffer, fileSize)) {
				for (int i = 0; i < WRAPPER_SIZE; i++) {
					buffer[fileSize + i] = ENV_COLOR_WRAPPER[i];
				}
			}

			zobj.close();

			// update alias table
			for (int i = 0; i < 3; i++) {
				buffer[LUT_DL_FPS_RARM_SLINGSHOT_OFFSET + 7 - i] = (fileSize >> 8 * i) & 0xFF;
			}

			p.replace_extension("");
			p.concat("_fixed.zobj");

			if (!filesystem::exists(p)) {
				ofstream f = ofstream(p.filename(), ios::binary);
				f.write(buffer, fileSize + WRAPPER_SIZE);
				f.close();
				cout << "zobj edited successfully\n";
			}
			else cout << "Error: " << p.filename() << " already exists!\n";

			delete[] buffer;
		}
	}
}