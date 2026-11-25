// Vaerk::Dtb::Blob::open(Bytes): flattened device tree header, structure and strings blocks,
// walked node by node and property by property like the riscv boot path and dtb-dump do.
#include <karm/macros>
import Karm.Core;
import Vaerk.Dtb;

using namespace Karm;

static usize walk(Vaerk::Dtb::Node node, usize depth) {
    usize sum = 0;
    (void)node.fullname();
    (void)node.name();
    (void)node.address();
    auto props = node.iterProp();
    while (auto prop = props.next()) {
        (void)prop->name();
        (void)prop->sniff();
        sum += prop->raw().len();
        sum += prop->regs32().len();
        sum += prop->regs64().len();
    }
    if (depth >= 64)
        return sum;
    auto children = node.iterChildren();
    while (auto child = children.next())
        sum += walk(*child, depth + 1);
    return sum;
}

extern "C" int LLVMFuzzerTestOneInput(u8 const* data, usize len) {
    if (len < 40) // the fdt header
        return -1;
    // The blob is read as big-endian u32 cells: hand it a u32-aligned copy, as firmware does.
    Vec<u32> buf;
    buf.resize((len + 3) / 4);
    std::memcpy(buf.buf(), data, len);
    auto res = Vaerk::Dtb::Blob::open(Bytes{reinterpret_cast<u8 const*>(buf.buf()), len});
    if (not res)
        return 0;
    auto& blob = res.unwrap();
    (void)blob.memoryReservations();
    (void)blob.initrd();
    (void)walk(blob.root(), 0);
    return 0;
}
