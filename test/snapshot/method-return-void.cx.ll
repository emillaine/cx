
%C = type {}

define i32 @main() {
  %c = alloca %C, align 8
  call void @_EN4main1C4initE(ptr %c)
  call void @_EN4main1C1fE(ptr %c)
  ret i32 0
}

define void @_EN4main1C4initE(ptr %this) {
  ret void
}

define void @_EN4main1C1fE(ptr %this) {
  ret void
}
