// Karm::Idl::parseModule(Str) + semaCheck: the IDL compiler's (karm-idl) front end.
#include <karm/macros>
import Karm.Core;
import Karm.Idl;

using namespace Karm;

extern "C" int LLVMFuzzerTestOneInput(u8 const* data, usize len) {
    auto mod = Idl::parseModule(Str{reinterpret_cast<char const*>(data), len});
    if (not mod)
        return 0;
    (void)Idl::semaCheck(mod.unwrap());
    return 0;
}
