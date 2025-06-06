
define i32 @main() {
  %p = alloca ptr, align 8
  %o = alloca ptr, align 8
  %p.load = load ptr, ptr %p, align 8
  store ptr %p.load, ptr %o, align 8
  ret i32 0
}
