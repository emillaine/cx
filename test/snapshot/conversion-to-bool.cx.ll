
define void @_EN4main1fEP4char(ptr %pb) {
  %b = alloca i1, align 1
  store i1 true, ptr %b, align 1
  %pb.load = load i8, ptr %pb, align 1
  %1 = icmp ne i8 %pb.load, 0
  store i1 %1, ptr %b, align 1
  %b.load = load i1, ptr %b, align 1
  ret void
}
