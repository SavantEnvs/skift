// Karm::Av::load(Bytes): the audio loader (RIFF chunk walker + WAV fmt/data decoder).
#include <karm/macros>
import Karm.Core;
import Karm.Av;

using namespace Karm;

extern "C" int LLVMFuzzerTestOneInput(u8 const* data, usize len) {
    (void)Av::load(Bytes{data, len});
    return 0;
}
