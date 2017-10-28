#ifndef SECTION_DATA_H
#define SECTION_DATA_H

#include "core/binary_file.h"
#include "elf/elf_file.h"
#include "core/event_section.h"

namespace lyn {

class section_data {
public:
	struct mapping {
		enum {
			Data,
			Thumb,
			ARM
		} type;
		unsigned int offset;
	};

	struct symbol {
		std::string name;
		unsigned int offset;
	};

	struct relocation {
		std::string symbol;
		int addend;

		int type;
		unsigned int offset;
	};

	enum output_type {
		NoOut,
		OutROM,
	};

public:
	const std::string& name() const { return mName; }
	const std::vector<relocation>& relocations() const { return mRelocations; }
	const std::vector<symbol>& symbols() const { return mSymbols; }

	event_section make_events() const;

	int mapping_type_at(unsigned int offset) const;

public:
	static std::vector<section_data> make_from_elf(const elf_file& elfFile);

private:
	lyn::binary_file mData;

	std::string mName;
	output_type mOutType = NoOut;

	std::vector<relocation> mRelocations;
	std::vector<symbol> mSymbols;
	std::vector<mapping> mMappings;
};

} // namespace lyn

#endif // SECTION_DATA_H
