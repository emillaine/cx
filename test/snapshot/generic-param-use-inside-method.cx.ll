
%"S<int>" = type {}

define i32 @main() {
  %s = alloca %"S<int>", align 8
  call void @_EN4main1SI3intE4initE(ptr %s)
  call void @_EN4main1SI3intE1fE(ptr %s)
  ret i32 0
}

define void @_EN4main1SI3intE4initE(ptr %this) {
  ret void
}

define void @_EN4main1SI3intE1fE(ptr %this) {
  %t = alloca i32, align 4
  call void @_EN4main1SI3intE1gE(ptr %this)
  ret void
}

define void @_EN4main1SI3intE1gE(ptr %this) {
  %t2 = alloca i32, align 4
  ret void
}
