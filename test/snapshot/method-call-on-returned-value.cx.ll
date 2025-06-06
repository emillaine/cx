
%S = type { i32 }
%C = type { i32 }

define %S @_EN4main1sE() {
  %1 = alloca %S, align 8
  call void @_EN4main1S4initE(ptr %1)
  %.load = load %S, ptr %1, align 4
  ret %S %.load
}

define void @_EN4main1S4initE(ptr %this) {
  %i = getelementptr inbounds %S, ptr %this, i32 0, i32 0
  store i32 0, ptr %i, align 4
  ret void
}

define %C @_EN4main1cE() {
  %1 = alloca %C, align 8
  call void @_EN4main1C4initE(ptr %1)
  %.load = load %C, ptr %1, align 4
  ret %C %.load
}

define void @_EN4main1C4initE(ptr %this) {
  %i = getelementptr inbounds %C, ptr %this, i32 0, i32 0
  store i32 0, ptr %i, align 4
  ret void
}

define i32 @main() {
  %1 = alloca %S, align 8
  %2 = alloca %C, align 8
  %3 = alloca %S, align 8
  %4 = alloca %C, align 8
  %5 = call %S @_EN4main1sE()
  store %S %5, ptr %1, align 4
  call void @_EN4main1S1fE(ptr %1)
  %6 = call %C @_EN4main1cE()
  store %C %6, ptr %2, align 4
  call void @_EN4main1C1fE(ptr %2)
  call void @_EN4main1S4initE(ptr %3)
  call void @_EN4main1S1fE(ptr %3)
  call void @_EN4main1C4initE(ptr %4)
  call void @_EN4main1C1fE(ptr %4)
  ret i32 0
}

define void @_EN4main1S1fE(ptr %this) {
  ret void
}

define void @_EN4main1C1fE(ptr %this) {
  ret void
}
