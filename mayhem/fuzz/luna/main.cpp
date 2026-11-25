// Luna::evalStr(Str): the Luna scripting language's parser and tree-walking evaluator.
#include <karm/macros>
import Karm.Core;
import Luna;

using namespace Karm;

extern "C" int LLVMFuzzerTestOneInput(u8 const* data, usize len) {
    if (len > 2048)
        return -1;
    (void)Luna::evalStr(Str{reinterpret_cast<char const*>(data), len});
    return 0;
}
