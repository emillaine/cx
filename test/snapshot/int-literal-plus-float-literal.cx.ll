
define i32 @main() {
  %a = alloca float, align 4
  %b = alloca float, align 4
  store float 8.000000e+00, ptr %a, align 4
  store float 8.000000e+00, ptr %b, align 4
  ret i32 0
}
