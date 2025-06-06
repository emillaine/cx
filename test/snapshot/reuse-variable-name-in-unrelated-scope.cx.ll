
%"X<int>" = type {}

define void @_EN4main1fE() {
  %index = alloca i32, align 4
  %x = alloca %"X<int>", align 8
  %1 = alloca %"X<int>", align 8
  store i32 0, ptr %index, align 4
  %2 = call %"X<int>" @_EN4main1gE()
  store %"X<int>" %2, ptr %x, align 1
  call void @_EN4main1XI3intE4initE(ptr %1)
  call void @_EN4main1XI3intE1gE(ptr %1)
  ret void
}

define %"X<int>" @_EN4main1gE() {
  %x = alloca %"X<int>", align 8
  call void @_EN4main1XI3intE4initE(ptr %x)
  %x.load = load %"X<int>", ptr %x, align 1
  ret %"X<int>" %x.load
}

define void @_EN4main1XI3intE4initE(ptr %this) {
  ret void
}

define void @_EN4main1XI3intE1gE(ptr %this) {
  %index = alloca i32, align 4
  store i32 0, ptr %index, align 4
  ret void
}
