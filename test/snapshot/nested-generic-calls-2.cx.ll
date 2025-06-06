
%"M<S>" = type { %S }
%S = type { i32 }
%"A<S>" = type {}

define i32 @main() {
  %m = alloca %"M<S>", align 8
  call void @_EN4main1MI1SE4initE(ptr %m)
  call void @_EN4main1MI1SE1fE(ptr %m)
  ret i32 0
}

define void @_EN4main1MI1SE4initE(ptr %this) {
  ret void
}

define void @_EN4main1MI1SE1fE(ptr %this) {
  %a = alloca %"A<S>", align 8
  %1 = alloca %S, align 8
  call void @_EN4main1AI1SE4initE(ptr %a)
  %2 = call %S @_EN4main1AI1SE1aE(ptr %a)
  store %S %2, ptr %1, align 4
  call void @_EN4main1S1iE(ptr %1)
  ret void
}

define void @_EN4main1AI1SE4initE(ptr %this) {
  ret void
}

define %S @_EN4main1AI1SE1aE(ptr %this) {
  %1 = alloca %S, align 8
  call void @_EN4main1S4initE(ptr %1)
  %.load = load %S, ptr %1, align 4
  ret %S %.load
}

define void @_EN4main1S1iE(ptr %this) {
  ret void
}

define void @_EN4main1S4initE(ptr %this) {
  ret void
}
