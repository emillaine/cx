
%"S<int>" = type {}

define i32 @main() {
  %s = alloca %"S<int>", align 8
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  call void @_EN4main1SI3intE4initE(ptr %s)
  store i32 0, ptr %1, align 4
  call void @_EN4main1SI3intE1fEP3int(ptr %s, ptr %1)
  store i32 0, ptr %2, align 4
  call void @_EN4main1SI3intE1fEP3int(ptr %s, ptr %2)
  ret i32 0
}

define void @_EN4main1SI3intE4initE(ptr %this) {
  ret void
}

define void @_EN4main1SI3intE1fEP3int(ptr %this, ptr %t) {
  ret void
}
