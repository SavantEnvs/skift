// Karm::Elf: ELF header / program header / section header parsing, as opstart's loadEntry
// and strata-cm's runner walk a kernel or service image.
#include <karm/macros>
import Karm.Core;
import Karm.Dl.Elf;

using namespace Karm;

extern "C" int LLVMFuzzerTestOneInput(u8 const* data, usize len) {
    Bytes bytes{data, len};
    (void)Elf::sniffAbi(bytes);

    Elf::ElfObject<Elf::CurrentAbi> obj{bytes};
    if (not obj.validate())
        return 0;

    usize sum = 0;
    for (auto prog : obj.iterProgram())
        for (auto b : prog.data)
            sum += b;
    (void)obj.interp();
    (void)obj.section(Str{".handover"});
    for (auto sec : obj.iterSection())
        sum += sec.sh_size;
    (void)sum;
    return 0;
}
