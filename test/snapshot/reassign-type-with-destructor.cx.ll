
%X = type {}

define i32 @main() {
  %x = alloca %X, align 8
  %1 = alloca %X, align 8
  call void @_EN4main1X4initE(ptr %x)
  call void @_EN4main1X6deinitE(ptr %x)
  call void @_EN4main1X4initE(ptr %1)
  %.load = load %X, ptr %1, align 1
  store %X %.load, ptr %x, align 1
  call void @_EN4main1X6deinitE(ptr %x)
  ret i32 0
}

define void @_EN4main1X4initE(ptr %this) {
  ret void
}

define void @_EN4main1X6deinitE(ptr %this) {
  ret void
}
