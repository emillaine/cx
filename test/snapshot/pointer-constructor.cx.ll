
define void @_EN4main3fooEPP4charP4char(ptr %p, ptr %x) {
  %pp = alloca ptr, align 8
  store ptr %p, ptr %pp, align 8
  %pp.load = load ptr, ptr %pp, align 8
  store ptr %x, ptr %pp.load, align 8
  %pp.load1 = load ptr, ptr %pp, align 8
  %pp.load.load = load ptr, ptr %pp.load1, align 8
  store i8 120, ptr %pp.load.load, align 1
  ret void
}
