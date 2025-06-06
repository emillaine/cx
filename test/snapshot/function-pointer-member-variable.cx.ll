
%X = type { ptr }

define i32 @_EN4main3fooE3int(i32 %a) {
  ret i32 %a
}

define i32 @main() {
  %x = alloca %X, align 8
  call void @_EN4main1X4initEF3int_3int(ptr %x, ptr @_EN4main3fooE3int)
  %p = getelementptr inbounds %X, ptr %x, i32 0, i32 0
  %p.load = load ptr, ptr %p, align 8
  %1 = call i32 %p.load(i32 42)
  call void @_EN4main1X1fE(ptr %x)
  ret i32 0
}

define void @_EN4main1X4initEF3int_3int(ptr %this, ptr %p) {
  %p1 = getelementptr inbounds %X, ptr %this, i32 0, i32 0
  store ptr %p, ptr %p1, align 8
  ret void
}

define void @_EN4main1X1fE(ptr %this) {
  %p2 = alloca ptr, align 8
  %p = getelementptr inbounds %X, ptr %this, i32 0, i32 0
  %p.load = load ptr, ptr %p, align 8
  %1 = call i32 %p.load(i32 42)
  %p1 = getelementptr inbounds %X, ptr %this, i32 0, i32 0
  %p.load2 = load ptr, ptr %p1, align 8
  %2 = call i32 %p.load2(i32 42)
  %p3 = getelementptr inbounds %X, ptr %this, i32 0, i32 0
  %p.load4 = load ptr, ptr %p3, align 8
  store ptr %p.load4, ptr %p2, align 8
  %p2.load = load ptr, ptr %p2, align 8
  %3 = call i32 %p2.load(i32 42)
  ret void
}
