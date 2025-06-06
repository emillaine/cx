
%X = type {}
%"Y<bool>" = type {}

define i32 @main() {
  %a = alloca %X, align 8
  %1 = alloca %"Y<bool>", align 8
  call void @_EN4main1X4initE(ptr %a)
  call void @_EN4main1YI4boolE4initE(ptr %1)
  %2 = call i32 @_EN4main1YI4boolE1fE(ptr %1)
  call void @_EN4main1X6deinitE(ptr %a)
  ret i32 0
}

define void @_EN4main1X4initE(ptr %this) {
  ret void
}

define void @_EN4main1YI4boolE4initE(ptr %this) {
  ret void
}

define i32 @_EN4main1YI4boolE1fE(ptr %this) {
  ret i32 1
}

define void @_EN4main1X6deinitE(ptr %this) {
  ret void
}
