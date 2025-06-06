
%B = type {}

define i32 @main() {
  %1 = alloca %B, align 8
  call void @_EN4main1B4initE(ptr %1)
  call void @_EN4main1B1fE(ptr %1)
  ret i32 0
}

define void @_EN4main1B4initE(ptr %this) {
  ret void
}

define void @_EN4main1B1fE(ptr %this) {
  ret void
}
