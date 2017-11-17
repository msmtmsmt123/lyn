#include <iostream>
#include <fstream>

#include "elf/elf_file.h"
#include "core/event_object.h"

lyn::elf_file make_elf(const std::string& fName) {
	lyn::elf_file result;
	std::ifstream file;

	file.open(fName, std::ios::in | std::ios::binary);

	if (!file.is_open())
		throw std::runtime_error(std::string("Couldn't open file for read: ").append(fName));

	result.load_from_stream(file);
	file.close();

	return result;
}

int main(int argc, char** argv) {
	if (argc < 2)
		return 1;

	struct {
		bool linkLocals      = true;
		bool linkAbsolutes   = true;
		bool makeTrampolines = false;
		bool printTemporary  = false;
		bool applyHooks      = true;
	} options;

	std::vector<std::string> elves;

	for (int i=1; i<argc; ++i) {
		std::string argument(argv[i]);

		if (argument.size() == 0)
			continue;

		if (argument[0] == '-') { // option
			if (argument == "-nolink") {
				options.linkLocals      = false;
				options.linkAbsolutes   = false;
			} else if (argument == "-linkabs") {
				options.linkLocals      = false;
				options.linkAbsolutes   = true;
			} else if (argument == "-linkall") {
				options.linkLocals      = true;
				options.linkAbsolutes   = true;
			} else if (argument == "-longcalls") {
				options.makeTrampolines = true;
			} else if (argument == "-raw") {
				options.linkLocals      = false;
				options.linkAbsolutes   = false;
				options.makeTrampolines = false;
				options.applyHooks      = false;
			} else if (argument == "-printtemp") {
				options.printTemporary  = true;
			} else if (argument == "-autohook") {
				options.applyHooks      = true;
			} else if (argument == "-nohook") {
				options.applyHooks      = false;
			}
		} else { // elf
			elves.push_back(std::move(argument));
		}
	}

	try {
		lyn::event_object object;

		for (auto& elf : elves)
			object.append_from_elf(make_elf(elf));

		if (options.linkLocals)
			object.link_locals();

		if (options.makeTrampolines)
			object.make_trampolines();

		if (!options.printTemporary) {
			object.link_temporaries();
			object.remove_temp_symbols();
		}

		if (options.linkAbsolutes)
			object.link_absolutes();

		if (options.applyHooks) {
			for (auto& hook : object.get_hooks()) {
				std::cout << "PUSH" << std::endl;
				std::cout << "ORG $" << std::hex << (hook.originalOffset & (~1)) << std::endl;
				lyn::event_object temp;
				temp.combine_with(lyn::arm_relocator::make_thumb_veneer(hook.name, 0));
				temp.write_events(std::cout);
				std::cout << "POP" << std::endl;
			}
		}

		object.write_events(std::cout);
	} catch (const std::exception& e) {
		std::cout << "ERROR: [lyn] " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
