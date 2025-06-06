
%"S<A>" = type {}
%A = type {}

define i32 @main() {
  %x = alloca %"S<A>", align 8
  call void @_EN4main1SI1AE4initE(ptr %x)
  call void @_EN4main1SI1AE1sE(ptr %x)
  ret i32 0
}

define void @_EN4main1SI1AE4initE(ptr %this) {
  %1 = alloca %A, align 8
  call void @_EN4main1A4initE(ptr %1)
  call void @_EN4main1A1hE(ptr %1)
  ret void
}

define void @_EN4main1SI1AE1sE(ptr %this) {
  ret void
}

define void @_EN4main1A4initE(ptr %this) {
  ret void
}

define void @_EN4main1A1hE(ptr %this) {
  ret void
}
