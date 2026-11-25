// Karm::Icc::ColorProfile::from(Bytes): ICC profile header and tag-table parsing.
#include <karm/macros>
import Karm.Core;
import Karm.Icc;

using namespace Karm;

extern "C" int LLVMFuzzerTestOneInput(u8 const* data, usize len) {
    // Parser::header() reads the 128-byte header through a panicking cursor without a length
    // check, so every input shorter than 128 bytes panics in the same place (a known upstream
    // bug, recorded in the PR). libFuzzer runs a 1-byte input before it starts mutating, so
    // without this check the target would abort before it fuzzes anything.
    if (len < 128)
        return -1;
    (void)Icc::ColorProfile::from(Bytes{data, len});
    return 0;
}
