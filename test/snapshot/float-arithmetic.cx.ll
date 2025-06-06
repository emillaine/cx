
define i32 @main() {
  %a = alloca float, align 4
  %b = alloca float, align 4
  store float 1.000000e+00, ptr %a, align 4
  %a.load = load float, ptr %a, align 4
  %1 = fneg float %a.load
  %2 = fmul float 2.000000e+00, %1
  store float %2, ptr %b, align 4
  %b.load = load float, ptr %b, align 4
  %3 = fadd float %b.load, 1.000000e+00
  store float %3, ptr %b, align 4
  %b.load1 = load float, ptr %b, align 4
  %4 = fadd float %b.load1, -1.000000e+00
  store float %4, ptr %b, align 4
  ret i32 0
}
