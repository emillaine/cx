
%"S<int>" = type {}
%"S<float>" = type {}

define i32 @main() {
  %1 = alloca %"S<int>", align 8
  %2 = alloca %"S<float>", align 8
  call void @_EN4main1SI3intE4initE(ptr %1)
  call void @_EN4main1SI3intE1fE(ptr %1)
  call void @_EN4main1SI5floatE4initE(ptr %2)
  call void @_EN4main1SI5floatE1fE(ptr %2)
  ret i32 0
}

define void @_EN4main1SI3intE4initE(ptr %this) {
  ret void
}

define void @_EN4main1SI3intE1fE(ptr %this) {
  call void @_EN4main1SI3intE1gE(ptr %this)
  ret void
}

define void @_EN4main1SI5floatE4initE(ptr %this) {
  ret void
}

define void @_EN4main1SI5floatE1fE(ptr %this) {
  call void @_EN4main1SI5floatE1gE(ptr %this)
  ret void
}

define void @_EN4main1SI3intE1gE(ptr %this) {
  ret void
}

define void @_EN4main1SI5floatE1gE(ptr %this) {
  ret void
}
