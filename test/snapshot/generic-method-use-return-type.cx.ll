
%"X<int>" = type {}
%"X<bool>" = type {}

define i32 @main(%"X<int>" %x) {
  %a = alloca %"X<bool>", align 8
  %1 = alloca %"X<int>", align 8
  store %"X<int>" %x, ptr %1, align 1
  %2 = call %"X<bool>" @_EN4main1XI3intE1fI4boolEE4bool(ptr %1, i1 false)
  store %"X<bool>" %2, ptr %a, align 1
  call void @_EN4main1XI4boolE1gE(ptr %a)
  ret i32 0
}

define %"X<bool>" @_EN4main1XI3intE1fI4boolEE4bool(ptr %this, i1 %u) {
  %x = alloca %"X<bool>", align 8
  %x.load = load %"X<bool>", ptr %x, align 1
  ret %"X<bool>" %x.load
}

define void @_EN4main1XI4boolE1gE(ptr %this) {
  ret void
}
