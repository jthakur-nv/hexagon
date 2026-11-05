#ifndef JKT_COMMON_HXX
#define JKT_COMMON_HXX

namespace JKT {
namespace common {

using u8  = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using i8  = signed char;
using i16 = signed short;
using i32 = signed int;
using i64 = signed long long;

using f32 = float;
using f64 = double;

using usize = decltype(sizeof(0));
using isize = decltype(sizeof(0) - sizeof(0));

}  // namespace common
}  // namespace JKT

#endif  // JKT_COMMON_HXX